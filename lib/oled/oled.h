// oled.h
#ifndef OLED_H
#define OLED_H

#include <stdbool.h>
#include "driverlib.h"      // uint8_t not defined

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: User-provided platform functions:
// SPI transmit function: send one byte over SPI
// Example implementation: void SPI_WriteByte(uint32_t spiBase, uint8_t byte);
extern void SPI_WriteByte(uint32_t spiBase, uint8_t byte);

// Delay in microseconds
extern void Delay_us(uint32_t micros);


// OLED display dimensions
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_PAGE_NUM (OLED_HEIGHT / 8)

// Configure which GPIO pins drive OLED_RST 和 OLED_DC
void oledConfig(uint32_t rstPin, uint32_t dcPin, uint32_t spiBase);
// Control OLED Reset and D/C pins
// level = false -> low; level = true -> high
static inline void OLED_Reset_Pin(bool level);
static inline void OLED_D_C_Pin(bool level);
// Initialize OLED
void oledInit();
// Clear display RAM and refresh
void oledClear(void);
// Push RAM buffer to display
void oledRefresh(void);
// Draw or clear a pixel at (x, y)
void oledDrawPoint(uint8_t x, uint8_t y, uint8_t t);
// Display ASCII character
void oledShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size);
// Display a null-terminated string on the OLED
void oledShowString(uint8_t x, uint8_t y, const char *str, uint8_t size);
// Display unsigned integer
void oledShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size);
// Display floating-point number
void oledShowFloat(uint8_t x, uint8_t y, float num, uint8_t int_len,
                   uint8_t dec_len, uint8_t size);

#ifdef __cplusplus
}
#endif

#endif  // OLED_H
