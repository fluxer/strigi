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
/*
 Include the strigi specific headers.
*/
#define STRIGI_IMPORT_API
#include "analyzerplugin.h"
#include "streamendanalyzer.h"
#include "analysisresult.h"
#include <strigi/fieldtypes.h>
#include <strigi/stringstream.h>
#include <strigi/textutils.h>
#include <exiv2/exiv2.hpp>
#include <math.h>
#include <time.h>
#include <iostream>

#if defined(__sun) || defined(sun)
#define HUGE_VALF HUGE_VAL
#endif

using namespace Strigi;
using namespace std;


/*
 Declare the factory.
*/
class ExivEndAnalyzerFactory;

/*
Define a class that inherits from StreamEndAnalyzer.
The only function we really need to implement is connectInputStream()
*/
class STRIGI_PLUGIN_API ExivEndAnalyzer : public StreamEndAnalyzer {
private:
    AnalysisResult* result;
    const ExivEndAnalyzerFactory* factory;

public:
    ExivEndAnalyzer(const ExivEndAnalyzerFactory* f) :factory(f) {}
    ~ExivEndAnalyzer() {}
    const char* name() const {
        return "ExivEndAnalyzer";
    }
    bool checkHeader(const char* header, int32_t headersize) const;
    signed char analyze(AnalysisResult& idx, ::InputStream* in);

private:
};

/*
 Define a factory class the provides information about the fields that an
 analyzer can extract. This has a function similar to KFilePlugin::addItemInfo.
*/
class STRIGI_PLUGIN_API ExivEndAnalyzerFactory : public StreamEndAnalyzerFactory {
friend class ExivEndAnalyzer;
private:
    /* This is why this class is a factory. */
    StreamEndAnalyzer* newInstance() const {
        return new ExivEndAnalyzer(this);
    }
    const char* name() const {
        return "ExivEndAnalyzer";
    }
    void registerFields(FieldRegister& );

    /* The RegisteredField instances are used to index specific fields quickly.
       We pass a pointer to the instance instead of a string.
    */
    map<string, const RegisteredField*> exifFields;
    const RegisteredField* commentField;
    const RegisteredField* manufacturerField;
    const RegisteredField* modelField;
    const RegisteredField* creationDateField;
    const RegisteredField* widthField;
    const RegisteredField* heightField;
    const RegisteredField* orientationField;
    const RegisteredField* flashUsedField;
    const RegisteredField* focalLengthField;
    const RegisteredField* _35mmEquivalentField;
    const RegisteredField* exposureTimeField;
    const RegisteredField* apertureField;
    const RegisteredField* exposureBiasField;
    const RegisteredField* whiteBalanceField;
    const RegisteredField* meteringModeField;
    const RegisteredField* exposureField;

    const RegisteredField* typeField;

};

#define NEXIF "http://www.semanticdesktop.org/ontologies/2007/05/10/nexif#"

const string commentFieldName("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#comment");
const string manufacturerFieldName(NEXIF "make");
const string modelFieldName(NEXIF "model");
const string creationDateFieldName("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#contentCreated");
const string widthFieldName("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#width");
const string heightFieldName("http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#height");
const string orientationFieldName(NEXIF "orientation");
const string flashUsedFieldName(NEXIF "flash");
const string focalLengthFieldName(NEXIF "focalLength");
const string _35mmEquivalentFieldName(NEXIF "focalLengthIn35mmFilm");
const string exposureTimeFieldName(NEXIF "exposureTime");
const string apertureFieldName(NEXIF "apertureValue");
const string exposureBiasFieldName(NEXIF "exposureBiasValue");
const string whiteBalanceFieldName(NEXIF "whiteBalance");
const string meteringModeFieldName(NEXIF "meteringMode");
const string exposureFieldName(NEXIF "exposureProgram");
const string ISOSpeedRatingsFieldName(NEXIF "isoSpeedRatings");

#undef NEXIF

/*
 Register the field names so that the StreamIndexer knows which analyzer
 provides what information.
*/
void
ExivEndAnalyzerFactory::registerFields(FieldRegister& r) {
    commentField = r.registerField(commentFieldName);
    addField(commentField);

    exifFields["Exif.Image.DateTime"] = r.registerField(creationDateFieldName);
    exifFields["Exif.Image.Make"] = r.registerField(manufacturerFieldName);
    exifFields["Exif.Image.Model"] = r.registerField(modelFieldName);
    exifFields["Exif.Photo.PixelXDimension"] = r.registerField(widthFieldName);
    exifFields["Exif.Photo.PixelYDimension"] = r.registerField(heightFieldName);
    exifFields["Exif.Image.Orientation"] = r.registerField(orientationFieldName);
    exifFields["Exif.Photo.Flash"] = r.registerField(flashUsedFieldName);
    exifFields["Exif.Photo.FocalLength"] = r.registerField(focalLengthFieldName);
    exifFields["Exif.Photo.FocalLengthIn35mmFilm"] = r.registerField(_35mmEquivalentFieldName);
    exifFields["Exif.Photo.ExposureTime"] = r.registerField(exposureTimeFieldName);
    exifFields["Exif.Photo.ApertureValue"] = r.registerField(apertureFieldName);
    exifFields["Exif.Photo.ExposureBiasValue"] = r.registerField(exposureBiasFieldName);
    exifFields["Exif.Photo.WhiteBalance"] = r.registerField(whiteBalanceFieldName);
    exifFields["Exif.Photo.MeteringMode"] = r.registerField(meteringModeFieldName);
    exifFields["Exif.Photo.ISOSpeedRatings"] = r.registerField(ISOSpeedRatingsFieldName);

map<string, const RegisteredField*>::const_iterator i = exifFields.begin();
    for (; i != exifFields.end(); ++i) {
        addField(i->second);
    }

    exposureField = r.registerField(exposureFieldName);
    typeField = r.typeField;

    addField(exposureField);
    addField(typeField);
}

// for reference:
// https://dev.exiv2.org/projects/exiv2/wiki/Supported_image_formats
bool
ExivEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
    static const unsigned char jpgmagic[] =
        { 0xFF, 0xD8, 0xFF };
    if (headersize >= 3 && ::memcmp(header, jpgmagic, 3) == 0) {
        return true;
    }

    static const unsigned char pngmagic[]
        = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };
    if (headersize >= 8 && ::memcmp(header, pngmagic, 8) == 0) {
        return true;
    }

    static const unsigned char epsmagic[]
        = { 0xd3, 0xc6 };
    if (headersize >= 2 && ::memcmp(header, epsmagic, 2) == 0) {
        return true;
    }

    // for reference:
    // https://www.iana.org/assignments/media-types/image/jp2
    static const unsigned char jp2magic[]
        = { 0x00, 0x00, 0x00, 0x0C, 0x6A, 0x50, 0x20, 0x20, 0x0D, 0x0A, 0x87, 0x0A };
    if (headersize >= 12 && ::memcmp(header, jp2magic, 12) == 0) {
        return true;
    }

    // for reference:
    // https://developers.google.com/speed/webp/docs/riff_container
    if (headersize >= 12 && ::memcmp(header + 8, "WEBP", 4) == 0) {
        return true;
    }

    return false;
}
namespace {

bool
fnumberToApertureValue(string& fnumber) {
    // Convert the F number to the aperture value
    // return true if the value in fnumber is changed to ApertureValue
    // F number = sqrt(pow(2, apex))
    char* end;
    double c = strtod(fnumber.c_str(), &end);
    if (c <= 0 || c == HUGE_VALF || *end == '\0') return false;
    double d = strtod(end + 1, &end);
    if (d <= 0 || d == HUGE_VALF) return false;
    c /= d;

    // do not use log2() or round() here to be cross-platform
    double apex = log(c*c) / M_LN2 * 65536;
    apex  = (apex < 0 ? ceil(apex - 0.5) : floor(apex + 0.5));
    std::ostringstream aperture;
    aperture << apex << "/65536";
    fnumber.assign(aperture.str());
    return true;
}

}

signed char
ExivEndAnalyzer::analyze(AnalysisResult& ar, ::InputStream* in) {
    // parse the file now
    Exiv2::Image::AutoPtr img;
    bool ok = false;
    if (ar.depth() == 0) {
        try {
            // try to open the file directly: this is faster
            if (ar.path().compare(0, 5, "file:") == 0) {
                img = Exiv2::ImageFactory::open(ar.path().substr(5));
            } else {
                img = Exiv2::ImageFactory::open(ar.path());
            }
            img->readMetadata();
            ok = true;
        } catch (...) {
            // no problem yet, we can read from the stream
            cerr << "error reading " << ar.path() << endl;
        }
    }

    const char* data;
    if (!ok) {
        // read the entire input stream
        int32_t nread = in->read(data, 1, 0);
        while (nread > 0 && in->status() != Eof) {
            in->reset(0);
            nread = in->read(data, 2*nread, 0);
        }
        in->reset(0);
        if (nread <= 0) {
            m_error.assign("no valid image");
            return -1;
        }

        try {
            const Exiv2::byte* d = (const Exiv2::byte*)data;
            img = Exiv2::ImageFactory::open(d, nread);
            img->readMetadata();
        } catch (Exiv2::Error& e) {
            // even though this is the child class of Exiv2::Error, we seem to need
            // to catch it separately
            m_error.assign(e.what());
            return -1;
        } catch (Exiv2::AnyError& e) {
            m_error.assign(e.what());
            return -1;
        } catch(std::exception& e) {
            // trueg: here Exiv2::Error is not caught above, until the reason is found
            // I added this generic exception handling which should do no harm whatsoever
            m_error.assign(e.what());
            return -1;
        }
    }

    ar.addValue(factory->exifFields.find("Exif.Photo.PixelYDimension")->second, img->pixelHeight());
    ar.addValue(factory->exifFields.find("Exif.Photo.PixelXDimension")->second, img->pixelWidth());
    
    if (img->comment().length()) {
        ar.addValue(factory->commentField, img->comment());
    }

    const Exiv2::ExifData& exif = img->exifData();
    // if there's exif data, this is a photo, otherwise just an image
    if( exif.empty() ) {
        ar.addValue(factory->typeField, "http://www.semanticdesktop.org/ontologies/2007/03/22/nfo#RasterImage");
        return 0;
    }

    ar.addValue(factory->typeField, "http://www.semanticdesktop.org/ontologies/2007/05/10/nexif#Photo");

    for (Exiv2::ExifData::const_iterator i = exif.begin(); i != exif.end();++i){
        if (i->key() == "Exif.Photo.FNumber") {
            string aperture(i->toString());
            if (fnumberToApertureValue(aperture)) {
                ar.addValue(
                   factory->exifFields.find("Exif.Photo.ApertureValue")->second,
                   aperture);
            }
        }
        else if(i->key() == "Exif.Image.DateTime") {
            // the exif datetime string format is as follows: "2005:06:03 17:13:33"
            struct tm date;
            if(sscanf(i->toString().c_str(), "%d:%d:%d %d:%d:%d", &date.tm_year, &date.tm_mon, &date.tm_mday, &date.tm_hour, &date.tm_min, &date.tm_sec) == 6) {
                ar.addValue(factory->exifFields.find("Exif.Image.DateTime")->second, uint32_t(mktime(&date)));
            }
        } else if (i->key() != "Exif.Photo.PixelXDimension" && i->key() != "Exif.Photo.PixelYDimension") {
            map<string,const RegisteredField*>::const_iterator f
                    = factory->exifFields.find(i->key());
            if (f != factory->exifFields.end() && f->second) {
                ar.addValue(f->second, i->toString());
                //        } else {
                //            cerr << i->key() << "\t" << i->value() << endl;
            }
        }
    }
#if EXIV2_TEST_VERSION(0,17,91)
    Exiv2::ExifThumbC thumb(exif);
    Exiv2::DataBuf thumbnail = thumb.copy();
#else
    Exiv2::DataBuf thumbnail = exif.copyThumbnail();
#endif
    data = (const char*)thumbnail.pData_;
    if (data) {
        StringInputStream thumbstream(data, (int32_t)thumbnail.size_, false);
        string thumbname("thumbnail");
#if EXIV2_TEST_VERSION(0,17,91)
        ar.indexChild(thumbname + thumb.extension(), ar.mTime(),
            &thumbstream);
        ar.finishIndexChild();
#else
        ar.indexChild(thumbname + exif.thumbnailExtension(), ar.mTime(),
            &thumbstream);
        ar.finishIndexChild();
#endif
    }

    return 0;
}

/*
 For plugins, we need to have a way to find out which plugins are defined in a
 plugin. One instance of AnalyzerFactoryFactory per plugin profides this
 information.
*/
class Factory : public AnalyzerFactoryFactory {
public:
    list<StreamEndAnalyzerFactory*>
    streamEndAnalyzerFactories() const {
        list<StreamEndAnalyzerFactory*> af;
        af.push_back(new ExivEndAnalyzerFactory());
        return af;
    }
};

/*
 Register the AnalyzerFactoryFactory
*/
STRIGI_ANALYZER_FACTORY(Factory)