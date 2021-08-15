//
// Created by Neo on 2021/7/14.
//

#ifndef XXPLAYER_JAVA_CALL_H
#define XXPLAYER_JAVA_CALL_H

#include <jni.h>

class JavaCall {
private :
    jobject _java_media_player;
    jobject _xx_media_codec;
    const char *player_class_name = "me/xfly/xxplayer/XXMediaPlayer";
    const char *post_event_method_name = "postEventFromNative";
    const char *post_event_method_sign = "(Ljava/lang/Object;IIILjava/lang/Object;)V";
    jmethodID post_event_method_id;

    const char *init_codec_method_name = "onInit";
    const char *init_codec_method_sign = "(III[B[B)V";
    jmethodID init_codec_method_id;

    const char *release_codec_method_name = "onRelease";
    const char *release_codec_method_sign = "()V";
    jmethodID release_codec_method_id;

    const char *decode_method_name = "onDecode";
    const char *decode_method_sign = "([BII)V";
    jmethodID decode_method_id;
    _JavaVM *_java_vm = NULL;

public:
    JNIEnv *_main_env = NULL;

    JavaCall(_JavaVM *javaVm,JNIEnv *jniEnv, jobject java_media_player,jobject xx_media_codec);

    ~JavaCall();

    void postEventFromNative(int thread_type,int what, int arg1, int arg2, jobject jobject1);

    void onInitMediaCodec(int type, int mimetype, int width, int height, int csd_0_size, int csd_1_size, uint8_t * csd_0, uint8_t * csd_1);

    void onDecode(int type, int size, uint8_t *data, int pts);

    void onReleaseMediaCodec(int type);
};


#endif //XXPLAYER_JAVA_CALL_H
