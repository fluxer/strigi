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
#include <strigi/streamanalyzer.h>
#include <strigi/fileinputstream.h>
#include <strigi/streamendanalyzer.h>
#include <strigi/streamthroughanalyzer.h>
#include <strigi/streamlineanalyzer.h>
#include <strigi/streameventanalyzer.h>
#include <strigi/streamsaxanalyzer.h>
#include "endanalyzers/bz2endanalyzer.h"
#include "endanalyzers/lzmaendanalyzer.h"
#include "eventanalyzers/mimeeventanalyzer.h"
#include "eventanalyzers/digesteventanalyzer.h"
#include "eventanalyzers/riffeventanalyzer.h"
#include "endanalyzers/bmpendanalyzer.h"
#include "endanalyzers/textendanalyzer.h"
#include "endanalyzers/tarendanalyzer.h"
#include "endanalyzers/arendanalyzer.h"
#include "endanalyzers/zipexeendanalyzer.h"
#include "endanalyzers/odfendanalyzer.h"
#include "endanalyzers/oleendanalyzer.h"
#include "endanalyzers/rpmendanalyzer.h"
#include "endanalyzers/cpioendanalyzer.h"
#include "endanalyzers/pdfendanalyzer.h"
#include "endanalyzers/sdfendanalyzer.h"
#include "endanalyzers/pngendanalyzer.h"
#include "endanalyzers/gzipendanalyzer.h"
#include "lineanalyzers/m3ustreamanalyzer.h"
#include "lineanalyzers/cpplineanalyzer.h"
#include "lineanalyzers/deblineanalyzer.h"
#include "lineanalyzers/txtlineanalyzer.h"
#include "lineanalyzers/xpmlineanalyzer.h"
#include "endanalyzers/mailendanalyzer.h"
#include "endanalyzers/helperendanalyzer.h"
#include <strigi/dataeventinputstream.h>
#include "endanalyzers/id3endanalyzer.h"
#include "throughanalyzers/oggthroughanalyzer.h"
#include "throughanalyzers/authroughanalyzer.h"
#include "throughanalyzers/ddsthroughanalyzer.h"
#include "throughanalyzers/gifthroughanalyzer.h"
#include "throughanalyzers/icothroughanalyzer.h"
#include "throughanalyzers/pcxthroughanalyzer.h"
#include "throughanalyzers/rgbthroughanalyzer.h"
#include "throughanalyzers/sidthroughanalyzer.h"
#include "throughanalyzers/xbmthroughanalyzer.h"
#include "endanalyzers/flacendanalyzer.h"
#include <strigi/analysisresult.h>
#include <strigi/indexwriter.h>
#include <strigi/analyzerconfiguration.h>
#include <strigi/textutils.h>
#include "analyzerloader.h"
#include "eventthroughanalyzer.h"
#include "saxanalyzers/htmlsaxanalyzer.h"
#include "saxanalyzers/namespaceharvestersaxanalyzer.h"
#include <config.h>

#include <sys/stat.h>
#include <iostream>

using namespace Strigi;

std::vector<std::string> getdirs(const std::string& direnv) {
    std::vector<std::string> dirs;
    std::string::size_type lastp = 0;
    std::string::size_type p = direnv.find(PATH_SEPARATOR);
    while (p != std::string::npos) {
        dirs.push_back(direnv.substr(lastp, p-lastp));
        lastp = p+1;
        p = direnv.find(PATH_SEPARATOR, lastp);
    }
    dirs.push_back(direnv.substr(lastp));
    return dirs;
}

namespace Strigi {

class StreamAnalyzerPrivate {
public:
    AnalyzerConfiguration& conf;
    std::vector<StreamThroughAnalyzerFactory*> throughfactories;
    std::vector<StreamEndAnalyzerFactory*> endfactories;
    std::vector<StreamSaxAnalyzerFactory*> saxfactories;
    std::vector<StreamLineAnalyzerFactory*> linefactories;
    std::vector<StreamEventAnalyzerFactory*> eventfactories;
    std::vector<std::vector<StreamEndAnalyzer*> > end;
    std::vector<std::vector<StreamThroughAnalyzer*> > through;
    IndexWriter* writer;

    AnalyzerLoader* moduleLoader;
    const RegisteredField* sizefield;
    const RegisteredField* errorfield;
    void initializeThroughFactories();
    void initializeEndFactories();
    void initializeSaxFactories();
    void initializeLineFactories();
    void initializeEventFactories();
    void addFactory(StreamThroughAnalyzerFactory* f);
    void addFactory(StreamEndAnalyzerFactory* f);
    void addFactory(StreamSaxAnalyzerFactory* f);
    void addFactory(StreamLineAnalyzerFactory* f);
    void addFactory(StreamEventAnalyzerFactory* f);
    void addThroughAnalyzers();
    void addEndAnalyzers();
    void removeIndexable(unsigned depth);
    signed char analyze(AnalysisResult& idx, StreamBase<char>* input);

    StreamAnalyzerPrivate(AnalyzerConfiguration& c);
    ~StreamAnalyzerPrivate();
};

} // namespace Strigi
StreamAnalyzerPrivate::StreamAnalyzerPrivate(AnalyzerConfiguration& c)
        :conf(c), writer(0) {
    moduleLoader = new AnalyzerLoader();
    sizefield = c.fieldRegister().sizeField;
    errorfield = c.fieldRegister().parseErrorField;

    // load the plugins from the environment setting
    const char* strigipluginpath(getenv("STRIGI_PLUGIN_PATH"));
    if (strigipluginpath) {
        std::vector<std::string> strigipluginpaths = getdirs(strigipluginpath);
        for (uint i=0; i<strigipluginpaths.size(); ++i) {
            moduleLoader->loadPlugins(strigipluginpaths[i].c_str());
        }
    } else {
        moduleLoader->loadPlugins( LIBINSTALLDIR "/strigi");
    }

    initializeSaxFactories();
    initializeLineFactories();
    initializeEventFactories();
    initializeThroughFactories();
    initializeEndFactories();
}
StreamAnalyzerPrivate::~StreamAnalyzerPrivate() {
    // delete all factories
    std::vector<StreamThroughAnalyzerFactory*>::iterator ta;
    for (ta = throughfactories.begin(); ta != throughfactories.end(); ++ta) {
        delete *ta;
    }
    std::vector<StreamEndAnalyzerFactory*>::iterator ea;
    for (ea = endfactories.begin(); ea != endfactories.end(); ++ea) {
        delete *ea;
    }
    std::vector<StreamSaxAnalyzerFactory*>::iterator sa;
    for (sa = saxfactories.begin(); sa != saxfactories.end(); ++sa) {
        delete *sa;
    }
    std::vector<StreamLineAnalyzerFactory*>::iterator la;
    for (la = linefactories.begin(); la != linefactories.end(); ++la) {
        delete *la;
    }
    std::vector<StreamEventAnalyzerFactory*>::iterator da;
    for (da = eventfactories.begin(); da != eventfactories.end(); ++da) {
        delete *da;
    }
    // delete the through analyzers and end analyzers
    std::vector<std::vector<StreamThroughAnalyzer*> >::iterator tIter;
    for (tIter = through.begin(); tIter != through.end(); ++tIter) {
        std::vector<StreamThroughAnalyzer*>::iterator t;
        for (t = tIter->begin(); t != tIter->end(); ++t) {
            delete *t;
        }
    }
    std::vector<std::vector<StreamEndAnalyzer*> >::iterator eIter;
    for (eIter = end.begin(); eIter != end.end(); ++eIter) {
        std::vector<StreamEndAnalyzer*>::iterator e;
        for (e = eIter->begin(); e != eIter->end(); ++e) {
            delete *e;
        }
    }
    delete moduleLoader;
    if (writer) {
        writer->releaseWriterData(conf.fieldRegister());
    }
}

StreamAnalyzer::StreamAnalyzer(AnalyzerConfiguration& c)
        :p(new StreamAnalyzerPrivate(c)) {
}
StreamAnalyzer::~StreamAnalyzer() {
    delete p;
}
void
StreamAnalyzer::setIndexWriter(IndexWriter& w) {
    if (p->writer != 0) {
        p->writer->releaseWriterData(p->conf.fieldRegister());
    }
    p->writer = &w;
    p->writer->initWriterData(p->conf.fieldRegister());
}
signed char
StreamAnalyzer::indexFile(const char *filepath) {
    std::string path(filepath);
    return indexFile(path);
}
signed char
StreamAnalyzer::indexFile(const std::string& filepath) {
    if (!checkUtf8(filepath.c_str())) {
        return 1;
    }
    if (p->writer == 0) {
        return 1;
    }
    struct stat s;
    stat(filepath.c_str(), &s);
    // ensure a decent buffer size
    std::string name;
    AnalysisResult analysisresult(filepath, s.st_mtime, *p->writer, *this);
    InputStream* file = FileInputStream::open(filepath.c_str());
    signed char r;
    if (file->status() == Ok) {
        r = analysisresult.index(file);
    } else {
        r = analysisresult.index(0);
    }
    delete file;
    return r;
}
void
StreamAnalyzerPrivate::addFactory(StreamThroughAnalyzerFactory* f) {
    f->registerFields(conf.fieldRegister());
    if (conf.useFactory(f)) {
        throughfactories.push_back(f);
    } else {
        delete f;
    }
}
void
StreamAnalyzerPrivate::initializeSaxFactories() {
    std::list<StreamSaxAnalyzerFactory*> plugins
        = moduleLoader->streamSaxAnalyzerFactories();
    std::list<StreamSaxAnalyzerFactory*>::iterator i;
    for (i = plugins.begin(); i != plugins.end(); ++i) {
        addFactory(*i);
    }
    addFactory(new HtmlSaxAnalyzerFactory());
    addFactory(new NamespaceHarvesterSaxAnalyzerFactory());
}
void
StreamAnalyzerPrivate::initializeLineFactories() {
    std::list<StreamLineAnalyzerFactory*> plugins
        = moduleLoader->streamLineAnalyzerFactories();
    std::list<StreamLineAnalyzerFactory*>::iterator i;
    for (i = plugins.begin(); i != plugins.end(); ++i) {
        addFactory(*i);
    }
//    addFactory(new OdfMimeTypeLineAnalyzerFactory());
    addFactory(new M3uLineAnalyzerFactory());
    addFactory(new CppLineAnalyzerFactory());
    addFactory(new DebLineAnalyzerFactory());
    addFactory(new TxtLineAnalyzerFactory());
    addFactory(new XpmLineAnalyzerFactory());
}
void
StreamAnalyzerPrivate::initializeEventFactories() {
    std::list<StreamEventAnalyzerFactory*> plugins
        = moduleLoader->streamEventAnalyzerFactories();
    std::list<StreamEventAnalyzerFactory*>::iterator i;
    addFactory(new MimeEventAnalyzerFactory());
    addFactory(new DigestEventAnalyzerFactory());
    addFactory(new RiffEventAnalyzerFactory());
    for (i = plugins.begin(); i != plugins.end(); ++i) {
        addFactory(*i);
    }
}
void
StreamAnalyzerPrivate::initializeThroughFactories() {
    std::list<StreamThroughAnalyzerFactory*> plugins
        = moduleLoader->streamThroughAnalyzerFactories();
    std::list<StreamThroughAnalyzerFactory*>::iterator i;
    for (i = plugins.begin(); i != plugins.end(); ++i) {
        addFactory(*i);
    }
    addFactory(new OggThroughAnalyzerFactory());
    addFactory(new EventThroughAnalyzerFactory(saxfactories, linefactories,
        eventfactories));
}
void
StreamAnalyzerPrivate::addFactory(StreamEventAnalyzerFactory* f) {
    f->registerFields(conf.fieldRegister());
    if (conf.useFactory(f)) {
        eventfactories.push_back(f);
    } else {
        delete f;
    }
}
void
StreamAnalyzerPrivate::addFactory(StreamLineAnalyzerFactory* f) {
    f->registerFields(conf.fieldRegister());
    if (conf.useFactory(f)) {
        linefactories.push_back(f);
    } else {
        delete f;
    }
}
void
StreamAnalyzerPrivate::addFactory(StreamSaxAnalyzerFactory* f) {
    f->registerFields(conf.fieldRegister());
    if (conf.useFactory(f)) {
        saxfactories.push_back(f);
    } else {
        delete f;
    }
}
void
StreamAnalyzerPrivate::addFactory(StreamEndAnalyzerFactory* f) {
    f->registerFields(conf.fieldRegister());
    if (conf.useFactory(f)) {
        endfactories.push_back(f);
    } else {
        delete f;
    }
}
/**
 * Instantiate factories for all analyzers.
 **/
void
StreamAnalyzerPrivate::initializeEndFactories() {
    std::list<StreamEndAnalyzerFactory*> plugins
        = moduleLoader->streamEndAnalyzerFactories();
    std::list<StreamEndAnalyzerFactory*>::iterator i;
    for (i = plugins.begin(); i != plugins.end(); ++i) {
        addFactory(*i);
    }
    addFactory(new Bz2EndAnalyzerFactory());
    addFactory(new GZipEndAnalyzerFactory());
    addFactory(new OleEndAnalyzerFactory());
    addFactory(new TarEndAnalyzerFactory());
    addFactory(new ArEndAnalyzerFactory());
    addFactory(new MailEndAnalyzerFactory());
    addFactory(new OdfEndAnalyzerFactory());
    addFactory(new ZipEndAnalyzerFactory());
    addFactory(new ZipExeEndAnalyzerFactory());
    addFactory(new RpmEndAnalyzerFactory());
    addFactory(new CpioEndAnalyzerFactory());
    addFactory(new PngEndAnalyzerFactory());
    addFactory(new BmpEndAnalyzerFactory());
    addFactory(new FlacEndAnalyzerFactory());
    addFactory(new ID3EndAnalyzerFactory());
    addFactory(new PdfEndAnalyzerFactory());
    addFactory(new SdfEndAnalyzerFactory());
    addFactory(new LzmaEndAnalyzerFactory());
    addFactory(new HelperEndAnalyzerFactory());
    addFactory(new TextEndAnalyzerFactory());
}
void
StreamAnalyzerPrivate::addThroughAnalyzers() {
    through.resize(through.size()+1);
    std::vector<std::vector<StreamThroughAnalyzer*> >::reverse_iterator tIter;
    tIter = through.rbegin();
    std::vector<StreamThroughAnalyzerFactory*>::iterator ta;
    for (ta = throughfactories.begin(); ta != throughfactories.end(); ++ta) {
        tIter->push_back((*ta)->newInstance());
    }
}
void
StreamAnalyzerPrivate::addEndAnalyzers() {
    end.resize(end.size()+1);
    std::vector<std::vector<StreamEndAnalyzer*> >::reverse_iterator eIter;
    eIter = end.rbegin();
    std::vector<StreamEndAnalyzerFactory*>::iterator ea;
    for (ea = endfactories.begin(); ea != endfactories.end(); ++ea) {
        eIter->push_back((*ea)->newInstance());
    }
}
signed char
StreamAnalyzer::analyze(AnalysisResult& idx, StreamBase<char>* input) {
    return p->analyze(idx, input);
}
signed char
StreamAnalyzerPrivate::analyze(AnalysisResult& idx, StreamBase<char>* input) {
    //std::cerr << "analyze " << idx.path().c_str() << std::endl;

    // retrieve or construct the through analyzers and end analyzers
    std::vector<std::vector<StreamThroughAnalyzer*> >::iterator tIter;
    std::vector<std::vector<StreamEndAnalyzer*> >::iterator eIter;
    while ((int)through.size() <= idx.depth()) {
        addThroughAnalyzers();
        addEndAnalyzers();
    }
    tIter = through.begin() + idx.depth();
    eIter = end.begin() + idx.depth();

    // read the headersize size before connecting the throughanalyzers
    // This ensures that the first read is at least this size, even if the
    // throughanalyzers read smaller chunks.
    bool finished = false;
    const char* header = 0;
    int32_t headersize = 1024;
    if (input) {
        headersize = input->read(header, headersize, headersize);
        input->reset(0);
        if (headersize < 0) finished = true;
    }

    // insert the through analyzers
    std::vector<StreamThroughAnalyzer*>::iterator ts;
    for (ts = tIter->begin(); (input == 0 || input->status() == Ok)
            && ts != tIter->end(); ++ts) {
        (*ts)->setIndexable(&idx);
        input = (*ts)->connectInputStream(input);
        if (input && input->position() != 0) {
            std::cerr << "Analyzer " << (*ts)->name() << " has left the stream in a bad state." << std::endl;
        }
    }

    // reread the header so we can use it for the endanalyzers
    if (input && headersize > 0) {
        headersize = input->read(header, headersize, headersize);
        if (headersize <= 0) {
            finished = true;
        } else if (input->reset(0) != 0) {
            std::cerr << "resetting is impossible!! pos: " << input->position()
                << " status: " << input->status() << std::endl;
        }
    } else {
        // indicate that we have no data in the stream
        headersize = -1;
        finished = true;
    }
    size_t es = 0;
    size_t itersize = eIter->size();
    while (!finished && es != itersize) {
        StreamEndAnalyzer* sea = (*eIter)[es];
        if (sea->checkHeader(header, headersize)) {
            idx.setEndAnalyzer(sea);
            char ar = sea->analyze(idx, input);
            if (ar) {
// FIXME: find either a NIE-compliant way to report errors or use some API for this
//                idx.addValue(errorfield, sea->name() + string(": ")
//                    + sea->error());
                if (!idx.config().indexMore()) {
                    removeIndexable(idx.depth());
                    return -1;
                }
                int64_t pos = input->reset(0);
                if (pos != 0) { // could not reset
                    std::cerr << "could not reset stream of " << idx.path().c_str()
                        << " from pos " << input->position()
                        << " to 0 after reading with " << sea->name()
                        << ": " << sea->error().c_str() << std::endl;
                    finished = true;
                } else {
                    // refresh the pointer to the start of the data
                    headersize = input->read(header, headersize, headersize);
                    if (input->reset(0) != 0) {
                        std::cerr << "resetting again is impossible!! pos: "
                             << input->position() << " status: "
                             << input->status() << std::endl;
                    }
                    if (headersize < 0) finished = true;
                }
            } else {
                finished = true;
            }
            eIter = end.begin() + idx.depth();
        }
        if (!finished) {
            finished = !conf.indexMore();
        }
        es++;
    }
    idx.setEndAnalyzer(0);
    if (input) {
        // make sure the entire stream is read if the size is not known
        bool ready;
        tIter = through.begin() + idx.depth();
        uint32_t skipsize = 4096;
        do {
            // ask the analyzerconfiguration if we should continue
            int64_t max = idx.config().maximalStreamReadLength(idx);
            if (!idx.config().indexMore()
                    || (max != -1 && input->position() >= max)) {
                // we are done
                return 0;
            }
            ready = input->size() != -1;
            std::vector<StreamThroughAnalyzer*>::iterator ts;
            for (ts = tIter->begin(); ready && ts != tIter->end(); ++ts) {
                ready = (*ts)->isReadyWithStream();
            }
            if (!ready) {
                input->skip(skipsize);
                if (skipsize < 131072) {
                    skipsize *= 4;
                }
            }
        } while (!ready && input->status() == Ok);
        if (input->status() == Error) {
            fprintf(stderr, "Error: %s\n", input->error());
            removeIndexable(idx.depth());
            return -2;
        }
    }

    // store the size of the stream
    if (input && input->status() != Error && input->size() >= 0) {
        // TODO remove cast
        idx.addValue(sizefield, (uint32_t)input->size());
    }

    // remove references to the analysisresult before it goes out of scope
    removeIndexable(idx.depth());
    return 0;
}
/**
 * Remove references to the analysisresult before it goes out of scope.
 **/
void
StreamAnalyzerPrivate::removeIndexable(uint depth) {
    std::vector<std::vector<StreamThroughAnalyzer*> >::iterator tIter;
    std::vector<StreamThroughAnalyzer*>::iterator ts;
    tIter = through.begin() + depth;
    for (ts = tIter->begin(); ts != tIter->end(); ++ts) {
        // remove references to the analysisresult before it goes out of scope
        (*ts)->setIndexable(0);
    }
}
AnalyzerConfiguration&
StreamAnalyzer::configuration() const {
    return p->conf;
}
