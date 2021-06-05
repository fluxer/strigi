/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 * Copyright (C) 2007 Christian Ehrlicher <ch.ehrlicher@gmx.de>
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
 
#ifndef STRIGICONFIG_H
#define STRIGICONFIG_H

#include "strigi_export.h"

#define STRIGI_VERSION_STRING "@STRIGI_VERSION_STRING@"

/// @brief The major Strigi version number at compile time
#define STRIGI_VERSION_MAJOR @STRIGI_VERSION_MAJOR@

/// @brief The minor Strigi version number at compile time
#define STRIGI_VERSION_MINOR @STRIGI_VERSION_MINOR@

/// @brief The Strigi release version number at compile time
#define STRIGI_VERSION_RELEASE @STRIGI_VERSION_PATCH@

/**
 * \brief Create a unique number from the major, minor and release number of a %Strigi version
 *
 * This function can be used for preprocessing.
 */
#define STRIGI_MAKE_VERSION( a,b,c ) (((a) << 16) | ((b) << 8) | (c))

/**
 * \brief %Strigi Version as a unique number at compile time
 *
 * This macro calculates the %Strigi version into a number. It is mainly used
 * through STRIGI_IS_VERSION in preprocessing.
 */
#define STRIGI_VERSION \
    STRIGI_MAKE_VERSION(STRIGI_VERSION_MAJOR,STRIGI_VERSION_MINOR,STRIGI_VERSION_RELEASE)

/**
 * \brief Check if the %Strigi version matches a certain version or is higher
 *
 * This macro is typically used to compile conditionally a part of code:
 * \code
 * #if STRIGI_IS_VERSION(2,1)
 * // Code for Strigi 2.1
 * #else
 * // Code for Strigi 2.0
 * #endif
 * \endcode
 */
#define STRIGI_IS_VERSION(a,b,c) ( STRIGI_VERSION >= STRIGI_MAKE_VERSION(a,b,c) )

#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>

namespace Strigi {
    STRIGI_EXPORT unsigned int versionMajor();

    STRIGI_EXPORT unsigned int versionMinor();

    STRIGI_EXPORT const char* versionString();
}

#endif // STRIGICONFIG_H
