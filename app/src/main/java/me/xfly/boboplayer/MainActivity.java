package me.xfly.boboplayer;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.animation.ObjectAnimator;
import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.widget.TextView;

import java.io.IOException;


import static android.content.pm.PackageManager.*;

public class MainActivity extends AppCompatActivity implements HolderCallBack {


    String path;
    VideoSurfaceView mSurfaceView;
    XXMediaPlayer mMediaPlayer;
    boolean isStarted = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        verifyStoragePermissions(this);

        path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/Black-Widow.mkv";

        path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/miandsu.mp4";
        path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/mi.mp4";

        //path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/yun.mp4";
        path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/ipx-612-c/ipx-612-c.mp4";
        path = "http://tx2play1.douyucdn.cn/live/920891rMypGSnYvc.xs";
        //path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/loki.mp4";
        //path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/yun.mp4";

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

        // 透明度动画
        ObjectAnimator.ofFloat(textView, "alpha", 1, 0, 1)
                .setDuration(4000)
                .start();
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

    private static final int REQUEST_EXTERNAL_STORAGE = 1;
    private static String[] PERMISSIONS_STORAGE = {
            "android.permission.READ_EXTERNAL_STORAGE",
            "android.permission.WRITE_EXTERNAL_STORAGE"};


    public static void verifyStoragePermissions(Activity activity) {

        try {
            //检测是否有写的权限
            int permission = ActivityCompat.checkSelfPermission(activity,
                    "android.permission.WRITE_EXTERNAL_STORAGE");
            if (permission != PERMISSION_GRANTED) {
                // 没有写的权限，去申请写的权限，会弹出对话框
                ActivityCompat.requestPermissions(activity, PERMISSIONS_STORAGE, REQUEST_EXTERNAL_STORAGE);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onSurfaceCreated() {
        mMediaPlayer.setSurface(mSurfaceView.getSurface());

    }

    @Override
    public void onSurfaceDestroyed() {
        mMediaPlayer.setSurface(null);
    }
}

interface HolderCallBack {
    void onSurfaceCreated();

    void onSurfaceDestroyed();
}
