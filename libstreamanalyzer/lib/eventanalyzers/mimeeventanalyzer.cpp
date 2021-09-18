/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
 *           (C) 2006 David Faure <dfaure@kde.org>
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
#include "mimeeventanalyzer.h"
#include <strigi/fieldtypes.h>
#include <strigi/textutils.h>
#include <strigi/fileinputstream.h>
#include <strigi/analysisresult.h>
#include <config.h>

#include <magic.h>

// http://standards.freedesktop.org/shared-mime-info-spec/shared-mime-info-spec-0.12.html

using namespace Strigi;

class MimeEventAnalyzer::Private {
public:
    magic_t magic;

    AnalysisResult* analysisResult;
    const MimeEventAnalyzerFactory* const factory;

    Private(const MimeEventAnalyzerFactory* f);
    ~Private();
};

MimeEventAnalyzer::Private::Private(const MimeEventAnalyzerFactory* f)
    : factory(f),
    magic(NULL)
{
    magic = magic_open(MAGIC_MIME_TYPE | MAGIC_ERROR);
    if (magic) {
        magic_load(magic, NULL);
    }
}

MimeEventAnalyzer::Private::~Private() {
    magic_close(magic);
}

void MimeEventAnalyzer::startAnalysis(AnalysisResult* ar) {
    p->analysisResult = ar;
    wasCalled = false;
}

void MimeEventAnalyzer::endAnalysis(bool complete) {
}

void MimeEventAnalyzer::handleData(const char* data, uint32_t length) {
    if (wasCalled) return;
    wasCalled = true;

    if (p->magic) {
        const std::string mime = magic_buffer(p->magic, data, length);
        if (mime.size() > 0) {
            p->analysisResult->addValue(p->factory->mimetypefield, mime);
            p->analysisResult->setMimeType(mime);
        }
    }
}

bool MimeEventAnalyzer::isReadyWithStream() {
    return wasCalled;
}

MimeEventAnalyzer::MimeEventAnalyzer(const MimeEventAnalyzerFactory* f)
    :p(new Private(f)) {
}

MimeEventAnalyzer::~MimeEventAnalyzer() {
    delete p;
}

void MimeEventAnalyzerFactory::registerFields(Strigi::FieldRegister& reg) {
    mimetypefield = reg.mimetypeField;
    addField(mimetypefield);
}
