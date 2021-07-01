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

#include "digesteventanalyzer.h"
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include <list>

using namespace Strigi;

DigestEventAnalyzer::DigestEventAnalyzer(const DigestEventAnalyzerFactory* f)
        :factory(f) {
    analysisresult = 0;
    hash.resize(40);
}
DigestEventAnalyzer::~DigestEventAnalyzer() {
}
void
DigestEventAnalyzer::startAnalysis(AnalysisResult* ar) {
    analysisresult = ar;
    sha1.Reset();
}
void
DigestEventAnalyzer::handleData(const char* data, uint32_t length) {
    sha1.Update((unsigned char*)data, length);
}
namespace {
    const std::string type("http://www.w3.org/1999/02/22-rdf-syntax-ns#type");
    const std::string nfoFileHash(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#FileHash");
    const std::string nfohashAlgorithm(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hashAlgorithm");
    const std::string SHA1("SHA1");
    const std::string hashValue(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hashValue");
}
void
DigestEventAnalyzer::endAnalysis(bool complete) {
    if (!complete) {
        return;
    }
    unsigned char digest[20];
    char d[41];
    sha1.Final();
    sha1.GetHash(digest);
    for (int i = 0; i < 20; ++i) {
        sprintf(d + 2 * i, "%02x", digest[i]);
    }
    hash.assign(d);
    const std::string hashUri = analysisresult->newAnonymousUri();
    analysisresult->addValue(factory->shafield, hashUri);
    analysisresult->addTriplet(hashUri, type, nfoFileHash);
    analysisresult->addTriplet(hashUri, nfohashAlgorithm, SHA1);
    analysisresult->addTriplet(hashUri, hashValue, hash);
    analysisresult = 0;
}
bool
DigestEventAnalyzer::isReadyWithStream() {
    return false;
}
void
DigestEventAnalyzerFactory::registerFields(FieldRegister& reg) {
    shafield = reg.registerField(
        "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#hasHash");
    addField(shafield);
}

#include "SHA1.cpp"
