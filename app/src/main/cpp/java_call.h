//
// Created by Neo on 2021/7/14.
//

#ifndef XXPLAYER_JAVA_CALL_H
#define XXPLAYER_JAVA_CALL_H

#include <jni.h>

class JavaCall {
private :
    jobject _java_media_player;
    const char *class_name = "me/xfly/xxplayer/XXMediaPlayer";
    const char *method_name = "postEventFromNative";
    const char *method_sign = "(Ljava/lang/Object;IIILjava/lang/Object;)V";
    jmethodID _post_event_method_id;
    _JavaVM *_java_vm = NULL;
    JNIEnv *_main_env = NULL;
public:
    JavaCall(_JavaVM *javaVm,JNIEnv *jniEnv, jobject java_media_player);

    ~JavaCall();

    void postEventFromNative(int thread_type,int what, int arg1, int arg2, jobject jobject1);
};


#endif //XXPLAYER_JAVA_CALL_H
