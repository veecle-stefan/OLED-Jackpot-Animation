#include <Arduino.h>
#include "oledconf.h"
#include <Adafruit_SSD1306.h>

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2
#define NUMFLAKES     7 // Number of snowflakes in the animation example

#define LOGO_WIDTH 16
#define LOGO_HEIGHT 16

#define NAME1 "ROMINA"
#define NAME2 "HANES"
#define CHARWIDTH 21

#define JP_SHIFT 32
uint8_t JPNums[3][3] = {{9,9,9}, {1,2,3}, {4,5,6}};
uint8_t JPanim = 0;
uint8_t JPFinal = 0;
#define JPNUM1 1
#define JPNUM2 0
#define JPNUM3 0

static const unsigned char PROGMEM logo_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

int8_t f, icons[NUMFLAKES][3];

void initAnimation(Adafruit_SSD1306& display)
{
  // Initialize 'snowflake' positions
  for(f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
    icons[f][YPOS]   = display.height() + LOGO_HEIGHT;
    icons[f][DELTAY] = random(1, 6);
  }
}

void centerText(Adafruit_SSD1306& display, const char* txt, uint8_t ypos)
{
    uint8_t txtLen = strlen(txt);
    display.setCursor(SCREEN_WIDTH / 2 - (txtLen * CHARWIDTH) / 2, ypos);
    display.print(txt);
}

void animateABit(Adafruit_SSD1306& display)
{
    
    display.clearDisplay(); // Clear the display buffer

    // Draw each snowflake:
    for(f=0; f< NUMFLAKES; f++) {
        display.drawBitmap(icons[f][XPOS], icons[f][YPOS], logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, SSD1306_WHITE);
    }

    display.setTextSize(3);             // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    centerText(display, NAME1, 0);
    centerText(display, NAME2, 44);
    display.display(); // Show the display buffer on the screen

    // Then update coordinates of each flake...
    for(f=0; f< NUMFLAKES; f++) {
        icons[f][YPOS] -= icons[f][DELTAY];
        // If snowflake is off the bottom of the screen...
        if (icons[f][YPOS] < LOGO_HEIGHT) {
            // Reinitialize to a random position, just off the top
            icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
            icons[f][YPOS]   = display.height() + LOGO_HEIGHT;
            icons[f][DELTAY] = random(1, 6);
        }
    }
}

void drawJackpot(Adafruit_SSD1306& display)
{
    display.clearDisplay();
    display.drawFastVLine(32, 0, 64, WHITE);
    display.drawFastVLine(64, 0, 64, WHITE);
    display.drawFastVLine(96, 0, 64, WHITE);
    display.setTextSize(4);
    display.setCursor(101, 20);
    display.print("C");
    display.fillRect(101, 28, 12, 3, WHITE);
    display.fillRect(101, 34, 12, 4, WHITE);
}

void printJPNumber(Adafruit_SSD1306& display, uint8_t num, uint8_t index, int8_t ypos)
{
    display.setTextSize(4);
    display.setCursor(index * 32 + 5, ypos);
    display.print(num);
}

void animateJackpot(Adafruit_SSD1306& display)
{
    drawJackpot(display);
    for(uint8_t n = 0; n < 3; n++)
    {
        for(uint8_t i = 0; i < 3; i++)
        {
            printJPNumber(display, JPNums[n][i], i, JPanim - JP_SHIFT + n * JP_SHIFT);    
        }
    }

    if (JPFinal < 3)
    {
        if (JPFinal == 2)
        {
            JPanim++;
        } else {
            JPanim += 6;
        }

        if ((JPFinal == 2) && (JPanim > JP_SHIFT / 2)){
                JPFinal = 3;
            }

        // everytime one row was shifted
        if (JPanim > JP_SHIFT)
        {
            JPanim = 0;
            
            
            
            for(uint8_t n = 2; n >= 1; n--) {
                for(uint8_t i = 0; i < 3; i++)
                {
                    JPNums[n][i] = JPNums[n-1][i];
                }
            }
            if ((JPFinal == 1) && (JPNums[1][0] == JPNUM1) && (JPNums[1][1] == JPNUM2) && (JPNums[1][2] == JPNUM3))
            {
                JPFinal++;
            }

            if (JPFinal == 1)
            {
                JPNums[0][0] = JPNUM1;
                JPNums[0][1] = JPNUM2;
                JPNums[0][2] = JPNUM3;
            } else {
                for(uint8_t i = 0; i < 3; i++)
                {
                    JPNums[0][i] = random(0,9);
                }
            }
        }
    }

    display.display();
}

void stopJackpot(Adafruit_SSD1306& display)
{
    if (JPFinal == 0) JPFinal = 1;
    animateJackpot(display);
}