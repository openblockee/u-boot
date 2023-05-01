/*
 * Driver for the Solomon SSD1306 OLED controller
 *
 * Copyright 2022 Arthur Zheng
 */

#include <common.h>
#include <i2c.h>
#include "ssd1306.h"
#include "ssd1306_font.h"

struct udevice *dev;
u8 ssd1306_GRAM[8][128];

void ssd1306_color_invert(bool invert)
{
    if (invert)
    {
        dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, SSD1306_INVERTDISPLAY);
    }
    else
    {
        dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, SSD1306_NORMALDISPLAY);
    }
}

void ssd1306_display_reverse(bool reverse)
{
    if (reverse)
    {
        dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, SSD1306_COMSCANINC);
        dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, 0xA0);
    }
    else
    {
        dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, SSD1306_COMSCANDEC);
        dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, 0xA1);
    }
}

void ssd1306_refresh(void)
{
    u8 i;
    for (i = 0; i < 8; i++)
    {
        dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, 0xb0 + i);
        dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, 0x00);
        dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, 0x10);
        dm_i2c_write(dev, SSD1306_I2C_DATA, ssd1306_GRAM[i], 128);
    }
}

void ssd1306_clear(void)
{
    u8 i, n;
    for (i = 0; i < 8; i++)
    {
        for (n = 0; n < 128; n++)
        {
            ssd1306_GRAM[i][n] = 0x00;
        }
    }
    ssd1306_refresh();
}

void ssd1306_draw_point(u8 x, u8 y, u8 color)
{
    u8 i, m, n;
    i = y / 8;
    m = y % 8;
    n = 1 << m;
    if (color)
    {
        ssd1306_GRAM[i][x] |= n;
    }
    else
    {
        ssd1306_GRAM[i][x] = ~ssd1306_GRAM[i][x];
        ssd1306_GRAM[i][x] |= n;
        ssd1306_GRAM[i][x] = ~ssd1306_GRAM[i][x];
    }
}

void ssd1306_draw_line(u8 x1, u8 y1, u8 x2, u8 y2, u8 color)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if (delta_x > 0)
        incx = 1;
    else if (delta_x == 0)
        incx = 0;
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0)
        incy = 0;
    else
    {
        incy = -1;
        delta_y = -delta_x;
    }
    if (delta_x > delta_y)
        distance = delta_x;
    else
        distance = delta_y;
    for (t = 0; t < distance + 1; t++)
    {
        ssd1306_draw_point(uRow, uCol, color);
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

void ssd1306_draw_circle(u8 x, u8 y, u8 r, u8 color)
{
    int a, b, num;
    a = 0;
    b = r;
    while (2 * b * b >= r * r)
    {
        ssd1306_draw_point(x + a, y - b, color);
        ssd1306_draw_point(x - a, y - b, color);
        ssd1306_draw_point(x - a, y + b, color);
        ssd1306_draw_point(x + a, y + b, color);

        ssd1306_draw_point(x + b, y + a, color);
        ssd1306_draw_point(x + b, y - a, color);
        ssd1306_draw_point(x - b, y - a, color);
        ssd1306_draw_point(x - b, y + a, color);

        a++;
        num = (a * a + b * b) - r * r;
        if (num > 0)
        {
            b--;
            a--;
        }
    }
}

void ssd1306_show_char(u8 x, u8 y, u8 chr, u8 size1, u8 color)
{
    u8 i, m, temp, size2, chr1;
    u8 x0 = x, y0 = y;
    if (size1 == 8)
        size2 = 6;
    else
        size2 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * (size1 / 2);
    chr1 = chr - ' ';
    for (i = 0; i < size2; i++)
    {
        if (size1 == 8)
        {
            temp = asc2_0806[chr1][i];
        }
        else if (size1 == 12)
        {
            temp = asc2_1206[chr1][i];
        }
        else if (size1 == 16)
        {
            temp = asc2_1608[chr1][i];
        }
        else if (size1 == 24)
        {
            temp = asc2_2412[chr1][i];
        }
        else
            return;
        for (m = 0; m < 8; m++)
        {
            if (temp & 0x01)
                ssd1306_draw_point(x, y, color);
            else
                ssd1306_draw_point(x, y, !color);
            temp >>= 1;
            y++;
        }
        x++;
        if ((size1 != 8) && ((x - x0) == size1 / 2))
        {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0;
    }
}

void ssd1306_show_string(u8 x, u8 y, u8 *chr, u8 size1, u8 color)
{
    while ((*chr >= ' ') && (*chr <= '~'))
    {
        ssd1306_show_char(x, y, *chr, size1, color);
        if (size1 == 8)
            x += 6;
        else
            x += size1 / 2;
        chr++;
    }
}

// m^n
u32 ssd1306_pow(u8 m, u8 n)
{
    u32 result = 1;
    while (n--)
    {
        result *= m;
    }
    return result;
}

void ssd1306_show_num(u8 x, u8 y, u32 num, u8 len, u8 size1, u8 color)
{
    u8 t, temp, m = 0;
    if (size1 == 8)
        m = 2;
    for (t = 0; t < len; t++)
    {
        temp = (num / ssd1306_pow(10, len - t - 1)) % 10;
        if (temp == 0)
        {
            ssd1306_show_char(x + (size1 / 2 + m) * t, y, '0', size1, color);
        }
        else
        {
            ssd1306_show_char(x + (size1 / 2 + m) * t, y, temp + '0', size1, color);
        }
    }
}

void ssd1306_show_chinese(u8 x, u8 y, u8 num, u8 size1, u8 color)
{
    u8 m, temp;
    u8 x0 = x, y0 = y;
    u16 i, size3 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * size1;
    for (i = 0; i < size3; i++)
    {
        if (size1 == 16)
        {
            temp = Hzk1[num][i];
        }
        else if (size1 == 24)
        {
            temp = Hzk2[num][i];
        }
        else if (size1 == 32)
        {
            temp = Hzk3[num][i];
        }
        else if (size1 == 64)
        {
            temp = Hzk4[num][i];
        }
        else
            return;
        for (m = 0; m < 8; m++)
        {
            if (temp & 0x01)
                ssd1306_draw_point(x, y, color);
            else
                ssd1306_draw_point(x, y, !color);
            temp >>= 1;
            y++;
        }
        x++;
        if ((x - x0) == size1)
        {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0;
    }
}

void ssd1306_show_picture(u8 x, u8 y, u8 sizex, u8 sizey, u8 BMP[], u8 color)
{
    u16 j = 0;
    u8 i, n, temp, m;
    u8 x0 = x, y0 = y;
    sizey = sizey / 8 + ((sizey % 8) ? 1 : 0);
    for (n = 0; n < sizey; n++)
    {
        for (i = 0; i < sizex; i++)
        {
            temp = BMP[j];
            j++;
            for (m = 0; m < 8; m++)
            {
                if (temp & 0x01)
                    ssd1306_draw_point(x, y, color);
                else
                    ssd1306_draw_point(x, y, !color);
                temp >>= 1;
                y++;
            }
            x++;
            if ((x - x0) == sizex)
            {
                x = x0;
                y0 = y0 + 8;
            }
            y = y0;
        }
    }
}

void ssd1306_init(void)
{
	int ret;
    
	ret = i2c_get_chip_for_busnum(SSD1306_I2C_BUS_NUM, SSD1306_I2C_ADDR, 1, &dev);
    if (ret) {
		printf("I2C: Bus %d has no device with address 0x%02X\n", 
            SSD1306_I2C_BUS_NUM, SSD1306_I2C_ADDR);
		return;
	}

    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, SSD1306_DISPLAYOFF);         /* display off */
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, SSD1306_SETLOWCOLUMN);       /* set lower column address */
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, SSD1306_SETHIGHCOLUMN);      /* set higher column address */
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, 0x40);                       /* set display start line  Set Mapping RAM Display Start Line (0x00~0x3F)*/
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, SSD1306_START_PAGE_ADDRESS); /* set page address */
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, SSD1306_SETCONTRAST);        /* contract control */
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, 0xff);
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, SSD1306_SEGREMAP);           /* set segment remap */
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, SSD1306_NORMALDISPLAY);      /* normal / reverse */
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, SSD1306_SETMULTIPLEX);       /* multiplex ratio */
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, 0x3F);                       /* duty = 1/64 */
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, SSD1306_COMSCANDEC);         /* Com scan direction */
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, SSD1306_SETDISPLAYOFFSET);   /* set display offset */
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, 0x00);
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, SSD1306_SETDISPLAYCLOCKDIV); /* set osc division */
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, 0x80);
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, SSD1306_SETPRECHARGE); /* set pre-charge period */
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, 0xf1);
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, SSD1306_SETCOMPINS); /* set COM pins */
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, 0x12);
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, SSD1306_SETVCOMDETECT); /* set vcomh */
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, 0x30);
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, SSD1306_CHARGEPUMP); /* set charge pump enable */
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, 0x14);
    ssd1306_clear();
    dm_i2c_reg_write(dev, SSD1306_I2C_COMMAND, 0xAF); /* display ON */
}