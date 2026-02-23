#include <Arduino.h>

#include "drawing_api.h"
#include "st7735_driver.h"

// Pin definitions matching your setup
constexpr uint8_t BLK_PIN = 3;
constexpr uint8_t CS_PIN = 4;
constexpr uint8_t DC_PIN = 5;
constexpr uint8_t RES_PIN = 8;
constexpr uint8_t SDA_PIN = 9;
constexpr uint8_t SCL_PIN = 10;

constexpr uint16_t SCREEN_W = 128;
constexpr uint16_t SCREEN_H = 128;

// Some ST7735 128x128 modules use non-zero RAM offsets.
constexpr uint8_t X_OFFSET = 0;
constexpr uint8_t Y_OFFSET = 0;

ST7735Driver display(CS_PIN, DC_PIN, RES_PIN, BLK_PIN, SCL_PIN, SDA_PIN, SCREEN_W,
                     SCREEN_H, X_OFFSET, Y_OFFSET);
DrawingApi gfx(display);

int16_t rectX = 0;
int16_t rectY = 0;
int16_t rectWidth = 20;
int16_t rectHeight = 20;
int8_t rectSpeedX = 2;
int8_t rectSpeedY = 1;

unsigned long frameCount = 0;
unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);
  display.begin();
  gfx.fillScreen(0x0000);
  lastTime = millis();
}

void loop() {
  gfx.drawRect(rectX, rectY, rectWidth, rectHeight, 0x0000);

  rectX += rectSpeedX;
  rectY += rectSpeedY;

  if (rectX <= 0 || rectX + rectWidth >= SCREEN_W) {
    rectSpeedX = -rectSpeedX;
    rectX += rectSpeedX;
  }
  if (rectY <= 0 || rectY + rectHeight >= SCREEN_H) {
    rectSpeedY = -rectSpeedY;
    rectY += rectSpeedY;
  }

  gfx.drawRect(rectX, rectY, rectWidth, rectHeight, 0xF800);

  frameCount++;
  const unsigned long currentTime = millis();
  if (currentTime - lastTime >= 1000) {
    const float fps = frameCount * 1000.0f / (currentTime - lastTime);
    Serial.print("FPS: ");
    Serial.println(fps, 1);
    frameCount = 0;
    lastTime = currentTime;
  }
}
