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

#include "analyzerloader.h"
#include <strigi/analyzerplugin.h>
#include <string>
#include <iostream>
#include <iterator>
#include <cstdio>
#include <cstring>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <dirent.h>

using namespace std;
using namespace Strigi;

class AnalyzerLoader::Private {
public:
    class Module {
    private:
        void* mod;
    public:
        Module(void* m, const AnalyzerFactoryFactory* f)
            :mod(m), factory(f) {}
        ~Module();
        const AnalyzerFactoryFactory* factory;
    };
    class ModuleList {
    public:
        std::map<std::string, Module*> modules;

        ModuleList();
        ~ModuleList();
    };
    static ModuleList modulelist;
    static void loadModule(const char* lib);
};

AnalyzerLoader::Private::ModuleList AnalyzerLoader::Private::modulelist;

AnalyzerLoader::Private::ModuleList::ModuleList() {
}
AnalyzerLoader::Private::ModuleList::~ModuleList() {
    map<string, Module*>::iterator i;
    for (i = modules.begin(); i != modules.end(); ++i) {
        delete i->second;
    }
}

AnalyzerLoader::Private::Module::~Module() {
    void(*f)(const AnalyzerFactoryFactory*)
        = (void(*)(const AnalyzerFactoryFactory*))
        dlsym(mod, "deleteStrigiAnalyzerFactory");
    if (f) {
        f(factory);
    }
    dlclose(mod);
}

void
AnalyzerLoader::loadPlugins(const char* d) {
    DIR *dir = opendir(d);
    if (dir == 0) {
        // TODO handle error
        return;
    }
    struct dirent* ent = readdir(dir);
    while(ent) {
        size_t len = strlen(ent->d_name);
        if ((strncmp(ent->d_name, "strigita_", 9) == 0
                || strncmp(ent->d_name, "strigiea_", 9) == 0
                || strncmp(ent->d_name, "strigila_", 9) == 0)
                && strcmp(ent->d_name+len-3, ".so") == 0) {
            string plugin = d;
            if (plugin[plugin.length()-1] != '/') {
                plugin.append("/");
            }
            plugin.append(ent->d_name);
            // check that the file is a regular file
#ifdef HAVE_DIRENT_D_TYPE
            const bool isfile = (ent->d_type == DT_REG);
#else
            struct stat s;
            const bool isfile = (stat(plugin.c_str(), &s) == 0 && (S_IFREG & s.st_mode));
#endif
            if (isfile) {
                Private::loadModule(plugin.c_str());
            }
        }
        ent = readdir(dir);
    }
    closedir(dir);
}
void
AnalyzerLoader::Private::loadModule(const char* lib) {
    //fprintf(stderr, "load lib %s\n", lib);
    if (modulelist.modules.find(lib) != modulelist.modules.end()) {
        // module was already loaded
        return;
    }
    // cerr << lib << endl;
    // do not use RTLD_GLOBAL here
    void* handle = dlopen(lib, RTLD_LAZY); //note: If neither RTLD_GLOBAL nor RTLD_LOCAL are specified, the default is RTLD_LOCAL.
    if (!handle) {
        cerr << "Could not load '" << lib << "':" << dlerror() << endl;
        return;
    }
    const AnalyzerFactoryFactory* (*f)() = (const AnalyzerFactoryFactory* (*)())
        dlsym(handle, "strigiAnalyzerFactory");
    if (!f) {
        fprintf(stderr, "%s\n", dlerror());
        dlclose(handle);
        return;
    }
    AnalyzerLoader::Private::modulelist.modules[lib] = new Module(handle, f());
}
list<StreamEndAnalyzerFactory*>
AnalyzerLoader::streamEndAnalyzerFactories() {
    list<StreamEndAnalyzerFactory*> l;
    map<string, Private::Module*>::iterator i;
    for (i = Private::modulelist.modules.begin();
            i != Private::modulelist.modules.end(); ++i) {
        list<StreamEndAnalyzerFactory*> ml
            = i->second->factory->streamEndAnalyzerFactories();
        copy(ml.begin(), ml.end(), back_inserter(l));
    }
    return l;
}
list<StreamThroughAnalyzerFactory*>
AnalyzerLoader::streamThroughAnalyzerFactories() {
    list<StreamThroughAnalyzerFactory*> l;
    map<string, Private::Module*>::iterator i;
    for (i = Private::modulelist.modules.begin();
            i != Private::modulelist.modules.end(); ++i) {
        list<StreamThroughAnalyzerFactory*> ml
            = i->second->factory->streamThroughAnalyzerFactories();
        copy(ml.begin(), ml.end(), back_inserter(l));
    }
    return l;
}
list<StreamSaxAnalyzerFactory*>
AnalyzerLoader::streamSaxAnalyzerFactories() {
    list<StreamSaxAnalyzerFactory*> l;
    map<string, Private::Module*>::iterator i;
    for (i = Private::modulelist.modules.begin();
            i != Private::modulelist.modules.end(); ++i) {
            assert(i->second);
            assert(i->second->factory);
        list<StreamSaxAnalyzerFactory*> ml
            = i->second->factory->streamSaxAnalyzerFactories();
        copy(ml.begin(), ml.end(), back_inserter(l));
    }
    return l;
}
list<StreamLineAnalyzerFactory*>
AnalyzerLoader::streamLineAnalyzerFactories() {
    list<StreamLineAnalyzerFactory*> l;
    map<string, Private::Module*>::iterator i;
    for (i = Private::modulelist.modules.begin();
            i != Private::modulelist.modules.end(); ++i) {
        list<StreamLineAnalyzerFactory*> ml
            = i->second->factory->streamLineAnalyzerFactories();
        copy(ml.begin(), ml.end(), back_inserter(l));
    }
    return l;
}
list<StreamEventAnalyzerFactory*>
AnalyzerLoader::streamEventAnalyzerFactories() {
    list<StreamEventAnalyzerFactory*> l;
    map<string, Private::Module*>::iterator i;
    for (i = Private::modulelist.modules.begin();
            i != Private::modulelist.modules.end(); ++i) {
        list<StreamEventAnalyzerFactory*> ml
            = i->second->factory->streamEventAnalyzerFactories();
        copy(ml.begin(), ml.end(), back_inserter(l));
    }
    return l;
}
