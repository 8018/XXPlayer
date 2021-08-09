# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

include_directories(${CMAKE_SOURCE_DIR}/includes/ffmpeg)

add_library( # Sets the name of the library.
        xxplayer_jni
        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        xx_play.cpp
        xxplayer_jni.cpp
        jvm_android.cpp
        xx_media_extractor.cpp
        xx_media_player.cpp
        xx_msg_queue.h
        opensl_audio_render.cpp
        ffmpeg_audio_decoder.cpp
        ffmpeg_video_decoder.cpp
        xx_media_channel.cpp
        native_window_video_render.cpp
        java_call.cpp
        )


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

set(ABIPATH ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})

add_library(avformat SHARED IMPORTED)
add_library(avcodec SHARED IMPORTED)
add_library(avfilter SHARED IMPORTED)
add_library(avutil SHARED IMPORTED)
add_library(swscale SHARED IMPORTED)
add_library(swresample SHARED IMPORTED)
set_target_properties(avformat PROPERTIES IMPORTED_LOCATION ${ABIPATH}/libavformat.so)
set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION ${ABIPATH}/libavcodec.so)
set_target_properties(avfilter PROPERTIES IMPORTED_LOCATION ${ABIPATH}/libavfilter.so)
set_target_properties(avutil PROPERTIES IMPORTED_LOCATION ${ABIPATH}/libavutil.so)
set_target_properties(swscale PROPERTIES IMPORTED_LOCATION ${ABIPATH}/libswscale.so)
set_target_properties(swresample PROPERTIES IMPORTED_LOCATION ${ABIPATH}/libswresample.so)

#add_subdirectory("BOBOThread")

target_link_libraries( # Specifies the target library.
        xxplayer_jni
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        # 注意有先后顺序
        avformat
        avcodec
        avfilter
        avutil
        swresample
        swscale
        android
        OpenSLES
        )

