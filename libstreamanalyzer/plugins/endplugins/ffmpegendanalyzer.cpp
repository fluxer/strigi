/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2010 Evgeny Egorochkin <phreedom.stdin@gmail.com>
 * Copyright (C) 2011 Tirtha Chatterjee <tirtha.p.chatterjee@gmail.com>
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

#include <strigi/analyzerplugin.h>
#include <strigi/streamendanalyzer.h>
#include <strigi/analysisresult.h>
#include <strigi/fieldtypes.h>
#include <strigi/textutils.h>
#include <rdfnamespaces.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <cstring>
#include <iostream>
#include <sstream>
#include <cassert>

using namespace Strigi;

class FFMPEGEndAnalyzerFactory;

class STRIGI_PLUGIN_API FFMPEGEndAnalyzer : public StreamEndAnalyzer {
private:
    const FFMPEGEndAnalyzerFactory* factory;
public:
    FFMPEGEndAnalyzer(const FFMPEGEndAnalyzerFactory* f) :factory(f) {}
    
    ~FFMPEGEndAnalyzer() {}
    
    const char* name() const {
        return "FFMPEGEndAnalyzer";
    }
    bool checkHeader(const char* header, int32_t headersize) const;
    signed char analyze(AnalysisResult& idx, ::InputStream* in);
};

class STRIGI_PLUGIN_API FFMPEGEndAnalyzerFactory : public StreamEndAnalyzerFactory {
friend class FFMPEGEndAnalyzer;
public:
    FFMPEGEndAnalyzerFactory() {
// 2018-02-06 - 0694d87024 - lavf 58.9.100 - avformat.h
#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(58, 9, 100)
        av_register_all();
#endif
    }
private:
    ~FFMPEGEndAnalyzerFactory() {
    }
    StreamEndAnalyzer* newInstance() const {
        return new FFMPEGEndAnalyzer(this);
    }
    const char* name() const {
        return "FFMPEGEndAnalyzer";
    }
    void registerFields(FieldRegister& );

    const RegisteredField* durationProperty;
    const RegisteredField* widthProperty;
    const RegisteredField* heightProperty;
    const RegisteredField* frameRateProperty;
    const RegisteredField* codecProperty;
    const RegisteredField* bitrateProperty;
    const RegisteredField* channelsProperty;
    const RegisteredField* samplerateProperty;
    const RegisteredField* titleProperty;
    const RegisteredField* creatorProperty;
    const RegisteredField* copyrightProperty;
    const RegisteredField* commentProperty;
    const RegisteredField* albumProperty;
    const RegisteredField* genreProperty;
    const RegisteredField* trackProperty;
    const RegisteredField* createdProperty;
    const RegisteredField* typeProperty;
    const RegisteredField* hasPartProperty;
};

const std::string
  videoClassName = 
    STRIGI_NFO "Video",
  audioClassName = 
    STRIGI_NFO "Audio",
  musicPieceClassName = 
    STRIGI_NMM_DRAFT "MusicPiece",
  albumClassName =
    STRIGI_NMM_DRAFT "MusicAlbum",
  embeddedClassName = 
    STRIGI_NFO "EmbeddedFileDataObject",
  contactClassName =
    STRIGI_NCO "Contact",

  typePropertyName =
    STRIGI_RDF "type",
  hasPartPropertyName =
    STRIGI_NIE "hasPart",
  partOfPropertyName =
    STRIGI_NIE "isPartOf",

  titlePropertyName = 
    STRIGI_NIE "title",
  fullnamePropertyName =
    STRIGI_NCO "fullname",
  commentPropertyName = 
    STRIGI_NIE "comment",
  languagePropertyName = 
    STRIGI_NIE "language",
  genrePropertyName =
    STRIGI_NMM_DRAFT "genre",
  trackPropertyName =
    STRIGI_NMM_DRAFT "trackNumber",
  createdPropertyName =
    STRIGI_NIE "contentCreated",
  creatorPropertyName =
    STRIGI_NCO "creator",
  copyrightPropertyName =
    STRIGI_NIE "copyright",
  albumPropertyName =
    STRIGI_NMM_DRAFT "musicAlbum",

  sampleratePropertyName = 
    STRIGI_NFO "sampleRate",
  codecPropertyName = 
    STRIGI_NFO "codec",
  channelsPropertyName = 
    STRIGI_NFO "channels",
  bitratePropertyName = 
    STRIGI_NFO "averageBitrate",
  durationPropertyName = 
    STRIGI_NFO "duration",
  widthPropertyName = 
    STRIGI_NFO "width",
  heightPropertyName = 
    STRIGI_NFO "height",
  aspectRatioPropertyName =
    STRIGI_NFO "aspectRatio",
  frameRatePropertyName = 
    STRIGI_NFO "frameRate";

void
FFMPEGEndAnalyzerFactory::registerFields(FieldRegister& r) {
  durationProperty = r.registerField(durationPropertyName);
  widthProperty = r.registerField(widthPropertyName);
  heightProperty = r.registerField(heightPropertyName);
  frameRateProperty = r.registerField(frameRatePropertyName);
  codecProperty = r.registerField(codecPropertyName);
  bitrateProperty = r.registerField(bitratePropertyName);
  typeProperty = r.typeField;
  channelsProperty = r.registerField(channelsPropertyName);
  samplerateProperty = r.registerField(sampleratePropertyName);
  titleProperty = r.registerField(titlePropertyName);
  creatorProperty = r.registerField(creatorPropertyName);
  copyrightProperty = r.registerField(copyrightPropertyName);
  commentProperty = r.registerField(commentPropertyName);
  albumProperty = r.registerField(albumPropertyName);
  genreProperty = r.registerField(genrePropertyName);
  trackProperty = r.registerField(trackPropertyName);
  createdProperty = r.registerField(createdPropertyName);
  hasPartProperty = r.registerField(hasPartPropertyName);
}

// Probe all input formats and obtain score.
// Evil FFMPEG hid av_probe_input_format2, the function that does just this.
AVInputFormat *probe_format(AVProbeData *pd, int *max_score) {
// 2010-05-01 - 8e2ee18 - lavf 52.62.0 - probe function
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(52, 62, 0)
  return av_probe_input_format2(pd, true, max_score);
#else
  AVInputFormat *result = NULL;
  
  for (AVInputFormat *fmt = av_iformat_next(NULL); fmt != NULL; fmt = av_iformat_next(fmt))
    // test only formats that are file-based and can detect the byte stream
    if (!(fmt->flags & AVFMT_NOFILE) && fmt->read_probe) {
         int score = fmt->read_probe(pd);
         if (score > *max_score) {
            *max_score = score;
            result = fmt;
         }
    }
    
  return result;
#endif
}

// Input format is probed twice, but compared to the expense of stream metadata extraction this isn't a huge deal.
// Unfortunately you can't save probe results in checkHeader because it's const
bool
FFMPEGEndAnalyzer::checkHeader(const char* header, int32_t headersize) const {
  
  // A workaround to let internal MP3, OGG and FLAC analyzers take priority
  if ((headersize>=64) && (
          (strncmp("ID3", header, 3) == 0 && ((unsigned char)header[3]) <= 4 && header[5] == 0)
       || ((readLittleEndianUInt32(header) == 0x43614c66) && ((readLittleEndianUInt32(header+4) & 0xFFFFFF7F) == 0x22000000))
       || (!strcmp("OggS", header) && !strcmp("vorbis", header+29) && !strcmp("OggS", header+58))) )
    return false;
  
  char* data_buffer = (char*)malloc(headersize + AVPROBE_PADDING_SIZE);
  if (!data_buffer)
    return false;

  std::memcpy(data_buffer, header, headersize);
  std::memset(data_buffer + headersize, 0, AVPROBE_PADDING_SIZE);

  AVProbeData pd;
  pd.buf = (unsigned char*)data_buffer;
  pd.buf_size = headersize;
  pd.filename = "";
// 2014-07-29 - 80a3a66 / 3a19405 - lavf 56.01.100 / 56.01.0 - avformat.h
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(56, 1, 100)
  pd.mime_type = "";
#endif
  int max_score = 0;
  probe_format(&pd, &max_score);

  free(data_buffer);

  //slog("Detection score:"<<max_score);
  // Most of formats return either 100 or nothing
  // MPG, however, can go as low as 25 while still being a real video
  return max_score >=25;
}

/*FIXME
make it produce the same data in stream and file mode when possible.
stream duration,size and bitrate reporting seem to be mostly nonexistent in ffmpeg
handle subtitles
*/

int read_data(void *opaque, uint8_t *buf, int buf_size) {
  //std::cout<<"READ";
  InputStream *s = (InputStream *) opaque;
  if (!s)
    return -1;

  const char *sbuf;
  //std::cout<<s->position()<<" "<<flush;
  int32_t len = s->read(sbuf, buf_size, buf_size);
  // std::cout<<s->position()<<" "<<buf_size<<" "<<len<<" "<<s->size()<<flush;
  if (len>0)
    memcpy( buf, sbuf, len);
  //std::cout<<" OK\n"<<flush;
  return len;
}

int64_t seek_data(void *opaque, int64_t offset, int whence) {
  InputStream *s = (InputStream *) opaque;
  int64_t target = -1;
  int64_t size;
  
  //std::cout<<"SEEK"<<offset<<" "<<whence<<"\n"<<flush;
  
  if ( whence== SEEK_SET) {
    target = offset;
  } else if ( whence == SEEK_CUR ) {
    target = s->position() + offset;
  } else if ( (whence == SEEK_END) && (size = s->size()>=0) ) {
    target = size+offset;
  } else if ( whence == AVSEEK_SIZE ) {
    return s->size();
  } else
    return -1;

  int64_t t= s->reset(target);
  //std::cout<<t<<"\n"<<flush;
  return (t == target ? target : -1);
}

int64_t const no_bitrate = 0x8000000000000000ULL;

signed char
FFMPEGEndAnalyzer::analyze(AnalysisResult& ar, ::InputStream* in) {
  uint8_t pDataBuffer[32768];//65536];
  long lSize = 32768;

#if (LIBAVUTIL_VERSION_MAJOR < 51)
  ByteIOContext ByteIOCtx;
  if(init_put_byte(&ByteIOCtx, pDataBuffer, lSize, 0, in, read_data, NULL, seek_data) < 0)
    return -1;

  //pAVInputFormat->flags |= AVFMT_NOFILE;
  ByteIOCtx.is_streamed = 0;

#else
  AVFormatContext *fc = avformat_alloc_context();
  fc->pb = avio_alloc_context(pDataBuffer, lSize, 0, in, read_data, NULL, seek_data);
  if(!fc || !(fc->pb))
    return -1;
#endif
  AVProbeData pd;
  const char *buf;
  const int buff_size = in->read(buf,262144,262144) - AVPROBE_PADDING_SIZE;
  if (buff_size <= 0) {
    return -1;
  }
  pd.filename ="";
  pd.buf_size = buff_size;
  pd.buf = (unsigned char*)buf;
// 2014-07-29 - 80a3a66 / 3a19405 - lavf 56.01.100 / 56.01.0 - avformat.h
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(56, 1, 100)
  pd.mime_type = "";
#endif
  in->reset(0);

  int score = 0;
  AVInputFormat* fmt = probe_format(&pd, &score);
  if(fmt == NULL)
    return 1;

#if (LIBAVUTIL_VERSION_MAJOR < 51)
  AVFormatContext *fc = NULL;
  if(av_open_input_stream(&fc, &ByteIOCtx, "", fmt, NULL) < 0)
    return -1;

  av_find_stream_info(fc);
  
  // Dump information about file onto standard error
  dump_format(fc, 0, ar.path().c_str(), false);

#else
  if(avformat_open_input(&fc, "", fmt, NULL) < 0)
    return -1;

  avformat_find_stream_info(fc,NULL);
  // Dump information about file onto standard error
  av_dump_format(fc, 0, ar.path().c_str(), false);
#endif

  if(fc->bit_rate)
    ar.addValue(factory->bitrateProperty, (uint32_t)fc->bit_rate);
  else if (fc->duration!= no_bitrate && fc->duration > 0) {
    std::cout<<"Trying to estimate bitrate\n";
    int64_t size;
    if ((size = in->size()) >= 0)
      ar.addValue(factory->bitrateProperty, (uint32_t)((size/(fc->duration/AV_TIME_BASE))*8) );
  }
  if(fc->duration!= no_bitrate)
    ar.addValue(factory->durationProperty, (uint32_t)(fc->duration / AV_TIME_BASE));
  else if(fc->bit_rate) {
    std::cout<<"Trying to estimate duration\n";
    int64_t size;
    if ((size = in->size()) >= 0)
      ar.addValue(factory->durationProperty, (uint32_t)(size/(fc->bit_rate/8)));
  }
// 2016-04-11 - 6f69f7a / 9200514 - lavf 57.33.100 / 57.5.0 - avformat.h
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(57, 33, 100)
  if(fc->nb_streams==1 && fc->streams[0]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
#else
  if(fc->nb_streams==1 && fc->streams[0]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
#endif
    ar.addValue(factory->typeProperty, STRIGI_NFO "Audio");
    ar.addValue(factory->typeProperty, STRIGI_NMM_DRAFT "MusicPiece");
  } else {
    ar.addValue(factory->typeProperty, STRIGI_NFO "Video");
  }

  for(uint32_t i=0; i<fc->nb_streams; i++) {
    const AVStream &stream = *fc->streams[i];
// 2016-04-11 - 6f69f7a / 9200514 - lavf 57.33.100 / 57.5.0 - avformat.h
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(57, 33, 100)
    const AVCodecParameters &codec = *stream.codecpar;
#else
    const AVCodecContext &codec = *stream.codec;
#endif

    if (codec.codec_type == AVMEDIA_TYPE_AUDIO || codec.codec_type == AVMEDIA_TYPE_VIDEO) {
      const std::string streamuri = ar.newAnonymousUri();
      ar.addValue(factory->hasPartProperty, streamuri);
      ar.addTriplet(streamuri, partOfPropertyName, ar.path());
      ar.addTriplet(streamuri, typePropertyName, embeddedClassName);
      
      if ((stream.duration != no_bitrate) && stream.time_base.num && stream.time_base.den) {
        std::ostringstream outs;
        outs << (stream.duration * stream.time_base.num / stream.time_base.den);
        ar.addTriplet(streamuri, durationPropertyName,outs.str());
      }

#if (LIBAVUTIL_VERSION_MAJOR < 51)
      //FIXME we must stop using the deprecated fuction av_metadata_get and use
      // av_dict_get once we are able to detect the version of FFMpeg being used
      // using version macros. same goes for all occurences of this function.
      AVMetadataTag *entry = av_metadata_get(stream.metadata, "language", NULL, 0);
#else
      AVDictionaryEntry *entry = av_dict_get(stream.metadata, "language", NULL, 0);
#endif
      if (entry != NULL) {
        const char *languageValue = entry->value;
        if (size_t len = strlen(languageValue)) {
          ar.addTriplet(streamuri, languagePropertyName, std::string(languageValue, len));
        }
      }
      const AVCodec *p = avcodec_find_decoder(codec.codec_id);
#if (LIBAVUTIL_VERSION_MAJOR >= 55 && LIBAVCODEC_VERSION_MINOR >= 11)
      const char *codecName = avcodec_get_name(codec.codec_id);
#else
      const char *codecName = codec.codec_name;
#endif
      if (p) {
        if (size_t len = strlen(p->name)) {
          ar.addTriplet(streamuri, codecPropertyName, std::string(p->name, len));
        }
      } else if (size_t len = strlen(codecName)) {
        ar.addTriplet(streamuri, codecPropertyName, std::string(codecName, len));
      }

      if (codec.bit_rate) {
        std::ostringstream outs;
        outs << codec.bit_rate;
        ar.addTriplet(streamuri, bitratePropertyName, outs.str());
      }

      if (codec.codec_type == AVMEDIA_TYPE_AUDIO) {
        
        ar.addTriplet(streamuri, typePropertyName, audioClassName);
        if (codec.channels) {
          std::ostringstream outs;
          outs << codec.channels;
          ar.addTriplet(streamuri, channelsPropertyName, outs.str());
        }
        if (codec.sample_rate) {
          std::ostringstream outs;
          outs << codec.sample_rate;
          ar.addTriplet(streamuri, sampleratePropertyName, outs.str());
        }
      } else { // video stream
        
        ar.addTriplet(streamuri, typePropertyName, videoClassName);
        if (codec.width) {
          std::ostringstream outs;
          outs << codec.width;
          ar.addTriplet(streamuri, widthPropertyName, outs.str());
          if (codec.sample_aspect_ratio.num) {
            AVRational aspectratio;
            std::ostringstream outs;
            av_reduce(&aspectratio.num, &aspectratio.den,
                      int64_t(codec.width) * int64_t(codec.sample_aspect_ratio.num),
                      int64_t(codec.height) * int64_t(codec.sample_aspect_ratio.den),
                      1024*1024);
            std::ostringstream aspect;
            aspect << roundf(100.0*float(aspectratio.num)/float(aspectratio.den))/100.0;
            ar.addTriplet(streamuri, aspectRatioPropertyName, aspect.str());
          }
        }
        if (codec.height) {
          std::ostringstream outs;
          outs << codec.height;
          ar.addTriplet(streamuri, heightPropertyName, outs.str());
        }
        if (stream.r_frame_rate.num && stream.r_frame_rate.den) {
          std::ostringstream outs;
          outs << stream.r_frame_rate.num / stream.r_frame_rate.den;
          ar.addTriplet(streamuri, frameRatePropertyName, outs.str());
        }
      }
      
    }
  }

  // Tags

#if (LIBAVUTIL_VERSION_MAJOR < 51)
  AVMetadataTag *entry = av_metadata_get(fc->metadata, "title", NULL, 0);
#else
  AVDictionaryEntry *entry = av_dict_get(fc->metadata, "title", NULL, 0);
#endif
  if (entry != NULL)
  {
    const char *titleValue = entry->value;
    if (int32_t len = strlen(titleValue)) {
      ar.addValue(factory->titleProperty, std::string(titleValue, len) );
    }
  }

#if (LIBAVUTIL_VERSION_MAJOR < 51)
  entry = av_metadata_get(fc->metadata, "author", NULL, 0);
#else
  entry = av_dict_get(fc->metadata, "author", NULL, 0);
#endif
  if (entry != NULL)
  {
    const char *authorValue = entry->value;
    if (int32_t len = strlen(authorValue)) {
      const std::string creatoruri = ar.newAnonymousUri();
      ar.addValue(factory->creatorProperty, creatoruri);
      ar.addTriplet(creatoruri, typePropertyName, contactClassName);
      ar.addTriplet(creatoruri, fullnamePropertyName, std::string(authorValue, len) );
    }
  }

#if (LIBAVUTIL_VERSION_MAJOR < 51)
  entry = av_metadata_get(fc->metadata, "copyright", NULL, 0);
#else
  entry = av_dict_get(fc->metadata, "copyright", NULL, 0);
#endif
  if (entry != NULL)
  {
    const char *copyrightValue = entry->value;
    if (int32_t len = strlen(copyrightValue)) {
      ar.addValue(factory->copyrightProperty, std::string(copyrightValue, len) );
    }
  }

#if (LIBAVUTIL_VERSION_MAJOR < 51)
  entry = av_metadata_get(fc->metadata, "comment", NULL, 0);
#else
  entry = av_dict_get(fc->metadata, "comment", NULL, 0);
#endif
  if (entry != NULL)
  {
    const char *commentValue = entry->value;
    if (int32_t len = strlen(commentValue)) {
      ar.addValue(factory->commentProperty, std::string(commentValue, len) );
    }
  }

#if (LIBAVUTIL_VERSION_MAJOR < 51)
  entry = av_metadata_get(fc->metadata, "album", NULL, 0);
#else
  entry = av_dict_get(fc->metadata, "album", NULL, 0);
#endif
  if (entry != NULL)
  {
    const char *albumValue = entry->value;
    if (int32_t len = strlen(albumValue)) {
      const std::string album = ar.newAnonymousUri();
      ar.addValue(factory->albumProperty, album);
    ar.addTriplet(album, typePropertyName, albumClassName);
    ar.addTriplet(album, titlePropertyName, std::string(albumValue, len) );
    }
  }

#if (LIBAVUTIL_VERSION_MAJOR < 51)
  entry = av_metadata_get(fc->metadata, "genre", NULL, 0);
#else
  entry = av_dict_get(fc->metadata, "genre", NULL, 0);
#endif
  if (entry != NULL)
  {
    const char *genreValue = entry->value;
    if (int32_t len = strlen(genreValue)) {
      ar.addValue(factory->genreProperty, std::string(genreValue, len) );
    }
  }

#if (LIBAVUTIL_VERSION_MAJOR < 51)
  entry = av_metadata_get(fc->metadata, "track", NULL, 0);
#else
  entry = av_dict_get(fc->metadata, "track", NULL, 0);
#endif
  if (entry != NULL)
  {
    const char *trackValue = entry->value;
    if (int32_t len = strlen(trackValue)) {
        //Deal with things like '1/12'
        std::string str = std::string(trackValue,len);
        std::string sub = str.substr(0,str.find('/'));
      ar.addValue(factory->trackProperty, sub );
    }
  }

#if (LIBAVUTIL_VERSION_MAJOR < 51)
  entry = av_metadata_get(fc->metadata, "year", NULL, 0);
#else
  entry = av_dict_get(fc->metadata, "year", NULL, 0);
#endif
  if (entry != NULL)
  {
    const char *yearValue = entry->value;
    if (int32_t len = strlen(yearValue)) {
      ar.addValue(factory->createdProperty, std::string(yearValue, len) );
    }
  }

#if (LIBAVUTIL_VERSION_MAJOR < 51)
  av_close_input_stream(fc);
#else
  avformat_close_input(&fc);
#endif
  //url_fclose(&ByteIOCtx);
  
  return 0;
}

/*
 For plugins, we need to have a way to find out which plugins are defined in a
 plugin. One instance of AnalyzerFactoryFactory per plugin profides this
 information.
*/
class Factory : public AnalyzerFactoryFactory {
public:
    std::list<StreamEndAnalyzerFactory*>
    streamEndAnalyzerFactories() const {
        std::list<StreamEndAnalyzerFactory*> af;
        af.push_back(new FFMPEGEndAnalyzerFactory());
        return af;
    }
};

/*
 Register the AnalyzerFactoryFactory
*/
STRIGI_ANALYZER_FACTORY(Factory)
