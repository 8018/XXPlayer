//
// Created by Neo on 2021/7/9.
//

#ifndef XXPLAYER_BASE_RENDER_H
#define XXPLAYER_BASE_RENDER_H

#include <cstdint>
#include "xx_play.h"

extern "C"
{
#include "includes/ffmpeg/libavcodec/avcodec.h"
}

class BaseRender {
public:
    virtual ~BaseRender() {
    };

    virtual int initRender() = 0;

    virtual void releaseRender() = 0;

    virtual void start() = 0;

    virtual void stop() = 0;

    virtual void pause() = 0;

    virtual void resume() = 0;

    std::shared_ptr<XXPlay> _xx_play;
    AVCodecContext *_av_codec_context;
};


#endif //XXPLAYER_BASE_RENDER_H
