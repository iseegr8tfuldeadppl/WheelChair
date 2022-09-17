package mustshop.arduino.wheelchair;

import androidx.appcompat.app.AppCompatActivity;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.SeekBar;

import java.io.IOException;
import java.net.ConnectException;
import java.net.SocketTimeoutException;
import java.util.ArrayList;
import java.util.List;

import okhttp3.OkHttpClient;
import okhttp3.Request;

public class MainActivity extends AppCompatActivity implements SensorEventListener {

    private Context context;
    private SensorManager senSensorManager;
    private Sensor senAccelerometer;
    private float x=(float) 0.0, y=(float) 0.0, z=(float) 0.0;
    private float previous_x=(float) 0.0, previous_y=(float) 0.0, previous_z=(float) 0.0;
    private boolean gyro_stream_on = true;
    private String robotIP = "192.168.4.1";
    private Thread thread;
    private List<Float> command_queue = new ArrayList<>();

    private ImageView accelerate, brake;

    private int maximal_gyroY_for_steering = 7;
    private int max_speed = 40; // default: 60 // good test value: 10
    private int gyro_to_speed_scalar = max_speed/maximal_gyroY_for_steering; // default: 3 // good test value: 1

    private int left_motor_speed = 0;
    private int right_motor_speed = 0;
    private boolean forth = false, back = false;
    private boolean moving = false;
    private Button lefth, righth, backh, forthh;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        context = this;
        Thread.setDefaultUncaughtExceptionHandler(new TopExceptionHandler());

        forthh = findViewById(R.id.forth);
        backh = findViewById(R.id.back);
        lefth = findViewById(R.id.left);
        righth = findViewById(R.id.right);

        forthh.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                if (motionEvent.getAction()==MotionEvent.ACTION_DOWN){
                    direction = "forth";
                    updated = true;
                }
                if (motionEvent.getAction()==MotionEvent.ACTION_UP){
                    direction = "stop";
                    updated = true;
                }
                return false;
            }
        });
        backh.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                if (motionEvent.getAction()==MotionEvent.ACTION_DOWN){
                    direction = "back";
                    updated = true;
                }
                if (motionEvent.getAction()==MotionEvent.ACTION_UP){
                    direction = "stop";
                    updated = true;
                }
                return false;
            }
        });
        lefth.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                if (motionEvent.getAction()==MotionEvent.ACTION_DOWN){
                    direction = "left";
                    updated = true;
                }
                if (motionEvent.getAction()==MotionEvent.ACTION_UP){
                    direction = "stop";
                    updated = true;
                }
                return false;
            }
        });
        righth.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                if (motionEvent.getAction()==MotionEvent.ACTION_DOWN){
                    direction = "right";
                    updated = true;
                }
                if (motionEvent.getAction()==MotionEvent.ACTION_UP){
                    direction = "stop";
                    updated = true;
                }
                return false;
            }
        });

        accelerate = findViewById(R.id.accelerate);
        brake = findViewById(R.id.brake);

        senSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        senAccelerometer = senSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        senSensorManager.registerListener(this, senAccelerometer , SensorManager.SENSOR_DELAY_NORMAL);

        thread = new Thread(new Runnable() {
            @Override
            public void run() {
                while(gyro_stream_on){
                    try {
                        if(updated){
                            updated =false;
                            if(direction=="forth"){
                                sendCommand("driving/?move=-6=3"); // 9 3
                            } else if(direction=="back"){
                                sendCommand("driving/?move=6=-3");
                            } else if(direction=="left"){
                                sendCommand("driving/?move=-6=-3");
                            } else if(direction=="right"){
                                sendCommand("driving/?move=6=3                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       ");
                            } else if(direction=="stop"){
                                sendCommand("driving/?move=0=0");
                            } else {
                                sendCommand("driving/?move=0=0");
                            }
                        }
                    } catch(Exception e){
                        log(e);
                        e.printStackTrace();
                    }
                }
            }
        });
        thread.start();

        // accelerate button
        accelerate.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                if(motionEvent.getAction() == MotionEvent.ACTION_DOWN){
                    forth = true;
                    return true;
                } else if(motionEvent.getAction() == MotionEvent.ACTION_UP){
                    forth = false;
                    return true;
                }
                return false;
            }
        });

        // accelerate button
        brake.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                if(motionEvent.getAction() == MotionEvent.ACTION_DOWN){
                    back = true;
                    return true;
                } else if(motionEvent.getAction() == MotionEvent.ACTION_UP){
                    back = false;
                    return true;
                }
                return false;
            }
        });
    }


    protected void onPause() {
        super.onPause();
        senSensorManager.unregisterListener(this);
    }


    protected void onResume() {
        super.onResume();
        senSensorManager.registerListener(this, senAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);
    }


    private long previousCommandTimeStamp;
    private int commandTransmissionRate = 3; // per second
    private int commandsPersecond = 1000/commandTransmissionRate; // per second
    @Override
    public void onSensorChanged(SensorEvent event) {
        Sensor mySensor = event.sensor;

        if (mySensor.getType() == Sensor.TYPE_ACCELEROMETER) {
            //x = event.values[0];
            y = Math.round(event.values[1]);
            //z = event.values[2];

            if(gyro_stream_on && System.currentTimeMillis() - previousCommandTimeStamp > commandsPersecond && (back || forth)){

                previousCommandTimeStamp = System.currentTimeMillis();
                command_queue.add(y);
                //if(!thread.isAlive()) thread.start();

            }
        }
    }

    OkHttpClient client = new OkHttpClient();

    void sendCommand(String url) {
        log("sending " + url);
        Request request = new Request.Builder()
                .url("http://" + robotIP + "/" + url)
                .build();

        try {
            client.newCall(request).execute();
        } catch(ConnectException e){
            log("couldn't connect");
        } catch(SocketTimeoutException e){
            log("couldn't connect");
        } catch(IOException e){
            log("error when sending http " + e);
            //log("error when sending http ");
            //e.printStackTrace();
        }

        //try (Response response = client.newCall(request).execute()) {
        //    return response.body().string();
        //} catch(IOException e){
        //            log("error when sending http " + e);
        //            e.printStackTrace();
        //        }
    }


    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {}


    void log(Object log){
        Log.i("HH", String.valueOf(log));
    }

    String direction = "forth";
    boolean updated = false;
}