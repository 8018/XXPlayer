package me.xfly.xxplayer;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.TextView;

import static android.content.pm.PackageManager.PERMISSION_GRANTED;

public class MainActivity extends AppCompatActivity {

    String path = "";

    TextView textView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById(R.id.btn_play).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                int permission = ActivityCompat.checkSelfPermission(MainActivity.this,
                        "android.permission.WRITE_EXTERNAL_STORAGE");
                if (permission != PERMISSION_GRANTED) {
                    return;
                }
                PlayActivity.startActivity(path,MainActivity.this);
            }
        });

        textView = findViewById(R.id.text);

        path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/Black-Widow.mkv";

        path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/miandsu.mp4";
        path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/mi.mp4";

        //path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/yun.mp4";
        path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/ipx-612-c/ipx-612-c.mp4";
        path = "http://tx2play1.douyucdn.cn/live/920891rMypGSnYvc.xs";
        //path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/loki.mp4";
        path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/yun.mp4";
        textView.setText(path);
        verifyStoragePermissions(this);

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
}