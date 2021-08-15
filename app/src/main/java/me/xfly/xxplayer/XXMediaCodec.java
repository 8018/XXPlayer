package me.xfly.xxplayer;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.util.Log;
import android.view.Surface;

import java.nio.ByteBuffer;

public class XXMediaCodec {
    MediaCodec mediaCodec;
    MediaFormat mediaFormat;
    Surface surface;
    long time = 0;
    int i;

    public void setSurface(Surface surface) {
        this.surface = surface;
    }

    public void onInit(int mimetype, int width, int height, byte[] csd0, byte[] csd1) {
        if (surface != null) {
            try {
                String mimeType = getMimeType(mimetype);
                mediaFormat = MediaFormat.createVideoFormat(mimeType, width, height);
                mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(csd0));
                mediaFormat.setByteBuffer("csd-1", ByteBuffer.wrap(csd1));
                mediaCodec = MediaCodec.createDecoderByType(mimeType);
                mediaCodec.configure(mediaFormat, surface, null, 0);
                mediaCodec.start();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    public void onDecode(byte[] bytes, int size, int pts) {
        long time2 = System.currentTimeMillis();
        time = time2;
        MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();

        if (bytes != null && mediaCodec != null && info != null) {
            try {
                int inputBufferIndex = mediaCodec.dequeueInputBuffer(10);

                if (inputBufferIndex >= 0) {
                    ByteBuffer buffer = mediaCodec.getInputBuffer(inputBufferIndex);
                    buffer.clear();
                    buffer.put(bytes);
                    mediaCodec.queueInputBuffer(inputBufferIndex, 0, size, pts, 0);
                }

                int index = mediaCodec.dequeueOutputBuffer(info, 10);

                while (index >= 0) {
                    mediaCodec.releaseOutputBuffer(index, true);
                    index = mediaCodec.dequeueOutputBuffer(info, 10);
                }


            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    public void onRelease() {
        try {
            mediaCodec.flush();
            mediaCodec.stop();
            mediaCodec.release();
        } catch (Exception e) {
            e.printStackTrace();
        }
        mediaCodec = null;
        mediaFormat = null;
    }

    private String getMimeType(int type) {
        if (type == 1) {
            return "video/avc";
        } else if (type == 2) {
            return "video/hevc";
        } else if (type == 3) {
            return "video/mp4v-es";
        }
        return "";
    }
}
