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
#include "Queue.h"

#define I2C_ADDR		(0x78)	// I2c address
#define OLED_WIDTH		(128)	// SSD1306 width in pixels
#define OLED_HEIGHT		(64)	// SSD1306 LCD height in pixels
#define FONT_HEIGHT		(10)
#define FONT_WIDTH		(7)
#define PAGE_SIZE 		(8)

// Enumeration for screen colors
enum OledColors{
	BLACK,		// Black color, no pixel
	BLUE,		// Pixel is set. Color depends on LCD
};

extern const uint16_t Font7x10[];


/**
 * @brief Provides an interface to control an OLED display over I2C.
 *
 * This class allows for writing text and updating the display
 * by managing a screen buffer and sending data over I2C.
 */
class OledI2cDriver{ // @suppress("Miss copy constructor or assignment operator")
private:
	I2C_HandleTypeDef* i2c_handle;
	uint16_t currentX;
	uint16_t currentY;
	uint8_t buffer[OLED_WIDTH * OLED_HEIGHT / 8];

	/**
	 * @brief Sends a single command byte to the OLED display.
	 * @param x command The command byte to send.
	 */
	void write_command(uint8_t command);

	/**
	 * @brief Sends a block of data to the OLED display at a specified memory address.
	 * @param x mem_addr The memory address in the OLED to write to.
	 * @param x buff Pointer to the buffer of data to send.
	 * @param x size The size of the data in bytes.
	 */
	void write_buffer(uint8_t mem_addr, uint8_t* buff, uint16_t size);

	/**
	 * @brief Draws a single pixel on the screen buffer.
	 * @param x x The x-coordinate of the pixel.
	 * @param x y The y-coordinate of the pixel.
	 * @param x color The color of the pixel.
	 */
	void draw_pixel(uint8_t x, uint8_t y, OledColors color);

	/**
	 * @brief Draws a single character to the screen buffer at the current cursor position.
	 * @param c ch The character to draw.
	 *
	 * If there is not enough space on the current line for the character,
	 * the function will not draw the character.
	 */
	void write_char(char ch);
public:
	/**
	 * @brief Constructs an OledI2cDriver object and initializes the screen buffer.
	 *
	 * Sets up the buffer to be entirely black and initializes the default
	 * cursor position at the top left of the screen.
	 */
	OledI2cDriver(void);

	/**
	 * @brief Initializes the I2C interface and OLED display.
	 * @param x hi2c Pointer to the I2C handle to be used for communication.
	 */
	void I2C_init(I2C_HandleTypeDef* hi2c);

	/**
	 * @brief Updates a specific row of the OLED display with the current buffer content.
	 * @param x row The row of the display to update.
	 */
	void update_screen(uint8_t row);

	/**
	 * @brief Writes a string to the screen buffer.
	 * @param x str Pointer to the string to be displayed.
	 *
	 * The function continues writing until the null byte is reached.
	 */
	void write_string(const char* str);

	/**
	 * @brief Sets the cursor position for text writing to follow.
	 * @param x x The x-coordinate for the cursor.
	 * @param x y The y-coordinate for the cursor.
	 */
	void set_cursor(uint8_t x, uint8_t y);
};


#endif  // _SSD1306_H

