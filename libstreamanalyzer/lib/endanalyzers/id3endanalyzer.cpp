/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2006 Jos van den Oever <jos@vandenoever.info>
 *               2009 Evgeny Egorochkin <phreedom.stdin@gmail.com>
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

#include "id3endanalyzer.h"
#include "analysisresult.h"
#include "../rdfnamespaces.h"
#include <strigi/strigiconfig.h>
#include <strigi/textutils.h>
#include <strigi/stringstream.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <iconv.h>

#ifdef ICONV_SECOND_ARGUMENT_IS_CONST
     #define ICONV_CONST const
#else
     #define ICONV_CONST
#endif

using namespace Strigi;

const std::string
    typePropertyName(
        STRIGI_RDF "type"),
    fullnamePropertyName(
        STRIGI_NCO "fullname"),
    titlePropertyName(
        STRIGI_NIE "title"),
    albumArtistPropertyName(
        STRIGI_NMM_DRAFT "albumArtist"),
    albumTrackCountPropertyName(
        STRIGI_NMM_DRAFT "albumTrackCount"),
    discNumberPropertyName(
        STRIGI_NMM_DRAFT "setNumber"),
    discCountPropertyName(
        STRIGI_NMM_DRAFT "setSize"),

    musicClassName(
        STRIGI_NMM_DRAFT "MusicPiece"),
    audioClassName(
        STRIGI_NFO "Audio"),
    albumClassName(
        STRIGI_NMM_DRAFT "MusicAlbum"),
    contactClassName(
        STRIGI_NCO "Contact");

/*
ENCA autodetection of broken encodings. First, need to make sure it's going to be actually useful.
ID3v2.0
play counter:needs nepomuk resolution
replaygain
+lyrics
+Improve:
  creation date:
  language: support multiple
  Genre
  album art type handling
VBR detection
*/

#define ID3_NUMBER_OF_GENRES 148

static const std::string genres[ID3_NUMBER_OF_GENRES] = {
  "Blues",
  "Classic Rock",
  "Country",
  "Dance",
  "Disco",
  "Funk",
  "Grunge",
  "Hip-Hop",
  "Jazz",
  "Metal",
  "New Age",
  "Oldies",
  "Other",
  "Pop",
  "R&B",
  "Rap",
  "Reggae",
  "Rock",
  "Techno",
  "Industrial",
  "Alternative",
  "Ska",
  "Death Metal",
  "Pranks",
  "Soundtrack",
  "Euro-Techno",
  "Ambient",
  "Trip-Hop",
  "Vocal",
  "Jazz+Funk",
  "Fusion",
  "Trance",
  "Classical",
  "Instrumental",
  "Acid",
  "House",
  "Game",
  "Sound Clip",
  "Gospel",
  "Noise",
  "Alternative Rock",
  "Bass",
  "Soul",
  "Punk",
  "Space",
  "Meditative",
  "Instrumental Pop",
  "Instrumental Rock",
  "Ethnic",
  "Gothic",
  "Darkwave",
  "Techno-Industrial",
  "Electronic",
  "Pop-Folk",
  "Eurodance",
  "Dream",
  "Southern Rock",
  "Comedy",
  "Cult",
  "Gangsta",
  "Top 40",
  "Christian Rap",
  "Pop/Funk",
  "Jungle",
  "Native American",
  "Cabaret",
  "New Wave",
  "Psychedelic",
  "Rave",
  "Showtunes",
  "Trailer",
  "Lo-Fi",
  "Tribal",
  "Acid Punk",
  "Acid Jazz",
  "Polka",
  "Retro",
  "Musical",
  "Rock & Roll",
  "Hard Rock",
  "Folk",
  "Folk/Rock",
  "National Folk",
  "Swing",
  "Fusion",
  "Bebop",
  "Latin",
  "Revival",
  "Celtic",
  "Bluegrass",
  "Avantgarde",
  "Gothic Rock",
  "Progressive Rock",
  "Psychedelic Rock",
  "Symphonic Rock",
  "Slow Rock",
  "Big Band",
  "Chorus",
  "Easy Listening",
  "Acoustic",
  "Humour",
  "Speech",
  "Chanson",
  "Opera",
  "Chamber Music",
  "Sonata",
  "Symphony",
  "Booty Bass",
  "Primus",
  "Porn Groove",
  "Satire",
  "Slow Jam",
  "Club",
  "Tango",
  "Samba",
  "Folklore",
  "Ballad",
  "Power Ballad",
  "Rhythmic Soul",
  "Freestyle",
  "Duet",
  "Punk Rock",
  "Drum Solo",
  "A Cappella",
  "Euro-House",
  "Dance Hall",
  "Goa",
  "Drum & Bass",
  "Club-House",
  "Hardcore",
  "Terror",
  "Indie",
  "BritPop",
  "Negerpunk",
  "Polsk Punk",
  "Beat",
  "Christian Gangsta Rap",
  "Heavy Metal",
  "Black Metal",
  "Crossover",
  "Contemporary Christian",
  "Christian Rock",
  "Merengue",
  "Salsa",
  "Thrash Metal",
  "Anime",
  "Jpop",
  "Synthpop"
};

const uint32_t bitrate [15] = {0, 32000, 40000, 48000, 56000, 64000, 80000, 96000, 112000, 128000, 160000, 192000, 224000, 256000, 320000 };

const uint32_t samplerate[3] = {44100, 48000, 32000};

const char * encodings[5] = {"ISO-8859-1", "UTF-16", "UTF-16BE", "UTF-8", "UTF-16LE"};

#ifndef _GNU_SOURCE
size_t
strnlen(const char *s, size_t maxlen) {
    for(size_t i=0; i<maxlen; i++)
        if (s[i]==0)
            return i;
    return maxlen;
}
#endif

class UTF8Convertor {
  private:
    iconv_t const conv;
    char *out;
    size_t capacity;
  public:
     UTF8Convertor(const char *encoding);
     const std::string convert(const char *data, size_t len);
     ~UTF8Convertor();
};
UTF8Convertor::UTF8Convertor(const char *encoding) :conv(iconv_open("UTF-8", encoding)), out(0), capacity(0) {
}
UTF8Convertor::~UTF8Convertor() {
    iconv_close(conv);
    if (out) free(out);
}
const std::string
UTF8Convertor::convert(const char *data, size_t len) {
  if (!len)
      return std::string();
  if ( capacity<len*3 ||	// is the buffer too small or too large?
      (capacity>10000 && capacity>len*8) ) {
      capacity = len*3;
      out = (char*)realloc(out, len*3);
  }

  char *result = out;
  size_t reslen = capacity;

  ICONV_CONST char *input = (char *)data;
  iconv(conv, &input, &len, &result, &reslen);

  return std::string(out,capacity-reslen);
}

void
ID3EndAnalyzerFactory::registerFields(FieldRegister& r) {
    createdField	= r.registerField(STRIGI_NIE "contentCreated");
    subjectField	= r.registerField(STRIGI_NIE "subject");
    titleField		= r.registerField(titlePropertyName);
    descriptionField	= r.registerField(STRIGI_NIE "description");
    commentField	= r.registerField(STRIGI_NIE "comment");
    albumField		= r.registerField(STRIGI_NMM_DRAFT "musicAlbum");
    genreField		= r.registerField(STRIGI_NMM_DRAFT "genre");
    composerField	= r.registerField(STRIGI_NMM_DRAFT "composer");
    performerField	= r.registerField(STRIGI_NMM_DRAFT "performer");
    lyricistField	= r.registerField(STRIGI_NMM_DRAFT "lyricist");
    publisherField	= r.registerField(STRIGI_NCO "publisher");
    languageField	= r.registerField(STRIGI_NIE "language");
    copyrightField	= r.registerField(STRIGI_NIE "copyright");
    trackNumberField	= r.registerField(STRIGI_NMM_DRAFT "trackNumber");
    discNumberField	= r.registerField(discNumberPropertyName);
    durationField	= r.registerField(STRIGI_NFO "duration");
    typeField		= r.typeField;

    bitrateField	= r.registerField(STRIGI_NFO "averageBitrate");
    samplerateField	= r.registerField(STRIGI_NFO "sampleRate");
    codecField		= r.registerField(STRIGI_NFO "codec");
    channelsField	= r.registerField(STRIGI_NFO "channels");
}

inline
void
addStatement(AnalysisResult &indexable, std::string& subject, const std::string& predicate, const std::string& object) {
    if (subject.empty())
        subject = indexable.newAnonymousUri();
    indexable.addTriplet(subject, predicate, object);
}

inline
int32_t readAsyncSize(const unsigned char* b) {
    return (((int32_t)b[0])<<21) + (((int32_t)b[1])<<14)
	    + (((int32_t)b[2])<<7) + ((int32_t)b[3]);
}

int32_t
readSize(const unsigned char* b, bool async) {
    const signed char* c = (const signed char*)b;
    if (async) {
        if (c[0] < 0 || c[1] < 0 || c[2] < 0 || c[3] < 0)
            return -1;
        return readAsyncSize(b);
    }
    return readBigEndianInt32(b);
}
bool
ID3EndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
  const unsigned char* usbuf = (const unsigned char*)header;
  int32_t i;

  for(i=0; (header[i] == '\0') && (i<headersize); i++);
  return (headersize>=6+i)
	  && (
	    (strncmp("ID3", header+i, 3) == 0	// check that it's ID3
	      && usbuf[3+i] <= 4 				// only handle version <= 4
	      && (usbuf[5+i]&~0x80) == 0)  // we're too dumb too handle other flags
	    ||
	    ((unsigned char)header[0+i] == 0xff && ((unsigned char)header[1+i]&0xfe) == 0xfa
	      && (unsigned char)header[2+i]>>4 != 0xf	// MP3 frame header is ok too
	      && (((unsigned char)header[2+i]>>2)&3) != 3)
	  );

}

static void trim(std::string& s,const std::string& drop = " ")
{
    std::string r = s.erase(s.find_last_not_of(drop)+1);
    r.erase(0, r.find_first_not_of(drop));
}

static bool extract_and_trim(const char* buf, int offset, int length, std::string& s)
{
    // We're extracting here the ID3v1 tags and doing some sanity checks:
    // 1) Strip of all leading and prefixed spaces
    // 2) Test if string contains at least something
    if (!buf[offset])
        return false;
    
    s = std::string(buf + offset, strnlen(buf + offset, length));
    trim(s);
    // Return true if the extracted value is not empty (read: contains something)
    return !s.empty();
}

/**
 * Functional helper class to get the right numbers out of a 'genre' string which
 * might be a number in a index
 */
class genre_number_parser {
  private:
    bool success;
    long result;
    void parse_string( std::string genre ) {
        char* endptr;
        int r = strtol(genre.c_str(),&endptr, 10);
        if(*endptr == '\0') { //to check if the convertion went more or less ok
            if(r >=0 && r < ID3_NUMBER_OF_GENRES ) { //to ensure it is within the range we have
                success=true;
                result=r;
            }
        }
    }
  public:
    /**
     * constructor taking the genre string you want parsed as a number
     */
    genre_number_parser(std::string genre) : success(false), result(-1) {
        if(genre.size()==0) {
            //if the string is empty, there is no need to try to parse it
            return;
        }
        //the string might start and end with parenthesis
        if(genre[0]=='(' && genre[genre.size()-1]==')') {
            parse_string(genre.substr(1,genre.length()-2));
            return;
        }
        parse_string(genre);
    }
    /**
     * wether or not parsing was successful
     */
    operator bool() {
        return success;
    }
    /**
     * the actual result of the parsing, or -1 if parsing wasn't successful
     */
    operator long() {
        return result;
    }
};

signed char
ID3EndAnalyzer::analyze(Strigi::AnalysisResult& indexable, Strigi::InputStream* in) {
  const int max_padding = 1000;
    if(!in)
        return -1;

    bool found_title = false, found_artist = false,
        found_album = false, found_comment = false,
        found_year = false, found_track = false,
        found_genre = false, found_tag = false;
    std::string albumUri;
    char albumArtNum = '\0';

    // read 10 byte header
    const char* buf;
    int32_t nread = in->read(buf, 10+max_padding, 10+max_padding);

    // parse ID3v2* tag

    if (nread == 10+max_padding && strncmp("ID3", buf, 3) == 0) { // check for ID3 header

	bool async = buf[3] >= 4;
	bool unsync = (buf[5] & 0x80)!=0;

	// calculate size from 4 syncsafe bytes
	int32_t size = readAsyncSize((unsigned char*)buf+6);
	if (size < 0 || size > 5000000)
	    return -1;
	size += 10+4+max_padding; // add the size of the ID3 header, MP3 frame header and possible padding generated by LAME(workaround)

	// read the entire tag
	in->reset(0);
	nread = in->read(buf, size, size);
	if (nread != size)
	    return -1;

	found_tag = true;

	const char* p = buf + 10;
	buf += size-4-max_padding;
	while (p < buf && *p) {
	    size = readSize((unsigned char*)p+4, async);
	    if (size <= 0 || size > (buf-p)-10) {
		//cerr << "size < 0: " << size << endl;
		break;
	    }

	    std::string value;
	    uint8_t enc = p[10];
	    const char *encoding = enc>4 ? encodings[0] : encodings[enc] ;
	    UTF8Convertor conv(encoding);
	    const char *decoded_value;
	    int32_t decoded_value_size;
	    std::string deunsyncbuf;
	    if (unsync) {
	      deunsyncbuf.reserve(size-1);
	      for(int32_t i = 0; i<size-1; i++)
		if ( (i==0) || (p[11+i]!=0) || (p[10+i]!=0xff) )
		  deunsyncbuf.push_back(p[11+i]);
	      decoded_value = deunsyncbuf.c_str();
	      decoded_value_size = deunsyncbuf.length();
	    } else {
	      decoded_value = p+11;
	      decoded_value_size = size-1;
	    };

	    if (strncmp("APIC", p, 4) == 0) {
		size_t mimelen = strnlen(decoded_value, decoded_value_size);
                if ((int32_t)mimelen < decoded_value_size-3) {
		    const char *desc = decoded_value+mimelen+1+1;
//		    uint8_t pictype = p[11+mimelen+1];
		    size_t desclen = strnlen(desc,decoded_value_size-mimelen-2-1);
		    const char *content = desc + desclen + 1 + (enc == 0 || enc == 3 ? 0:1) ;

		    if(content<decoded_value+decoded_value_size) {
                        StringInputStream picstream(content,
                                          (uint32_t)(decoded_value+decoded_value_size-content), false);
			std::string picname;
			picname = (char)('0'+albumArtNum++);
			indexable.indexChild(picname, indexable.mTime(), &picstream);

			if (desclen && indexable.child()) {
			    if (enc == 0 || enc == 3) {
				indexable.child()->addValue(factory->descriptionField, std::string(desc, desclen) );
			    } else {
				indexable.child()->addValue(factory->descriptionField, conv.convert(desc, desclen) );
			    }
			}

                        indexable.finishIndexChild();
		    }
		}
	    }

	    if (enc == 0 || enc == 3) {
		value = std::string(decoded_value, strnlen(decoded_value, decoded_value_size));
	    } else {
		value = conv.convert(decoded_value, decoded_value_size); // FIXME: add similar workaround
	    }

	    if (!value.empty()) {
		if (strncmp("TIT1", p, 4) == 0) {
		    indexable.addValue(factory->subjectField, value);
		} else if (strncmp("TIT2", p, 4) == 0) {
		    indexable.addValue(factory->titleField, value);
		    found_title = true;
		} else if (strncmp("TIT3", p, 4) == 0) {
		    indexable.addValue(factory->descriptionField, value);
		} else if (strncmp("TLAN", p, 4) == 0) {
		    indexable.addValue(factory->languageField, value);
		} else if (strncmp("TCOP", p, 4) == 0) {
		    indexable.addValue(factory->copyrightField, value);
		} else if ((strncmp("TDRL", p, 4) == 0) ||
			    (strncmp("TDAT", p, 4) == 0) ||
			    (strncmp("TYER", p, 4) == 0) ||
			    (strncmp("TDRC", p, 4) == 0)) {
		    indexable.addValue(factory->createdField, value);
		    found_year = true;
		} else if ((strncmp("TPE1", p, 4) == 0) ||
			    (strncmp("TPE3", p, 4) == 0) ||
			    (strncmp("TPE4", p, 4) == 0)) {
		    std::string performerUri = indexable.newAnonymousUri();

		    indexable.addValue(factory->performerField, performerUri);
		    indexable.addTriplet(performerUri, typePropertyName, contactClassName);
		    indexable.addTriplet(performerUri, fullnamePropertyName, value);
		    found_artist = true;
		} else if (strncmp("TPE2", p, 4) == 0) {
		    const std::string albumArtistUri( indexable.newAnonymousUri() );

		    addStatement(indexable, albumUri, albumArtistPropertyName, albumArtistUri);
		    indexable.addTriplet(albumArtistUri, typePropertyName, contactClassName);
		    indexable.addTriplet(albumArtistUri, fullnamePropertyName, value);
		} else if ((strncmp("TPUB", p, 4) == 0) ||
			    (strncmp("TENC", p, 4) == 0)) {
		    std::string publisherUri = indexable.newAnonymousUri();

		    indexable.addValue(factory->publisherField, publisherUri);
		    indexable.addTriplet(publisherUri, typePropertyName, contactClassName);
		    indexable.addTriplet(publisherUri, fullnamePropertyName, value);
		} else if ((strncmp("TALB", p, 4) == 0) ||
			    (strncmp("TOAL", p, 4) == 0)) {
		    addStatement(indexable, albumUri, titlePropertyName, value);
		    found_album = true;
		} else if (strncmp("TCON", p, 4) == 0) {
		    genre_number_parser p(value);
		    if(p) {
			indexable.addValue(factory->genreField, genres[ p ]);
			found_genre = true;
		    } else {
			// We must not forget that genre could be a string.
			if (!value.empty()) {
			    indexable.addValue(factory->genreField, value);
			    found_genre = true;
			}
		    }
		} else if (strncmp("TLEN", p, 4) == 0) {
		    indexable.addValue(factory->durationField, value);
		} else if (strncmp("TEXT", p, 4) == 0) {
		    std::string lyricistUri = indexable.newAnonymousUri();

		    indexable.addValue(factory->lyricistField, lyricistUri);
		    indexable.addTriplet(lyricistUri, typePropertyName, contactClassName);
		    indexable.addTriplet(lyricistUri, fullnamePropertyName, value);
		} else if (strncmp("TCOM", p, 4) == 0) {
		    std::string composerUri = indexable.newAnonymousUri();

		    indexable.addValue(factory->composerField, composerUri);
		    indexable.addTriplet(composerUri, typePropertyName, contactClassName);
		    indexable.addTriplet(composerUri, fullnamePropertyName, value);
		} else if (strncmp("TRCK", p, 4) == 0) {
		    std::istringstream ins(value);
		    int tnum;
		    ins >> tnum;
		    if (!ins.fail()) {
			indexable.addValue(factory->trackNumberField, tnum);
			found_track = true;
			ins.ignore(10,'/');
			int tcount;
			ins >> tcount;
			if (!ins.fail()) {
			    std::ostringstream outs;
			    outs << tcount;
			    addStatement(indexable, albumUri, albumTrackCountPropertyName, outs.str());
			}
		    }
		} else if (strncmp("TPOS", p, 4) == 0) {
		    std::istringstream ins(value);
		    int dnum;
		    ins >> dnum;
		    if (!ins.fail()) {
			indexable.addValue(factory->discNumberField, dnum);
			ins.ignore(10,'/');
			int dcount;
			ins >> dcount;
			if (!ins.fail()) {
			    std::ostringstream outs;
			    outs << dcount;
			    addStatement(indexable, albumUri, discCountPropertyName, outs.str());
			}
		    }
		}
	    }
	    p += size + 10;
	}
    }
    // parse MP3 frame header

    int bitrateindex, samplerateindex;
    int i;
    for(i=0; (buf[i]=='\0') && (i<max_padding) ; i++);
    if (((unsigned char)buf[0+i] == 0xff) && (((unsigned char)buf[1+i]&0xfe) == 0xfa)
      && ((bitrateindex = ((unsigned char)buf[2+i]>>4)) != 0xf)
      && ((samplerateindex = (((unsigned char)buf[2+i]>>2)&3)) != 3 )) { // is this MP3?

        indexable.addValue(factory->typeField, audioClassName);
        // FIXME: no support for VBR :(
        // ideas: compare bitrate from the frame with stream size/duration from ID3 tags
        // check several consecutive frames to see if bitrate is different
        // in neither case you can be sure to properly detected VBR :(
        indexable.addValue(factory->bitrateField, bitrate[bitrateindex]);
        indexable.addValue(factory->samplerateField, samplerate[samplerateindex]);
        indexable.addValue(factory->codecField, "MP3");
        indexable.addValue(factory->channelsField, ((buf[3+i]>>6) == 3 ? 1:2 ) );
    }

    // Parse ID3v1 tag

    int64_t insize;
    if ( (insize = in->size()) > (128+nread)) {

        // read the tag and check signature
        int64_t nskip = insize-128-nread;
        if (nskip == in->skip(nskip))
        if (in->read(buf, 128, 128)==128)
        if (!strncmp("TAG", buf, 3)) {

            found_tag = true;
            
            std::string s;

            if (!found_title && extract_and_trim(buf, 3, 30, s)) {
                indexable.addValue(factory->titleField, s);
            }
            if (!found_artist && extract_and_trim(buf, 33, 30, s)) {
                const std::string performerUri = indexable.newAnonymousUri();
                indexable.addValue(factory->performerField, performerUri);
                indexable.addTriplet(performerUri, typePropertyName, contactClassName);
                indexable.addTriplet(performerUri, fullnamePropertyName, s);
            }
            if (!found_album && extract_and_trim(buf, 63, 30, s))
                addStatement(indexable, albumUri, titlePropertyName, s);
            if (!found_year && extract_and_trim(buf, 93, 4, s))
                indexable.addValue(factory->createdField, s);
            if (!found_comment && extract_and_trim(buf, 97, 30, s)) {
                indexable.addValue(factory->commentField, s);
            }
            if (!found_track && !buf[125] && buf[126]) {
                indexable.addValue(factory->trackNumberField, (int)(buf[126]));
            }
            if (!found_genre && (unsigned char)(buf[127]) < ID3_NUMBER_OF_GENRES)
                indexable.addValue(factory->genreField, genres[(uint8_t)buf[127]]);
        }
    }

    if(!albumUri.empty()) {
        indexable.addValue(factory->albumField, albumUri);
        indexable.addTriplet(albumUri, typePropertyName, albumClassName);
    }

    if (found_tag)
        indexable.addValue(factory->typeField, musicClassName);

    return 0;
}
