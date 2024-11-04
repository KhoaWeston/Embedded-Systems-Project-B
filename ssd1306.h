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

#define I2C_ADDR		(0x78)	// I2c address
#define OLED_WIDTH		(128)	// SSD1306 width in pixels
#define OLED_HEIGHT		(64)	// SSD1306 LCD height in pixels
#define FONT_HEIGHT		(10)
#define FONT_WIDTH		(7)

// Enumeration for screen colors
enum OledColors{
    BLACK,		// Black color, no pixel
    BLUE,		// Pixel is set. Color depends on LCD
};

extern const uint16_t Font7x10[];


class OledI2cDriver{
private:
	I2C_HandleTypeDef* i2c_handle;
	uint16_t currentX;
    uint16_t currentY;
    uint8_t buffer[OLED_WIDTH * OLED_HEIGHT / 8];

    void write_command(uint8_t );
    void write_buffer(uint8_t , uint8_t* , uint16_t );
	void draw_pixel(uint8_t, uint8_t, OledColors);
	char write_char(char);
public:
	// Function definitions
	OledI2cDriver(void);
	void I2C_init(I2C_HandleTypeDef*);
	void update_screen(void);
	char write_string(const char*);
	void set_cursor(uint8_t, uint8_t);
};


#endif  // _SSD1306_H

