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

#ifndef STRIGI_STREAMBUFFER_H
#define STRIGI_STREAMBUFFER_H

#include <cstdlib>
#include <cstring>
#include <cassert>

namespace Strigi {

/**
 * @internal
 * @brief Provides a buffer for the use of BufferedStream
 */
template <class T>
class StreamBuffer {
private:
public:
    /**
     * @internal
     * @brief Pointer to the start of the buffer.
     */
    T* start;
    /**
     * @internal
     * @brief Size of the buffer.
     *
     * Size of the memory pointed to by @p start,
     * in multiples of sizeof(T)
     */
    int32_t size;
    /**
     * @internal
     * @brief Pointer to the current position the buffer.
     */
    T* readPos;
    /**
     * @internal
     * @brief The amount of data available in the buffer.
     *
     * The size of the used memory in the buffer, starting
     * from @p readPos.  @p readPos + @p avail must be
     * greater than @p start + @p size.
     */
    int32_t avail;

    /**
     * @internal
     * @brief Constructor: initialises members to sane defaults.
     */
    StreamBuffer();
    /**
     * @internal
     * @brief Destructor: frees the memory used by the buffer.
     */
    ~StreamBuffer();
    /**
     * @internal
     * @brief Sets the size of the buffer, allocating the necessary memory
     *
     * @param size the size that the buffer should be, in multiples
     * of sizeof(T)
     */
    void setSize(int32_t size);
    /**
     * @internal
     * @brief Read data from the buffer
     *
     * Sets @p start to point to the data, starting
     * at the item of data following the last item
     * of data read.
     *
     * @param start pointer passed by reference. It will
     * be set to point to the data read from the buffer
     * @param max the maximum amount of data to read from
     * the buffer
     * @return the size of the data pointed to by @p start
     * (always less than or equal to @p max)
     */
    int32_t read(const T*& start, int32_t max=0);

    /**
     * @internal
     * @brief Prepares the buffer for a new write.
     *
     * This function invalidates any pointers
     * previously obtained from read.
     *
     * @return the number of available places
     **/
     int32_t makeSpace(int32_t needed);
};

template <class T>
StreamBuffer<T>::StreamBuffer() {
    readPos = start = 0;
    size = avail = 0;
}
template <class T>
StreamBuffer<T>::~StreamBuffer() {
    std::free(start);
}
template <class T>
void
StreamBuffer<T>::setSize(int32_t size) {
    // store pointer information
    int32_t offset = (int32_t)(readPos - start);
    assert(size >= 0);
    assert(avail >= 0);
    assert(offset >= 0);    
    assert(avail+offset <= size); // catch broken offset and avail values when shrinking the buffer

    // allocate memory in the buffer
    start = (T*)std::realloc(start, size*sizeof(T));
    this->size = size;

    // restore pointer information
    readPos = start + offset;
}
template <class T>
int32_t
StreamBuffer<T>::makeSpace(int32_t needed) {
    // determine how much space is available for writing
    int32_t offset = (int32_t)(readPos - start);
    int32_t space = size - offset - avail;

    assert(offset >= 0);
    assert(size >= 0);
    assert(avail >= 0);
    assert(avail+offset <= size);
    
    if (space >= needed) {
        // there's enough space
        return space;
    }

    if (avail) {
        if (offset != 0) {
            // move data to the start of the buffer
            std::memmove(start, readPos, avail*sizeof(T));
            space += offset;
            readPos = start;
        }
    } else {
        // we may start writing at the start of the buffer
        readPos = start;
        space = size;
    }
    if (space >= needed) {
        // there's enough space now
        return space;
    }

    // still not enough space, we have to allocate more
//    printf("resize %i %i %i %i %i\n", avail, needed, space, size + needed - space, size);
    if (size > needed - space) {
        // double the buffer size
        space += size;
        setSize(2 * size);
        return space;
    }
    // just allocate what is needed
    setSize(size + needed - space);
    return needed;
}
template <class T>
int32_t
StreamBuffer<T>::read(const T*& start, int32_t max) {
    assert(size >= 0);
    assert(avail >= 0);
    assert(readPos >= this->start);
    assert(avail+(readPos-this->start) <= size);
    
    start = readPos;
    if (max <= 0 || max > avail) {
        max = avail;
    }
    readPos += max;
    avail -= max;
    return max;
}

} // end namespace Strigi

#endif
