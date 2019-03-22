#include <SoftwareSerial.h>
#include <String.h>
 
SoftwareSerial mySerial(6,7);

#include "U8glib.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN  2        // Pin which is connected to the DHT sensor.

int nRainDigitalIn = 3;
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

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;


void setup()
{
  mySerial.begin(9600);               // the GPRS baud rate   
  Serial.begin(9600);    // the GPRS baud rate
   dht.begin();
   pinMode(nRainDigitalIn,INPUT);
   sensor_t sensor;
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }
    mySerial.println("AT");
  delay(1000);

  mySerial.println("AT+CPIN?");
  delay(1000);

  mySerial.println("AT+CREG?");
  delay(1000);

  mySerial.println("AT+CGATT?");
  delay(1000);

  mySerial.println("AT+CIPSHUT");
  delay(1000);

  mySerial.println("AT+CIPSTATUS");
  delay(2000);

  mySerial.println("AT+CIPMUX=0");
  delay(2000);
 
  ShowSerialData();
}
 
void loop()
{

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

 
  mySerial.println("AT+CSTT=\"gpinternet\"");//start task and setting the APN,
  updatedisplay();
  delay(700);
 
  ShowSerialData();
 
  mySerial.println("AT+CIICR");//bring up wireless connection
  updatedisplay();
  delay(2700);
 
  ShowSerialData();
 
  mySerial.println("AT+CIFSR");//get local IP adress
  updatedisplay();
  delay(1700);
 
  ShowSerialData();
 
  mySerial.println("AT+CIPSPRT=0");
  updatedisplay();
  delay(2700);
 
  ShowSerialData();
 
  mySerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");//start up the connection
  updatedisplay();
  delay(4700);
 
  ShowSerialData();
 
  mySerial.println("AT+CIPSEND");//begin send data to remote server
  updatedisplay();
  delay(3700);
  ShowSerialData();
 
  String str1="GET http://api.thingspeak.com/update?api_key=BBXV6RSJMBXLEBHK&field1=" + String(temp)+"&field2="+String(humidity)+"&field3="+String(pre)+"&field4="+String(rain);
   
  //String str2="GET http://api.thingspeak.com/update?api_key=KO67GCLMXT31KE7J&field2=" + String(humidity);
  mySerial.println(str1);//begin send data to remote server
  updatedisplay();
  delay(3700);
  ShowSerialData();
  

  mySerial.println((char)26);//sending
  updatedisplay();
  delay(3700);//waitting for reply, important! the time is base on the condition of internet
  mySerial.println();
 
  ShowSerialData();
 
  mySerial.println("AT+CIPSHUT");//close the connection
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
  
  //u8g.setFont(u8g_font_unifont);  // select font
   u8g.setFont(u8g_font_6x12);
  u8g.drawStr(0, 32, "T: ");  // put string of display at position X, Y
  u8g.drawStr(0, 52, "H: ");
  u8g.drawStr(57, 32, "P: ");  // put string of display at position X, Y
  u8g.drawStr(57, 52, "R: ");
  u8g.setPrintPos(20, 32);  // set position
  u8g.print(temp, 0);  // display temperature from DHT11
  u8g.drawStr(33, 30, "c ");
  u8g.setPrintPos(20, 52);
  u8g.print(humidity, 0);  // display humidity from DHT11
  u8g.drawStr(33, 52, "% ");
  u8g.drawStr(110, 32, "Pa");
  u8g.setPrintPos(70, 32);  // set position
  pre=String(bmp.readPressure());
  Serial.println(bmp.readPressure());
  u8g.print(pre);  // display temperature from DHT11
  //u8g.drawStr(77, 32, pre);
  //u8g.drawStr(110, 32, "Pa");
  //u8g.drawStr(80, 52, strRaining);
  u8g.setPrintPos(80, 52);
  u8g.print(strRaining);
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
  Serial.println(String(temp)+"*C\t"+String(humidity)+"%");
      delay(2);
      u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
          
}
