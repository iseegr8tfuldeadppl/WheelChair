#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
//#include <EEPROM.h>

Servo servo;
Servo servo2;
int rest_servo = 93, rest_servo2 = 15;
int current_servo_delta = 0, current_servo2_delta = 0;
long value, value2;

WiFiClient client;
ESP8266WebServer server(80);

const char* ssid = "WheelChair - MustShop Arduino";   //enter your wi-fi user id
const char* password = "88888888";  //enter the wi-fi password

int servoPin = 5; /* GPIO14(D5) */
int servo2Pin = 4; /* GPIO12(D6) */


//boolean Connect = true;
void connectWiFi() {

  // login to a wifi
  /*if(Connect){
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while ((!(WiFi.status() == WL_CONNECTED))) {
      delay(300);
      //Serial.print("..");
    }
    //Serial.print((WiFi.localIP()));
  } else {*/
    // create a wifi
    WiFi.softAP(ssid, password);
    //server.begin();
  //}
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

const int arrSize = 10;
String* angles = new String[arrSize]; // according to the largest string array we'll need (ex. ANGLES 90 90 90 90 90)


unsigned int j = 0;
void getValues(String msg){
    j = 0;
    angles[0] = "";
    for (unsigned int i = 0; i < msg.length(); i++) {
        if(j>=arrSize)
          break;
        if(msg[i]=='='){ j += 1; angles[j] = ""; }
        else angles[j] += msg[i];
    }
}


void processCommand(String command, String data) {

  if (command == "MOVE") { // MOVE <speed in delta out of 90 (because rest angle is in the middle)>
    
    getValues(data);
    String str = angles[0];   value = str.toInt();

    current_servo_delta = value;

    str = angles[1];   value = str.toInt();

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

void handleDriving(){
  server.send(200, "text/plain", "ok");
  processCommand("MOVE", server.arg("move"));
}

void handleRoot(){
  server.send(200, "text/plain", "ok");
}


void setup(void) {
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

  /*WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }*/
  servo.attach(servoPin);
  servo2.attach(servo2Pin);
  setAngles(0, 0);

  boolean result = WiFi.softAP(ssid, password);
  if(result == true){
    Serial.println("Ready");
  } else {
    Serial.println("Failed!");
  }
  
  server.on("/", handleRoot);
  server.on("/driving/", handleDriving);
  //server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}
  
void loop() {
  server.handleClient();
}
