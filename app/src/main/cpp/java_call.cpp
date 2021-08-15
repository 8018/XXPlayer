//
// Created by Neo on 2021/7/14.
//

#include "java_call.h"

JavaCall::JavaCall(_JavaVM *javaVm, JNIEnv *main_env, jobject java_media_player,
                   jobject xx_media_codec)
        : _java_vm(javaVm),
          _main_env(main_env),
          _java_media_player(
                  java_media_player),
          _xx_media_codec(xx_media_codec) {

    jclass clazz = _main_env->GetObjectClass(_xx_media_codec);
    init_codec_method_id = _main_env->GetMethodID(clazz,init_codec_method_name,init_codec_method_sign);
    release_codec_method_id = _main_env->GetMethodID(clazz,release_codec_method_name,release_codec_method_sign);
    decode_method_id = _main_env->GetMethodID(clazz,decode_method_name,decode_method_sign);
}

JavaCall::~JavaCall() {
}

void JavaCall::postEventFromNative(int thread_type, int what, int arg1, int arg2, jobject jobject1) {

    if (thread_type == 1) {
        jclass clazz = _main_env->FindClass(player_class_name);

        post_event_method_id
                = _main_env->GetStaticMethodID(clazz, post_event_method_name, post_event_method_sign);

        _main_env->CallStaticVoidMethod(clazz, post_event_method_id, _java_media_player,
                                        what,
                                        arg1, arg2, jobject1);
    } else {
        JNIEnv *env;

        int status = _java_vm->AttachCurrentThread(&env, 0);

        if (status < 0) {
            return;
        }

        jclass clazz = env->FindClass(player_class_name);

        post_event_method_id
                = env->GetStaticMethodID(clazz, post_event_method_name, post_event_method_sign);

        env->CallStaticVoidMethod(clazz, post_event_method_id, _java_media_player,
                                  1,
                                  1, 1, nullptr);
        _java_vm->DetachCurrentThread();
    }
}

void JavaCall::onInitMediaCodec(int type, int mimetype, int width, int height, int csd_0_size,
                                int csd_1_size, uint8_t *csd_0, uint8_t *csd_1) {

    if (type == 1) {
        jbyteArray csd0 = _main_env->NewByteArray(csd_0_size);
        _main_env->SetByteArrayRegion(csd0, 0, csd_0_size, (jbyte *) csd_0);
        jbyteArray csd1 = _main_env->NewByteArray(csd_1_size);
        _main_env->SetByteArrayRegion(csd1, 0, csd_1_size, (jbyte *) csd_1);

        _main_env->CallVoidMethod(_xx_media_codec, init_codec_method_id, mimetype, width, height, csd0, csd1);

        _main_env->DeleteLocalRef(csd0);
        _main_env->DeleteLocalRef(csd1);
    } else {
        JNIEnv *env;

        int status = _java_vm->AttachCurrentThread(&env, 0);

        if (status < 0) {
            return;
        }

        jbyteArray csd0 = env->NewByteArray(csd_0_size);
        env->SetByteArrayRegion(csd0, 0, csd_0_size, (jbyte *) csd_0);
        jbyteArray csd1 = env->NewByteArray(csd_1_size);
        env->SetByteArrayRegion(csd1, 0, csd_1_size, (jbyte *) csd_1);

        env->CallVoidMethod(_xx_media_codec, init_codec_method_id, mimetype, width, height, csd0, csd1);

        env->DeleteLocalRef(csd0);
        env->DeleteLocalRef(csd1);

        _java_vm->DetachCurrentThread();
    }

}

void JavaCall::onReleaseMediaCodec(int type) {
    if (type == 1) {
        _main_env->CallVoidMethod(_xx_media_codec, release_codec_method_id);

    } else {
        JNIEnv *env;

        int status = _java_vm->AttachCurrentThread(&env, 0);

        if (status < 0) {
            return;
        }

        env->CallVoidMethod(_xx_media_codec, release_codec_method_id);
        _java_vm->DetachCurrentThread();
    }
}

void JavaCall::onDecode(int type, int size, uint8_t *packet_data, int pts) {
    if (type == 1) {
        jbyteArray data = _main_env->NewByteArray(size);
        _main_env->SetByteArrayRegion(data, 0, size, (jbyte *) packet_data);
        _main_env->CallVoidMethod(_xx_media_codec, decode_method_id, data, size, pts);
        _main_env->DeleteLocalRef(data);

    } else {
        JNIEnv *env;
        int status = _java_vm->AttachCurrentThread(&env, 0);
        if (status < 0) {
            return;
        }
        jbyteArray data = env->NewByteArray(size);
        env->SetByteArrayRegion(data, 0, size, (jbyte *) packet_data);
        env->CallVoidMethod(_xx_media_codec, decode_method_id, data, size, pts);
        env->DeleteLocalRef(data);
        _java_vm->DetachCurrentThread();
    }
}
