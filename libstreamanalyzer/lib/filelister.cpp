/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Jos van den Oever <jos@vandenoever.info>
 * Copyright (C) 2007 Flavio Castelli <flavio.castelli@gmail.com>
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

#include <strigi/filelister.h>
#include <strigi/strigiconfig.h>
#include <strigi/analyzerconfiguration.h>
#include <mutex>
#include <set>
#include <list>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <errno.h>

using namespace Strigi;

class FileLister::Private {
public:
    char path[10000];
    std::mutex mutex;
    DIR** dirs;
    DIR** dirsEnd;
    DIR** curDir;
    std::string::size_type* len;
    std::string::size_type* lenEnd;
    std::string::size_type* curLen;
    time_t mtime;
    struct dirent* subdir;
    struct stat dirstat;
    std::set<std::string> listedDirs;
    const AnalyzerConfiguration* const config;

    Private(const AnalyzerConfiguration* ic);
    ~Private();
    int nextFile(std::string& p, time_t& time) {
        int r;
        std::lock_guard<std::mutex> lock(mutex);
        r = nextFile();
        if (r > 0) {
            p.assign(path, r);
            time = mtime;
        }
        return r;
    }
    void startListing(const std::string&);
    int nextFile();
};
FileLister::Private::Private(
            const AnalyzerConfiguration* ic) :
        config(ic) {
    int nOpenDirs = 100;
    dirs = (DIR**)malloc(sizeof(DIR*)*nOpenDirs);
    dirsEnd = dirs + nOpenDirs;
    len = (std::string::size_type*)malloc(sizeof(std::string::size_type)*nOpenDirs);
    lenEnd = len + nOpenDirs;
    curDir = dirs - 1;
}
void
FileLister::Private::startListing(const std::string& dir){
    listedDirs.clear();
    curDir = dirs;
    curLen = len;
    std::string::size_type len = dir.length();
    *curLen = len;
    strcpy(path, dir.c_str());
    if (len) {
        if (path[len-1] != '/') {
            path[len++] = '/';
            path[len] = 0;
            *curLen = len;
        }
        DIR* d = opendir(path);
        if (d) {
            *curDir = d;
            listedDirs.insert (path);
        } else {
            curDir--;
        }
    } else {
        curDir--;
    }
}
FileLister::Private::~Private() {
    while (curDir >= dirs) {
        if (*curDir) {
            closedir(*curDir);
        }
        curDir--;
    }
    free(dirs);
    free(len);
}
int
FileLister::Private::nextFile() {

    while (curDir >= dirs) {
        DIR* dir = *curDir;
        std::string::size_type l = *curLen;
        subdir = readdir(dir);
        while (subdir) {
            // skip the directories '.' and '..'
            char c1 = subdir->d_name[0];
            if (c1 == '.') {
                char c2 = subdir->d_name[1];
                if (c2 == '.' || c2 == '\0') {
                    subdir = readdir(dir);
                    continue;
                }
            }
            strcpy(path + l, subdir->d_name);
            std::string::size_type sl = l + strlen(subdir->d_name);
            if (lstat(path, &dirstat) == 0) {
                if (S_ISREG(dirstat.st_mode)) {
                    if (config == 0 || config->indexFile(path, path+l)) {
                        mtime = dirstat.st_mtime;
                        return (int)sl;
                    }
                } else if (dirstat.st_mode & S_IFDIR && (config == 0
                        || config->indexDir(path, path+l))) {
                    mtime = dirstat.st_mtime;
                    strcpy(this->path+sl, "/");
                    DIR* d = opendir(path);
                    if (d) {
                        curDir++;
                        curLen++;
                        dir = *curDir = d;
                        l = *curLen = sl+1;
                        listedDirs.insert ( path);
                    }
                }
            }
            subdir = readdir(dir);
        }
        closedir(dir);
        curDir--;
        curLen--;
    }
    return -1;
}
FileLister::FileLister(const AnalyzerConfiguration* ic)
    : p(new Private(ic)) {
}
FileLister::~FileLister() {
    delete p;
}
void
FileLister::startListing(const std::string& dir) {
    p->startListing(dir);
}
int
FileLister::nextFile(std::string& path, time_t& time) {
    return p->nextFile(path, time);
}
int
FileLister::nextFile(const char*& path, time_t& time) {
    int r = p->nextFile();
    if (r >= 0) {
        time = p->mtime;
        path = p->path;
    }
    return r;
}
void
FileLister::skipTillAfter(const std::string& lastToSkip) {
    int r = p->nextFile();
    while (r >= 0 && p->path != lastToSkip) {
        r = p->nextFile();
    }
}

class DirLister::Private {
public:
    std::mutex mutex;
    std::list<std::string> todoPaths;
    const AnalyzerConfiguration* const config;

    Private(const AnalyzerConfiguration* ic) :config(ic) {}
    int nextDir(std::string& path,
        std::vector<std::pair<std::string, struct stat> >& dirs);
};

DirLister::DirLister(const AnalyzerConfiguration* ic)
    : p(new Private(ic)) {
}
DirLister::~DirLister() {
    delete p;
}
void
DirLister::startListing(const std::string& dir) {
    std::lock_guard<std::mutex> lock(p->mutex);
    p->todoPaths.push_back(dir);
}
void
DirLister::stopListing() {
    std::lock_guard<std::mutex> lock(p->mutex);
    p->todoPaths.clear();
}
int
DirLister::Private::nextDir(std::string& path,
        std::vector<std::pair<std::string, struct stat> >& dirs) {
    std::string entryname;
    std::string entrypath;
    size_t entrypathlength;
    // check if there are more directories to work on
    // open the directory
    std::lock_guard<std::mutex> lock(mutex);
    if (todoPaths.empty()) {
        return -1;
    }
    path.assign(todoPaths.front());
    todoPaths.pop_front();
    entrypathlength = path.length()+1;
    entrypath.assign(path);
    entrypath.append("/");
    dirs.clear();
    DIR* dir;
    if (path.size()) {
        dir = opendir(path.c_str());
    } else {
        // special case for root directory '/' on unix systems
        dir = opendir("/");
    }
    if (!dir) {
        int e = errno;
        // if permission is denied, this is not an error
        return (e == EACCES) ?0 :-1;
    }
    struct dirent* entry = readdir(dir);
    struct stat entrystat;
    while (entry) {
        entryname.assign(entry->d_name);
        if (entryname != "." && entryname != "..") {
            entrypath.resize(entrypathlength);
            entrypath.append(entryname);
            if (lstat(entrypath.c_str(), &entrystat) == 0) {
                if (S_ISDIR(entrystat.st_mode)) {
                    if (config == 0 ||
                            config->indexDir(
                                entrypath.c_str(), entryname.c_str())) {
                        todoPaths.push_back(entrypath);
                        dirs.push_back(std::make_pair(entrypath, entrystat));
                    }
                } else if (config == 0 || config->indexFile(entrypath.c_str(),
                        entryname.c_str())) {
                    dirs.push_back(std::make_pair(entrypath, entrystat));
                }
            }
        }
        entry = readdir(dir);
    }
    closedir(dir);
    return 0;
}
int
DirLister::nextDir(std::string& path,
        std::vector<std::pair<std::string, struct stat> >& dirs) {
    return p->nextDir(path, dirs);
}
void
DirLister::skipTillAfter(const std::string& lastToSkip) {
    std::string path;
    std::vector<std::pair<std::string, struct stat> > dirs;
    while (nextDir(path, dirs) >= 0 && path != lastToSkip) {}
}
