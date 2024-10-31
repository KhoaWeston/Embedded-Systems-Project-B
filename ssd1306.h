/*
 * This Library is written and optimized by Olivier Van den Eede(4ilo) in 2016
 * for Stm32 Uc and HAL-i2c lib's.
 *
 * To use this library with ssd1306 oled display you will need to customize the defines below.
 */


#ifndef _SSD1306_H
#define _SSD1306_H

#include "main.h"
#include "stm32l4xx_ll_i2c.h"

#define SSD1306_I2C_ADDR        (0x78)	// I2c address
#define SSD1306_WIDTH           (128)	// SSD1306 width in pixels
#define SSD1306_HEIGHT          (64)	// SSD1306 LCD height in pixels
#define FONT_HEIGHT 			(10)
#define FONT_WIDTH 				(7)

// Enumeration for screen colors
enum SSD1306_COLOR{
    BLACK,		// Black color, no pixel
    BLUE,		// Pixel is set. Color depends on LCD
};

extern const uint16_t Font7x10[];


class OledI2CDriver{
private:
	uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Inverted;
    uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
    void ssd1306_write_command(I2C_HandleTypeDef *, uint8_t );
    void ssd1306_write_buffer(I2C_HandleTypeDef *, uint8_t , uint8_t* , uint16_t );
	void ssd1306_draw_pixel(uint8_t, uint8_t, SSD1306_COLOR);
	char ssd1306_write_char(char);
public:
	// Function definitions
	void ssd1306_init(I2C_HandleTypeDef*);
	void ssd1306_update_screen(I2C_HandleTypeDef*);
	char ssd1306_write_string(const char*);
	void ssd1306_set_cursor(uint8_t, uint8_t);
};


#endif  // _SSD1306_H

