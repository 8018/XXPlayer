package me.xfly.xxplayer;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Rect;
import android.net.Uri;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.PowerManager;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;

import java.io.FileDescriptor;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.Map;

public class XXMediaPlayer extends AbstractMediaPlayer {

    private final static String TAG = XXMediaPlayer.class.getName();

    private static final int MEDIA_NOP = 0; // interface test message
    private static final int MEDIA_PREPARED = 1;
    private static final int MEDIA_PLAYBACK_COMPLETE = 2;
    private static final int MEDIA_BUFFERING_UPDATE = 3;
    private static final int MEDIA_SEEK_COMPLETE = 4;
    private static final int MEDIA_SET_VIDEO_SIZE = 5;
    private static final int MEDIA_TIMED_TEXT = 99;
    private static final int MEDIA_ERROR = 100;
    private static final int MEDIA_INFO = 200;

    protected static final int MEDIA_SET_VIDEO_SAR = 10001;


    public static final int FFP_PROPV_DECODER_AVCODEC = 1;

    private SurfaceHolder mSurfaceHolder;
    private EventHandler mEventHandler;
    private PowerManager.WakeLock mWakeLock = null;
    private boolean mScreenOnWhilePlaying;
    private boolean mStayAwake;
    private XXMediaCodec mXXMediaCodec;

    private int mVideoWidth;
    private int mVideoHeight;
    private int mVideoSarNum;
    private int mVideoSarDen;

    private String mDataSource;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("xxplayer_jni");
    }

    public XXMediaPlayer(long time) {
        mXXMediaCodec = new XXMediaCodec();
        native_init(new WeakReference<XXMediaPlayer>(this),mXXMediaCodec);
        mXXMediaCodec.time = time;
        mEventHandler = new EventHandler(this, Looper.getMainLooper());
    }

    @Override
    public void setDisplay(SurfaceHolder sh) {

    }

    @Override
    public void setDataSource(Context context, Uri uri) throws IOException, IllegalArgumentException, SecurityException, IllegalStateException {

    }

    @Override
    public void setDataSource(Context context, Uri uri, Map<String, String> headers) throws IOException, IllegalArgumentException, SecurityException, IllegalStateException {

    }

    @Override
    public void setDataSource(FileDescriptor fd) throws IOException, IllegalArgumentException, IllegalStateException {

    }

    @Override
    public void setDataSource(String path) throws IOException, IllegalArgumentException, SecurityException, IllegalStateException {
        mDataSource = path;
        _setDataSource(path);
        _prepare();
    }

    @Override
    public String getDataSource() {
        return null;
    }

    @Override
    public int getVideoWidth() {
        return 0;
    }

    @Override
    public int getVideoHeight() {
        return 0;
    }

    @Override
    public boolean isPlaying() {
        return false;
    }

    @Override
    public void seekTo(long msec) throws IllegalStateException {

    }

    @Override
    public long getCurrentPosition() {
        return 0;
    }

    @Override
    public long getDuration() {
        return 0;
    }

    @Override
    public void release() {

    }

    @Override
    public void reset() {

    }

    @Override
    public void setVolume(float leftVolume, float rightVolume) {

    }

    @Override
    public int getAudioSessionId() {
        return 0;
    }

    @Override
    public MediaInfo getMediaInfo() {
        return null;
    }

    @Override
    public void setLogEnabled(boolean enable) {

    }

    @Override
    public boolean isPlayable() {
        return false;
    }

    @Override
    public void setAudioStreamType(int streamtype) {

    }

    @Override
    public void setKeepInBackground(boolean keepInBackground) {

    }

    @Override
    public int getVideoSarNum() {
        return 0;
    }

    @Override
    public int getVideoSarDen() {
        return 0;
    }

    @Override
    public void setLooping(boolean looping) {

    }

    @Override
    public boolean isLooping() {
        return false;
    }

    @Override
    public void setSurface(Surface surface) {
        mXXMediaCodec.setSurface(surface);
        setVideoSurface(surface);
    }

    public void finalize(){
        stayAwake(false);
        native_finalize();
    }

    public static native void native_init(Object XXMediaPlayer_this,Object XXMediaCodec);

    private native void native_finalize();

    private static class EventHandler extends Handler {
        private final WeakReference<XXMediaPlayer> mWeakPlayer;

        public EventHandler(XXMediaPlayer mp, Looper looper) {
            super(looper);
            mWeakPlayer = new WeakReference<XXMediaPlayer>(mp);
        }

        @Override
        public void handleMessage(Message msg) {
            XXMediaPlayer player = mWeakPlayer.get();
            if (player == null) {
                Log.w(TAG,
                        "XXMediaPlayer went away with unhandled events");
                return;
            }

            switch (msg.what) {
                case MEDIA_PREPARED:
                    player.notifyOnPrepared();
                    return;

                case MEDIA_PLAYBACK_COMPLETE:
                    player.stayAwake(false);
                    player.notifyOnCompletion();
                    return;

                case MEDIA_BUFFERING_UPDATE:
                    long bufferPosition = msg.arg1;
                    if (bufferPosition < 0) {
                        bufferPosition = 0;
                    }

                    long percent = 0;
                    long duration = player.getDuration();
                    if (duration > 0) {
                        percent = bufferPosition * 100 / duration;
                    }
                    if (percent >= 100) {
                        percent = 100;
                    }

                    // DebugLog.efmt(TAG, "Buffer (%d%%) %d/%d",  percent, bufferPosition, duration);
                    player.notifyOnBufferingUpdate((int) percent);
                    return;

                case MEDIA_SEEK_COMPLETE:
                    player.notifyOnSeekComplete();
                    return;

                case MEDIA_SET_VIDEO_SIZE:
                    player.mVideoWidth = msg.arg1;
                    player.mVideoHeight = msg.arg2;
                    player.notifyOnVideoSizeChanged(player.mVideoWidth, player.mVideoHeight,
                            player.mVideoSarNum, player.mVideoSarDen);
                    return;

                case MEDIA_ERROR:
                    Log.e(TAG, "Error (" + msg.arg1 + "," + msg.arg2 + ")");
                    if (!player.notifyOnError(msg.arg1, msg.arg2)) {
                        player.notifyOnCompletion();
                    }
                    player.stayAwake(false);
                    return;

                case MEDIA_INFO:
                    switch (msg.arg1) {
                        case MEDIA_INFO_VIDEO_RENDERING_START:
                            Log.i(TAG, "Info: MEDIA_INFO_VIDEO_RENDERING_START\n");
                            break;
                    }
                    player.notifyOnInfo(msg.arg1, msg.arg2);
                    // No real default action so far.
                    return;
                case MEDIA_TIMED_TEXT:
                    if (msg.obj == null) {
                        player.notifyOnTimedText(null);
                    } else {
                        TimedText text = new TimedText(new Rect(0, 0, 1, 1), (String) msg.obj);
                        player.notifyOnTimedText(text);
                    }
                    return;
                case MEDIA_NOP: // interface test message - ignore
                    break;

                case MEDIA_SET_VIDEO_SAR:
                    player.mVideoSarNum = msg.arg1;
                    player.mVideoSarDen = msg.arg2;
                    player.notifyOnVideoSizeChanged(player.mVideoWidth, player.mVideoHeight,
                            player.mVideoSarNum, player.mVideoSarDen);
                    break;

                default:
                    Log.e(TAG, "Unknown message type " + msg.what);
            }
        }
    }

    /*
     * Called from native code when an interesting event happens. This method
     * just uses the EventHandler system to post the event back to the main app
     * thread. We use a weak reference to the original XXMediaPlayer object so
     * that the native code is safe from the object disappearing from underneath
     * it. (This is the cookie passed to native_setup().)
     */
    public static void postEventFromNative(Object weakThiz, int what,
                                           int arg1, int arg2, Object obj) {
        if (weakThiz == null)
            return;

        XXMediaPlayer mp = (XXMediaPlayer) ((WeakReference) weakThiz).get();
        if (mp == null) {
            return;
        }

        if (mp.mEventHandler != null) {
            Message m = mp.mEventHandler.obtainMessage(what, arg1, arg2, obj);
            mp.mEventHandler.sendMessage(m);
        }
    }


    @Override
    public void prepareAsync() throws IllegalStateException {
        _prepareAsync();
    }

    public native void _prepareAsync() throws IllegalStateException;

    @Override
    public void start() throws IllegalStateException {
        stayAwake(true);
        _start();
    }

    private native void _start() throws IllegalStateException;

    @Override
    public void resume() throws IllegalStateException {
        stayAwake(true);
        _resume();
    }

    private native void _resume() throws IllegalStateException;

    private native void _prepare() throws IllegalStateException;

    @Override
    public void stop() throws IllegalStateException {
        stayAwake(false);
        _stop();
    }

    private native void _stop() throws IllegalStateException;

    @Override
    public void pause() throws IllegalStateException {
        stayAwake(false);
        _pause();
    }

    private native void _pause() throws IllegalStateException;

    private native void _setDataSource(String dataSource);

    @SuppressLint("Wakelock")
    @Override
    public void setWakeMode(Context context, int mode) {
        boolean washeld = false;
        if (mWakeLock != null) {
            if (mWakeLock.isHeld()) {
                washeld = true;
                mWakeLock.release();
            }
            mWakeLock = null;
        }

        PowerManager pm = (PowerManager) context
                .getSystemService(Context.POWER_SERVICE);
        mWakeLock = pm.newWakeLock(mode | PowerManager.ON_AFTER_RELEASE,
                XXMediaPlayer.class.getName());
        mWakeLock.setReferenceCounted(false);
        if (washeld) {
            mWakeLock.acquire();
        }
    }

    @Override
    public void setScreenOnWhilePlaying(boolean screenOn) {
        if (mScreenOnWhilePlaying != screenOn) {
            if (screenOn && mSurfaceHolder == null) {
                Log.w(TAG,
                        "setScreenOnWhilePlaying(true) is ineffective without a SurfaceHolder");
            }
            mScreenOnWhilePlaying = screenOn;
            updateSurfaceScreenOn();
        }
    }

    @SuppressLint("Wakelock")
    private void stayAwake(boolean awake) {
        if (mWakeLock != null) {
            if (awake && !mWakeLock.isHeld()) {
                mWakeLock.acquire();
            } else if (!awake && mWakeLock.isHeld()) {
                mWakeLock.release();
            }
        }
        mStayAwake = awake;
        updateSurfaceScreenOn();
    }

    private void updateSurfaceScreenOn() {
        if (mSurfaceHolder != null) {
            mSurfaceHolder.setKeepScreenOn(mScreenOnWhilePlaying && mStayAwake);
        }
    }

    // TODO: @Override
    public void selectTrack(int track) {
        _setStreamSelected(track, true);
    }

    // TODO: @Override
    public void deselectTrack(int track) {
        _setStreamSelected(track, false);
    }

    private native void _setStreamSelected(int stream, boolean select);

    private native void setFrameAtTime(String imgCachePath, long startTime, long endTime, int num, int imgDefinition)
            throws IllegalArgumentException, IllegalStateException;

    private native void setVideoSurface(Surface surface);
}
