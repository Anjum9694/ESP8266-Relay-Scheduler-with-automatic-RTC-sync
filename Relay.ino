#include "WifiPass.h"
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <NTPClient.h> //NTPClient by Arduino
#include <WiFiUdp.h>

#include <Wire.h>
#include <Time.h>
#include <TimeAlarms.h>
#include <RTClib.h>               //Library for RTC module
RTC_DS3231 RTC;                   //Declaring RTC module

const int relay = 14;

//closest NTP Server
#define NTP_SERVER "0.us.pool.ntp.org"

//GMT Time Zone with sign
#define GMT_TIME_ZONE +5

//Force RTC update and store on EEPROM
//change this to a random number between 0-255 to force time update
#define FORCE_RTC_UPDATE 2

WiFiUDP ntpUDP;

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
NTPClient timeClient(ntpUDP, NTP_SERVER, GMT_TIME_ZONE * 3600 , 60000);

int timeUpdated = 0;


void setup() {
 pinMode(relay, OUTPUT);
 digitalWrite(relay, HIGH);
 Serial.begin(9600);   //Declare serial communication baudrate
 EEPROM.begin(4);

 if (!RTC.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
 }

  Serial.println("Waiting to Start....");
  delay(5000);

  //Read the EEPROM to check if time has been synced

 byte addvalue = EEPROM.read(timeUpdated);
 Serial.print("EEPROM: ");
 Serial.print(addvalue);
 Serial.print(" == ");
 Serial.print(FORCE_RTC_UPDATE);
 Serial.println(" ?");
 if (addvalue != FORCE_RTC_UPDATE) {
   //if(true == false){
   //time hasn' it been setup
   Serial.println("Forcing Time Update");
   syncTime();
   Serial.println("Updating EEPROM..");
   EEPROM.write(timeUpdated, FORCE_RTC_UPDATE);
   EEPROM.commit();

 } else {
   Serial.println("Time has been updated before...EEPROM CHECKED");
   Serial.print("EEPROM: ");
   Serial.print(addvalue);
   Serial.print(" = ");
   Serial.print(FORCE_RTC_UPDATE);
   Serial.println("!");
  }


 DateTime now = RTC.now();
 setTime(now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());  //To set date and time use setTime(HH,MM,SS,DD,MM,YY)
 
 Alarm.timerRepeat(15, Repeats); // timer for every 15 seconds
 Serial.println(Alarm.getTriggeredAlarmId());
 Alarm.delay(1000);

 Alarm.alarmRepeat(15, 8, 0, MorningAlarm);        //Set daily Morning Alarm
 Serial.println(Alarm.getTriggeredAlarmId());
 Alarm.delay(1000);


}

void syncTime(void) {

  //Connect to Wifi
  //SSID and Password on WifiPass.h file
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();
  timeClient.update();

  long actualTime = timeClient.getEpochTime();
  Serial.print("Internet Epoch Time: ");
  Serial.println(actualTime);
  RTC.adjust(DateTime(actualTime));



  //Turn Off WIFI after update
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();

}

void loop()
{
 
 
 Serial.print(day());
 Serial.print('/');
 Serial.print(month());
 Serial.print('/');
 Serial.print(year());
 Serial.print(' ');
 Serial.print(hour());
 Serial.print(':');
 Serial.print(minute());
 Serial.print(':');
 Serial.print(second());
 Serial.println();
 Alarm.delay(1000);         //Delay when using TimeAlarms.h library


}

void MorningAlarm()
{
  Serial.println("Morning alarm Ringing is working");
  Serial.println(Alarm.getTriggeredAlarmId());
  Alarm.delay(5000);
}

void EveningAlarm()
{
  Serial.println("Evening alarm Ringing");
  Serial.println(Alarm.getTriggeredAlarmId());
  Alarm.delay(5000);
}

void Repeats()
{
  Serial.println("Repeating alarm Ringing");
  digitalWrite(relay, HIGH);
  Alarm.delay(1000);
}
