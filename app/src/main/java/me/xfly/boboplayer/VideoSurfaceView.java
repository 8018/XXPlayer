package me.xfly.boboplayer;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

public class VideoSurfaceView extends SurfaceView implements SurfaceHolder.Callback {
    SurfaceHolder holder;
    HolderCallBack callBack;

    public VideoSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        holder = getHolder();
        holder.addCallback(this);
    }

    public Surface getSurface() {
        return holder.getSurface();
    }

    public void setHolderCallBack(HolderCallBack callBack) {
        this.callBack = callBack;
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        callBack.onSurfaceCreated();
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        Log.e("surface ", "destroyed");
        callBack.onSurfaceDestroyed();
    }
}
