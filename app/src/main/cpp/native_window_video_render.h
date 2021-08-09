//
// Created by Neo on 2021/7/9.
//

#ifndef XXPLAYER_NATIVE_WINDOW_VIDEO_RENDER_H
#define XXPLAYER_NATIVE_WINDOW_VIDEO_RENDER_H


#include "base_render.h"
#include <pthread.h>

#include <android/native_window.h>
#include <android/native_window_jni.h>

class NativeWindowVideoRender : public BaseRender {
public:
    char *const TAG = "NativeWindowVideoRender";

    NativeWindowVideoRender();

    ~NativeWindowVideoRender();

    void start() override;

    void pause() override;

    void resume() override;

    void stop() override;

    void startRender();
    void release();

    int initRender() override;

    void releaseRender() override;

    double getOffsetPts(AVFrame *srcFrame, double pts);

    double getDelayTime(double diff);

    pthread_t _video_render_thread;

    uint8_t *out_buffer = nullptr;
    // 本地窗口
    ANativeWindow *m_native_window = nullptr;
    SwsContext *m_sws_ctx = nullptr;
    AVFrame *m_rgb_frame = nullptr;

    uint8_t *dst;
    uint8_t *src;
};


#endif //XXPLAYER_NATIVE_WINDOW_VIDEO_RENDER_H
