//
// Created by Neo on 2021/7/10.
//

#ifndef XXPLAYER_XX_PLAY_H
#define XXPLAYER_XX_PLAY_H

#include <jni.h>
#include <queue>
#include "xx_media_channel.h"
#include "java_call.h"

#define XXP_PLAY_STATUS_UN_CREATED            -3
#define XXP_PLAY_STATUS_CREATED               -2
#define XXP_PLAY_STATUS_STOP                  -1
#define XXP_PLAY_STATUS_PAUSE                 0
#define XXP_PLAY_STATUS_PLAY                  1
#define XXP_PLAY_STATUS_SEEK                  2

#define VIDEO_RENDER_STATUS_NO_SURFACE                0
#define VIDEO_RENDER_STATUS_SURFACE_ATTACHED          1
#define VIDEO_RENDER_STATUS_CREATED                   2
#define VIDEO_RENDER_STATUS_RELEASED                  3


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

class XXPlay {
private:

    std::shared_ptr<JavaCall> _java_call;
public:
    XXPlay(std::shared_ptr<JavaCall> javaCall);

    ~XXPlay();

    std::shared_ptr<JavaCall> getJavaCall();

    int getAudioPacketSize();

    int getVideoPacketSize();

    int putAudioPacket(AVPacket *packet);

    int putVideoPacket(AVPacket *packet);

    int getAudioPacket(AVPacket *packet);

    int getVideoPacket(AVPacket *packet);

    int getAudioFrame(AVFrame *frame);

    int getVideoFrame(AVFrame *frame);

    void clearAudioPacket();

    void clearVideoPacket();

    int getAudioFrameSize();

    int getVideoFrameSize();

    int putAudioFrame(AVFrame *packet);

    int putVideoFrame(AVFrame *packet);

    void clearAudioFrame();

    void clearVideoFrame();

    void clearVideoMediaChannel();

    void clearAudioMediaChannel();

    void release();

    int duration;

    AVFormatContext *_av_format_context;
    jobject _surface;
    int _mime_type = -1;
    bool media_codec = false;
    bool is_avi = false;
    std::deque<XXMediaChannel *> _audio_channels;
    std::deque<XXMediaChannel *> _video_channels;

    std::queue<AVPacket *> audio_packet_queue;
    pthread_mutex_t audio_packet_mutex;
    pthread_cond_t audio_packet_cond;

    std::queue<AVPacket *> video_packet_queue;
    pthread_mutex_t video_packet_mutex;
    pthread_cond_t video_packet_cond;

    std::queue<AVFrame *> audio_frame_queue;
    pthread_mutex_t audio_frame_mutex;
    pthread_cond_t audio_frame_cond;

    std::queue<AVFrame *> video_frame_queue;
    pthread_mutex_t video_frame_mutex;
    pthread_cond_t video_frame_cond;

    AVCodecContext *audio_codec_context = nullptr;
    AVCodecContext *video_codec_context = nullptr;

    int _play_status = XXP_PLAY_STATUS_UN_CREATED;
    int _video_render_status = VIDEO_RENDER_STATUS_NO_SURFACE;
    int _audio_stream_index = 0;
    int _video_stream_index = 0;
    double clock = -1;
    double video_clock = -1;
    int delay_time = 0;
    double video_rate = 0;
    AVRational audio_time_base;
    AVRational video_time_base;
    bool audio_frame_end = false;
    bool video_frame_end = false;
    bool extractor_end = false;
    AVCodecParameters *parameters;
};


#endif //XXPLAYER_XX_PLAY_H
