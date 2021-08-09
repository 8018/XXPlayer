//
// Created by Neo on 2021/7/9.
//

#include "jvm_android.h"
JVM* g_jvm;

/*// JavaClass implementation.
jmethodID JavaClass::GetMethodId(const char* name, const char* signature) {
    return GetMethodID(jni_, j_class_, name, signature);
}

jmethodID JavaClass::GetStaticMethodId(const char* name,
                                       const char* signature) {
    return GetStaticMethodID(jni_, j_class_, name, signature);
}*/

jobject JavaClass::CallStaticObjectMethod(jmethodID methodID, ...) {
    va_list args;
    va_start(args, methodID);
    jobject res = jni_->CallStaticObjectMethodV(j_class_, methodID, args);
    return res;
}

jint JavaClass::CallStaticIntMethod(jmethodID methodID, ...) {
    va_list args;
    va_start(args, methodID);
    jint res = jni_->CallStaticIntMethodV(j_class_, methodID, args);
    return res;
}

// static
void JVM::Initialize(JavaVM* jvm) {
    g_jvm = new JVM(jvm);
}


// static
void JVM::Uninitialize() {
    delete g_jvm;
    g_jvm = nullptr;
}

// static
JVM* JVM::GetInstance() {
    return g_jvm;
}

JVM::JVM(JavaVM* jvm) : jvm_(jvm) {
}

JVM::~JVM() {
}

void JVM::GetEnv(JavaVM* jvm,JNIEnv *jniEnv) {
    if (jvm->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
//            LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
        return;
    }
}
