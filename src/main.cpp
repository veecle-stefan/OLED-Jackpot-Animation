#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_bt.h>
#include <driver/adc.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "oledconf.h"
#include "pins.h"
#include "animation.h"

#define GOTO_SLEEP_AFTER 30

// global variables
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RTC_DATA_ATTR int bootCount = 0;
touch_pad_t touchPin;
bool btnPressed = false;
uint8_t jackpotMode = 0;
uint32_t secondsRunning = 0;
uint16_t frameCounter = 0;

void displayCharging();

void buttonPressed()
{

}

void setup(){
  // turn off all radios 
  WiFi.mode(WIFI_OFF);
  //esp_wifi_stop();
  esp_bt_controller_disable();
   // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_BTN, INPUT_PULLUP);

  // set up the serial monitor for debugging
  Serial.begin(115200);
  delay(100); //Take some time to open up the Serial Monitor

  // set up the OLED display
  Wire.begin(4,15); 
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Clear the buffer
  display.clearDisplay();
  display.dim(false); // full brightness

  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason == ESP_SLEEP_WAKEUP_TOUCHPAD)
  {
    //buttonPressed();
  } else {
    // normal boot
    displayCharging();
  }

  

  initAnimation(display);
}

void gotoSleep()
{
  // disable display
  // Clear the buffer
  display.clearDisplay();
  display.dim(true);

  //Configure Touchpad as wakeup source
  touchAttachInterrupt(T4, buttonPressed, THRESHOLD);
  esp_sleep_enable_touchpad_wakeup();
  esp_deep_sleep_start();

}

void displayCharging()
{
  display.setTextSize(3);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("CHARGING...");
  display.display();
}

void loop(){
  // this loop is executed whenver no button has been pressed yet and
  // the chip is not sleeping yet


  frameCounter++;
  delay(40); // wait 1/25th second
  if (frameCounter >= 25)
  {
    // one second has passed
    frameCounter = 0;
    secondsRunning++;
  }

  if (jackpotMode == 0)
  {
    // display a frame every 1/25 s
    animateABit(display);
  } else if (jackpotMode == 1) {
    animateJackpot(display);
  } else if (jackpotMode == 2) {
    stopJackpot(display);
  }
  


  bool physicalButtonState = (LOW == digitalRead(PIN_BTN) ? true : false);
  if (!btnPressed && physicalButtonState)
  {
    // button pressed
    btnPressed = true;
    if (jackpotMode == 0)
    {
      jackpotMode = 1;
    } else if (jackpotMode == 1) {
      jackpotMode = 2;
    }
    secondsRunning = 0; // reset timer for sleep
  }
  if (btnPressed && !physicalButtonState)
  {
    btnPressed = false;
  }

  if (secondsRunning >= GOTO_SLEEP_AFTER)
  {
    gotoSleep();
  }
}