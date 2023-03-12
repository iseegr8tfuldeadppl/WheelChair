#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>


// APP: global variables
const char* ssid = "WheelChair - MustShop Arduino";
const char* password = "88888888";
WiFiClient client;
ESP8266WebServer server(80);
long value;

// WHEELS: global variables
Servo servo, servo2;
int rest_servo = 90, rest_servo2 = 85;
int current_servo_delta = 0, current_servo2_delta = 0;

// WHEELS: pwm pins
#define LEFT_WHEEL_PIN D1
#define RIGHT_WHEEL_PIN D2

// ULTRASONIC: digital pins
#define ULTRASONIC_TRIGGER_PIN D6
#define ULTRASONIC_ECHO_PIN D5


// ULTRASONIC: global variables
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701
long duration;
float distanceCm;
float minimumAntiCollisionDistanceCm = 30.0;
long ultrasonicPeriod = 500;
long lastUltrasonicPing = 0;
bool allow_driving = true;





void setup(void) {

  // SERIAL: init serial
  Serial.begin(9600);

  // ULTRASONIC: init digital pins
  pinMode(ULTRASONIC_TRIGGER_PIN, OUTPUT); // Sets the trigPin as an Output
  pinMode(ULTRASONIC_ECHO_PIN, INPUT); // Sets the echoPin as an Input

  // WHEELS: init pwm pins
  servo.attach(LEFT_WHEEL_PIN);
  servo2.attach(RIGHT_WHEEL_PIN);

  // WHEELS: set 360 servo mouvement speed to zero
  setAngles(0, 0);

  // APP: initialize ESP8266 wifi
  boolean result = WiFi.softAP(ssid, password);

  // APP: initialize server routes
  server.on("/driving/", handleDriving);

  // APP: start server
  server.begin();
  
}


void loop() {

  // APP: serve requests
  server.handleClient();

  // ULTRASONIC: measure every little bit of time
  if(millis() - lastUltrasonicPing >= ultrasonicPeriod){
    // Clears the trigPin
    digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(ULTRASONIC_TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);
    
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);
    
    // Calculate the distance
    distanceCm = duration * SOUND_VELOCITY/2;
    
    // Prints the distance on the Serial Monitor
    allow_driving = distanceCm > minimumAntiCollisionDistanceCm;
  }

  // ULTRASONIC & WHEELS: don't let the car move if the sensor is near a wall
  if(allow_driving){
    setAngles(current_servo_delta, current_servo2_delta);
  } else {
    setAngles(0, 0);
  }
}



// serial communication variables
const int arrSize = 3;
String* parts = new String[arrSize]; // according to the largest string array we'll need (ex. ANGLES 90 90 90 90 90)


unsigned int j = 0;
void getValues(String msg){
    j = 0;
    parts[0] = "";
    for (unsigned int i = 0; i < msg.length(); i++) {
        if(j>=arrSize)
          break;
        if(msg[i]=='='){ j += 1; parts[j] = ""; }
        else parts[j] += msg[i];
    }
}

void setAngles(int servo_delta, int servo2_delta) {
  servo.write(rest_servo + servo_delta);
  servo2.write(rest_servo2 + servo2_delta);
}

void handleDriving(){

  // STEP 1: tell the  app we received it cause it's on a timer
  server.send(200, "text/plain", "ok");

  // STEP 2: obtain parameter from URL (url example: http://192.168.4.1/?move=90=78
  String data = server.arg("move");

  // STEP 3: split string by =
  getValues(data);

  // STEP 4: convert each of them into integers
  value = parts[0].toInt();
  current_servo_delta = value;
  
  value = parts[1].toInt();
  current_servo2_delta = value;

  // STEP 5: apply the speed
  // the speed is applied in the loop function
}
