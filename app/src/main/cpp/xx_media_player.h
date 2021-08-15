//
// Created by Neo on 2021/7/9.
//

#ifndef XXPLAYER_XX_MEDIA_PLAYER_H
#define XXPLAYER_XX_MEDIA_PLAYER_H

#include <jni.h>
#include <pthread.h>
#include <string>
#include "xx_play.h"
#include "xx_media_extractor.h"
#include "base_render.h"
#include "base_decoder.h"

extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/error.h"
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
#include "libavutil/frame.h"
#include "libavcodec/packet.h"
#include "libswscale/swscale.h"
#include "libavcodec/packet.h"
}

class XXMediaPlayer {

private:
    const char *_path = nullptr;
    pthread_mutex_t _init_mutex;
    std::shared_ptr<JavaCall> _javaCall;

public:
    XXMediaPlayer(JavaVM *javaVm, JNIEnv *main_env, jobject media_player_java_ref,jobject xx_media_codec);

    ~XXMediaPlayer();

    int prepare();

    std::shared_ptr<JavaCall> getJavaCall();

    void onPause();

    void onStart();

    void setDataSource(const char *datasource);

    void onStop();

    void onResume();

    void setSurface(jobject surface);

    int release();

    int getMimeType(const char *codecName);

    void setVideoChannel(int id);

    void setAudioChannel(int id);

    int getAvCodecContext(AVCodecParameters *parameters, int codec_type);

    std::shared_ptr<XXMediaExtractor> _media_extractor;
    std::shared_ptr<BaseRender> _audio_render;
    std::shared_ptr<BaseRender> _video_render;
    std::shared_ptr<BaseDecoder> _audio_decoder;
    std::shared_ptr<BaseDecoder> _video_decoder;
    std::shared_ptr<XXPlay> _xx_play = nullptr;
    bool isPrepared = false;
};


#endif //XXPLAYER_XX_MEDIA_PLAYER_H
