//
// Created by Neo on 2021/7/9.
//

#include "mediacodec_video_decoder.h"
#include "LogUtil.h"

extern "C" {
#include <libavutil/time.h>
#include <libavutil/imgutils.h>
}

void *decodeThread(void *data) {
    auto *mediaCodecDecoder = (MediaCodecVideoDecoder *) data;
    mediaCodecDecoder->decode();
    pthread_exit(&mediaCodecDecoder->_decode_thread);
}

MediaCodecVideoDecoder::MediaCodecVideoDecoder() {
    pthread_mutex_init(&_decode_mutex, NULL);
}

MediaCodecVideoDecoder::~MediaCodecVideoDecoder() {}

void MediaCodecVideoDecoder::initDecoder() {
    _xx_play->getJavaCall()->onInitMediaCodec(1, _xx_play->_mime_type, _av_codec_context->width,
                                              _av_codec_context->height,
                                              _av_codec_context->extradata_size,
                                              _av_codec_context->extradata_size,
                                              _av_codec_context->extradata,
                                              _av_codec_context->extradata);


}

void MediaCodecVideoDecoder::start() {
    pthread_create(&_decode_thread, NULL, decodeThread, this);
}

void MediaCodecVideoDecoder::decode() {
    double frame_pts = 0;
    double diff = 0;
    while (_xx_play->_play_status != XXP_PLAY_STATUS_STOP) {
        if (_xx_play->_play_status == XXP_PLAY_STATUS_PAUSE ||
            _xx_play->_play_status == XXP_PLAY_STATUS_CREATED) {
            av_usleep(1000 * 100);
            continue;
        }
        if (_xx_play->_play_status == XXP_PLAY_STATUS_SEEK) {
            continue;
        }

        AVPacket *packet = av_packet_alloc();
        if (_xx_play->getVideoPacket(packet) != 0) {
            av_free(packet->data);
            av_free(packet->buf);
            av_free(packet->side_data);
            packet = NULL;
            continue;
        }
        double time = packet->pts * av_q2d(_xx_play->video_time_base);

        if (time < 0) {
            time = packet->dts * av_q2d(_xx_play->video_time_base);
        }

        if (time < _xx_play->video_clock) {
            time = _xx_play->video_clock;
        }
        _xx_play->video_clock = time;
        if (_xx_play->clock != -1) {
            diff = _xx_play->clock - _xx_play->video_clock;
        }
        if (diff >= 0.5) {
            av_free(packet->data);
            av_free(packet->buf);
            av_free(packet->side_data);
            packet = NULL;
            continue;
        }

        _xx_play->delay_time = getDelayTime(diff);

        _xx_play->getJavaCall()->onDecode(0, packet->size, packet->data, 0);
        av_usleep(_xx_play->delay_time * 1000);
        av_free(packet->data);
        av_free(packet->buf);
        av_free(packet->side_data);
        packet = NULL;
    }

}

void MediaCodecVideoDecoder::releaseDecoder() {
    release();
}

void MediaCodecVideoDecoder::pause() {}

void MediaCodecVideoDecoder::stop() {}

void MediaCodecVideoDecoder::release() {
    _xx_play->getJavaCall()->onReleaseMediaCodec(1);
}

double MediaCodecVideoDecoder::getDelayTime(double diff) {

    if (diff > 0.003) {
        _xx_play->delay_time = _xx_play->delay_time / 3 * 2;
        if (_xx_play->delay_time < _xx_play->video_rate / 2) {
            _xx_play->delay_time = _xx_play->video_rate / 3 * 2;
        } else if (_xx_play->delay_time > _xx_play->video_rate * 2) {
            _xx_play->delay_time = _xx_play->video_rate * 2;
        }

    } else if (diff < -0.003) {
        _xx_play->delay_time = _xx_play->delay_time * 3 / 2;
        if (_xx_play->delay_time < _xx_play->video_rate / 2) {
            _xx_play->delay_time = _xx_play->video_rate / 3 * 2;
        } else if (_xx_play->delay_time > _xx_play->video_rate * 2) {
            _xx_play->delay_time = _xx_play->video_rate * 2;
        }
    } else if (diff == 0) {
        _xx_play->delay_time = _xx_play->video_rate;
    }
    if (diff > 1.0) {
        _xx_play->delay_time = 0;
    }
    if (diff < -1.0) {
        _xx_play->delay_time = _xx_play->video_rate * 2;
    }
    if (fabs(diff) > 10) {
        _xx_play->delay_time = _xx_play->video_rate;
    }
    return _xx_play->delay_time;
}