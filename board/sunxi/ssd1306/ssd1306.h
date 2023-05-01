#ifndef _SSD1306_H
#define _SSD1306_H

#define SSD1306_I2C_BUS_NUM     0
#define SSD1306_I2C_ADDR        (0x78 >> 1)

#define SSD1306_I2C_COMMAND		0x00
#define SSD1306_I2C_DATA	    0x40

#define SSD1306_MEMORYMODE          0x20          
#define SSD1306_COLUMNADDR          0x21          
#define SSD1306_PAGEADDR            0x22            
#define SSD1306_SETCONTRAST         0x81         
#define SSD1306_CHARGEPUMP          0x8D          
#define SSD1306_SEGREMAP            0xA1            
#define SSD1306_DISPLAYALLON_RESUME 0xA4 
#define SSD1306_DISPLAYALLON        0xA5        
#define SSD1306_NORMALDISPLAY       0xA6       
#define SSD1306_INVERTDISPLAY       0xA7       
#define SSD1306_SETMULTIPLEX        0xA8        
#define SSD1306_DISPLAYOFF          0xAE          
#define SSD1306_DISPLAYON           0xAF           
#define SSD1306_START_PAGE_ADDRESS	0xB0
#define SSD1306_COMSCANINC          0xC0          
#define SSD1306_COMSCANDEC          0xC8          
#define SSD1306_SETDISPLAYOFFSET    0xD3    
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5  
#define SSD1306_SETPRECHARGE        0xD9        
#define SSD1306_SETCOMPINS          0xDA          
#define SSD1306_SETVCOMDETECT       0xDB       

#define SSD1306_SETLOWCOLUMN        0x00 
#define SSD1306_SETHIGHCOLUMN       0x10
#define SSD1306_SETSTARTLINE        0x40 

#define SSD1306_EXTERNALVCC         0x01 // External display voltage source
#define SSD1306_SWITCHCAPVCC        0x02 // Gen. display voltage from 3.3V

#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26 // Init rt scroll
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27 // Init left scroll
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29 // Init diag scroll
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A // Init diag scroll
#define SSD1306_DEACTIVATE_SCROLL                    0x2E // Stop scroll
#define SSD1306_ACTIVATE_SCROLL                      0x2F // Start scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3 // Set scroll range

#define SSD1306_BLACK   0   ///< Draw 'off' pixels
#define SSD1306_WHITE   1   ///< Draw 'on' pixels
#define SSD1306_INVERSE 2   ///< Invert pixels

#define SSD1306_FONT_6X8    8
#define SSD1306_FONT_6X12  12
#define SSD1306_FONT_8X16  16
#define SSD1306_FONT_12x24 24

#define SSD1306_CHINESE_FONT_16x16 16
#define SSD1306_CHINESE__FONT24x24 24
#define SSD1306_CHINESE_FONT_32x32 32
#define SSD1306_CHINESE_FONT_64x64 64

void ssd1306_color_invert(bool invert);
void ssd1306_refresh(void);
void ssd1306_clear(void);
void ssd1306_display_reverse(bool reverse);
void ssd1306_draw_point(u8 x, u8 y, u8 color);
void ssd1306_draw_line(u8 x1, u8 y1, u8 x2, u8 y2, u8 color);
void ssd1306_draw_circle(u8 x, u8 y, u8 r, u8 color);
void ssd1306_show_char(u8 x, u8 y, u8 chr, u8 size1, u8 color);
void ssd1306_show_string(u8 x, u8 y, u8 *chr, u8 size1, u8 color);
void ssd1306_show_num(u8 x, u8 y, u32 num, u8 len, u8 size1, u8 color);
void ssd1306_show_chinese(u8 x, u8 y, u8 num, u8 size1, u8 color);
void ssd1306_show_picture(u8 x, u8 y, u8 sizex, u8 sizey, u8 BMP[], u8 color);
void ssd1306_init(void);

#endif
