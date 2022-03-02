/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
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
#include "grepindexwriter.h"
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>

#include <regex>

class GrepIndexWriter::Private
{
public:
    Private() {}
    std::regex indexregex;
};

GrepIndexWriter::GrepIndexWriter(const char* re)
  : d(new Private()) {
    try {
        d->indexregex = std::regex(re, std::regex_constants::optimize);
    } catch (const std::regex_error &err) {
        fprintf(stderr, "%s\n", err.what());
    } catch (...) {
        fprintf(stderr, "Exception caught during regex creation\n");
    }
}
GrepIndexWriter::~GrepIndexWriter() {
    delete d;
}

void
GrepIndexWriter::startAnalysis(const Strigi::AnalysisResult* idx) {
}

void
GrepIndexWriter::finishAnalysis(const Strigi::AnalysisResult* idx) {
}

void
GrepIndexWriter::addText(const Strigi::AnalysisResult* idx, const char* text,
        int32_t length) {
    // unfortunately we have to copy the incoming stream because regexec()
    // assumes a null-terminated string and we are not allowed to modify the
    // incoming message
    std::string s;
    const char* start = text;
    const char* end = text+length;
    const char* p = start;
    while (p < end) {
        // look at each line separately
        if (*p == '\n' || *p == '\r') {
            s.assign(start, p-start);
            if (std::regex_match(s.c_str(), d->indexregex) == true) {
                printf("%s:%s\n", idx->path().c_str(), s.c_str());
            }
            start = p+1;
        }
        p++;
    }
    s.assign(start, p-start);
    if (std::regex_match(s.c_str(), d->indexregex) == true) {
        printf("%s:%s\n", idx->path().c_str(), s.c_str());
    }
}

void
GrepIndexWriter::addValue(const Strigi::AnalysisResult* idx,
            const Strigi::RegisteredField* field, const std::string& value) {
    if (std::regex_match(value.c_str(), d->indexregex) == true) {
        printf("%s:%s:%s\n", idx->path().c_str(),
            field->key().c_str(), value.c_str());
    }
}

void
GrepIndexWriter::addValue(const Strigi::AnalysisResult* idx,
            const Strigi::RegisteredField* field,
            const unsigned char* data, uint32_t size) {
    if (!field->properties().binary()) {
        std::string value((const char*)data, size);
        addValue(idx, field, value);
    }
}
