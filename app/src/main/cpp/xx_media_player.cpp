//
// Created by Neo on 2021/7/9.
//

#include <filesystem>
#include "xx_media_player.h"
#include "ffmpeg_audio_decoder.h"
#include "ffmpeg_video_decoder.h"
#include "opensl_audio_render.h"
#include "native_window_video_render.h"
#include "xx_msg.h"
#include "LogUtil.h"

XXMediaPlayer::XXMediaPlayer(JavaVM *javaVm, JNIEnv *main_env, jobject media_player_java_ref) {
    pthread_mutex_init(&_init_mutex, nullptr);

    _javaCall = std::shared_ptr<JavaCall>(new JavaCall(javaVm, main_env, media_player_java_ref));
    _xx_play = std::shared_ptr<XXPlay>(new XXPlay(_javaCall));
    _xx_play->_play_status = XXP_PLAY_STATUS_CREATED;
}


std::shared_ptr<JavaCall> XXMediaPlayer::getJavaCall() {
    return _javaCall;
}

void XXMediaPlayer::setSurface(jobject surface) {
    pthread_mutex_lock(&_init_mutex);
    _xx_play->_surface = surface;

    if (!isPrepared) {
        return;
    }

    if (!surface) {
        _xx_play->_video_render_status = VIDEO_RENDER_STATUS_NO_SURFACE;
        _video_render->releaseRender();
    } else {
        _xx_play->_video_render_status = VIDEO_RENDER_STATUS_SURFACE_ATTACHED;
        _video_render->start();
    }
    pthread_mutex_unlock(&_init_mutex);
}

void XXMediaPlayer::setDataSource(const char *datasource) {
    _path = datasource;
}

int XXMediaPlayer::prepare() {

    pthread_mutex_lock(&_init_mutex);

    avformat_network_init(nullptr);

    AVFormatContext *pFormatCtx = avformat_alloc_context();

    if (pFormatCtx == nullptr) {
        return -1;
    }

    if (avformat_open_input(&pFormatCtx, _path, nullptr, nullptr) != 0) {
        pthread_mutex_unlock(&_init_mutex);
        return -1;
    }

    if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
        pthread_mutex_unlock(&_init_mutex);
        return -1;
    }

    _xx_play->duration = pFormatCtx->duration / 1000000;
    _xx_play->_av_format_context = pFormatCtx;

    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            XXMediaChannel *wl = new XXMediaChannel(i, pFormatCtx->streams[i]->time_base);
            _xx_play->_audio_channels.push_front(wl);
        } else if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            int num = pFormatCtx->streams[i]->avg_frame_rate.num;
            int den = pFormatCtx->streams[i]->avg_frame_rate.den;
            if (num != 0 && den != 0) {
                int fps = pFormatCtx->streams[i]->avg_frame_rate.num /
                          pFormatCtx->streams[i]->avg_frame_rate.den;
                XXMediaChannel *vl = new XXMediaChannel(i, pFormatCtx->streams[i]->time_base,
                                                        fps);
                _xx_play->_video_channels.push_front(vl);
            }
        }
    }

    if (_xx_play->_audio_channels.size() > 0) {
        _audio_decoder = std::shared_ptr<BaseDecoder>(new FFMpegAudioDecoder());
        _audio_decoder->_xx_play = _xx_play;
        setAudioChannel(0);
        if (_xx_play->_audio_stream_index >= 0 &&
            _xx_play->_audio_stream_index < pFormatCtx->nb_streams) {
            if (getAvCodecContext(
                    pFormatCtx->streams[_xx_play->_audio_stream_index]->codecpar,
                    _audio_decoder.get()) !=
                0) {
                pthread_mutex_unlock(&_init_mutex);
                return -1;
            }
        }
    }

    if (_xx_play->_video_channels.size() > 0) {
        _video_decoder = std::shared_ptr<BaseDecoder>(new FFMpegVideoDecoder());
        _video_decoder->_xx_play = _xx_play;
        setVideoChannel(0);
        if (_xx_play->_video_stream_index >= 0 &&
            _xx_play->_video_stream_index < pFormatCtx->nb_streams) {
            if (getAvCodecContext(
                    pFormatCtx->streams[_xx_play->_video_stream_index]->codecpar,
                    _video_decoder.get()) !=
                0) {
                pthread_mutex_unlock(&_init_mutex);
                return -1;
            }
        }
    }

    if (_audio_decoder == nullptr && _video_decoder == nullptr) {
        pthread_mutex_unlock(&_init_mutex);
        return -1;;
    }

    if (_audio_decoder != nullptr) {
        _xx_play->duration = pFormatCtx->duration / 1000000;
    }

    if (_video_decoder != nullptr) {
        _xx_play->_mime_type = getMimeType(_video_decoder->_av_codec_context->codec->name);
        _xx_play->duration = pFormatCtx->duration / 1000000;
    }

    _media_extractor = std::shared_ptr<XXMediaExtractor>(new XXMediaExtractor());
    _media_extractor->_xx_play = _xx_play;

    _audio_render = std::shared_ptr<BaseRender>(new OpenSLAudioRender());
    _audio_render->_xx_play = _xx_play;
    _audio_render->_av_codec_context = _audio_decoder->_av_codec_context;

    _video_render = std::shared_ptr<BaseRender>(new NativeWindowVideoRender());
    _video_render->_xx_play = _xx_play;
    _xx_play->_video_render_status = VIDEO_RENDER_STATUS_CREATED;
    _video_render->_av_codec_context = _video_decoder->_av_codec_context;

    _media_extractor->start();
    isPrepared = true;
    pthread_mutex_unlock(&_init_mutex);
    return 0;
}

void XXMediaPlayer::onStart() {
    if (!isPrepared) {
        return;
    }

    if (_xx_play->_play_status == XXP_PLAY_STATUS_CREATED) {
        _xx_play->_play_status = XXP_PLAY_STATUS_PLAY;

        _video_decoder->start();
        _audio_decoder->start();

        _audio_render->initRender();
        _audio_render->start();
        //_video_render->start();
        return;
    }

    if (_xx_play->_play_status == XXP_PLAY_STATUS_PAUSE) {
        _xx_play->_play_status = XXP_PLAY_STATUS_PLAY;
        _audio_render->resume();
        return;
    }
}

void XXMediaPlayer::onPause() {
    _xx_play->_play_status = XXP_PLAY_STATUS_PAUSE;
    if (_audio_render != nullptr) {
        _audio_render->pause();
    }
}

void XXMediaPlayer::onStop() {
    _xx_play->_play_status = XXP_PLAY_STATUS_STOP;
    if (_audio_render != nullptr) {
        _audio_render->stop();
    }
}

void XXMediaPlayer::onResume() {
}

int XXMediaPlayer::getAvCodecContext(AVCodecParameters *parameters, BaseDecoder *baseDecoder) {

    AVCodec *dec = avcodec_find_decoder(parameters->codec_id);
    if (!dec) {
        return -1;
    }
    baseDecoder->_av_codec_context = avcodec_alloc_context3(dec);
    if (!baseDecoder->_av_codec_context) {
        return -1;;
    }
    if (avcodec_parameters_to_context(baseDecoder->_av_codec_context, parameters) != 0) {
        return -1;;
    }
    if (avcodec_open2(baseDecoder->_av_codec_context, dec, 0) != 0) {
        return -1;;
    }
    return 0;
}

void XXMediaPlayer::setAudioChannel(int index) {
    if (_audio_decoder != nullptr) {
        int channel_size = _xx_play->_audio_channels.size();
        if (index < channel_size) {
            for (int i = 0; i < channel_size; i++) {
                if (i == index) {
                    _xx_play->audio_time_base = _xx_play->_audio_channels.at(
                            i)->time_base;
                    _xx_play->_audio_stream_index = _xx_play->_audio_channels.at(
                            i)->channelId;
                }
            }
        }
    }

}

void XXMediaPlayer::setVideoChannel(int id) {
    if (_video_decoder != nullptr) {
        _xx_play->_video_stream_index = _xx_play->_video_channels.at(id)->channelId;
        _xx_play->video_time_base = _xx_play->_video_channels.at(id)->time_base;
        _xx_play->video_rate = 1000 / _xx_play->_video_channels.at(id)->fps;
    }
}

int XXMediaPlayer::getMimeType(const char *codecName) {

    if (strcmp(codecName, "h264") == 0) {
        return 1;
    }
    if (strcmp(codecName, "hevc") == 0) {
        return 2;
    }
    if (strcmp(codecName, "mpeg4") == 0) {
        _xx_play->is_avi = true;
        return 3;
    }
    if (strcmp(codecName, "wmv3") == 0) {
        _xx_play->is_avi = true;
        return 4;
    }

    return -1;
}


XXMediaPlayer::~XXMediaPlayer() {
}

int XXMediaPlayer::release() {

    if (_media_extractor != nullptr) {
        _media_extractor->release();
    }
    if (_audio_render != nullptr) {
        _audio_render->releaseRender();
    }
    if (_video_render != nullptr &&
        _xx_play->_video_render_status != VIDEO_RENDER_STATUS_RELEASED) {
        _video_render->releaseRender();
    }
    if (_audio_decoder != nullptr) {
        _audio_decoder->releaseDecoder();
    }
    if (_video_decoder != nullptr) {
        _video_decoder->releaseDecoder();
    }

    avformat_close_input(&_xx_play->_av_format_context);


    if (_xx_play != nullptr) {
        _xx_play->release();
    }

    return 0;
}