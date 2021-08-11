//
// Created by Neo on 2021/7/14.
//

#include "java_call.h"

JavaCall::JavaCall(_JavaVM *javaVm, JNIEnv *main_env, jobject java_media_player)
        : _java_vm(javaVm),
          _main_env(main_env),
          _java_media_player(
                  java_media_player) {
}

JavaCall::~JavaCall() {
}

void JavaCall::postEventFromNative(int thread_type, int what, int arg1, int arg2, jobject jobject1) {

    if (thread_type == 1) {
        jclass clazz = _main_env->FindClass(class_name);

        _post_event_method_id
                = _main_env->GetStaticMethodID(clazz, method_name, method_sign);

        _main_env->CallStaticVoidMethod(clazz, _post_event_method_id, _java_media_player,
                                        what,
                                        arg1, arg2, jobject1);
    } else {
        JNIEnv *env;

        int status = _java_vm->AttachCurrentThread(&env, 0);

        if (status < 0) {
            return;
        }

        jclass clazz = env->FindClass(class_name);

        _post_event_method_id
                = env->GetStaticMethodID(clazz, method_name, method_sign);

        env->CallStaticVoidMethod(clazz, _post_event_method_id, _java_media_player,
                                  1,
                                  1, 1, nullptr);
        _java_vm->DetachCurrentThread();
    }


}
