#include "st7735_driver.h"

namespace {
constexpr uint8_t ST7735_SWRESET = 0x01;
constexpr uint8_t ST7735_SLPOUT = 0x11;
constexpr uint8_t ST7735_COLMOD = 0x3A;
constexpr uint8_t ST7735_MADCTL = 0x36;
constexpr uint8_t ST7735_CASET = 0x2A;
constexpr uint8_t ST7735_RASET = 0x2B;
constexpr uint8_t ST7735_RAMWR = 0x2C;
constexpr uint8_t ST7735_NORON = 0x13;
constexpr uint8_t ST7735_DISPON = 0x29;
}  // namespace

ST7735Driver::ST7735Driver(uint8_t csPin, uint8_t dcPin, uint8_t resetPin,
                           uint8_t backlightPin, uint8_t sckPin, uint8_t mosiPin,
                           uint16_t width, uint16_t height, uint8_t xOffset,
                           uint8_t yOffset, uint32_t spiFrequency)
    : csPin_(csPin),
      dcPin_(dcPin),
      resetPin_(resetPin),
      backlightPin_(backlightPin),
      sckPin_(sckPin),
      mosiPin_(mosiPin),
      width_(width),
      height_(height),
      xOffset_(xOffset),
      yOffset_(yOffset),
      spiFrequency_(spiFrequency) {}

void ST7735Driver::begin() {
  pinMode(backlightPin_, OUTPUT);
  pinMode(csPin_, OUTPUT);
  pinMode(dcPin_, OUTPUT);
  pinMode(resetPin_, OUTPUT);

  digitalWrite(csPin_, HIGH);
  digitalWrite(backlightPin_, HIGH);

  // SPI bus setup (ESP32-style pin remap API)
  SPI.begin(sckPin_, -1, mosiPin_, csPin_);
  initDisplay();
}

void ST7735Driver::beginSpiTransaction() {
  SPI.beginTransaction(SPISettings(spiFrequency_, MSBFIRST, SPI_MODE0));
}

void ST7735Driver::endSpiTransaction() { SPI.endTransaction(); }

void ST7735Driver::initDisplay() {
  digitalWrite(resetPin_, LOW);
  delay(20);
  digitalWrite(resetPin_, HIGH);
  delay(120);

  sendCommand(ST7735_SWRESET);
  delay(150);

  sendCommand(ST7735_SLPOUT);
  delay(120);

  sendCommand(ST7735_COLMOD);
  sendData(0x05);  // RGB565
  delay(10);

  sendCommand(ST7735_MADCTL);
  sendData(0x00);

  sendCommand(ST7735_NORON);
  delay(10);

  sendCommand(ST7735_DISPON);
  delay(100);
}

void ST7735Driver::sendCommand(uint8_t cmd) {
  beginSpiTransaction();
  digitalWrite(dcPin_, LOW);
  digitalWrite(csPin_, LOW);
  SPI.transfer(cmd);
  digitalWrite(csPin_, HIGH);
  endSpiTransaction();
}

void ST7735Driver::sendData(uint8_t data) {
  beginSpiTransaction();
  digitalWrite(dcPin_, HIGH);
  digitalWrite(csPin_, LOW);
  SPI.transfer(data);
  digitalWrite(csPin_, HIGH);
  endSpiTransaction();
}

void ST7735Driver::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1,
                                 uint16_t y1) {
  x0 += xOffset_;
  x1 += xOffset_;
  y0 += yOffset_;
  y1 += yOffset_;

  sendCommand(ST7735_CASET);
  digitalWrite(dcPin_, HIGH);
  digitalWrite(csPin_, LOW);
  SPI.transfer(x0 >> 8);
  SPI.transfer(x0 & 0xFF);
  SPI.transfer(x1 >> 8);
  SPI.transfer(x1 & 0xFF);
  digitalWrite(csPin_, HIGH);

  sendCommand(ST7735_RASET);
  digitalWrite(dcPin_, HIGH);
  digitalWrite(csPin_, LOW);
  SPI.transfer(y0 >> 8);
  SPI.transfer(y0 & 0xFF);
  SPI.transfer(y1 >> 8);
  SPI.transfer(y1 & 0xFF);
  digitalWrite(csPin_, HIGH);

  sendCommand(ST7735_RAMWR);
}

void ST7735Driver::startWrite() {
  beginSpiTransaction();
  digitalWrite(dcPin_, HIGH);
  digitalWrite(csPin_, LOW);
}

void ST7735Driver::endWrite() {
  digitalWrite(csPin_, HIGH);
  endSpiTransaction();
}

void ST7735Driver::writeColor16(uint16_t color) {
  SPI.transfer(color >> 8);
  SPI.transfer(color & 0xFF);
}

uint16_t ST7735Driver::width() const { return width_; }

uint16_t ST7735Driver::height() const { return height_; }
