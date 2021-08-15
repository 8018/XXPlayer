package me.xfly.xxplayer;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.content.Intent;
import android.media.MediaCodec;
import android.os.Bundle;
import android.os.PowerManager;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.WindowManager;
import android.widget.TextView;

import java.io.IOException;

public class PlayActivity extends AppCompatActivity implements SurfaceHolder.Callback {


    String path;
    SurfaceView mSurfaceView;
    XXMediaPlayer mMediaPlayer;
    boolean isStarted = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_play);

        path = getIntent().getStringExtra("path");

        mSurfaceView = findViewById(R.id.video_view);
        mSurfaceView.getHolder().addCallback(this);

        mMediaPlayer = new XXMediaPlayer();
        mMediaPlayer.setOnPreparedListener(mp -> {
            isStarted = true;
            mMediaPlayer.start();
        });
        try {
            mMediaPlayer.setDataSource(path);
        } catch (IOException e) {
            e.printStackTrace();
        }
        mMediaPlayer.setWakeMode(this, PowerManager.SCREEN_BRIGHT_WAKE_LOCK);

    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mMediaPlayer.pause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMediaPlayer.finalize();
    }

    public static void startActivity(String path,Activity activity){
        Intent intent = new Intent(activity,PlayActivity.class);
        intent.putExtra("path",path);
        activity.startActivity(intent);
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        mMediaPlayer.setSurface(holder.getSurface());
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        mMediaPlayer.setSurface(null);
    }
}

