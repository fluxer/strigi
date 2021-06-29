/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2002 Shane Wright <me@shanewright.co.uk>
 * Copyright (C) 2002 Ryan Cumming <bodnar42@phalynx.dhs.org>
 * Copyright (C) 2007 Flavio Castelli <flavio.castelli@gmail.com>
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

#ifndef STRIGI_RIFFEVENTANALYZER_H
#define STRIGI_RIFFEVENTANALYZER_H

#include <strigi/streameventanalyzer.h>
#include <strigi/analyzerplugin.h>
#include <stack>

namespace Strigi {

class RiffEventAnalyzerFactory;
class RiffEventAnalyzer : public Strigi::StreamEventAnalyzer {
private:
    class RiffChunk {
    public:
        uint32_t type;
        uint32_t size;
        uint64_t start;
    };
    enum State {
        StartOfChunkHeader, StartOfChunkList, ChunkBody
    };

    Strigi::AnalysisResult* analysisresult;
    const RiffEventAnalyzerFactory* const factory;
    char* left;
    uint32_t leftSize;
    uint32_t nLeft;
    uint64_t offset;
    bool valid;
    State state;
    std::stack<RiffChunk> chunks;
    char chunkBuffer[56];
    bool inAudioStream;
    uint32_t bytes_per_second;

    const char* name() const { return "RiffEventAnalyzer"; }
    void startAnalysis(Strigi::AnalysisResult*);
    void endAnalysis(bool complete);
    void handleData(const char* data, uint32_t length);
    bool isReadyWithStream();

    bool processAvih();
    bool processStrh();
    bool processStrf();
    bool processFmt();
    void handleChunkData(uint64_t off, const char* data, uint32_t length);
    void appendData(const char* data, uint32_t length);
public:
    RiffEventAnalyzer(const RiffEventAnalyzerFactory*);
    ~RiffEventAnalyzer();
};

class RiffEventAnalyzerFactory
        : public Strigi::StreamEventAnalyzerFactory {
friend class RiffEventAnalyzer;
public:
    const Strigi::RegisteredField* shafield;
private:
    const char* name() const {
        return "RiffEventAnalyzer";
    }
    const Strigi::RegisteredField* typeField;
    const Strigi::RegisteredField* lengthField;
    const Strigi::RegisteredField* resolutionHeightField;
    const Strigi::RegisteredField* resolutionWidthField;
    const Strigi::RegisteredField* frameRateField;
    const Strigi::RegisteredField* videoCodecField;
    const Strigi::RegisteredField* audioCodecField;
    const Strigi::RegisteredField* sampleSizeField;
    const Strigi::RegisteredField* sampleRateField;
    const Strigi::RegisteredField* channelsField;
    void registerFields(Strigi::FieldRegister&);
    Strigi::StreamEventAnalyzer* newInstance() const {
        return new RiffEventAnalyzer(this);
    }
};

}

#endif
