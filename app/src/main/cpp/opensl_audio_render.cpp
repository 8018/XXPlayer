//
// Created by Neo on 2021/7/11.
//

#include "opensl_audio_render.h"
#include "LogUtil.h"

void *audioRenderThread(void *data) {
    auto *audioRender = (OpenSLAudioRender *) data;
    audioRender->startRender();
    pthread_exit(&audioRender->_audio_render_thread);
}

void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void *context) {
    auto *audioRender = (OpenSLAudioRender *) context;
    int pcm_size = audioRender->getPcmData();
    if (pcm_size > 0) {
        (*bf)->Enqueue(bf,audioRender->out_buffer, pcm_size);
    }
}

OpenSLAudioRender::OpenSLAudioRender() {
}

OpenSLAudioRender::~OpenSLAudioRender() {
}

int OpenSLAudioRender::initRender() {

    out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
    out_sample_size = av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
    // 2(通道数) * 2（16bit=2字节）*44100（采样率）
    out_buffer_size = out_channels * out_sample_size * out_sample_rate;
    out_buffer = static_cast<uint8_t *>(malloc(out_buffer_size));
    memset(out_buffer, 0, out_buffer_size);

    swr_ctx = swr_alloc_set_opts(nullptr, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16,
                                 out_sample_rate,
                                 _av_codec_context->channel_layout,
                                 _av_codec_context->sample_fmt,
                                 _av_codec_context->sample_rate,
                                 0, nullptr);
    //初始化重采样上下文
    swr_init(swr_ctx);


    /**
       * 1、创建引擎并获取引擎接口
       */
    SLresult result;
    // 1.1 创建引擎对象：SLObjectItf engineObject
    result = slCreateEngine(&engineObject, 0, nullptr, 0, nullptr, nullptr);
    if (SL_RESULT_SUCCESS != result) {
        return-1;
    }
    // 1.2 初始化引擎
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return -1;
    }
    // 1.3 获取引擎接口 SLEngineItf engineInterface
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineInterface);
    if (SL_RESULT_SUCCESS != result) {
        return-1;
    }
    /**
     * 2、设置混音器
     */
    // 2.1 创建混音器：SLObjectItf outputMixObject
    result = (*engineInterface)->CreateOutputMix(engineInterface, &outputMixObject, 0,
                                                 nullptr, nullptr);
    if (SL_RESULT_SUCCESS != result) {
        return-1;
    }
    // 2.2 初始化混音器
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return-1;
    }
    /**
     * 3、创建播放器
     */
    //3.1 配置输入声音信息
    //创建buffer缓冲类型的队列 2个队列
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                       2};
    //pcm数据格式
    //SL_DATAFORMAT_PCM：数据格式为pcm格式
    //2：双声道
    //SL_SAMPLINGRATE_44_1：采样率为44100
    //SL_PCMSAMPLEFORMAT_FIXED_16：采样格式为16bit
    //SL_PCMSAMPLEFORMAT_FIXED_16：数据大小为16bit
    //SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT：左右声道（双声道）
    //SL_BYTEORDER_LITTLEENDIAN：小端模式
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 2, SL_SAMPLINGRATE_44_1,
                                   SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
                                   SL_BYTEORDER_LITTLEENDIAN};

    //数据源 将上述配置信息放到这个数据源中
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    //3.2 配置音轨（输出）
    //设置混音器
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, nullptr};
    //需要的接口 操作队列的接口
    const SLInterfaceID ids[1] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};
    //3.3 创建播放器
    result = (*engineInterface)->CreateAudioPlayer(engineInterface, &bqPlayerObject, &audioSrc,
                                                   &audioSnk, 1, ids, req);
    if (SL_RESULT_SUCCESS != result) {
        return-1;
    }
    //3.4 初始化播放器：SLObjectItf bqPlayerObject
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return-1;
    }
    //3.5 获取播放器接口：SLPlayItf bqPlayerPlay
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    if (SL_RESULT_SUCCESS != result) {
        return-1;
    }
    /**
     * 4、设置播放回调函数
     */
    //4.1 获取播放器队列接口：SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue
    (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE, &bqPlayerBufferQueue);

    //4.2 设置回调 void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
    (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, pcmBufferCallBack, this);

    return 0;
}

void OpenSLAudioRender::resume() {
    (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
}

void OpenSLAudioRender::releaseRender() {
    release();
}

void OpenSLAudioRender::start() {
    pthread_create(&_audio_render_thread, nullptr, audioRenderThread, this);
}

void OpenSLAudioRender::pause() {
    (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PAUSED);
}

void OpenSLAudioRender::stop() {
    (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_STOPPED);
}

void OpenSLAudioRender::startRender() {


    /**
     * 5、设置播放器状态为播放状态
     */
    (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);

    /**
     * 6、手动激活回调函数
     */
    pcmBufferCallBack(bqPlayerBufferQueue, this);

}

int OpenSLAudioRender::getPcmData() {

    int pcmDataSize;
    if (_xx_play->_play_status == XXP_PLAY_STATUS_PAUSE ) {
        return 0;
    }

    AVFrame *frame = av_frame_alloc();
    if (_xx_play->getAudioFrame(frame) != 0) {
        av_frame_free(&frame);
        av_free(frame);
        frame = nullptr;

        if (_xx_play->audio_frame_end) {
            LOGE("OpenSLAudioRender", "end of file")
            (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_STOPPED);
        }

        return 0;
    }
    // 设置通道数或channel_layout
    if (frame->channels > 0 && frame->channel_layout == 0)
        frame->channel_layout = av_get_default_channel_layout(frame->channels);
    else if (frame->channels == 0 && frame->channel_layout > 0)
        frame->channels = av_get_channel_layout_nb_channels(frame->channel_layout);

    //pcm数据在 frame中
    //这里获得的解码后pcm格式的音频原始数据，有可能与创建的播放器中设置的pcm格式不一样, 需要进行重采样 resample

    //假设输入10个数据，有可能这次转换只转换了8个，还剩2个数据（delay）
    //断点：1024 * 48000

    //swr_get_delay: 下一个输入数据与下下个输入数据之间的时间间隔
    int64_t delay = swr_get_delay(swr_ctx, frame->sample_rate);

    //LOGE(TAG, "getPcmData delay %d", delay)

    //a * b / c
    //AV_ROUND_UP：向上取整
    int64_t out_max_samples = av_rescale_rnd(frame->nb_samples + delay, frame->sample_rate,
                                             frame->sample_rate,
                                             AV_ROUND_INF);
    //上下文
    //输出缓冲区
    //输出缓冲区能容纳的最大数据量
    //输入数据
    //输入数据量
    int out_samples = swr_convert(swr_ctx, &out_buffer, out_max_samples,
                                  (const uint8_t **) (frame->data), frame->nb_samples);

    // 获取swr_convert转换后 out_samples个 *2 （16位）*2（双声道）
    pcmDataSize = out_samples * out_sample_size * out_channels;
//        frame->best_effort_timestamp*时间单位



    double now_time = frame->pts * av_q2d(_xx_play->audio_time_base);
    if (now_time < _xx_play->clock) {
        now_time = _xx_play->clock;
    }
    _xx_play->clock = now_time;

    av_frame_free(&frame);
    av_free(frame);
    frame = nullptr;
    return pcmDataSize;
}

void OpenSLAudioRender::release() {
    if (bqPlayerObject != nullptr) {
        (*bqPlayerObject)->Destroy(bqPlayerObject);
        bqPlayerObject = nullptr;
        bqPlayerPlay = nullptr;
        pcmBufferQueue = nullptr;
        engineInterface = nullptr;

        bqPlayerBufferQueue = nullptr;
    }

    // destroy output mix object, and invalidate all associated interfaces
    if (outputMixObject != nullptr) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = nullptr;
    }

    // destroy engine object, and invalidate all associated interfaces
    if (engineObject != nullptr) {
        (*engineObject)->Destroy(engineObject);
        engineObject = nullptr;
    }

    if (bqPlayerBufferQueue != nullptr) {
        av_free(&bqPlayerBufferQueue);
    }

    if (buffer != nullptr) {
        av_free(buffer);
        buffer = nullptr;
    }

    if (out_buffer != nullptr) {
        av_free(out_buffer);
        out_buffer = nullptr;
    }

    if (swr_ctx != nullptr) {
        swr_free(&swr_ctx);
        swr_ctx = nullptr;
    }

    if (_av_codec_context != nullptr) {
        _av_codec_context = nullptr;
    }
}