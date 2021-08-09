//
// Created by Neo on 2021/7/9.
//

#ifndef XXPLAYER_JVM_ANDROID_H
#define XXPLAYER_JVM_ANDROID_H
#include <jni.h>
#include <memory>
#include <string>

class JavaClass {
public:
    JavaClass(JNIEnv* jni, jclass clazz) : jni_(jni), j_class_(clazz) {}
    ~JavaClass() {}

    jmethodID GetMethodId(const char* name, const char* signature);
    jmethodID GetStaticMethodId(const char* name, const char* signature);
    jobject CallStaticObjectMethod(jmethodID methodID, ...);
    jint CallStaticIntMethod(jmethodID methodID, ...);

protected:
    JNIEnv* const jni_;
    jclass const j_class_;
};

class JVM {
public:
    // Stores global handles to the Java VM interface.
    // Should be called once on a thread that is attached to the JVM.
    static void Initialize(JavaVM* jvm);
    static void Uninitialize();
    // Gives access to the global Java VM interface pointer, which then can be
    // used to create a valid JNIEnvironment object or to get a JavaClass object.
    static JVM* GetInstance();

    JavaVM* jvm() const { return jvm_; }

protected:
    JVM(JavaVM* jvm);
    ~JVM();

    void GetEnv(JavaVM* jvm,JNIEnv *jniEnv);

private:

    JavaVM* const jvm_;
};
#endif //XXPLAYER_JVM_ANDROID_H
