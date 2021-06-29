/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2008 Jos van den Oever <jos@vandenoever.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <strigi/strigiconfig.h>
#include <strigi/indexmanager.h>
#include <strigi/indexwriter.h>
#include <strigi/diranalyzer.h>
#include <strigi/analyzerconfiguration.h>
#include <iostream>
#include <map>
#include <cassert>
#include <cmath>
#include <climits>
#include <sys/time.h>
#include <time.h>

using namespace Strigi;

float
elapsed(const struct timeval& a, const struct timeval& b) {
    return (float)(a.tv_sec - b.tv_sec)
            + (float)(a.tv_usec - b.tv_usec) / 1.0e6f;
}

class LatencyMeasurer : public AnalyzerConfiguration {
private:
    class Private;
    Private* const d;

    // We implement this function so we can count the number of files analyzed.
    bool indexFile(const char* path, const char* filename) const;
    // This is the function that analyzers should call often to reduce latency.
    // Here we always let the measurement continue so we can measure more.
    bool indexMore() const;
public:
    LatencyMeasurer();
    ~LatencyMeasurer();
    void printReport();
};

class LatencyMeasurer::Private {
public:
    struct timeval starttime, lasttime;
    int32_t numberOfChecks;
    long numberOfFiles;
    std::map<int, int> histogram;
    std::string beforeLastFile;
    struct timeval beforeLastTime;
    std::string lastFile;
    struct timeval lastTime;
    Private() :numberOfChecks(0), numberOfFiles(0) {
        starttime.tv_sec = -1;
    }
    void print();
};
// We implement this function so we can count the number of files analyzed.
bool
LatencyMeasurer::indexFile(const char* path, const char* filename) const {
    d->beforeLastFile.assign(d->lastFile);
    d->beforeLastTime = d->lastTime;
    d->lastFile.assign(path);
    gettimeofday(&d->lastTime, NULL);
    d->numberOfFiles++;
    return true;
}
// This is the function that analyzers should call often to reduce latency.
// Here we always let the measurement continue so we can measure more.
bool
LatencyMeasurer::indexMore() const {
    d->numberOfChecks++;
    struct timeval now;
    gettimeofday(&now, NULL);
    if (d->starttime.tv_sec == -1) {
        d->lasttime = d->starttime = now;
    }
    d->histogram[static_cast<int>(10*log10(elapsed(now, d->lasttime)))]++;
    if (elapsed(now, d->lasttime) > 1) {
        std::cerr << d->beforeLastFile << " started "
            << elapsed(now, d->beforeLastTime) << " seconds ago." << std::endl;
        std::cerr << d->lastFile << " started "
            << elapsed(now, d->lastTime) << " seconds ago." << std::endl;
        assert(elapsed(now, d->lasttime) < 3);
    }
    d->lasttime = now;
    return true;
}
LatencyMeasurer::LatencyMeasurer() :d(new Private()) {
}
LatencyMeasurer::~LatencyMeasurer() {
    delete d;
}
void
LatencyMeasurer::printReport() {
    d->print();
}
void
LatencyMeasurer::Private::print() {
    struct timeval now;
    gettimeofday(&now, NULL);
    std::cout << numberOfChecks << " checks in " << numberOfFiles << " files."
        << std::endl;
    std::cout << "On average " << (elapsed(now, starttime)/(float)numberOfChecks)
        << " seconds between checks." << std::endl;
    int smallestTime = INT_MAX;
    int largestTime = INT_MIN;
    double total = 0;
    for (std::map<int,int>::const_iterator i = histogram.begin();
            i != histogram.end(); ++i) {
        int n = i->first;
        total += pow(10.0, 0.1*n) * histogram[n];
        if (n > largestTime && n < 1000) largestTime = n;
        if (n < smallestTime && n > -1000) smallestTime = n;
    }
    double sum = 0;
    for (int n=smallestTime; n<=largestTime; ++n) {
        sum += pow(10.0,0.1*n) * histogram[n]/total;
        std::cout << pow(10.0,0.1*n) << '\t' << 1-sum << std::endl;
    }
}

class DummyWriter : public IndexWriter {
private:
    void startAnalysis(const AnalysisResult*) {}
    void addText(const AnalysisResult*, const char*, int32_t) {}
    void addValue(const AnalysisResult*, const RegisteredField*, const std::string&) {}
    void addValue(const AnalysisResult*, const RegisteredField*, const unsigned char*, uint32_t) {}
    void addValue(const AnalysisResult*, const RegisteredField*, int32_t) {}
    void addValue(const AnalysisResult*, const RegisteredField*, uint32_t) {}
    void addValue(const AnalysisResult*, const RegisteredField*, double) {}
    void addValue(const AnalysisResult*, const RegisteredField*, const std::string&, const std::string&) {} 
    void finishAnalysis(const AnalysisResult*) {}
    void addTriplet(const std::string&, const std::string&, const std::string&) {}
    void deleteEntries(const std::vector<std::string>&) {}
    void deleteAllEntries() {}
};

class DummyManager : public IndexManager {
private:
    DummyWriter dummywriter;
    IndexReader* indexReader() { return 0; }
    IndexWriter* indexWriter() { return &dummywriter; }
};

int
main(int argc, char** argv) {
    if (argc == 1) {
        std::cerr << argv[0]
            << " is a tool for testing the latency of the analyzers." << std::endl;
        std::cerr << "Provide a directory to test on." << std::endl;
        return 1;
    }

    LatencyMeasurer measurer;
    DummyManager manager;
    DirAnalyzer analyzer(manager, measurer);
    int nthreads = 1;
    for (int32_t i=1; i<argc; ++i) {
        analyzer.analyzeDir(argv[i], nthreads);
    }

    measurer.printReport();
    return 0;
}
