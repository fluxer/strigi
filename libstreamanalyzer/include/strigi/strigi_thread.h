/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *                    Ben van Klinken <bvanklinken@gmail.com>
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
#ifndef STRIGI_THREAD_H
#define STRIGI_THREAD_H

#include <pthread.h>

#define STRIGI_MUTEX_DEFINE(x) pthread_mutex_t x
#define STRIGI_MUTEX_INIT(x) pthread_mutex_init(x, 0)
#define STRIGI_MUTEX_DESTROY(x) pthread_mutex_destroy(x)
#define STRIGI_MUTEX_LOCK(x) pthread_mutex_lock(x)
#define STRIGI_MUTEX_UNLOCK(x) pthread_mutex_unlock(x)

#define STRIGI_THREAD_TYPE pthread_t
#define STRIGI_THREAD_CREATE(threadObject, function, data) pthread_create(threadObject, NULL, function, data)
#define STRIGI_THREAD_JOIN(object) pthread_join(object,0)
#define STRIGI_THREAD_EXIT(ret) pthread_exit(ret)
#define STRIGI_THREAD_SELF() pthread_self()

#endif
