#pragma once

#include <Arduino.h>
#include <SPI.h>

class ST7735Driver {
 public:
  ST7735Driver(uint8_t csPin, uint8_t dcPin, uint8_t resetPin, uint8_t backlightPin,
               uint8_t sckPin, uint8_t mosiPin, uint16_t width, uint16_t height,
               uint8_t xOffset = 0, uint8_t yOffset = 0,
               uint32_t spiFrequency = 24000000);

  void begin();
  void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
  void startWrite();
  void endWrite();
  void writeColor16(uint16_t color);

  uint16_t width() const;
  uint16_t height() const;

 private:
  void initDisplay();
  void sendCommand(uint8_t cmd);
  void sendData(uint8_t data);
  void beginSpiTransaction();
  void endSpiTransaction();

  uint8_t csPin_;
  uint8_t dcPin_;
  uint8_t resetPin_;
  uint8_t backlightPin_;
  uint8_t sckPin_;
  uint8_t mosiPin_;
  uint16_t width_;
  uint16_t height_;
  uint8_t xOffset_;
  uint8_t yOffset_;
  uint32_t spiFrequency_;
};
