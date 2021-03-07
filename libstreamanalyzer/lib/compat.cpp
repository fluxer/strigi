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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef HAVE_STRCASECMP
int strcasecmp(const char* sa, const char* sb){
    char ca,cb;
    if (sa == sb)
        return 0;
    int i=0;

    do{
        ca = tolower( (*(sa++)) );
        cb = tolower( (*(sb++)) );

        i++;
    } while ( ca != L'\0' && (ca == cb) );

    return (int)(ca - cb);
}
#endif

#ifndef HAVE_MKSTEMP
int mkstemp(char *tmpl)
{
   mktemp(tmpl);
   return open(tmpl,O_RDWR|O_BINARY|O_CREAT|O_EXCL|_O_SHORT_LIVED, _S_IREAD|_S_IWRITE);
}
#endif
