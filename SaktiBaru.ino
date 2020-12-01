#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>
#include <Servo.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

#define relayPin 13

SoftwareSerial mySerial(D5, D6);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t idx;
String  idx4;

LiquidCrystal_I2C lcd(0x27, 16, 2);

//SSID & Password Wifi
const char *ssid = "POGUNG GAMING"; 
const char *pass = "YOWHAT'SUPGUYS";

//Device key
String deviceKey = "DC-49711";

//Data Status
String statusDoor = "";
int idxSidikJari;

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

//Variabel EEPROM
#define EEPROM_SIZE 30
int charLength;

//Variabel Servo
Servo myServo;
int pintuTutup = 91;
int pintuBuka  = 10;
String statusPintu = "close";

void setup()
{ 
Serial.begin(9600);
EEPROM.begin(EEPROM_SIZE);

pinMode(relayPin, OUTPUT);
digitalWrite(relayPin, 1);
while (!Serial); 
lcd.begin();

startLCD();
delay(2000);

lcd.clear();
delay(2000);

finger.begin(57600);

 myServo.attach(D8); 
 digitalWrite(relayPin, LOW);
 delay(1000);
 myServo.write(pintuTutup); 
 delay(100);
digitalWrite(relayPin, HIGH);

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

void selenoitOK()
{
  digitalWrite(relayPin, LOW);
 
  delay(5000);
  digitalWrite(relayPin, HIGH);

  delay(500);  
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
         idx  = doc["massage"][0]["id_sidikJari"];
         
  http.end();

  //input data EEPROM
  String qsid=data;
  charLength=qsid.length();
 
   
    for (int i = 0; i < qsid.length(); ++i)
        {
         EEPROM.write(i, qsid[i]);
        }  
  //End Input EEPROM  
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
  if (bacaEEPROM() == "ready")
  {
  
   getFingerprintID(); 
   
  }else if(bacaEEPROM() == "daftar")
  {
     if (idx == 0){
       Serial.println("Menunggu Id Sidik Jari Dari server.");
       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("Menunggu Id Jari");
       lcd.setCursor(1,1);
       lcd.print("Dari Server . . .");
       delay(2000);
       return;
      }
       responDaftar1();
       Serial.println("Daata Masuk dengan Id Sidik Jari "+idx);
        Serial.println(idx);
       lcd.clear();
       lcd.setCursor(3,0);
       lcd.print("Data Masuk");
       lcd.setCursor(0,1);
       lcd.print("Id Jari adalah "+idx);
      delay(3000);
      while (!  getFingerprintEnroll() )
      {delay(2000);};

      resetDevice();

       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("Daftar Sidik Jari");
       lcd.setCursor(5,1);
       lcd.print("Berhasil");
       delay(2000);
  }
  waktuSebelum2 = millis();
}

}


void doorOpen()
{
   
  digitalWrite(relayPin, LOW);
  delay(2000);
  myServo.write(pintuBuka);
  delay(2000);
  digitalWrite(relayPin, HIGH);

   lcd.clear();
   lcd.setCursor(2,0);
   lcd.print("Pintu Berhasil");
   lcd.setCursor(5,1);
   lcd.print("Di Buka");
   delay(1000);
}

void doorClose()
{
   
  digitalWrite(relayPin, LOW);
  delay(2000);
  myServo.write(pintuTutup);
  delay(2000);
  digitalWrite(relayPin, HIGH);

   lcd.clear();
   lcd.setCursor(2,0);
   lcd.print("Pintu Berhasil");
   lcd.setCursor(5,1);
   lcd.print("Di Tutup");
   delay(1000);
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

  if (idxSidikJari != 0)
  {
  HTTPClient http;
  http.begin (taapingHost);  
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "device_key="+deviceKey+"&idSidikJari="+idxSidikJari+"&satus_door="+statusPintu;
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
}

void tapingFlase()
{

  HTTPClient http;
  http.begin (taapingHost);  
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "device_key="+deviceKey+"&idSidikJari=11"+"&satus_door="+statusPintu;
  int httpCode = http.POST(httpRequestData);
   
  String payload = http.getString();
  
  Serial.println(payload);
            
  http.end();

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
Serial.print ("Mengirim respon Mikrokontroler 1");
Serial.println(payload);          
 


http.end();
  

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
Serial.print ("Mengirim Respon daftar sidik jari 2 ke server");
Serial.println(payload);          
 
http.end();
  

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
  Serial.print ("Melakukan Respon Reset Server.");
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
    tapingFlase();
    delay(1000);
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  idxSidikJari = finger.fingerID;
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Akses Berhasil");
  lcd.setCursor(4,1);
  lcd.print("Welcome");

  if (statusPintu == "close")
  {
    statusPintu = "open";
    taping();
    doorOpen();
    
  }else if (statusPintu == "open")
  {
    statusPintu = "close";
    taping();
    doorClose();
    
    
  }

 
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


uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(idx);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
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
  
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
    delay(1000);
  }
  Serial.print("ID "); Serial.println(idx);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
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
  Serial.print("Creating model for #");  Serial.println(idx);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(idx);
  p = finger.storeModel(idx);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    responDaftar2();
    delay(3000);
    return 1;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}


String bacaEEPROM()
{
  String esid;
  for (int i = 0; i < charLength; ++i)
    {
      esid += char(EEPROM.read(i));
    }
  return esid;
}
