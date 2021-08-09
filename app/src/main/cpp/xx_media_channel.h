//
// Created by Neo on 2021/7/10.
//

#ifndef XXPLAYER_XX_MEDIA_CHANNEL_H
#define XXPLAYER_XX_MEDIA_CHANNEL_H


extern "C"
{
#include "includes/ffmpeg/libavutil/rational.h"
};


class XXMediaChannel {
public:
    int channelId = -1;
    AVRational time_base;
    int fps;

public:
    XXMediaChannel(int id, AVRational base);

    XXMediaChannel(int id, AVRational base, int fps);

    ~XXMediaChannel();
};


#endif //XXPLAYER_XX_MEDIA_CHANNEL_H
