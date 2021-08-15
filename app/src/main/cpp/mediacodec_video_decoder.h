//
// Created by Neo on 2021/7/9.
//

#ifndef XXPLAYER_MEDIACODEC_VIDEO_DECODER_H
#define XXPLAYER_MEDIACODEC_VIDEO_DECODER_H

#include <jni.h>
#include "base_decoder.h"

class MediaCodecVideoDecoder : public BaseDecoder {
private:

public :
    MediaCodecVideoDecoder();

    ~MediaCodecVideoDecoder();

    void initDecoder() override;

    void releaseDecoder() override;

    void start() override;

    void stop() override;

    void pause() override;

    void decode() override;

    void release();

    double getDelayTime(double diff);
};


#endif //XXPLAYER_MEDIACODEC_VIDEO_DECODER_H
