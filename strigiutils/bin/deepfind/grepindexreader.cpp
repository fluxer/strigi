/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
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
#include "grepindexreader.h"
#include <strigi/analyzerconfiguration.h>
#include <strigi/query.h>
#include <strigi/variant.h>
#include <strigi/indexwriter.h>
#include <strigi/filelister.h>
#include <set>

using namespace Strigi;

/**
 * Custom configuration that extracts specific fields.
 **/
class FieldAnalyzerConfiguration : public AnalyzerConfiguration {
private:
    const std::set<std::string> neededFields;
    mutable std::set<std::string> availableFields;
    signed char m_maxDepth;
    const bool needsAllFields;
public:
    FieldAnalyzerConfiguration(const std::set<std::string>& fields);
    bool useFactory(StreamAnalyzerFactory*) const;
    bool useFactory(StreamEndAnalyzerFactory*) const;
    bool hasAllFields() const;
};
FieldAnalyzerConfiguration::FieldAnalyzerConfiguration(const std::set<std::string>& f)
        :neededFields(f), m_maxDepth(-1), needsAllFields(f.find("")!=f.end()) {
}
bool
FieldAnalyzerConfiguration::useFactory(StreamAnalyzerFactory* f) const {
    bool use = needsAllFields;
    const std::vector<const RegisteredField*>& fields = f->registeredFields();
    std::vector<const RegisteredField*>::const_iterator i;
    for (i = fields.begin(); i != fields.end(); ++i) {
        const RegisteredField* field = *i;
        do {
            if (neededFields.find(field->key()) != neededFields.end()) {
                availableFields.insert(field->key());
                use = true;
            }
            field = field->parent();
        } while (field);
    }
    return use;
}
bool
FieldAnalyzerConfiguration::useFactory(StreamEndAnalyzerFactory* f) const {
    return f->analyzesSubStreams() || static_cast<StreamAnalyzerFactory*>(f);
}

class QueryIndexWriter : public IndexWriter {
public:
    void startAnalysis(const AnalysisResult*) {}
    void addText(const AnalysisResult* result, const char* text, int32_t length) {}
    void addValue(const AnalysisResult* result, const RegisteredField* field,
        const std::string& value) {}
    void addValue(const AnalysisResult* result, const RegisteredField* field,
        const unsigned char* data, uint32_t size) {}
    void addValue(const AnalysisResult* result, const RegisteredField* field,
        int32_t value) {}
    void addValue(const AnalysisResult* result, const RegisteredField* field,
        uint32_t value) {}
    void addValue(const AnalysisResult* result, const RegisteredField* field,
        double value) {}
    void addValue(const AnalysisResult* result, const RegisteredField* field,
        const std::string& name, const std::string& value) {}
    void finishAnalysis(const AnalysisResult* result) {}
    void addTriplet(const std::string& subject,
        const std::string& predicate, const std::string& object) {}
public:
    void commit() { return; }
    void deleteEntries(const std::vector<std::string>& entries) {}
    void deleteAllEntries() {}
    /**
     * @brief Return the number of objects that are currently in the cache.
     **/
    virtual int itemsInCache() { return 0; }
    void optimize() {}
    void initWriterData(const Strigi::FieldRegister& fieldRegister) {}
    void releaseWriterData(const Strigi::FieldRegister& fieldRegister) {}
};

class GrepIndexReader::Private {
public:
    const std::string dir;

    Private(const std::string& d) :dir(d) {}
};

GrepIndexReader::GrepIndexReader(const std::string& dir) :p(new Private(dir)) {
}
GrepIndexReader::~GrepIndexReader() {
    delete p;
}
void
getFields(std::set<std::string>& fields, const Query& query) {
    copy(query.fields().begin(), query.fields().end(),
        inserter(fields, fields.begin()));
    for (std::vector<Query>::const_iterator i = query.subQueries().begin();
            i != query.subQueries().end(); ++i) {
        getFields(fields, *i);
    }
}
int32_t
GrepIndexReader::countHits(const Query& query) {
    QueryIndexWriter qiw;
    // make an analyzerconfiguration with a limited set of fields
    std::set<std::string> fields;
    getFields(fields, query);
    FieldAnalyzerConfiguration conf(fields);
    StreamAnalyzer analyzer(conf);
    analyzer.setIndexWriter(qiw);
    return -1;
}
std::vector<IndexedDocument>
GrepIndexReader::query(const Query&, int offset, int max) {
    std::vector<IndexedDocument> hits;
    return hits;
}
void
GrepIndexReader::getHits(const Strigi::Query&,
        const std::vector<std::string>& fields,
        const std::vector<Strigi::Variant::Type>& types,
        std::vector<std::vector<Strigi::Variant> >& result, int off, int max) {
    result.clear();
}
std::map<std::string, time_t>
GrepIndexReader::files(char depth) {
    std::map<std::string, time_t> files;
    return files;
}
int32_t
GrepIndexReader::countDocuments() { return -1; }
int32_t
GrepIndexReader::countWords() { return -1; }
int64_t
GrepIndexReader::indexSize() {
    // we have no index :-)
    return 0;
}
/**
 * This does not have to be implemented since we have not index.
 **/
time_t
GrepIndexReader::mTime(const std::string& uri) {
    return -1;
}
std::vector<std::string>
GrepIndexReader::fieldNames() {
    std::vector<std::string> fieldnames;
    return fieldnames;
}
std::vector<std::pair<std::string,uint32_t> >
GrepIndexReader::histogram(const std::string& query, const std::string& fieldname,
            const std::string& labeltype) {
    std::vector<std::pair<std::string,uint32_t> > histogram;
    return histogram;
}
int32_t
GrepIndexReader::countKeywords(const std::string& keywordprefix,
        const std::vector<std::string>& fieldnames) {
    return -1;
}
std::vector<std::string>
GrepIndexReader::keywords(
        const std::string& keywordmatch,
        const std::vector<std::string>& fieldnames,
        uint32_t max, uint32_t offset) {
    std::vector<std::string> keywords;
    return keywords;
}
int
main() {
    GrepIndexReader("/home");
    return 0;
}
