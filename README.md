# OLED Driver Library

A lightweight SSD1306 OLED C driver for TMS320F28004x real-time microcontroller projects.
Simply plug in your own SPI and GPIO routines, then use the provided APIs to initialize the display and render text, numbers, and graphics with ease.
---

## Features

- Hardware‐independent: inject SPI and GPIO functions via `oledConfig()`
- Supports 6×8 and 8×16 ASCII fonts
- Display integers and floating-point values
- Draw individual pixels
- Full‐screen clear and refresh

---

## Integration

1. **Copy source files**  
   - `oled.h`  
   - `oled.c`  
   - `oled_font.h` (font tables)

2. **Implement platform hooks**  
   In your board code, provide:
   ```c
   // Transmit one byte over SPI
   void SPI_WriteByte(uint32_t spiBase, uint8_t data);

   // Microsecond delay
   void Delay_us(uint32_t micros);

   // Drive OLED reset pin
   void GPIO_WritePin(uint32_t pin, bool level);
