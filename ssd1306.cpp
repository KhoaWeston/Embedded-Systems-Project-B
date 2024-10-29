#include "ssd1306.h"


// Screenbuffer
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

// Screen object
static SSD1306_t SSD1306;


//  Send a byte to the command register
static uint8_t ssd1306_WriteCommand(I2C_HandleTypeDef *hi2c, uint8_t command){
    return HAL_I2C_Mem_Write(hi2c, SSD1306_I2C_ADDR, 0x00, 1, &command, 1, 10);
}


//  Initialize the oled screen
uint8_t ssd1306_Init(I2C_HandleTypeDef *hi2c){
    // Wait for the screen to boot
    HAL_Delay(100);
    int status = 0;

    // Init LCD
    status += ssd1306_WriteCommand(hi2c, 0xAE);   // Display off
    status += ssd1306_WriteCommand(hi2c, 0x20);   // Set Memory Addressing Mode
    status += ssd1306_WriteCommand(hi2c, 0x10);   // 00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    status += ssd1306_WriteCommand(hi2c, 0xB0);   // Set Page Start Address for Page Addressing Mode,0-7
    status += ssd1306_WriteCommand(hi2c, 0xC8);   // Set COM Output Scan Direction
    status += ssd1306_WriteCommand(hi2c, 0x00);   // Set low column address
    status += ssd1306_WriteCommand(hi2c, 0x10);   // Set high column address
    status += ssd1306_WriteCommand(hi2c, 0x40);   // Set start line address
    status += ssd1306_WriteCommand(hi2c, 0x81);   // set contrast control register
    status += ssd1306_WriteCommand(hi2c, 0xFF);
    status += ssd1306_WriteCommand(hi2c, 0xA1);   // Set segment re-map 0 to 127
    status += ssd1306_WriteCommand(hi2c, 0xA6);   // Set normal display

    status += ssd1306_WriteCommand(hi2c, 0xA8);   // Set multiplex ratio(1 to 64)
    status += ssd1306_WriteCommand(hi2c, SSD1306_HEIGHT - 1);

    status += ssd1306_WriteCommand(hi2c, 0xA4);   // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    status += ssd1306_WriteCommand(hi2c, 0xD3);   // Set display offset
    status += ssd1306_WriteCommand(hi2c, 0x00);   // No offset
    status += ssd1306_WriteCommand(hi2c, 0xD5);   // Set display clock divide ratio/oscillator frequency
    status += ssd1306_WriteCommand(hi2c, 0xF0);   // Set divide ratio
    status += ssd1306_WriteCommand(hi2c, 0xD9);   // Set pre-charge period
    status += ssd1306_WriteCommand(hi2c, 0x22);

    status += ssd1306_WriteCommand(hi2c, 0xDA);   // Set com pins hardware configuration
    status += ssd1306_WriteCommand(hi2c, SSD1306_COM_LR_REMAP << 5 | SSD1306_COM_ALTERNATIVE_PIN_CONFIG << 4 | 0x02);

    status += ssd1306_WriteCommand(hi2c, 0xDB);   // Set vcomh
    status += ssd1306_WriteCommand(hi2c, 0x20);   // 0x20,0.77xVcc
    status += ssd1306_WriteCommand(hi2c, 0x8D);   // Set DC-DC enable
    status += ssd1306_WriteCommand(hi2c, 0x14);   //
    status += ssd1306_WriteCommand(hi2c, 0xAF);   // Turn on SSD1306 panel

    if (status != 0) {
        return 1;
    }

    // Clear screen
    for(uint32_t i = 0; i < sizeof(SSD1306_Buffer); i++){
		SSD1306_Buffer[i] = 0x00;
	}

    // Flush buffer to screen
    ssd1306_UpdateScreen(hi2c);

    // Set default values for screen object
    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;

    SSD1306.Initialized = 1;

    return 0;
}


//  Write the screenbuffer with changed to the screen
void ssd1306_UpdateScreen(I2C_HandleTypeDef *hi2c){
    for (uint8_t i = 0; i < 8; i++) {
        ssd1306_WriteCommand(hi2c, 0xB0 + i);
        ssd1306_WriteCommand(hi2c, 0x00);
        ssd1306_WriteCommand(hi2c, 0x10);

        HAL_I2C_Mem_Write(hi2c, SSD1306_I2C_ADDR, 0x40, 1, &SSD1306_Buffer[SSD1306_WIDTH * i], SSD1306_WIDTH, 100);
    }
}


//  Draw one pixel in the screenbuffer
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color){
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT){
        // Don't write outside the buffer
        return;
    }

	uint16_t index = x + (y / 8) * SSD1306_WIDTH;
	uint8_t bit_mask = 1 << (y % 8);

	if (color != SSD1306.Inverted) {
		SSD1306_Buffer[index] |= bit_mask; // Set pixel
	} else {
		SSD1306_Buffer[index] &= ~bit_mask; // Clear pixel
	}
}


//
//  Draw 1 char to the screen buffer
//  ch      => Character to write
//
char ssd1306_WriteChar(char ch)
{
    uint32_t b;

    // Check remaining space on current line
    if (SSD1306_WIDTH <= (SSD1306.CurrentX + FONT_WIDTH) || SSD1306_HEIGHT <= (SSD1306.CurrentY + FONT_HEIGHT)) {
        // Not enough space on current line
        return 0;
    }

    // Translate font to screenbuffer
    for (uint32_t i = 0; i < FONT_HEIGHT; i++){
        b = Font7x10[(ch - 32) * FONT_HEIGHT + i];
        for (uint32_t j = 0; j < FONT_WIDTH; j++) {
            if ((b << j) & 0x8000){
                ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), Blue);
            }else{
                ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), Black);
            }
        }
    }

    // The current space is now taken
    SSD1306.CurrentX += FONT_WIDTH;

    // Return written char for validation
    return ch;
}


//  Write full string to screenbuffer
char ssd1306_WriteString(const char* str){
    // Write until null-byte
    while (*str){
        if (ssd1306_WriteChar(*str) != *str){
            // Char could not be written
            return *str;
        }

        // Next char
        str++;
    }

    // Everything ok
    return *str;
}


//  Set cursor position
void ssd1306_SetCursor(uint8_t x, uint8_t y){
    SSD1306.CurrentX = x;
    SSD1306.CurrentY = y;
}
