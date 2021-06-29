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
#include "tagmapping.h"
#include <iostream>
#include <fstream>

TagMapping::TagMapping(const char* path) {
    if (path == 0) return;
    std::ifstream file(path);
    std::string line;
    for (;;) {
        getline(file, line);
        if (!file.good()) {
            break;
        }
        std::string::size_type p = line.find('\t');
        if (p != std::string::npos) {
            mapping[line.substr(0, p)] = line.substr(p+1);
        } else {
            p = line.find(':');
            if (p != std::string::npos) {
                m_namespaces[line.substr(0, p)] = line.substr(p+1);
            }
        }
    }
}

