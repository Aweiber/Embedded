package com.sprd.validationtools.itemstest.aging;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.util.Log;
import android.widget.Toast;

import androidx.annotation.NonNull;

import com.simcom.testtools.R;

public class VibratorAgingTestActivity extends BaseAgingTestActivity {

    private static final String TAG = "VibratorTestActivity_wen.liu";
    private static final int MAX_AMPLITUDE = 255;
    private static final int ACTION_VIBRATION = 1;

    private Vibrator mVibrator;
    private long startTime;
    private VibrationEffect vibrationEffect;

    private Handler handler = new Handler() {
        @Override
        public void handleMessage(@NonNull Message msg) {
            switch (msg.what) {
                case ACTION_VIBRATION:
                    //loop();
                    cancelVibrator();
                    VibratorAgingTestActivity.this.removeCallbacks();
                    callback(true);
                    break;
                default:
                    break;
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setTitle(getString(R.string.vibrator_test));
        Log.d(TAG,"onCreate");
        mVibrator = (Vibrator) this.getSystemService(Context.VIBRATOR_SERVICE);
        vibrationEffect = VibrationEffect.createOneShot(10 * 1000L, MAX_AMPLITUDE);
        startTime = System.currentTimeMillis();
        removeButton();
    }

    @Override
    protected void receivedAgingTestOver() {
        super.receivedAgingTestOver();
        Log.d(TAG, "receivedAgingTestOver");
    }

    @Override
    protected void onResume() {
        Log.i(TAG, "onResume");
        super.onResume();
        loop();
    }

    @Override
    protected void onDestroy() {
        Log.i(TAG, "onDestroy");
        cancelVibrator();
        removeCallbacks();
        super.onDestroy();
    }

    private void loop() {
        Log.i(TAG, "loop");
//        long currentTime = System.currentTimeMillis();
//        if (currentTime - startTime >= Const.MAXIMUM_TIME) {
//            cancelVibrator();
//            removeCallbacks();
//            callback(true);
//        } else {
        startVibrator();
        sendMsg();
        //}
    }

    private Runnable runnable = () -> handler.sendEmptyMessage(ACTION_VIBRATION);

    private void sendMsg() {
        handler.postDelayed(runnable, 5 * 1000L);
    }

    private void startVibrator() {
        if (mVibrator == null || !mVibrator.hasVibrator() || vibrationEffect == null) {
            Toast.makeText(VibratorAgingTestActivity.this,"start Vibrator failed",Toast.LENGTH_SHORT).show();
            Log.i(TAG, "start Vibrator failed");
            postDelayed();
            return;
        }

        try {
            cancelVibrator();
            Log.i(TAG, "startVibrator start");
            mVibrator.vibrate(vibrationEffect);
        } catch (Exception e) {
            Toast.makeText(VibratorAgingTestActivity.this,e.getMessage(),Toast.LENGTH_SHORT).show();
            Log.i(TAG, "startVibrator Exception:" + e.getMessage());
            cancelVibrator();
            postDelayed();
            e.printStackTrace();
        }
    }

    private void postDelayed(){
        handler.postDelayed(() -> {
            callback(false);
            removeCallbacks();
        }, 2000);
    }

    private void cancelVibrator() {
        if (mVibrator != null) {
            Log.i(TAG, "cancel Vibrator");
            mVibrator.cancel();
        }
    }

    private void callback(boolean isPass) {
        Log.i(TAG, "callback isPass:" + isPass);
        storeRusult(isPass);
        finish();
    }

    private void removeCallbacks() {
        if (handler != null) {
            Log.i(TAG, "removeCallbacks");
            handler.removeCallbacks(runnable);
        }
    }
}
