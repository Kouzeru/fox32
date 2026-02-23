#pragma once

#include <Arduino.h>

#include "st7735_driver.h"

class DrawingApi {
 public:
  explicit DrawingApi(ST7735Driver& driver);

  void fillScreen(uint16_t color);
  void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

 private:
  ST7735Driver& driver_;
};
