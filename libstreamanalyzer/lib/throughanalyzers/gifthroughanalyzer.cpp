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

#include "gifthroughanalyzer.h"
#include <strigi/strigiconfig.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include <strigi/textutils.h>
#include <strigi/streamthroughanalyzer.h>
#include <strigi/analyzerplugin.h>

#include <string.h>

// this version does not extract comment at the moment
// read http://www.w3.org/Graphics/GIF/spec-gif89a.txt on how to do that

using namespace std;
using namespace Strigi;

namespace Strigi {
    class RegisteredField;
}

// AnalyzerFactory

void
GifThroughAnalyzerFactory::registerFields(FieldRegister& reg) {
    colorDepthField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#colorDepth");
    widthField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#width");
    heightField = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#height");
    typeField = reg.typeField;

    addField(colorDepthField);
    addField(widthField);
    addField(heightField);
    addField(typeField);
}

// Analyzer
void
GifThroughAnalyzer::setIndexable(AnalysisResult* i) {
    analysisResult = i;
}

InputStream*
GifThroughAnalyzer::connectInputStream(InputStream* in) {
    if (in == 0) return 0;
    const char* buf;
    int32_t nread = in->read(buf, 12, -1);
    in->reset(0);

    if (nread < 12 || strncmp(buf, "GIF8", 4) || buf[5] != 'a'
            || (buf[4] != '7' && buf[4] != '9')) {
        return in;
    }

    uint16_t width = readLittleEndianUInt16(buf+6);
    uint16_t height = readLittleEndianUInt16(buf+8);
    analysisResult->addValue(factory->widthField, (uint32_t)width);
    analysisResult->addValue(factory->heightField, (uint32_t)height);

    if (buf[4] == 7) {
        uint8_t colorDepth = (uint8_t)((buf[9] & 0x07) + 1);
        analysisResult->addValue(factory->colorDepthField, (uint32_t)colorDepth);
    }

    analysisResult->addValue(factory->typeField, "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#RasterImage");

    return in;
}

bool
GifThroughAnalyzer::isReadyWithStream() {
    return true;
}
