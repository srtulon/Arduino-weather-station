//created by srtulon

#include <SoftwareSerial.h>
#include <String.h>
 
SoftwareSerial mySerial(6,7); // RX, TX

#include "U8glib.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN  2        // Pin which is connected to the DHT sensor.

#define nRainDigitalIn  3   // Pin which is connected to the Rain sensor.

#define gmsPower 5    // Pin which is used to turn on SIM900

boolean bIsRaining = false;
String strRaining;
int rain=0;
String pre;
U8GLIB_SH1106_128X64 u8g(13, 11, 10, 9, 8);  // D0=13, D1=11, CS=10, DC=9, Reset=8

Adafruit_BMP085 bmp;

float temp=0.0;
float humidity=0.0;

// Uncomment the type of sensor in use:
//#define DHTTYPE           DHT11     // DHT 11
#define DHTTYPE           DHT22     // DHT 22 (AM2302)
//#define DHTTYPE           DHT21     // DHT 21 (AM2301)


DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;


void setup()
{
  mySerial.begin(9600);               // the GPRS baud rate   
  Serial.begin(9600);    // the GPRS baud rate
   dht.begin();
   pinMode(nRainDigitalIn,INPUT);
   pinMode(gmsPower, OUTPUT);
   digitalWrite(gmsPower,HIGH);
   sensor_t sensor;
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }
    mySerial.println(F("AT"));
  delay(1000);

  mySerial.println(F("AT+CPIN?"));
  delay(1000);

  mySerial.println(F("AT+CREG?"));
  delay(1000);

  mySerial.println(F("AT+CGATT?"));
  delay(1000);

  mySerial.println(F("AT+CIPSHUT"));
  delay(1000);

  mySerial.println(F("AT+CIPSTATUS"));
  delay(2000);

  mySerial.println(F("AT+CIPMUX=0"));
  delay(2000);
 
  ShowSerialData();
}
 
void loop()
{
  digitalWrite(gmsPower,HIGH);
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  temp=event.temperature;
  dht.humidity().getEvent(&event);
  humidity=event.relative_humidity;
  u8g.firstPage(); 
  Serial.println(String(temp)+"*C\t"+String(humidity)+"%");
      delay(2);         
      Send2Pachube();
   
  if (mySerial.available())
    Serial.write(mySerial.read());
}
void Send2Pachube()
{

 
  mySerial.println(F("AT+CSTT=\"internet\""));// setting the APN , replace internet with proper APN
  updatedisplay();
  delay(700);
 
  ShowSerialData();
 
  mySerial.println(F("AT+CIICR"));//bring up wireless connection
  updatedisplay();
  delay(1000);
 
  ShowSerialData();
 
  mySerial.println(F("AT+CIFSR"));//get local IP adress
  updatedisplay();
  delay(1000);
 
  ShowSerialData();
 
  mySerial.println(F("AT+CIPSPRT=0"));
  updatedisplay();
  delay(2000);
 
  ShowSerialData();
 
  mySerial.println(F("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\""));//start up the connection
  updatedisplay();
  delay(2000);
 
  ShowSerialData();
 
  mySerial.println(F("AT+CIPSEND"));//begin send data to remote server
  updatedisplay();
  delay(2000);
  ShowSerialData();
 
  String str="GET http://api.thingspeak.com/update?api_key=xxxxxxxxxxxxx&field1=" + String(temp)+"&field2="+String(humidity)+"&field3="+String(pre)+"&field4="+String(rain);  // replace xxxxxxxxxxxxx with API  
   
  
  mySerial.println(str);//begin send data to remote server
  updatedisplay();
  delay(1000);
  ShowSerialData();
  

  mySerial.println((char)26);//sending
  updatedisplay();
  delay(3700);     //waitting for reply
  mySerial.println();
 
  ShowSerialData();
 
  mySerial.println(F("AT+CIPSHUT"));//close the connection
  updatedisplay();
  delay(100);
  ShowSerialData();
}
void ShowSerialData()
{
  while(mySerial.available()!=0)
    Serial.write(mySerial.read());
}

void draw(void) {
    
   u8g.setFont(u8g_font_6x12); // select font
  u8g.drawStr(0, 32, "T: ");  // put string of display at position X, Y
  u8g.drawStr(0, 52, "H: ");
  u8g.drawStr(57, 32, "P: ");  
  u8g.drawStr(57, 52, "R: ");
  u8g.setPrintPos(20, 32);  // set position
  u8g.print(temp, 0);  // display temperature from DHT22
  u8g.drawStr(33, 30, "c ");
  u8g.setPrintPos(20, 52);
  u8g.print(humidity, 0);  // display humidity from DHT22
  u8g.drawStr(33, 52, "% ");
  u8g.drawStr(110, 32, "Pa");
  u8g.setPrintPos(70, 32);  
  pre=String(bmp.readPressure());
  u8g.print(pre);  // display air pressure from BMP180
  u8g.setPrintPos(80, 52);
  u8g.print(strRaining); // display rain status
}

void updatedisplay(){
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  temp=event.temperature;
  dht.humidity().getEvent(&event);
  humidity=event.relative_humidity;
  bIsRaining = !(digitalRead(nRainDigitalIn));
  if(bIsRaining){
    strRaining = "YES";
    rain=1;
  }
  else{
    strRaining = "NO";
    rain=0;
  }
  u8g.firstPage(); 
  
      delay(2);
      u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
          
}
