//
// Created by Neo on 2021/7/9.
//

#include "xx_media_extractor.h"
#include <stdint.h>
#include "jvm_android.h"
#include "LogUtil.h"

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

    if (_xx_play->_mime_type != -1) {
        filter = const_cast<AVBitStreamFilter *>(av_bsf_get_by_name(
                "h264_mp4toannexb"));
    }
    int ret = av_bsf_alloc(filter, &bsf_ctx);

    if (ret < 0) {
        return;
    }


    while (_xx_play->_play_status != XXP_PLAY_STATUS_STOP) {

        if (_xx_play->_play_status == XXP_PLAY_STATUS_PAUSE ||
            _xx_play->_play_status == XXP_PLAY_STATUS_NO_SURFACE) {
            av_usleep(1000 * 100);
            continue;
        }

        if (_xx_play != nullptr &&
            (_xx_play->getVideoPacketSize() + _xx_play->getAudioPacketSize() > 5* 1024) ) {
            av_usleep(1000 * 100);
            continue;
        }

        AVPacket *packet = av_packet_alloc();
        ret = av_read_frame(_xx_play->_av_format_context, packet);

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
                        continue;
                    }
                    ret = av_bsf_receive_packet(bsf_ctx, packet);
                    if (ret < 0) { continue; }
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
    }
    pthread_mutex_unlock(&_prepare_mutex);
}

void XXMediaExtractor::release() {
    pthread_mutex_destroy(&_prepare_mutex);
    pthread_mutex_destroy(&_seek_mutex);
}