//
// Created by Neo on 2021/7/9.
//

#include "xx_media_extractor.h"
#include <stdint.h>
#include <unistd.h>
#include "jvm_android.h"
#include "LogUtil.h"
#include<pthread.h>

extern "C" {
#include <libavutil/time.h>
}

void *extractorPrepareThread(void *data) {
    auto *xxMediaExtractor = (XXMediaExtractor *) data;
    xxMediaExtractor->startExtractor();
    pthread_exit(&xxMediaExtractor->_extractor_thread);
}

XXMediaExtractor::XXMediaExtractor() {
    pthread_mutex_init(&_prepare_mutex, nullptr);
    pthread_mutex_init(&_seek_mutex, nullptr);
}

XXMediaExtractor::~XXMediaExtractor() {
}

void XXMediaExtractor::start() {
    pthread_create(&_extractor_thread, nullptr, extractorPrepareThread, this);
}

void XXMediaExtractor::startExtractor() {
    pthread_mutex_lock(&_prepare_mutex);

    AVBSFContext *bsf_ctx;
    AVBitStreamFilter *filter = nullptr;
    if (_xx_play->_mime_type == 1) {
        //1 找到对应的解码过滤器
        filter = const_cast<AVBitStreamFilter *>(av_bsf_get_by_name(
                "h264_mp4toannexb"));

    } else if (_xx_play->_mime_type == 2) {
        //1 找到对应的解码过滤器
        filter = const_cast<AVBitStreamFilter *>(av_bsf_get_by_name(
                "hevc_mp4toannexb"));
    }

    if (filter != nullptr) {
        //2 根据过滤器开辟上下文
        av_bsf_alloc(filter, &bsf_ctx);
        //3 添加解码器属性
        avcodec_parameters_copy(bsf_ctx->par_in, _xx_play->parameters);
        //4 初始化过滤器上下文
        av_bsf_init(bsf_ctx);
        bsf_ctx->time_base_in = _xx_play->video_codec_context->time_base;
    }

    while (_xx_play->_play_status != XXP_PLAY_STATUS_STOP) {
        if (_xx_play->_play_status == XXP_PLAY_STATUS_PAUSE) {
            av_usleep(1000 * 100);
            continue;
        }

        if (_xx_play != nullptr &&
            (_xx_play->getVideoPacketSize() + _xx_play->getAudioPacketSize() > 5 * 1024)) {
            av_usleep(1000 * 100);
            continue;
        }
        if (_xx_play->_play_status == XXP_PLAY_STATUS_STOP) {
            break;
        }
        AVPacket *packet = av_packet_alloc();
        int ret = av_read_frame(_xx_play->_av_format_context, packet);

        if (ret == AVERROR_EOF) {
            av_packet_free(&packet);
            av_free(packet);
            packet = nullptr;
            _xx_play->extractor_end = true;
            LOGE("XXMediaExtractor", "end of file")
            break;
        }

        if (ret == 0) {
            if (_xx_play != nullptr && packet->stream_index == _xx_play->_audio_stream_index) {
                _xx_play->putAudioPacket(packet);
            } else if (_xx_play != nullptr &&
                       packet->stream_index == _xx_play->_video_stream_index) {
                if (filter != NULL && !_xx_play->is_avi) {
                    int ret = 0;
                    ret = av_bsf_send_packet(bsf_ctx, packet);
                    if (ret < 0) {
                        av_packet_free(&packet);
                        av_free(packet);
                        packet = nullptr;
                        continue;
                    }
                    ret = av_bsf_receive_packet(bsf_ctx, packet);
                    if (ret < 0) {
                        av_packet_free(&packet);
                        av_free(packet);
                        packet = nullptr;
                        continue;
                    }
                }
                _xx_play->putVideoPacket(packet);

            } else {
                av_packet_free(&packet);
                av_free(packet);
                packet = nullptr;
            }
        } else {
            av_packet_free(&packet);
            av_free(packet);
            packet = nullptr;
            continue;
        }
    }

    if (filter != nullptr) {
        av_bsf_free(&bsf_ctx);
        bsf_ctx = nullptr;

        free(filter);
        filter = nullptr;
    }
    pthread_mutex_unlock(&_prepare_mutex);
}

void XXMediaExtractor::release() {
    if (_extractor_thread != 0) {
        pthread_join(_extractor_thread, NULL);
    }
    pthread_mutex_destroy(&_prepare_mutex);
    pthread_mutex_destroy(&_seek_mutex);
}