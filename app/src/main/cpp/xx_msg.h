//
// Created by Neo on 2021/7/9.
//

#ifndef XXPLAYER_XX_MSG_H
#define XXPLAYER_XX_MSG_H

#define XXP_MSG_FLUSH                       0
#define XXP_MSG_PREPARED                    1
#define XXP_MSG_ERROR                       100     /* arg1 = error */
#define XXP_MSG_COMPLETED                   300
#define XXP_MSG_VIDEO_SIZE_CHANGED          400     /* arg1 = width, arg2 = height */
#define XXP_MSG_SAR_CHANGED                 401     /* arg1 = sar.num, arg2 = sar.den */
#define XXP_MSG_VIDEO_RENDERING_START       402
#define XXP_MSG_AUDIO_RENDERING_START       403
#define XXP_MSG_VIDEO_ROTATION_CHANGED      404     /* arg1 = degree */
#define XXP_MSG_AUDIO_DECODED_START         405
#define XXP_MSG_VIDEO_DECODED_START         406
#define XXP_MSG_OPEN_INPUT                  407
#define XXP_MSG_FIND_STREAM_INFO            408
#define XXP_MSG_COMPONENT_OPEN              409
#define XXP_MSG_VIDEO_SEEK_RENDERING_START  410
#define XXP_MSG_AUDIO_SEEK_RENDERING_START  411

#define XXP_MSG_BUFFERING_START             500
#define XXP_MSG_BUFFERING_END               501
#define XXP_MSG_BUFFERING_UPDATE            502     /* arg1 = buffering head position in time, arg2 = minimum percent in time or bytes */
#define XXP_MSG_BUFFERING_BYTES_UPDATE      503     /* arg1 = cached data in bytes,            arg2 = high water mark */
#define XXP_MSG_BUFFERING_TIME_UPDATE       504     /* arg1 = cached duration in milliseconds, arg2 = high water mark */
#define XXP_MSG_SEEK_COMPLETE               600     /* arg1 = seek position,                   arg2 = error */
#define XXP_MSG_PLAYBACK_STATE_CHANGED      700
#define XXP_MSG_TIMED_TEXT                  800
#define XXP_MSG_ACCURATE_SEEK_COMPLETE      900     /* arg1 = current position*/
#define XXP_MSG_GET_IMG_STATE               1000    /* arg1 = timestamp, arg2 = result code, obj = file name*/

#define XXP_MSG_VIDEO_DECODER_OPEN          10001

#define XXP_REQ_START                       20001
#define XXP_REQ_PAUSE                       20002
#define XXP_REQ_SEEK                        20003

#define XXP_PROP_FLOAT_VIDEO_DECODE_FRAMES_PER_SECOND   10001
#define XXP_PROP_FLOAT_VIDEO_OUTPUT_FRAMES_PER_SECOND   10002
#define XXP_PROP_FLOAT_PLAYBACK_RATE                    10003
#define XXP_PROP_FLOAT_PLAYBACK_VOLUME                  10006
#define XXP_PROP_FLOAT_AVDELAY                          10004
#define XXP_PROP_FLOAT_AVDIFF                           10005
#define XXP_PROP_FLOAT_DROP_FRAME_RATE                  10007

#define XXP_PROP_INT64_SELECTED_VIDEO_STREAM            20001
#define XXP_PROP_INT64_SELECTED_AUDIO_STREAM            20002
#define XXP_PROP_INT64_SELECTED_TIMEDTEXT_STREAM        20011
#define XXP_PROP_INT64_VIDEO_DECODER                    20003
#define XXP_PROP_INT64_AUDIO_DECODER                    20004
#define     XXP_PROPV_DECODER_UNKNOWN                   0
#define     XXP_PROPV_DECODER_AVCODEC                   1
#define     XXP_PROPV_DECODER_MEDIACODEC                2
#define     XXP_PROPV_DECODER_VIDEOTOOLBOX              3
#define XXP_PROP_INT64_VIDEO_CACHED_DURATION            20005
#define XXP_PROP_INT64_AUDIO_CACHED_DURATION            20006
#define XXP_PROP_INT64_VIDEO_CACHED_BYTES               20007
#define XXP_PROP_INT64_AUDIO_CACHED_BYTES               20008
#define XXP_PROP_INT64_VIDEO_CACHED_PACKETS             20009
#define XXP_PROP_INT64_AUDIO_CACHED_PACKETS             20010

#define XXP_PROP_INT64_BIT_RATE                         20100

#define XXP_PROP_INT64_TCP_SPEED                        20200

#define XXP_PROP_INT64_ASYNC_STATISTIC_BUF_BACKWARDS    20201
#define XXP_PROP_INT64_ASYNC_STATISTIC_BUF_FORWARDS     20202
#define XXP_PROP_INT64_ASYNC_STATISTIC_BUF_CAPACITY     20203
#define XXP_PROP_INT64_TRAFFIC_STATISTIC_BYTE_COUNT     20204

#define XXP_PROP_INT64_LATEST_SEEK_LOAD_DURATION        20300

#define XXP_PROP_INT64_CACHE_STATISTIC_PHYSICAL_POS     20205

#define XXP_PROP_INT64_CACHE_STATISTIC_FILE_FORWARDS    20206

#define XXP_PROP_INT64_CACHE_STATISTIC_FILE_POS         20207

#define XXP_PROP_INT64_CACHE_STATISTIC_COUNT_BYTES      20208

#define XXP_PROP_INT64_LOGICAL_FILE_SIZE                20209
#define XXP_PROP_INT64_SHARE_CACHE_DATA                 20210
#define XXP_PROP_INT64_IMMEDIATE_RECONNECT              20211

#endif //XXPLAYER_XX_MSG_H
