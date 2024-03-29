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

#ifndef STRIGI_ZIPINPUTSTREAM_H
#define STRIGI_ZIPINPUTSTREAM_H

#include <strigi/substreamprovider.h>
#include <strigi/gzipinputstream.h>

namespace Strigi {

/**
 * Partial implementation of the zip file format according to
 * http://www.pkware.com/business_and_developers/developer/popups/appnote.txt
 * http://www.pkware.com/documents/casestudies/APPNOTE.TXT
 * 99% of zip files on my system can be read with this class.
 * Exceptions are files that are (at least)
 * - files generated by writing to stdout
 * - files using other compression as deflated
 * - encrypted files
 **/
class STRIGI_EXPORT ZipInputStream : public SubStreamProvider {
private:
    // information relating to the current entry
    InputStream* compressedEntryStream;
    GZipInputStream *uncompressionStream;
    int32_t entryCompressedSize;
    int32_t compressionMethod;

    void readFileName(int32_t len);
    void readHeader();
public:
    explicit ZipInputStream(InputStream* input);
    ~ZipInputStream();
    InputStream* nextEntry();
    static bool checkHeader(const char* data, int32_t datasize);
    static SubStreamProvider* factory(InputStream* input) {
        return new ZipInputStream(input);
    }
};

} // end namespace Strigi

#endif
