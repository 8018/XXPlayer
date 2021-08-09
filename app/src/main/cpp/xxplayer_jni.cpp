//
// Created by Neo on 2021/7/9.
//
#include <jni.h>
#include <string>
#include "LogUtil.h"
#include "jvm_android.h"
#include <queue>
#include "xx_media_player.h"
#include "xx_msg.h"

XXMediaPlayer *xxMediaPlayer = NULL;

char* jstringToChar(JNIEnv* env, jstring jstr) {
    char* rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("GB2312");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char*) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}

extern "C" jint JNIEXPORT JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
    JVM::Initialize(jvm);
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT void JNICALL
Java_me_xfly_boboplayer_XXMediaPlayer_native_1init(JNIEnv *env, jclass clazz,
        jobject xxmedia_player_this) {
    xxMediaPlayer = new XXMediaPlayer(JVM::GetInstance()->jvm(),env,env->NewGlobalRef(xxmedia_player_this));
}

extern "C"
JNIEXPORT void JNICALL
Java_me_xfly_boboplayer_XXMediaPlayer_native_1finalize(JNIEnv *env, jobject thiz) {
    if (xxMediaPlayer!= nullptr){
        xxMediaPlayer->onStop();
        delete xxMediaPlayer;
        xxMediaPlayer = nullptr;
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_me_xfly_boboplayer_XXMediaPlayer__1start(JNIEnv *env, jobject thiz) {
    if(!xxMediaPlayer){
        return;
    }
    xxMediaPlayer->onStart();
}

extern "C"
JNIEXPORT void JNICALL
Java_me_xfly_boboplayer_XXMediaPlayer__1setStreamSelected(JNIEnv *env, jobject thiz, jint stream,
                                                          jboolean select) {
    // TODO: implement _setStreamSelected()
}

extern "C"
JNIEXPORT void JNICALL
Java_me_xfly_boboplayer_XXMediaPlayer__1pause(JNIEnv *env, jobject thiz) {
    if(!xxMediaPlayer){
        return;
    }
    xxMediaPlayer->onPause();
}

extern "C"
JNIEXPORT void JNICALL
Java_me_xfly_boboplayer_XXMediaPlayer__1stop(JNIEnv *env, jobject thiz) {
    if(!xxMediaPlayer){
        return;
    }
    xxMediaPlayer->onStop();
}

extern "C"
JNIEXPORT void JNICALL
Java_me_xfly_boboplayer_XXMediaPlayer__1prepareAsync(JNIEnv *env, jobject thiz) {
    if(!xxMediaPlayer){
        return;
    }
    xxMediaPlayer->onPause();
}

extern "C"
JNIEXPORT void JNICALL
Java_me_xfly_boboplayer_XXMediaPlayer_setVideoSurface(JNIEnv *env, jobject thiz, jobject surface) {
    if(!xxMediaPlayer){
        return;
    }

    xxMediaPlayer->setSurface((*env).NewGlobalRef(surface));

    if(surface!= nullptr && xxMediaPlayer->isPrepared){
        xxMediaPlayer->getJavaCall()->postEventFromNative(1,XXP_MSG_PREPARED,0,0, nullptr);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_me_xfly_boboplayer_XXMediaPlayer_setFrameAtTime(JNIEnv *env, jobject thiz,
                                                     jstring img_cache_path, jlong start_time,
                                                     jlong end_time, jint num,
                                                     jint img_definition) {
    // TODO: implement setFrameAtTime()
}

extern "C"
JNIEXPORT void JNICALL
Java_me_xfly_boboplayer_XXMediaPlayer__1setDataSource(JNIEnv *env, jobject thiz,
                                                      jstring data_source) {
    if(xxMediaPlayer == NULL){
        return;
    }
    xxMediaPlayer->setDataSource(jstringToChar(env,data_source));
}

extern "C"
JNIEXPORT void JNICALL
Java_me_xfly_boboplayer_XXMediaPlayer__1prepare(JNIEnv *env, jobject thiz) {
    if(xxMediaPlayer == NULL){
        return;
    }

    if(xxMediaPlayer->isPrepared){
        xxMediaPlayer->release();
    }

    if(xxMediaPlayer->prepare() != 0){
        xxMediaPlayer->getJavaCall()->postEventFromNative(1,XXP_MSG_ERROR,0,0, nullptr);
        return;
    }
    if ( xxMediaPlayer->_xx_play->_surface != nullptr){
        xxMediaPlayer->getJavaCall()->postEventFromNative(1,XXP_MSG_PREPARED,0,0, nullptr);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_me_xfly_boboplayer_XXMediaPlayer__1resume(JNIEnv *env, jobject thiz) {
    if(xxMediaPlayer == NULL){
        return;
    }

    xxMediaPlayer->onResume();
}


