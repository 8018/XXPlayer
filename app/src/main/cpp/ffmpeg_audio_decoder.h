//
// Created by Neo on 2021/7/9.
//

#ifndef XXPLAYER_FFMPEG_AUDIO_DECODER_H
#define XXPLAYER_FFMPEG_AUDIO_DECODER_H

#include "base_decoder.h"
#include "base_render.h"

class FFMpegAudioDecoder : public BaseDecoder {

public:
    char *const TAG = "FFMpegAudioDecoder";

    FFMpegAudioDecoder();

    ~FFMpegAudioDecoder();

    void initDecoder() override;

    void releaseDecoder() override;

    void start() override;

    void stop() override;

    void pause() override;

    void decode() override;

    void release();
};


#endif //XXPLAYER_FFMPEG_AUDIO_DECODER_H
