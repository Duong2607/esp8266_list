#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "RTClib.h"

#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 4 on the Arduino
#define ONE_WIRE_BUS 04
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

RTC_DS1307 rtc;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
//#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

// Địa chỉ bắt đầu của EEPROM để lưu trữ dữ liệu
int EEPROM_ADDRESS = 0;

int isActive = 1;
float miTemp = 50, maTemp = 0;

// Cấu trúc dữ liệu của ngày tháng, giờ
struct DateTimeData {
  float temperature;
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
};
void setup() {
  Serial.begin(9600);
  // Khởi tạo EEPROM
  EEPROM.begin(sizeof(DateTimeData));
  //bai3.6
  sensors.begin();

  // bai3.5
#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  //bai3.4-lcd

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
}

void loop() {
 testdrawstyles(); 
}

void testdrawstyles(void) {
  if (Serial.available()) { // check if any data is available on the serial port
  String inputString = Serial.readString(); // read the string from the serial port
  Serial.println(inputString);
  if(inputString == "START\n" && isActive == 0) isActive = 1;
  else if(inputString == "STOP\n") isActive = 0;
  else if(inputString == "GETMIN\n"){
  // Serial.println(minTemp);
  Serial.print("Temp: ");
  Serial.print(miTemp);
  Serial.print("|Date: ");
  Serial.print(min.year);
  Serial.print("/");
  Serial.print(min.month);
  Serial.print("/");
  Serial.print(min.day);
  Serial.print("|Time: ");
  Serial.print(min.hour);
  Serial.print(":");
  Serial.print(min.minute);
  Serial.print(":");
  Serial.println(min.second);
  }
  else if(inputString == "GETMAX\n"){
  // Serial.println(maxTemp);
  Serial.print("Temp: ");
  Serial.print(maTemp);
  Serial.print("|Date: ");
  Serial.print(max.year);
  Serial.print("/");
  Serial.print(max.month);
  Serial.print("/");
  Serial.print(max.day);
  Serial.print("|Time: ");
  Serial.print(max.hour);
  Serial.print(":");
  Serial.print(max.minute);
  Serial.print(":");
  Serial.println(max.second);
  }
  }
  if(isActive){
  //display temperature
    sensors.requestTemperatures(); // Send the command to get temperatures
  // Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
    float tempC = sensors.getTempCByIndex(0);
    if(int(tempC) > miTemp){
      miTemp = tempC;
      DateTimeData max;
      max.year = now.year();
      max.month = now.month();
      max.day = now.day();
      max.hour = now.hour();
      max.minute = now.minute();
      max.second = now.second();
    }
    if(int(tempC) < maTemp){
      maTemp = tempC;
      DateTimeData min;
      min.year = now.year();
      min.month = now.month();
      min.day = now.day();
      min.hour = now.hour();
      min.minute = now.minute();
      min.second = now.second();
    }
      // Check if reading was successful
      display.clearDisplay();

      display.setCursor(0,0); 
      display.setTextSize(1);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);
      display.println("Current Temperature:");
      if(tempC != DEVICE_DISCONNECTED_C) 
      {
        display.setCursor(0,15); 
        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);
        display.print(tempC);
        display.drawCircle(32, 15, 2, WHITE);
        display.print(" C");
      } 
      else
      {
         
        display.setCursor(0,15); 
        display.setTextSize(1);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);
        display.println("Error: Could not read temperature data");
      }

    //display time
    DateTime now = rtc.now();
    display.setCursor(0,30); 
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);  
    display.print(now.year(), DEC);
    display.print('/');
    display.print(now.month(), DEC);
    display.print('/');
    display.print(now.day(), DEC);
    display.print(' ');
    display.print(now.hour(), DEC);
    display.print(':');
    display.print(now.minute(), DEC);
    display.print(':');
    display.print(now.second(), DEC);
    display.println();
    display.display();
    
  // Ghi dữ liệu vào EEPROM
  DateTimeData data;
  sensors.requestTemperatures();
  data.temperature = sensors.getTempCByIndex(0);
  data.year = now.year();
  data.month = now.month();
  data.day = now.day();
  data.hour = now.hour();
  data.minute = now.minute();
  data.second = now.second();
  saveDataToEEPROM(data);

  // Đọc dữ liệu từ EEPROM và in ra Serial Monitor
  DateTimeData readData = readDataFromEEPROM();
  Serial.print("Temp: ");
  Serial.print(readData.temperature);
  Serial.print("|Date: ");
  Serial.print(readData.year);
  Serial.print("/");
  Serial.print(readData.month);
  Serial.print("/");
  Serial.print(readData.day);
  Serial.print("|Time: ");
  Serial.print(readData.hour);
  Serial.print(":");
  Serial.print(readData.minute);
  Serial.print(":");
  Serial.println(readData.second);
  }
}

void saveDataToEEPROM(const DateTimeData& data) {
  // Ghi dữ liệu vào EEPROM
  EEPROM.put(EEPROM_ADDRESS, data);
  EEPROM.commit();
}

DateTimeData readDataFromEEPROM() {
  // Đọc dữ liệu từ EEPROM
  DateTimeData data;
  EEPROM.get(EEPROM_ADDRESS, data);
  return data;
}