//
// Created by Neo on 2021/7/9.
//

#include "ffmpeg_video_decoder.h"
#include "LogUtil.h"

extern "C" {
#include <libavutil/time.h>
}

void *videoDecodeThread(void *data) {
    auto *videoDecoder = (FFMpegVideoDecoder *) data;
    videoDecoder->decode();
    pthread_exit(&videoDecoder->_decode_thread);
}

FFMpegVideoDecoder::FFMpegVideoDecoder() {
    pthread_mutex_init(&_decode_mutex, NULL);
}

FFMpegVideoDecoder::~FFMpegVideoDecoder() {
    release();
}

void FFMpegVideoDecoder::start() {
    pthread_create(&_decode_thread, NULL, videoDecodeThread, this);
}

void FFMpegVideoDecoder::decode() {
    int ret = -1;
    while (_xx_play->_play_status != XXP_PLAY_STATUS_STOP) {

        if (_xx_play->_play_status == XXP_PLAY_STATUS_PAUSE ||
            _xx_play->_play_status == XXP_PLAY_STATUS_NO_SURFACE) {
            av_usleep(1000 * 100);
            continue;
        }

        if (_xx_play->_play_status == XXP_PLAY_STATUS_SEEK) {
            continue;
        }

        if (_xx_play->getVideoFrameSize() >= 30) {
            av_usleep(10 * 1000);
            continue;
        }

        while (1) {
            AVFrame *frame = av_frame_alloc();
            ret = avcodec_receive_frame(_av_codec_context, frame);

            if (ret == AVERROR_EOF) {
                av_frame_free(&frame);
                av_free(frame);
                frame = NULL;
                avcodec_flush_buffers(_av_codec_context);
                _xx_play->video_frame_end = true;
                LOGE("FFMpegVideoDecoder", "end of file")
                return;
            } else if (ret == 0) {

                _xx_play->putVideoFrame(frame);
            } else {
                av_frame_free(&frame);
                av_free(frame);
                frame = NULL;
                break;
            }

        }

        AVPacket *packet = av_packet_alloc();
        int result = _xx_play->getVideoPacket(packet);

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

void FFMpegVideoDecoder::initDecoder() {}

void FFMpegVideoDecoder::releaseDecoder() {
    release();
}

void FFMpegVideoDecoder::stop() {}

void FFMpegVideoDecoder::pause() {}

void FFMpegVideoDecoder::release() {
    if (_av_codec_context != nullptr) {
        avcodec_free_context(&_av_codec_context);
        _av_codec_context = nullptr;
    }
}


