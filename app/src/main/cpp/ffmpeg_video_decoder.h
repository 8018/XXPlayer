//
// Created by Neo on 2021/7/9.
//

#ifndef XXPLAYER_FFMPEG_VIDEO_DECODER_H
#define XXPLAYER_FFMPEG_VIDEO_DECODER_H

#include "base_decoder.h"
#include "base_render.h"

class FFMpegVideoDecoder : public BaseDecoder {

public:

    FFMpegVideoDecoder();

    ~FFMpegVideoDecoder();

    void decode() override;

    void initDecoder() override;

    void releaseDecoder() override;

    void start() override;

    void stop() override;

    void pause() override;

    void release();

    int rate = 0;
};


#endif //XXPLAYER_FFMPEG_VIDEO_DECODER_H
