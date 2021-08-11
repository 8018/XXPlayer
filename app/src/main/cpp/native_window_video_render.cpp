//
// Created by Neo on 2021/7/9.
//

#include "native_window_video_render.h"
#include "jvm_android.h"
#include "LogUtil.h"

extern "C" {
#include <libavutil/time.h>
#include <libavutil/imgutils.h>
}

void *videoRenderThread(void *context) {
    auto *videoRender = (NativeWindowVideoRender *) context;
    videoRender->initRender();
    videoRender->startRender();
    pthread_exit(&videoRender->_video_render_thread);
}

NativeWindowVideoRender::NativeWindowVideoRender() {
}

NativeWindowVideoRender::~NativeWindowVideoRender() {
}

void NativeWindowVideoRender::resume() {}

void NativeWindowVideoRender::start() {
    pthread_create(&_video_render_thread, nullptr, videoRenderThread, this);
}

void NativeWindowVideoRender::pause() {}

void NativeWindowVideoRender::stop() {}

void NativeWindowVideoRender::startRender() {
    double frame_pts = 0;
    double diff = 0;

    while (_xx_play->_play_status != XXP_PLAY_STATUS_STOP && _xx_play->_surface != nullptr) {
        if (_xx_play->_video_render_status == VIDEO_RENDER_STATUS_NO_SURFACE) {
            break;
        }
        if (_xx_play->_play_status == XXP_PLAY_STATUS_PAUSE||_xx_play->_play_status == XXP_PLAY_STATUS_CREATED) {
            av_usleep(1000 * 100);
            continue;
        }
        if (_xx_play->_play_status == XXP_PLAY_STATUS_SEEK) {
            continue;
        }

        AVFrame *frame = av_frame_alloc();
        if (_xx_play->getVideoFrame(frame) != 0) {

            av_frame_free(&frame);
            av_free(frame);
            frame = NULL;

            if (_xx_play->video_frame_end) {
                LOGE("NativeWindowVideoRender", "end of file")
                break;
            }

            continue;
        }

        frame_pts = frame->pts;
        frame_pts *= av_q2d(_xx_play->video_time_base);

        _xx_play->video_clock = getOffsetPts(frame, frame_pts);
        if (_xx_play->clock != -1) {
            diff = _xx_play->clock - _xx_play->video_clock;
        }
        _xx_play->delay_time = getDelayTime(diff);
        if (diff >= 0.5) {
            av_frame_free(&frame);
            av_free(frame);
            frame = NULL;
            continue;
        }

        av_usleep(_xx_play->delay_time * 1000);

        ANativeWindow_setBuffersGeometry(m_native_window, _av_codec_context->width,
                                         _av_codec_context->height, WINDOW_FORMAT_RGBA_8888);

        // 存放输出到屏幕的缓存数据
        ANativeWindow_Buffer m_out_buffer;
        if (0 != ANativeWindow_lock(m_native_window, &m_out_buffer, 0)) {
            av_frame_free(&frame);
            av_free(frame);
            frame = NULL;
            continue;
        }

        // 格式转换
        sws_scale(m_sws_ctx, (uint8_t const *const *) frame->data,
                  frame->linesize, 0, _av_codec_context->height,
                  m_rgb_frame->data, m_rgb_frame->linesize);
        // 获取stride
        dst = (uint8_t *) m_out_buffer.bits;
        int dstStride = m_out_buffer.stride * 4;
        src = (uint8_t *) (m_rgb_frame->data[0]);
        int srcStride = m_rgb_frame->linesize[0];


        // 由于window的stride和帧的stride不同,因此需要逐行复制
        int h;
        for (h = 0; h < _av_codec_context->height; h++) {
            memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
        }
        ANativeWindow_unlockAndPost(m_native_window);

        av_frame_free(&frame);
        av_free(frame);
        frame = NULL;
    }
}

int NativeWindowVideoRender::initRender() {
    JVM *jvm = JVM::GetInstance();
    JNIEnv *env = nullptr;

    int status = (jvm->jvm())->GetEnv((void **) &env, JNI_VERSION_1_6);

    if (status == JNI_EDETACHED || env == NULL) {
        status = jvm->jvm()->AttachCurrentThread(&env, NULL);
    }

    if (!status == 0) {
        return -1;
    }

    // 获取native window
    m_native_window = ANativeWindow_fromSurface(env, _xx_play->_surface);

    m_rgb_frame = av_frame_alloc();

    // Determine required buffer size and allocate buffer
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, _av_codec_context->width,
                                            _av_codec_context->height, 1);
    out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(m_rgb_frame->data, m_rgb_frame->linesize, out_buffer, AV_PIX_FMT_RGBA,
                         _av_codec_context->width, _av_codec_context->height, 1);

    // 由于解码出来的帧格式不是RGBA的,在渲染之前需要进行格式转换
    m_sws_ctx = sws_getContext(_av_codec_context->width,
                               _av_codec_context->height,
                               _av_codec_context->pix_fmt,
                               _av_codec_context->width,
                               _av_codec_context->height,
                               AV_PIX_FMT_RGBA,
                               SWS_BILINEAR,
                               NULL,
                               NULL,
                               NULL);


    jvm->jvm()->DetachCurrentThread();
    return 0;
}

void NativeWindowVideoRender::releaseRender() {
    release();
}

double NativeWindowVideoRender::getOffsetPts(AVFrame *srcFrame, double pts) {
    double frame_delay;

    if (pts != 0)
        _xx_play->video_clock = pts; // Get pts,then set video clock to it
    else
        pts = _xx_play->video_clock; // Don't get pts,set it to video clock

    frame_delay = av_q2d(_xx_play->video_time_base);
    frame_delay += srcFrame->repeat_pict * (frame_delay * 0.5);

    _xx_play->video_clock += frame_delay;

    return pts;
}


double NativeWindowVideoRender::getDelayTime(double diff) {

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

void NativeWindowVideoRender::release() {
    LOGE("NativeWindowVideoRender", "release")

    if (_xx_play->_video_render_status == VIDEO_RENDER_STATUS_RELEASED) {
        return;
    }
    if(_video_render_thread != 0){
         pthread_join(_video_render_thread, NULL);
    }
    _xx_play->_video_render_status = VIDEO_RENDER_STATUS_RELEASED;
   // LOGE("NativeWindowVideoRender", "release2")

    if (out_buffer != nullptr) {
        free(out_buffer);
        out_buffer = nullptr;;
    }

    if (m_native_window != nullptr) {
        //free(m_native_window);
        ANativeWindow_release(m_native_window);
    }
    if (m_sws_ctx != nullptr) {
        sws_freeContext(m_sws_ctx);
        m_sws_ctx = nullptr;
    }

    if (m_rgb_frame != nullptr) {
        av_frame_free(&m_rgb_frame);
        av_free(m_rgb_frame);
        m_rgb_frame = NULL;
    }

}