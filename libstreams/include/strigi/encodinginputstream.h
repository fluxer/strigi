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

#ifndef STRIGI_ENCODINGINPUTSTREAM_H
#define STRIGI_ENCODINGINPUTSTREAM_H

#include "bufferedstream.h"

namespace Strigi {

/**
 *
 */
class STRIGI_EXPORT EncodingInputStream : public BufferedInputStream {
private:
    class Private;
    Private* const p;

    int32_t fillBuffer(char* start, int32_t space);
public:
    /**
     *
     * @param i the input stream to encode
     * @param inenc the encoding of the input stream
     * @param outenc the encoding of the input stream, default is UTF-8
     * no encoding is given
     */
    EncodingInputStream(InputStream *i, const char* inenc,
        const char* outenc=0);
    ~EncodingInputStream();
};

} // end namespace Strigi

#endif
