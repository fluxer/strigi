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
#include <strigi/fileinputstream.h>
#include <strigi/stringstream.h>
#include <strigi/stringterminatedsubstream.h>
#include "../sharedtestcode/inputstreamtests.h"
#include <iostream>

using namespace Strigi;

int
StringTerminatedSubStreamTest(int argc, char* argv[]) {
    if (argc < 2) return 1;
    founderrors = 0;
    VERIFY(chdir(argv[1]) == 0);

    StringInputStream sr("abc");
    StringTerminatedSubStream sub(&sr, "b");
    VERIFY(sub.size() == -1);
    const char* start;
    int64_t nread = sub.read(start, 10, 10);
    VERIFY(nread == 1);

    TESTONFILE2(StringTerminatedSubStream, "THEEND", "a.zip");
    return founderrors;
}

