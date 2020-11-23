#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(D5, D6);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

LiquidCrystal_I2C lcd(0x27, 16, 2);

//SSID & Password Wifi
const char *ssid = "POGUNG GAMING"; 
const char *pass = "YOWHAT'SUPGUYS";

//Device key
String deviceKey = "DC-49711";

//Data Status
String statusDoor = "";

//Host API
String getDeviceHost = "http://192.168.100.199/Iot-Sakti/Api/device";
String responDaftar1Host = "http://192.168.100.199/Iot-Sakti/Api/respondDevice1";
String responDaftar2Host = "http://192.168.100.199/Iot-Sakti/Api/respondDevice2";
String resetDeviceHost   = "http://192.168.100.199/Iot-Sakti/Api/risetDevice";
String taapingHost = "http://192.168.100.199/Iot-Sakti/Api/taping";

WiFiClient client;

// Millis()
unsigned long interval = 1000;
unsigned long waktuSebelum = 0;

unsigned long interval2 = 50;
unsigned long waktuSebelum2 = 0;



void setup()
{ 
Serial.begin(9600);
while (!Serial); 
lcd.begin();

startLCD();
delay(2000);

lcd.clear();
delay(2000);

finger.begin(57600);


welcome();
delay(3000);


lcd.clear();
connectToWifi();
delay(1000);
lcd.clear();

 lcd.setCursor(4,0);
 lcd.print("Ceking..");
 lcd.setCursor(2,1);
 lcd.print("Mode Device.");
 delay(2000);

 if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  Serial.println("Waiting for valid finger...");


}



void loop() 
{

 cekMode();
  
}



void startLCD()
{
  lcd.setCursor(2,0);
  lcd.print("Set Up ..");
  lcd.setCursor(2,1);
  lcd.print("Sistem.");
}

void welcome()
{
  lcd.setCursor(5,0);
  lcd.print("WELCOME");
  lcd.setCursor(3,1);
  lcd.print("IoT - Sakti");
 
}


void selenoitOK()
{
 
    
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




//Function Ceking Mode Device
void cekMode()
{
unsigned long waktuSekarang = millis();
unsigned long waktuSekarang2 = millis();
  
if ((unsigned long)(waktuSekarang - waktuSebelum) >= interval)
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
  
  String data = doc["massage"][0]["device_status"];
      
  http.end();

   if (data == "daftar")
  {
   lcd.clear();
   lcd.setCursor(3,0);
   lcd.print("Mode Device");
   lcd.setCursor(5,1);
   lcd.print("Daftar");
   
   
   
  }else if (data == "ready"){
   lcd.clear();
   lcd.setCursor(3,0);
   lcd.print("Mode Device");
   lcd.setCursor(5,1);
   lcd.print("Ready"); 
  }
  waktuSebelum = millis();
}

if ((unsigned long)(waktuSekarang2 - waktuSebelum2) >= interval2)
{
   getFingerprintID();
   waktuSebelum2 = millis();
}

}


String statusDoorOK()
{
  HTTPClient http;
  http.begin (getDeviceHost);  
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "device_key="+deviceKey;
  int httpCode = http.POST(httpRequestData);
  String payload = http.getString();
 //  Parsing
  char json[700];
  payload.toCharArray(json, 700);
  StaticJsonDocument<300> doc;
  deserializeJson(doc, json);
  String data = doc["massage"][0]["status_door"];

  if (data == "open")
  {
    //Menjalankan Fungsi close  
     http.end();
     String data1 = "close";
     Serial.println(data1);
     return data1;
        
  }else if (data == "close")
  {
    //Menjalankan Fungsi Open
    http.end();
    String data1 = "open";
    Serial.println(data1);
    return data1;  
  }
 
  
  Serial.println(httpCode);
  Serial.print ("Ini Pesan Nya");
  Serial.println(data);  
  
  http.end();
}

//Fanction taping
void taping()
{
//  statusDoor = statusDoorOK();
  
  HTTPClient http;
  http.begin (taapingHost);  
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "device_key="+deviceKey+"&idSidikJari=1"+"&satus_door="+statusDoor;
  int httpCode = http.POST(httpRequestData);
   
   
   
  String payload = http.getString();
  
  Serial.println ("Ini Hasil Parsing");
  Serial.println(payload);

//  Parsing
  char json[700];
  payload.toCharArray(json, 700);
  StaticJsonDocument<300> doc;
  deserializeJson(doc, json);
  
  String data = doc["data"]["status_door"];
            
  http.end();

  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Berhasil.");
  lcd.setCursor(2,1);
  lcd.print("Welcome");
}

//Function daftar
void daftar()
{

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

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.println(".");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Ga ada yang cocok boo");
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Akses Di Tolak");
    lcd.setCursor(0,1);
    lcd.print("Tidak Terdaftar");
    delay(1000);
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Akses Berhasil");
  lcd.setCursor(4,1);
  lcd.print("Welcome");
  selenoitOK();
  delay(1000);

  return finger.fingerID;
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
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Konekting To.");
  lcd.setCursor(2,1);
  lcd.print(ssid);

 
}
Serial.print("\n");
Serial.print("IP address : ");
Serial.print(WiFi.localIP());
Serial.print("\n");
Serial.print("Connect to : ");
Serial.println(ssid);

  delay(1000);
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("connected.");
  lcd.setCursor(2,1);
  lcd.print(ssid);

  
}
