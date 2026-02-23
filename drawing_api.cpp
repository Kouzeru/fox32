#include "drawing_api.h"

DrawingApi::DrawingApi(ST7735Driver& driver) : driver_(driver) {}

void DrawingApi::fillScreen(uint16_t color) {
  driver_.setAddrWindow(0, 0, driver_.width() - 1, driver_.height() - 1);
  driver_.startWrite();
  for (uint32_t i = 0; i < (uint32_t)driver_.width() * driver_.height(); ++i) {
    driver_.writeColor16(color);
  }
  driver_.endWrite();
}

void DrawingApi::drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                          uint16_t color) {
  if (x >= driver_.width() || y >= driver_.height() || w == 0 || h == 0) {
    return;
  }

  if (x + w > driver_.width()) {
    w = driver_.width() - x;
  }
  if (y + h > driver_.height()) {
    h = driver_.height() - y;
  }

  driver_.setAddrWindow(x, y, x + w - 1, y + h - 1);
  driver_.startWrite();
  for (uint32_t i = 0; i < (uint32_t)w * h; ++i) {
    driver_.writeColor16(color);
  }
  driver_.endWrite();
}
