//
// Created by cxp on 2019-07-31.
//

#ifndef LEARNVIDEO_LOGGER_H
#define LEARNVIDEO_LOGGER_H

#include <android/log.h>

#define LOG_SHOW true
#define LOGD(TAG, FORMAT,...) __android_log_print(ANDROID_LOG_DEBUG,TAG,FORMAT,##__VA_ARGS__);
#define LOGI(TAG, FORMAT,...) __android_log_print(ANDROID_LOG_INFO,TAG,FORMAT,##__VA_ARGS__);
#define LOGW(TAG, FORMAT,...) __android_log_print(ANDROID_LOG_WARN,TAG,FORMAT,##__VA_ARGS__);
#define LOGE(TAG, FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,TAG,FORMAT,##__VA_ARGS__);

#endif //LEARNVIDEO_LOGGER_H
