//
// Created by Neo on 2021/7/10.
//

#ifndef XXPLAYER_BASE_DECODER_H
#define XXPLAYER_BASE_DECODER_H

#include "xx_play.h"

extern "C" {
#include "includes/ffmpeg/libavcodec/avcodec.h"
#include "includes/ffmpeg/libavutil/rational.h"
}

class BaseDecoder {
public:
    virtual ~BaseDecoder() {};

    virtual void initDecoder() = 0;

    virtual void releaseDecoder() = 0;

    virtual void start() = 0;

    virtual void stop() = 0;

    virtual void pause() = 0;

    virtual void decode() = 0;

    AVCodecContext *_av_codec_context = nullptr;
    std::shared_ptr<XXPlay> _xx_play;
    pthread_mutex_t _decode_mutex;
    pthread_t _decode_thread;
};


#endif //XXPLAYER_BASE_DECODER_H
