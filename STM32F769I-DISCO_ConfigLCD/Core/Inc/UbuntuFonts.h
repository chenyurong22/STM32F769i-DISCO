/*
 * Ubuntu40.h
 *
 *  Created on: Mar 18, 2026
 *      Author: debasish
 */

#ifndef INC_UBUNTU40_H_
#define INC_UBUNTU40_H_

#include <stdint.h>
#include <stdbool.h>


#define LCD_WIDTH 480
#define LCD_FRAMEBUFFER 0xC0000000

/* Define Color code RGB565 */
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0

void DD_LCD_DrawChar24(uint16_t x, uint16_t y, char c, uint16_t color);
void DD_LCD_DrawChar40(uint16_t x, uint16_t y, char c, uint16_t color);

void LCD_DrawChar(uint16_t x, uint16_t y, const uint8_t *glyph, uint16_t color);

void DD_LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color);


void LCD_DrawRectangle(uint16_t x, uint16_t y,
		uint16_t width, uint16_t height, uint16_t color);

#endif /* INC_UBUNTU40_H_ */
