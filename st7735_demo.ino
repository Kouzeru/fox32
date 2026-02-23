#include <SPI.h>

// Pin definitions matching your setup
#define BLK_PIN 3
#define CS_PIN 4
#define DC_PIN 5
#define RES_PIN 8
#define SDA_PIN 9
#define SCL_PIN 10

// ST7735 commands
#define ST7735_SWRESET 0x01
#define ST7735_SLPOUT  0x11
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_NORON   0x13
#define ST7735_DISPON  0x29

// Display geometry
constexpr uint16_t SCREEN_W = 128;
constexpr uint16_t SCREEN_H = 128;

// Some ST7735 128x128 modules use non-zero RAM offsets.
// If your image appears shifted, change these (common values: X=2, Y=1).
constexpr uint8_t X_OFFSET = 0;
constexpr uint8_t Y_OFFSET = 0;

// Variables for moving rectangle
int16_t rectX = 0;
int16_t rectY = 0;
int16_t rectWidth = 20;
int16_t rectHeight = 20;
int8_t rectSpeedX = 2;
int8_t rectSpeedY = 1;

// Variables for FPS calculation
unsigned long frameCount = 0;
unsigned long lastTime = 0;
float fps = 0;

void sendCommand(uint8_t cmd);
void sendData(uint8_t data);
void sendData16(uint16_t data);
void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void fillScreen(uint16_t color);
void initDisplay();

void setup() {
  Serial.begin(115200);

  pinMode(BLK_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  pinMode(DC_PIN, OUTPUT);
  pinMode(RES_PIN, OUTPUT);

  digitalWrite(CS_PIN, HIGH);
  digitalWrite(BLK_PIN, HIGH); // Turn backlight on.

  // ESP-style SPI pin mapping (SCK, MISO, MOSI, SS)
  SPI.begin(SCL_PIN, -1, SDA_PIN, CS_PIN);

  initDisplay();
  fillScreen(0x0000);

  lastTime = millis();
}

void loop() {
  drawRect(rectX, rectY, rectWidth, rectHeight, 0x0000);

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

  drawRect(rectX, rectY, rectWidth, rectHeight, 0xF800);

  frameCount++;
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= 1000) {
    fps = frameCount * 1000.0f / (currentTime - lastTime);
    Serial.print("FPS: ");
    Serial.println(fps, 1);
    frameCount = 0;
    lastTime = currentTime;
  }
}

void initDisplay() {
  digitalWrite(RES_PIN, LOW);
  delay(20);
  digitalWrite(RES_PIN, HIGH);
  delay(120);

  sendCommand(ST7735_SWRESET);
  delay(150);

  sendCommand(ST7735_SLPOUT);
  delay(120);

  sendCommand(ST7735_COLMOD);
  sendData(0x05); // RGB565
  delay(10);

  sendCommand(ST7735_MADCTL);
  sendData(0x00); // Row/column order; tweak for rotation if needed.

  sendCommand(ST7735_NORON);
  delay(10);

  sendCommand(ST7735_DISPON);
  delay(100);
}

void sendCommand(uint8_t cmd) {
  digitalWrite(DC_PIN, LOW);
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(cmd);
  digitalWrite(CS_PIN, HIGH);
}

void sendData(uint8_t data) {
  digitalWrite(DC_PIN, HIGH);
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(data);
  digitalWrite(CS_PIN, HIGH);
}

void sendData16(uint16_t data) {
  SPI.transfer(data >> 8);
  SPI.transfer(data & 0xFF);
}

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  x0 += X_OFFSET;
  x1 += X_OFFSET;
  y0 += Y_OFFSET;
  y1 += Y_OFFSET;

  sendCommand(ST7735_CASET);
  digitalWrite(DC_PIN, HIGH);
  digitalWrite(CS_PIN, LOW);
  sendData16(x0);
  sendData16(x1);
  digitalWrite(CS_PIN, HIGH);

  sendCommand(ST7735_RASET);
  digitalWrite(DC_PIN, HIGH);
  digitalWrite(CS_PIN, LOW);
  sendData16(y0);
  sendData16(y1);
  digitalWrite(CS_PIN, HIGH);

  sendCommand(ST7735_RAMWR);
}

void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
  if (x >= SCREEN_W || y >= SCREEN_H || w == 0 || h == 0) {
    return;
  }

  if (x + w > SCREEN_W) {
    w = SCREEN_W - x;
  }
  if (y + h > SCREEN_H) {
    h = SCREEN_H - y;
  }

  setAddrWindow(x, y, x + w - 1, y + h - 1);

  digitalWrite(DC_PIN, HIGH);
  digitalWrite(CS_PIN, LOW);
  for (uint32_t i = 0; i < (uint32_t)w * h; i++) {
    sendData16(color);
  }
  digitalWrite(CS_PIN, HIGH);
}

void fillScreen(uint16_t color) {
  setAddrWindow(0, 0, SCREEN_W - 1, SCREEN_H - 1);

  digitalWrite(DC_PIN, HIGH);
  digitalWrite(CS_PIN, LOW);
  for (uint32_t i = 0; i < (uint32_t)SCREEN_W * SCREEN_H; i++) {
    sendData16(color);
  }
  digitalWrite(CS_PIN, HIGH);
}
