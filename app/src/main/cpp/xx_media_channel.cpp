//
// Created by Neo on 2021/7/10.
//

#include "xx_media_channel.h"

XXMediaChannel::XXMediaChannel(int id, AVRational base) : channelId(id), time_base(base) {}


XXMediaChannel::XXMediaChannel(int id, AVRational base, int fps) : channelId(id), time_base(base),
                                                                   fps(fps) {};

XXMediaChannel::~XXMediaChannel() {
}