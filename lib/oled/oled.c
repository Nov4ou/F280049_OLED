// oled.c
#include "oled.h"

#include "oled_font.h"  // contains F8X16[], asc2_0806[] tables

// NOTE: Initizalize these three variables with your board's definitions:
static uint32_t pinRST;  // OLED_RST corresponding GPIO number
static uint32_t hspi;    // SPI peripheral base
static uint32_t pinDC;   // OLED_DC corresponding GPIO number

static uint8_t OLED_GRAM[OLED_WIDTH][OLED_PAGE_NUM];  // Display buffer

// Internal: send a byte (cmd or data)
static void oledSendByte(uint8_t byte, bool isData) {
  OLED_D_C_Pin(isData);
  Delay_us(1);
  SPI_WriteByte(hspi, byte);
  // set D/C back to data by default
  OLED_D_C_Pin(true);
}

#define oledSendCmd(c) oledSendByte((c), false)
#define oledSendData(d) oledSendByte((d), true)

// Control OLED Reset and D/C pins
static inline void OLED_Reset_Pin(bool level) {
  GPIO_writePin(pinRST, level ? 1 : 0);
}
static inline void OLED_D_C_Pin(bool level) {
  GPIO_writePin(pinDC, level ? 1 : 0);
}

// Set cursor to column x and page y
static void oledSetPos(uint8_t x, uint8_t y) {
  oledSendCmd(0xB0 | y);
  oledSendCmd(((x & 0xF0) >> 4) | 0x10);
  oledSendCmd((x & 0x0F) | 0x01);
}

void oledConfig(uint32_t rstPin, uint32_t dcPin, uint32_t spiBase) {
  pinRST = rstPin;
  pinDC = dcPin;
  hspi = spiBase;
}

void oledInit() {
  // Reset sequence
  OLED_Reset_Pin(true);
  Delay_us(200000);
  OLED_Reset_Pin(false);
  Delay_us(200000);
  OLED_Reset_Pin(true);
  Delay_us(6);

  // Init commands (SSD1306)
  oledSendCmd(0xAE);  //--turn off oled panel
  oledSendCmd(0x00);  //---set low column address
  oledSendCmd(0x10);  //---set high column address
  oledSendCmd(0x40);  //--set start line address  Set Mapping RAM Display Start
                      // Line (0x00~0x3F)
  oledSendCmd(0x81);  //--set contrast control register
  oledSendCmd(0xCF);  // Set SEG Output Current Brightness
  oledSendCmd(0xA1);  //--Set SEG/Column Mapping     0xa0???? 0xa1??
  oledSendCmd(0xC8);  // Set COM/Row Scan Direction   0xc0???? 0xc8??
  oledSendCmd(0xA6);  //--set normal display
  oledSendCmd(0xA8);  //--set multiplex ratio(1 to 64)
  oledSendCmd(0x3f);  //--1/64 duty
  oledSendCmd(
      0xD3);  //-set display offset   Shift Mapping RAM Counter (0x00~0x3F)
  oledSendCmd(0x00);  //-not offset
  oledSendCmd(0xd5);  //--set display clock divide ratio/oscillator frequency
  oledSendCmd(0x80);  //--set divide ratio, Set Clock as 100 Frames/Sec
  oledSendCmd(0xD9);  //--set pre-charge period
  oledSendCmd(0xF1);  // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
  oledSendCmd(0xDA);  //--set com pins hardware configuration
  oledSendCmd(0x12);
  oledSendCmd(0xDB);  //--set vcomh
  oledSendCmd(0x40);  // Set VCOM Deselect Level
  oledSendCmd(0x20);  //-Set Page Addressing Mode (0x00/0x01/0x02)
  oledSendCmd(0x02);  //
  oledSendCmd(0x8D);  //--set Charge Pump enable/disable
  oledSendCmd(0x14);  //--set(0x10) disable
  oledSendCmd(0xA4);  // Disable Entire Display On (0xa4/0xa5)
  oledSendCmd(0xA6);  // Disable Inverse Display On (0xa6/a7)
  oledSendCmd(0xAF);  //--turn on oled panel

  oledClear();
}

void oledClear(void) {
  uint8_t p, x;
  for (p = 0; p < OLED_PAGE_NUM; p++) {
    for (x = 0; x < OLED_WIDTH; x++) {
      OLED_GRAM[x][p] = 0;
    }
  }
  oledRefresh();
}

void oledRefresh(void) {
  uint8_t p, x;
  for (p = 0; p < OLED_PAGE_NUM; p++) {
    oledSendCmd(0xB0 | p);
    oledSendCmd(0x02);
    oledSendCmd(0x10);
    for (x = 0; x < OLED_WIDTH; x++) {
      oledSendData(OLED_GRAM[x][p]);
    }
  }
}

void oledDrawPoint(uint8_t x, uint8_t y, uint8_t t) {
  uint8_t p = y >> 3;
  uint8_t mask = 1 << (y & 7);
  if (t)
    OLED_GRAM[x][p] |= mask;
  else
    OLED_GRAM[x][p] &= ~mask;
  // update just that column
  oledSendCmd(0xB0 | p);
  oledSendCmd((x & 0x0F));
  oledSendCmd((x >> 4) | 0x10);
  oledSendData(OLED_GRAM[x][p]);
}

void oledShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size) {
  uint8_t index = chr - ' ', i;
  if (size == 16) {
    oledSetPos(x, y);
    for (i = 0; i < 8; i++) oledSendData(F8X16[index * 16 + i]);
    oledSetPos(x, y + 1);
    for (i = 0; i < 8; i++) oledSendData(F8X16[index * 16 + i + 8]);
  } else {
    oledSetPos(x, y + 1);
    for (i = 0; i < 6; i++) oledSendData(asc2_0806[index][i]);
  }
}

void oledShowString(uint8_t x, uint8_t y, const char *str, uint8_t size) {
  uint8_t charWidth;
  uint8_t idx = 0;

  // Determine character width based on font size
  if (size == 16) {
    charWidth = 8;  // 16×8 font: width = 8 pixels
  } else if (size == 8) {
    charWidth = 6;  //  8×6 font: width = 6 pixels
  } else {
    return;  // unsupported font size
  }

  // Iterate over the string until null terminator
  while (str[idx] != '\0') {
    // Draw one character at (x + idx*charWidth, y)
    oledShowChar(x + idx * charWidth, y, (uint8_t)str[idx], size);
    idx++;
  }
}

uint32_t oled_pow(uint8_t base, uint8_t exp) {
  uint32_t result = 1;
  while (exp--) result *= base;
  return result;
}

void oledShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len,
                 uint8_t size) {
  bool lead = false;
  uint8_t t;
  for (t = 0; t < len; t++) {
    uint8_t val = (num / oled_pow(10, len - t - 1)) % 10;
    if (!lead && t < (len - 1) && val == 0) {
      oledShowChar(x + (size / 2) * t, y, ' ', size);
    } else {
      lead = true;
      oledShowChar(x + (size / 2) * t, y, val + '0', size);
    }
  }
}

void oledShowFloat(uint8_t x, uint8_t y, float num, uint8_t int_len,
                   uint8_t dec_len, uint8_t size) {
  if (num < 0) {
    oledShowChar(x, y, '-', size);
    x += size / 2;
    num = -num;
  }
  uint8_t i;
  uint32_t ip = (uint32_t)num;
  float fp = num - (float)ip;
  oledShowNum(x, y, ip, int_len, size);
  x += (size / 2) * int_len;
  oledShowChar(x, y, '.', size);
  x += size / 2;
  for (i = 0; i < dec_len; i++) {
    fp *= 10;
    uint8_t d = (uint8_t)fp;
    oledShowChar(x + (size / 2) * i, y, d + '0', size);
    fp -= d;
  }
}
