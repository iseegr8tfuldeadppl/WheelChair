#include <Servo.h>
#include <ESP8266WiFi.h>
//#include <EEPROM.h>

Servo servo;
Servo servo2;
int rest_servo = 90, rest_servo2 = 85;
int current_servo_delta = 0, current_servo2_delta = 0;
long value, value2;

WiFiClient client;
WiFiServer server(80);

const char* ssid = "SubNetBroad";   //enter your wi-fi user id
const char* password = "privatesetgetcast2069@@";  //enter the wi-fi password

//const char* ssid = "Robot WheelChair 1337";   //enter your wi-fi user id
//const char* password = "robotwheelchair1337";  //enter the wi-fi password

int servoPin = 14; /* GPIO14(D5) */
int servo2Pin = 12; /* GPIO12(D6) */


//boolean Connect = true;
void connectWiFi() {

  // login to a wifi
  //if(Connect){
    WiFi.begin(ssid, password);
    while ((!(WiFi.status() == WL_CONNECTED))) {
      delay(300);
      //Serial.print("..");
    }
    //Serial.print((WiFi.localIP()));
  /*} else {
    // create a wifi
    WiFi.softAP(ssid, password);
    server.begin();
    }*/
}


String checkClient (void) {
  while (!client.available()) delay(1);
  String request = client.readStringUntil('\r');
  server.send(200, "text/plain", "gucci!"); // \r\n
  //Serial.println("request " + String(request));
  request.remove(0, 5);
  request.remove(request.length() - 9, 9);
  return request;
}


void setAngles(int servo_delta, int servo2_delta) {
  servo.write(rest_servo + servo_delta);
  servo2.write(rest_servo2 + servo2_delta);
}


String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


void processCommand(String request) {
  String command = getValue(request, '=', 0);

  if (command == "MOVE") { // MOVE <speed in delta out of 90 (because rest angle is in the middle)>
    String str = getValue(request, '=', 1);   value = str.toInt();

    if (isnan(value))
      value = current_servo_delta;

    current_servo_delta = value;

    str = getValue(request, '=', 2);   value = str.toInt();
    if (isnan(value))
      value = current_servo_delta;

    current_servo2_delta = value;
    
    setAngles(current_servo_delta, current_servo2_delta);

  } /*else if (command == "SETRESTANGLES") { // SETRESTANGLES
    
    rest_servo = rest_servo + current_servo_delta;
    current_servo_delta = 0;
    
    rest_servo2 = rest_servo2 + current_servo2_delta;
    current_servo2_delta = 0;
    
    saveRestValues();
  }*/

  //Serial.println("Nope, unrecognized command " + String(command) + " with request " + String(request));
}


/*void loadRestValues() {
  EEPROM.get( 10, rest_servo );
  EEPROM.get( 20, rest_servo2 );
  //Serial.println("Loaded values " + String(firstV) + " " + String(secondV) + " "+ String(thirdV) + " " + String(fourthV) + " " + String(fifthV));
}


void saveRestValues() {
  EEPROM.put( 10, rest_servo );
  EEPROM.put( 20, rest_servo2 );
}*/


void setup() {
  Serial.begin(9600);

  // check if this is the first time this arduino was used for this code, if so pre-save default values
  /*float first_time = 0.0;
  EEPROM.get( 30, first_time );
  Serial.println(first_time);

  // first time launch
  if (isnan(first_time) || first_time != 6.9) {
    EEPROM.put( 30, 6.9 ); // proves db was inited
    saveRestValues();
    Serial.println("First time inited");
  } else {
    loadRestValues();
  }*/

  servo.attach(servoPin);
  servo2.attach(servo2Pin);
  setAngles(0, 0);

  connectWiFi();
  server.begin();
}


/*void detectCommand(){
  if (Serial.available() > 0) {
    String request = Serial.readStringUntil('\n');
    processCommand(request);
  }
}*/

  
void loop() {
  //detectCommand();

  client = server.available();
  if (!client) return;
  processCommand(checkClient());
}
