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
#include "eventthroughanalyzer.h"
#include <strigi/streamsaxanalyzer.h>
#include <strigi/streameventanalyzer.h>
#include "saxeventanalyzer.h"
#include "lineeventanalyzer.h"
#include <strigi/streamlineanalyzer.h>
#include <iostream>

using namespace Strigi;

EventThroughAnalyzer::~EventThroughAnalyzer() {
    if (datastream) {
        delete datastream;
    }
    std::vector<StreamEventAnalyzer*>::iterator e;
    for (e = event.begin(); e != event.end(); ++e) {
        delete *e;
    }
}
void
EventThroughAnalyzer::setIndexable(AnalysisResult* r) {
    result = r;
}
InputStream*
EventThroughAnalyzer::connectInputStream(InputStream* in) {
    if (!in) return in;
    if (datastream) {
        delete datastream;
        datastream = 0;
    }
    if (event.size()) {
        datastream = new DataEventInputStream(in, *this);
        ready = false;
        std::vector<StreamEventAnalyzer*>::iterator i;
        for (i = event.begin(); i != event.end(); ++i) {
            (*i)->startAnalysis(result);
        }
    }
    return (datastream) ?datastream :in;
}
bool
EventThroughAnalyzer::isReadyWithStream() {
    return ready;
}
bool
EventThroughAnalyzer::handleData(const char* data, uint32_t size) {
    if (ready) return false;
    std::vector<StreamEventAnalyzer*>::iterator i;
    bool more = false;
    for (i = event.begin(); i != event.end(); ++i) {
        (*i)->handleData(data, size);
        more = more || !(*i)->isReadyWithStream();
        //if (!(*i)->isReadyWithStream()) {
        //     cerr << "hungry analyzer: " << (*i)->name() << endl;
        //}
    }
    ready = !more;
    return more;
}
void
EventThroughAnalyzer::handleEnd() {
    std::vector<StreamEventAnalyzer*>::iterator i;
    for (i = event.begin(); i != event.end(); ++i) {
        (*i)->endAnalysis(datastream->status() == Eof);
    }
}
StreamThroughAnalyzer*
EventThroughAnalyzerFactory::newInstance() const {
    std::vector<StreamEventAnalyzer*> event;
    std::vector<StreamEventAnalyzerFactory*>::iterator ea;
    for (ea = eventfactories.begin(); ea != eventfactories.end(); ++ea) {
        event.push_back((*ea)->newInstance());
    }
    std::vector<StreamSaxAnalyzer*> sax;
    std::vector<StreamSaxAnalyzerFactory*>::iterator sa;
    for (sa = saxfactories.begin(); sa != saxfactories.end(); ++sa) {
        sax.push_back((*sa)->newInstance());
    }
    event.push_back(new SaxEventAnalyzer(sax));
    std::vector<StreamLineAnalyzer*> line;
    std::vector<StreamLineAnalyzerFactory*>::iterator la;
    for (la = linefactories.begin(); la != linefactories.end(); ++la) {
        line.push_back((*la)->newInstance());
    }
    event.push_back(new LineEventAnalyzer(line));
    StreamThroughAnalyzer* sta = new EventThroughAnalyzer(event);
    return sta;
}
void
EventThroughAnalyzerFactory::registerFields(Strigi::FieldRegister& reg) {
    std::vector<StreamEventAnalyzerFactory*>::iterator ea;
    for (ea = eventfactories.begin(); ea != eventfactories.end(); ++ea) {
        (*ea)->registerFields(reg);
    }
    std::vector<StreamSaxAnalyzerFactory*>::iterator sa;
    for (sa = saxfactories.begin(); sa != saxfactories.end(); ++sa) {
        (*sa)->registerFields(reg);
    }
    std::vector<StreamLineAnalyzerFactory*>::iterator la;
    for (la = linefactories.begin(); la != linefactories.end(); ++la) {
        (*la)->registerFields(reg);
    }
}
