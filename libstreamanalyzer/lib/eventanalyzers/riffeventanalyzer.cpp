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

#include "riffeventanalyzer.h"
#include <iostream>
#include "rdfnamespaces.h"
#include <strigi/textutils.h>
#include <strigi/strigiconfig.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include <cstring>
#include <cstdlib>

const std::string
    videoClassName( STRIGI_NFO "Video"),
    audioClassName( STRIGI_NFO "Audio");

using namespace Strigi;

// AnalyzerFactory


RiffEventAnalyzer::RiffEventAnalyzer(const RiffEventAnalyzerFactory* f)
        :factory(f) {
    analysisresult = 0;
    leftSize = 0;
    left = 0;
}
RiffEventAnalyzer::~RiffEventAnalyzer() {
    free(left);
}
bool
RiffEventAnalyzer::processAvih() {
    AnalysisResult* a = analysisresult;
    const char* c = chunkBuffer;
    const RiffEventAnalyzerFactory* f = factory;

    uint32_t avih_microsecperframe = readLittleEndianUInt32(c);
    a->addValue(f->frameRateField, uint32_t(1000000/avih_microsecperframe));
    uint32_t avih_totalframes = readLittleEndianUInt32(c+16);
    uint32_t length = avih_microsecperframe/1000*avih_totalframes/1000;
    a->addValue(f->lengthField, length);
    a->addValue(f->resolutionWidthField, readLittleEndianUInt32(c+32));
    a->addValue(f->resolutionHeightField, readLittleEndianUInt32(c+36));
    return true;
}

bool
RiffEventAnalyzer::processStrh() {
    AnalysisResult* a = analysisresult;
    const char* c = chunkBuffer;
    const RiffEventAnalyzerFactory* f = factory;

    inAudioStream = false;
    uint32_t type = readLittleEndianUInt32(c);
    if (type == 0x73646976) { // vids
        if (isalnum(c[4]) && isalnum(c[5]) && isalnum(c[6]) && isalnum(c[7])) {
            a->addValue(f->videoCodecField, std::string(c+4, 4));
        } else {
            return false;
        }
    } else if (type == 0x73647561) { // auds
        inAudioStream = true;
    }

    // Add the correct type
    if( inAudioStream )
        analysisresult->addValue(factory->typeField, audioClassName);
    else
        analysisresult->addValue(factory->typeField, videoClassName);

    return true;
}
const char*
resolve_audio(uint16_t id) {
    /*
    this really wants to use some sort of KDE global
    list.  To avoid bloat for the moment it only does
    a few common codecs
    */

    static const char codec_unknown[] = "Unknown";
    static const char codec_01[]  = "Microsoft PCM";
    static const char codec_02[]  = "Microsoft ADPCM";
    static const char codec_50[]  = "MPEG";
    static const char codec_55[]  = "MP3";
    static const char codec_92[]  = "AC3";
    static const char codec_160[] = "WMA1";
    static const char codec_161[] = "WMA2";
    static const char codec_162[] = "WMA3";
    static const char codec_2000[] = "DVM";
    switch (id) {
        case 0x000 : return codec_unknown; break;
        case 0x001 : return codec_01; break;
        case 0x002 : return codec_02; break;
        case 0x050 : return codec_50; break;
        case 0x055 : return codec_55; break;
        case 0x092 : return codec_92; break;
        case 0x160 : return codec_160; break;
        case 0x161 : return codec_161; break;
        case 0x162 : return codec_162; break;
        case 0x2000 : return codec_2000; break;
        default : return codec_unknown;
    }

    return NULL;
}
bool
RiffEventAnalyzer::processStrf() {
    if (inAudioStream) {
        uint16_t audiotype = readLittleEndianUInt16(chunkBuffer);
        const char* type = resolve_audio(audiotype);
        if (type) {
            analysisresult->addValue(factory->audioCodecField, type);
        }
    }
    return true;
}
bool
RiffEventAnalyzer::processFmt() {
    AnalysisResult* a = analysisresult;
    const char* c = chunkBuffer;
    const RiffEventAnalyzerFactory* f = factory;

    a->addValue(f->channelsField, readLittleEndianUInt16(c+2));
    a->addValue(f->sampleRateField, readLittleEndianUInt32(c+4));
    bytes_per_second = readLittleEndianUInt32(c+8);
    a->addValue(f->sampleSizeField, readLittleEndianUInt16(c+14));
    a->addValue(factory->typeField, audioClassName);
    return true;
}
void
RiffEventAnalyzer::startAnalysis(AnalysisResult* ar) {
    analysisresult = ar;
    valid = true;
    nLeft = 0;
    offset = 0;
    bytes_per_second = 0;
    state = StartOfChunkHeader;
    while (!chunks.empty()) chunks.pop();
}
void
RiffEventAnalyzer::appendData(const char* data, uint32_t length) {
    if (leftSize - nLeft < length) {
        leftSize += length;
        left = (char*)realloc(left, leftSize);
    }
    memcpy(left + nLeft, data, length);
    nLeft += length;
}
uint32_t
getMaxForType(uint32_t type) {
    switch (type) {
    case 0x68697661: // avih
        return 52;
    case 0x68727473: // strh
        return 40;
    case 0x66727473: // strf
        return 2; // atm we do not need more
    case 0x20746D66: // 'fmt '
        return 16;
    };
   return 0;
}
void
RiffEventAnalyzer::handleChunkData(uint64_t off, const char* data,
        uint32_t length) {
    const RiffChunk &chunk = chunks.top();

    // short WAVE intermezzo ...
    if (chunk.type == 0x61746164) { // data
        if (bytes_per_second) {
            float wav_seconds = chunk.size / (float)bytes_per_second;
            analysisresult->addValue(factory->lengthField, wav_seconds);
        }
        bytes_per_second = 0;
    }

    uint32_t max = getMaxForType(chunk.type);
    if (max <= 0) return;

    uint32_t s = off - chunk.start;
    if (s >= max) return;

    // determine how many bytes to copy to the buffer
    uint32_t l = std::min(max, length - s);
    memmove(chunkBuffer + s, data, l);

    if (max > s + l) { // not enough data yet
        return;
    }

    if (chunk.type == 0x68697661) { // avih
        processAvih();
    } else if (chunk.type == 0x68727473) { // strh
        processStrh();
    } else if (chunk.type == 0x66727473) { //strf
        processStrf();
    } else if (chunk.type == 0x20746D66) { // 'fmt '
        processFmt();
    }
}
void
RiffEventAnalyzer::handleData(const char* data, uint32_t length) {
    if (!valid) return;
    // if data was left over from the last analysis, append the new data
    // after it
    if (nLeft > 0) {
        // this can be more efficient, we really only need 8 bytes of buffer
        // but that requires a bit more careful programming
        appendData(data, length);
        data = left;
        length = nLeft;
    }
    uint32_t pos = 0;
    while (length - pos > 0) {
        if (state == StartOfChunkHeader) {
            if (length - pos < 8) {
                break;
            }
            RiffChunk r;
            r.type = readLittleEndianUInt32(data + pos);
            if (offset + pos == 0 && r.type != 0x46464952) { // RIFF
                valid = false;
                return;
            }
            r.size = readLittleEndianUInt32(data + pos + 4);
            r.start = offset + pos + 8;
            // add padding byte if needed
            if (r.size % 2 == 1) {
                r.size++;
            }
            if (r.size > 0 || r.type == 0x46464952) {
                chunks.push(r);
                // is this a RIFF or a LIST?
                if (r.type == 0x46464952 || r.type == 0x5453494C) {
                    state = StartOfChunkList;
                } else {
                    state = ChunkBody;
                }
            } else {
                valid = false;
                return;
            }
            pos += 8;
        } else if (state == StartOfChunkList) {
            if (length - pos < 4) {
                break;
            }
            pos += 4;
            state = StartOfChunkHeader;
        } else {
            handleChunkData(offset + pos, data + pos, length - pos);
            uint32_t fp = chunks.top().start + chunks.top().size;
            if (fp <= offset + length) {
                pos = fp - offset;
                do {
                    chunks.pop();
                } while (chunks.size()
                    && fp == chunks.top().start + chunks.top().size);
                state = StartOfChunkHeader;
            }
            if (state != StartOfChunkHeader ||
                pos > length ) {
                pos = length;
            }
        }
    }

    if (data == left) {
        // move the data that's left to the start
        nLeft = length - pos;
        memmove(left, left+pos, nLeft);
    } else {
        nLeft = 0;
    }
    // store the unprocessed data
    appendData(data+pos, length-pos);
    // advance the position
    offset += pos;
}
void
RiffEventAnalyzer::endAnalysis(bool complete) {
    analysisresult = 0;
}
bool
RiffEventAnalyzer::isReadyWithStream() {
    return !valid;
}
void
RiffEventAnalyzerFactory::registerFields(Strigi::FieldRegister& reg) {
    typeField = reg.typeField;
    sampleSizeField = reg.registerField(
        STRIGI_NFO "bitsPerSample");
    sampleRateField = reg.registerField(
        STRIGI_NFO "sampleRate");
    channelsField = reg.registerField(
        STRIGI_NFO "channels");
    lengthField = reg.registerField(
        STRIGI_NFO "duration");
    resolutionHeightField = reg.registerField(
        STRIGI_NFO "height");
    resolutionWidthField = reg.registerField(
        STRIGI_NFO "width");
    frameRateField = reg.registerField(
        STRIGI_NFO "frameRate");
    videoCodecField = reg.registerField(
        STRIGI_NFO "codec");
    audioCodecField = reg.registerField(
        STRIGI_NFO "codec");

    addField(sampleSizeField);
    addField(sampleRateField);
    addField(channelsField);
    addField(lengthField);
    addField(resolutionHeightField);
    addField(resolutionWidthField);
    addField(frameRateField);
    addField(videoCodecField);
    addField(audioCodecField);
}
