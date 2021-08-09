//
// Created by Neo on 2021/7/11.
//

#ifndef XXPLAYER_OPENSL_AUDIO_RENDER_H
#define XXPLAYER_OPENSL_AUDIO_RENDER_H

#include "base_render.h"
#include "xx_play.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

extern "C"
{
#include "includes/ffmpeg/libswresample/swresample.h"
#include "includes/ffmpeg/libavutil/time.h"
#include "includes/ffmpeg/libavcodec/avcodec.h"
#include "includes/ffmpeg/libswscale/swscale.h"
}

class OpenSLAudioRender : public BaseRender {
private:
    int out_sample_size = 0;
    int out_channels = 0;
    int out_buffer_size = 0;

    //引擎
    SLObjectItf engineObject = 0;
    //引擎接口
    SLEngineItf engineInterface = 0;
    //混音器
    SLObjectItf outputMixObject = 0;
    //播放器
    SLObjectItf bqPlayerObject = 0;
    //播放器接口
    SLPlayItf bqPlayerPlay = 0;

    //缓冲器队列接口
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;

    SwrContext *swr_ctx = 0;

public:

    OpenSLAudioRender();

    ~OpenSLAudioRender();

    void start() override;

    void pause() override;

    void resume() override;

    void release();

    void stop() override;

    void startRender();

    int initRender() override;

    void releaseRender() override;

    int getPcmData();

    void *buffer = NULL;

    int pcm_size = 0;
    int out_sample_rate = 44100;
    bool isExit = false;
    pthread_t _audio_render_thread;
    uint8_t *out_buffer = 0;
    //播放器队列接口
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue = 0;
};


#endif //XXPLAYER_OPENSL_AUDIO_RENDER_H
