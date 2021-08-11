//
// Created by Neo on 2021/7/10.
//

#include "xx_play.h"
#include "LogUtil.h"

XXPlay::XXPlay(std::shared_ptr<JavaCall> javaCall) : _java_call(javaCall) {
    pthread_mutex_init(&audio_packet_mutex, NULL);
    pthread_cond_init(&audio_packet_cond, NULL);
    pthread_mutex_init(&video_packet_mutex, NULL);
    pthread_cond_init(&video_packet_cond, NULL);

    pthread_mutex_init(&audio_frame_mutex, NULL);
    pthread_cond_init(&audio_frame_cond, NULL);
    pthread_mutex_init(&video_frame_mutex, NULL);
    pthread_cond_init(&video_frame_cond, NULL);
}

XXPlay::~XXPlay() {
    if(_av_format_context != nullptr){
        avformat_free_context(_av_format_context);
        _av_format_context = nullptr;
    }
    pthread_mutex_destroy(&audio_packet_mutex);
    pthread_mutex_destroy(&video_packet_mutex);
    pthread_mutex_destroy(&audio_frame_mutex);
    pthread_mutex_destroy(&video_frame_mutex);
    pthread_cond_destroy(&audio_packet_cond);
    pthread_cond_destroy(&audio_frame_cond);
    pthread_cond_destroy(&video_packet_cond);
    pthread_cond_destroy(&video_frame_cond);
}

std::shared_ptr<JavaCall> XXPlay::getJavaCall() {
    return _java_call;
}

int XXPlay::getAudioPacketSize() {
    int size = audio_packet_queue.size();
    return size;
}

int XXPlay::getVideoPacketSize() {
    int size = video_packet_queue.size();
    return size;
}

int XXPlay::putAudioPacket(AVPacket *packet) {
    pthread_mutex_lock(&audio_packet_mutex);
    audio_packet_queue.push(packet);
    pthread_cond_signal(&audio_packet_cond);
    pthread_mutex_unlock(&audio_packet_mutex);

    return 0;
}

int XXPlay::putVideoPacket(AVPacket *packet) {
    pthread_mutex_lock(&video_packet_mutex);
    video_packet_queue.push(packet);
    pthread_cond_signal(&video_packet_cond);
    pthread_mutex_unlock(&video_packet_mutex);

    return 0;
}

int XXPlay::getAudioPacket(AVPacket *packet) {

    pthread_mutex_lock(&audio_packet_mutex);

    if (audio_packet_queue.size() > 0) {
        AVPacket *pkt = audio_packet_queue.front();
        if (av_packet_ref(packet, pkt) == 0) {
            audio_packet_queue.pop();
        }
        av_packet_free(&pkt);
        av_free(pkt);
        pkt = nullptr;
        pthread_mutex_unlock(&audio_packet_mutex);
        return 0;
    }

    if (extractor_end) {
        pthread_mutex_unlock(&audio_packet_mutex);
        return -1;
    }

    pthread_cond_wait(&audio_packet_cond, &audio_packet_mutex);
    pthread_mutex_unlock(&audio_packet_mutex);
    return -1;
}

int XXPlay::getVideoPacket(AVPacket *packet) {

    pthread_mutex_lock(&video_packet_mutex);

    if (video_packet_queue.size() > 0) {
        AVPacket *pkt = video_packet_queue.front();
        if (av_packet_ref(packet, pkt) == 0) {
            video_packet_queue.pop();
        }
        av_packet_free(&pkt);
        av_free(pkt);
        pkt = NULL;
        pthread_mutex_unlock(&video_packet_mutex);
        return 0;
    }

    if (extractor_end) {
        pthread_mutex_unlock(&video_packet_mutex);
        return -1;
    }

    pthread_cond_wait(&video_packet_cond, &video_packet_mutex);
    pthread_mutex_unlock(&video_packet_mutex);
    return -1;
}

int XXPlay::getAudioFrame(AVFrame *frame) {

    pthread_mutex_lock(&audio_frame_mutex);
    if (audio_frame_queue.size() > 0) {
        AVFrame *frame1 = audio_frame_queue.front();
        if (av_frame_ref(frame, frame1) == 0) {
            audio_frame_queue.pop();
        }
        av_frame_free(&frame1);
        av_free(frame1);
        frame1 = NULL;

        pthread_mutex_unlock(&audio_frame_mutex);
        return 0;

    }

    if (audio_frame_end) {
        pthread_mutex_unlock(&audio_frame_mutex);
        return -1;
    }

    pthread_cond_wait(&audio_frame_cond, &audio_frame_mutex);
    pthread_mutex_unlock(&audio_frame_mutex);
    return -1;
}

int XXPlay::getVideoFrame(AVFrame *frame) {

    pthread_mutex_lock(&video_frame_mutex);

    if (video_frame_queue.size() > 0) {
        AVFrame *frame1 = video_frame_queue.front();
        if (av_frame_ref(frame, frame1) == 0) {
            video_frame_queue.pop();
        }
        av_frame_free(&frame1);
        av_free(frame1);
        frame1 = NULL;
        pthread_mutex_unlock(&video_frame_mutex);
        return 0;
    }

    if (video_frame_end) {
        pthread_mutex_unlock(&video_frame_mutex);
        return -1;
    }

    pthread_cond_wait(&video_frame_cond, &video_frame_mutex);
    pthread_mutex_unlock(&video_frame_mutex);
    return -1;
}


void XXPlay::clearAudioPacket() {
    pthread_mutex_lock(&audio_packet_mutex);
    pthread_cond_signal(&audio_packet_cond);
    while (!audio_packet_queue.empty()) {
        AVPacket *pkt = audio_packet_queue.front();
        audio_packet_queue.pop();
        av_free(pkt->data);
        av_free(pkt->buf);
        av_free(pkt->side_data);
        pkt = NULL;
    }
    pthread_mutex_unlock(&audio_packet_mutex);
}

void XXPlay::clearVideoPacket() {

    pthread_mutex_lock(&video_packet_mutex);
    pthread_cond_signal(&video_packet_cond);
    while (!video_packet_queue.empty()) {
        AVPacket *pkt = video_packet_queue.front();
        video_packet_queue.pop();
        av_free(pkt->data);
        av_free(pkt->buf);
        av_free(pkt->side_data);
        pkt = NULL;
    }
    pthread_mutex_unlock(&video_packet_mutex);
}


int XXPlay::getAudioFrameSize() {
    int size = audio_frame_queue.size();
    return size;
}

int XXPlay::getVideoFrameSize() {
    int size = video_frame_queue.size();
    return size;
}

int XXPlay::putAudioFrame(AVFrame *frame) {
    pthread_mutex_lock(&audio_frame_mutex);
    audio_frame_queue.push(frame);
    pthread_cond_signal(&audio_frame_cond);
    pthread_mutex_unlock(&audio_frame_mutex);
    return 0;
}

int XXPlay::putVideoFrame(AVFrame *frame) {
    pthread_mutex_lock(&video_frame_mutex);
    video_frame_queue.push(frame);
    pthread_cond_signal(&video_frame_cond);
    pthread_mutex_unlock(&video_frame_mutex);

    return 0;
}

void XXPlay::clearAudioFrame() {
    pthread_mutex_lock(&audio_frame_mutex);
    pthread_cond_signal(&audio_frame_cond);
    while (!audio_frame_queue.empty()) {
        AVFrame *frame = audio_frame_queue.front();
        audio_frame_queue.pop();
        av_frame_free(&frame);
        frame = NULL;
    }
    pthread_mutex_unlock(&audio_frame_mutex);
}

void XXPlay::clearVideoFrame() {

    pthread_mutex_lock(&video_frame_mutex);
    pthread_cond_signal(&video_frame_cond);
    while (!video_frame_queue.empty()) {
        AVFrame *frame = video_frame_queue.front();
        video_frame_queue.pop();
        av_frame_free(&frame);
        frame = NULL;
    }
    pthread_mutex_unlock(&video_frame_mutex);
}

void XXPlay::clearVideoMediaChannel() {

    while (!_video_channels.empty()) {
        XXMediaChannel *channel = _video_channels.front();
        _video_channels.pop_front();
        delete channel;
        channel = NULL;
    }
}

void XXPlay::clearAudioMediaChannel() {

    while (!_audio_channels.empty()) {
        XXMediaChannel *channel = _audio_channels.front();
        _audio_channels.pop_front();
        delete channel;
        channel = NULL;
    }
}

void XXPlay::release() {

    clearAudioPacket();
    clearAudioFrame();
    clearVideoFrame();
    clearVideoPacket();
    clearAudioMediaChannel();
    clearVideoMediaChannel();

}
