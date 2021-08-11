//
// Created by Neo on 2021/7/9.
//

#include <unistd.h>
#include "ffmpeg_audio_decoder.h"
#include "LogUtil.h"

extern "C" {
#include <libavutil/time.h>
}

void *audioDecodeThread(void *data) {
    auto *audioDecoder = (FFMpegAudioDecoder *) data;
    audioDecoder->decode();
    pthread_exit(&audioDecoder->_decode_thread);
}

FFMpegAudioDecoder::FFMpegAudioDecoder() {
    pthread_mutex_init(&_decode_mutex, NULL);
}

FFMpegAudioDecoder::~FFMpegAudioDecoder() {
}

void FFMpegAudioDecoder::start() {
    pthread_create(&_decode_thread, NULL, audioDecodeThread, this);
}

void FFMpegAudioDecoder::decode() {

    while (_xx_play->_play_status != XXP_PLAY_STATUS_STOP) {
        int ret = -1;
        if (_xx_play->_play_status == XXP_PLAY_STATUS_PAUSE ) {
            av_usleep(1000 * 100);
            continue;
        }
        if (_xx_play->_play_status == XXP_PLAY_STATUS_SEEK) {
            av_usleep(1000 * 10);
            continue;
        }

        if (_xx_play->getAudioFrameSize() >= 30) {
            av_usleep(50 * 1000);
            continue;
        }

        while (1) {
            AVFrame *frame = av_frame_alloc();
            ret = avcodec_receive_frame(_av_codec_context, frame);

            if (ret == AVERROR_EOF) {
                avcodec_flush_buffers(_av_codec_context);
                av_frame_free(&frame);
                av_free(frame);
                frame = NULL;
                LOGE("FFMpegAudioDecoder", "end of file")
                _xx_play->audio_frame_end = true;
                return;
            } else if (ret == 0) {
                // 设置通道数或channel_layout
                if (frame->channels > 0 && frame->channel_layout == 0)
                    frame->channel_layout = av_get_default_channel_layout(frame->channels);
                else if (frame->channels == 0 && frame->channel_layout > 0)
                    frame->channels = av_get_channel_layout_nb_channels(frame->channel_layout);
                _xx_play->putAudioFrame(frame);

            } else {
                av_frame_free(&frame);
                av_free(frame);
                frame = NULL;
                break;
            }
        }

        AVPacket *packet = av_packet_alloc();
        int result = _xx_play->getAudioPacket(packet);

        if (result < 0) {
            if (ret == -11) {
                avcodec_send_packet(_av_codec_context, packet);

            }
            av_packet_free(&packet);
            av_free(packet);
            packet = NULL;
            continue;
        }
        avcodec_send_packet(_av_codec_context, packet);

        av_packet_free(&packet);
        av_free(packet);
        packet = NULL;
    }
}

void FFMpegAudioDecoder::initDecoder() {}

void FFMpegAudioDecoder::releaseDecoder() {
    release();
}

void FFMpegAudioDecoder::stop() {}

void FFMpegAudioDecoder::pause() {}

void FFMpegAudioDecoder::release() {
    if(_decode_thread != 0){
        pthread_join(_decode_thread, NULL);
    }

    if (_av_codec_context != nullptr){
        avcodec_free_context(&_av_codec_context);
        _av_codec_context = nullptr;
    }
}