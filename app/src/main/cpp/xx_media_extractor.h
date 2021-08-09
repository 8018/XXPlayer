//
// Created by Neo on 2021/7/9.
//

#ifndef XXPLAYER_XX_MEDIA_EXTRACTOR_H
#define XXPLAYER_XX_MEDIA_EXTRACTOR_H

#include <queue>
#include <pthread.h>
#include <string>
#include "xx_play.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

class XXMediaExtractor {
public:
    XXMediaExtractor();

    ~XXMediaExtractor();

    void start();

    void startExtractor();

    void release();

    pthread_t _extractor_thread = 0;
    pthread_mutex_t _prepare_mutex;
    pthread_mutex_t _seek_mutex;
    std::shared_ptr<XXPlay> _xx_play;
};


#endif //XXPLAYER_XX_MEDIA_EXTRACTOR_H
