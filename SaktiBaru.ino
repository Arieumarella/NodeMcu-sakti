#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include  <LiquidCrystal.h>


//SSID & Password Wifi
const char *ssid = "POGUNG GAMING"; 
const char *pass = "YOWHAT'SUPGUYS";

//Device key
String deviceKey = "DC-49711";

//Host API
String getDeviceHost = "http://192.168.100.199/IoT-Sakti/Api/device";
String responDaftar1Host = "http://192.168.100.199/IoT-Sakti/Api/respondDevice1";
String responDaftar2Host = "http://192.168.100.199/IoT-Sakti/Api/respondDevice2";
String resetDeviceHost   = "http://192.168.100.199/IoT-Sakti/Api/risetDevice";

WiFiClient client;
void setup()
{
Serial.begin(9600);
delay(10);

connectToWifi();


}
void loop() 
{
  getDevice();
 
}

//Function getDevice
void getDevice()
{
  HTTPClient http;
  http.begin (getDeviceHost);  
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "device_key="+deviceKey;
int httpCode = http.POST(httpRequestData);
 
 
 
String payload = http.getString();

char json[700];
payload.toCharArray(json, 700);
StaticJsonDocument<300> doc;
deserializeJson(doc, json);

String massage = doc["massage"][0];

Serial.println(httpCode);
Serial.println ("Ini Hasil request");
Serial.println(payload);

Serial.println ("Ini Hasil Parsing");
Serial.println(massage);
          
 


http.end();
delay(10000);  

}

//Function responDaftar1
void responDaftar1()
{
  HTTPClient http;
  http.begin (responDaftar1Host);  
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "device_key="+deviceKey;
  int httpCode = http.POST(httpRequestData);
 
 
 
String payload = http.getString();
Serial.println(httpCode);
Serial.print ("Ini Pesan Nya");
Serial.println(payload);          
 


http.end();
delay(10000);  

}

//Function responDaftar2
void responDaftar2()
{
  HTTPClient http;
  http.begin (responDaftar2Host);  
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "device_key="+deviceKey;
int httpCode = http.POST(httpRequestData);
 
 
 
String payload = http.getString();
Serial.println(httpCode);
Serial.print ("Ini Pesan Nya");
Serial.println(payload);          
 


http.end();
delay(10000);  

}


//Function resetDevice 
void resetDevice()
{
  HTTPClient http;
  http.begin (resetDeviceHost);  
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "device_key="+deviceKey;
int httpCode = http.POST(httpRequestData);
 
 
 
String payload = http.getString();
Serial.println(httpCode);
Serial.print ("Ini Pesan Nya");
Serial.println(payload);          
 


http.end();
delay(10000);  

}

//Function Konek Wifi
void connectToWifi()
{
  
Serial.print(" Connect to : ");
Serial.println(ssid);
WiFi.begin(ssid, pass);
while (WiFi.status() != WL_CONNECTED)
{
  delay(500);
  Serial.print("….");
}
Serial.print("\n");
Serial.print("IP address : ");
Serial.print(WiFi.localIP());
Serial.print("\n");
Serial.print("Connect to : ");
Serial.println(ssid);
}
