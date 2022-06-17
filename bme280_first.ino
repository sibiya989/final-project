
#include <ESP8266WebServer.h>
#include <Wire.h>
//#include <Firebase_ESP_Client.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <FirebaseArduino.h>    
#define SEALEVELPRESSURE_HPA (1013.25)
#define FIREBASE_HOST "test-837fc-default-rtdb.firebaseio.com"                          // the project name address from firebase id
#define FIREBASE_AUTH "wUAxaQmpI8Wyaiz4vWHgFgtgNXxgxaC8PcdYXhy4" 

Adafruit_BME280 bme;

float temperature, humidity, pressure, altitude;
const int sensorPin = 17; 
float sensorState;
float lastState;

float readD1;
float readD2;
int Pin_D4 = 13;
int Pin_D0 = 16;

/*Put your SSID & Password*/
const char* ssid = "VodafoneMobileWiFi-01A925";  // Enter SSID here
const char* password = "8551701125";  //Enter Password here

ESP8266WebServer server(80);              
 
void setup() 
{
  Serial.begin(115200);
  delay(2000);
  
  bme.begin(0x76);   

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED)
  {
  delay(2000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);   

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
  pinMode(sensorPin, INPUT);
  pinMode(Pin_D4, OUTPUT);
  pinMode(Pin_D0, OUTPUT);

}
void loop() 
{
  server.handleClient();

    readD1 = analogRead1(); // Read Analog value of first sensor
   delay(2000);
    readD2 = analogRead2(); // Read Analog value of second sensor
    delay(2000);
   Serial.print("sensor 1 = ");
   Serial.print(readD1);
     Serial.println();
    Serial.print(" sensor 2 = ");
    Serial.println(readD2);
  Firebase.pushString("/sensors/temperature", String(readD1)); 
   Firebase.pushString("/sensors/humidity", String(readD2)); 
   Firebase.setFloat("number", 42.0);



  
}

int analogRead1() {
    digitalWrite(Pin_D4, HIGH); // Turn D1 On
    digitalWrite(Pin_D0, LOW); // Turn D2 Off
    return analogRead(sensorPin);
}
 
int analogRead2() {
    digitalWrite(Pin_D4, LOW); //  Turn D1 On
    digitalWrite(Pin_D0, HIGH); // Turn D2 Off
    return analogRead(sensorPin);
}

void handle_OnConnect() 
{
  readD1 = analogRead1(); // Read Analog value of first sensor
  delay(2000);
  readD2 = analogRead2(); // Read Analog value of second sensor
  sensorState = analogRead(sensorPin);
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  server.send(200, "text/html", SendHTML(temperature,humidity,pressure,altitude,readD1,readD2)); 
  
   Firebase.pushString("/sensors/temperature", String(readD1)); 
   Firebase.pushString("/sensors/humidity", String(readD2)); 
   Firebase.setFloat("number", 42.0);
}

void handle_NotFound()
{
  server.send(404, "text/plain", "Not found"); 
}

String SendHTML(float temperature,float humidity,float pressure,float altitude, float readD1, float readD2)
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP8266 Weather Station</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>ESP8266 Weather Station</h1>\n";
  ptr +="<p>Temperature: ";
  ptr +=temperature;
  ptr +="&deg;C</p>";
  ptr +="<p>Humidity: ";
  ptr +=humidity;
  ptr +="%</p>";
  ptr +="<p>Pressure: ";
  ptr +=pressure;
  ptr +="hPa</p>";
  ptr +="<p>Altitude: ";
  ptr +=altitude;
  ptr +="m</p>";
  ptr +="<p>Tank Level: ";
  ptr +=readD1;
  ptr +="L</p>";
  ptr +="<p>Rain Sensor: ";
  ptr +=readD2;
  ptr +="mm</p>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
