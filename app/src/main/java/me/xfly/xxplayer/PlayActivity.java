package me.xfly.xxplayer;

import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.widget.TextView;

import java.io.IOException;

public class PlayActivity extends AppCompatActivity implements HolderCallBack {


    String path;
    VideoSurfaceView mSurfaceView;
    XXMediaPlayer mMediaPlayer;
    boolean isStarted = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_play);

        path = getIntent().getStringExtra("path");

        mSurfaceView = findViewById(R.id.video_view);
        mSurfaceView.setHolderCallBack(this);

        TextView textView = findViewById(R.id.text);

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
    protected void onStop() {
        super.onStop();
        mMediaPlayer.finalize();
    }

    @Override
    public void onSurfaceCreated() {
        mMediaPlayer.setSurface(mSurfaceView.getSurface());

    }

    @Override
    public void onSurfaceDestroyed() {
        mMediaPlayer.setSurface(null);
    }

    public static void startActivity(String path,Activity activity){
        Intent intent = new Intent(activity,PlayActivity.class);
        intent.putExtra("path",path);
        activity.startActivity(intent);
    }
}

interface HolderCallBack {
    void onSurfaceCreated();

    void onSurfaceDestroyed();
}
