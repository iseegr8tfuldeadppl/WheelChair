#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

Servo servo, servo2;
int rest_servo = 90, rest_servo2 = 85;
int current_servo_delta = 0, current_servo2_delta = 0;
long value;

WiFiClient client;
ESP8266WebServer server(80);

const char* ssid = "WheelChair - MustShop Arduino";
const char* password = "88888888";


String checkClient (void) {
  while (!client.available()) delay(1);
  String request = client.readStringUntil('\r');
  server.send(200, "text/plain", "gucci!"); // \r\n
  //Serial.println("request " + String(request));
  request.remove(0, 5);
  request.remove(request.length() - 9, 9);
  return request;
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

void handleRoot(){
  server.send(200, "text/plain", "ok");
}

void setAngles(int servo_delta, int servo2_delta) {
  servo.write(rest_servo + servo_delta);
  servo2.write(rest_servo2 + servo2_delta);
}

void handleDriving(){
  server.send(200, "text/plain", "ok");
  String data = server.arg("move");
  
  String str = getValue(data, '=', 0);
  value = str.toInt();
  if (isnan(value))
    value = current_servo_delta;
  current_servo_delta = value;
  
  str = getValue(data, '=', 1);
  value = str.toInt();
  if (isnan(value))
    value = current_servo_delta;
  current_servo2_delta = value;
  
  setAngles(current_servo_delta, current_servo2_delta);
}


void setup(void) {
  Serial.begin(9600);

  servo.attach(D0);
  servo2.attach(D1);
  setAngles(0, 0);

  boolean result = WiFi.softAP(ssid, password);

  server.on("/", handleRoot);
  server.on("/driving/", handleDriving);
  server.begin();
}
  
void loop() {
  server.handleClient();
}
