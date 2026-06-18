/**
 * @file    lcd.c
 * @brief   SSD1306 OLED 上层绘图函数 (帧缓冲模式)
 * @note    所有绘图操作先修改 OLED_GRAM 缓冲区, 最后调用 OLED_Refresh() 刷新到屏幕
 */

#include "lcd.h"
#include "lcd_init.h"
#include "lcdfont.h"

/******************************************************************************
 * 函数: LCD_Fill
 * 说明: 在指定区域填充颜色
 *       对OLED: color=0填充黑色(熄灭), color=1填充白色(点亮)
 ******************************************************************************/
void LCD_Fill(u8 xsta, u8 ysta, u8 xend, u8 yend, u8 color)
{
    u8 x, y;

    // 边界裁剪
    if (xend > OLED_W) xend = OLED_W;
    if (yend > OLED_H) yend = OLED_H;

    for (y = ysta; y < yend; y++)
    {
        for (x = xsta; x < xend; x++)
        {
            OLED_DrawPoint(x, y, color);
        }
    }
}

/******************************************************************************
 * 函数: LCD_DrawPoint
 * 说明: 在指定位置画一个点 (修改帧缓冲)
 ******************************************************************************/
void LCD_DrawPoint(u8 x, u8 y, u8 color)
{
    OLED_DrawPoint(x, y, color);
}

/******************************************************************************
 * 函数: LCD_DrawLine
 * 说明: Bresenham画线算法 (帧缓冲)
 ******************************************************************************/
void LCD_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 color)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;

    delta_x = x2 - x1;
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;

    if (delta_x > 0) incx = 1;
    else if (delta_x == 0) incx = 0;
    else { incx = -1; delta_x = -delta_x; }

    if (delta_y > 0) incy = 1;
    else if (delta_y == 0) incy = 0;
    else { incy = -1; delta_y = -delta_y; }

    if (delta_x > delta_y) distance = delta_x;
    else distance = delta_y;

    for (t = 0; t < distance + 1; t++)
    {
        LCD_DrawPoint(uRow, uCol, color);
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

/******************************************************************************
 * 函数: LCD_DrawRectangle
 * 说明: 画矩形边框
 ******************************************************************************/
void LCD_DrawRectangle(u8 x1, u8 y1, u8 x2, u8 y2, u8 color)
{
    LCD_DrawLine(x1, y1, x2, y1, color);
    LCD_DrawLine(x1, y1, x1, y2, color);
    LCD_DrawLine(x1, y2, x2, y2, color);
    LCD_DrawLine(x2, y1, x2, y2, color);
}

/******************************************************************************
 * 函数: Draw_Circle
 * 说明: 画圆 (Bresenham中点算法)
 ******************************************************************************/
void Draw_Circle(u8 x0, u8 y0, u8 r, u8 color)
{
    int a, b;
    a = 0;
    b = r;
    while (a <= b)
    {
        LCD_DrawPoint(x0 - b, y0 - a, color);
        LCD_DrawPoint(x0 + b, y0 - a, color);
        LCD_DrawPoint(x0 - a, y0 + b, color);
        LCD_DrawPoint(x0 - a, y0 - b, color);
        LCD_DrawPoint(x0 + b, y0 + a, color);
        LCD_DrawPoint(x0 + a, y0 - b, color);
        LCD_DrawPoint(x0 + a, y0 + b, color);
        LCD_DrawPoint(x0 - b, y0 + a, color);
        a++;
        if ((a * a + b * b) > (r * r))
        {
            b--;
        }
    }
}

/******************************************************************************
 * 函数: LCD_ShowChar
 * 说明: 在帧缓冲中显示一个ASCII字符 (使用内置字库)
 *       sizey: 12/16/24/32
 *       mode:  0=覆盖(含背景色), 1=叠加(仅画前景)
 ******************************************************************************/
void LCD_ShowChar(u8 x, u8 y, u8 num, u8 fc, u8 bc, u8 sizey, u8 mode)
{
    u8 temp, sizex, t;
    u16 i, TypefaceNum;
    u8 col, row;

    sizex = sizey / 2;  // 字符宽度 = 高度 / 2
    TypefaceNum = (sizex / 8 + ((sizex % 8) ? 1 : 0)) * sizey;
    num = num - ' ';    // 偏移到字库索引

    for (i = 0; i < TypefaceNum; i++)
    {
        // 取一行的字模数据
        if (sizey == 12)      temp = ascii_1206[num][i];
        else if (sizey == 16) temp = ascii_1608[num][i];
        else if (sizey == 24) temp = ascii_2412[num][i];
        else if (sizey == 32) temp = ascii_3216[num][i];
        else return;

        // 当前行在整个字符中的行号
        row = y + i;

        for (t = 0; t < 8; t++)
        {
            // 字模格式: 每字节为一行, LSB=左侧像素, MSB对齐
            // 对于宽度<8的字体, 有效位在低sizex位
            if (t >= sizex) break;  // 超出字体宽度

            col = x + t;

            if (temp & (0x01 << t))
                OLED_GRAM[col][row / 8] |= (1 << (row % 8));       // 前景色
            else if (!mode)  // 非叠加模式才画背景
                OLED_GRAM[col][row / 8] &= ~(1 << (row % 8));      // 背景色(黑色)
        }
    }
}

/******************************************************************************
 * 函数: LCD_ShowString
 * 说明: 在帧缓冲中显示字符串
 ******************************************************************************/
void LCD_ShowString(u8 x, u8 y, const u8 *p, u8 fc, u8 bc, u8 sizey, u8 mode)
{
    while (*p != '\0')
    {
        LCD_ShowChar(x, y, *p, fc, bc, sizey, mode);
        x += sizey / 2;
        p++;
    }
}

/******************************************************************************
 * 函数: LCD_ShowChinese (分发到具体字号函数)
 ******************************************************************************/
void LCD_ShowChinese(u8 x, u8 y, u8 *s, u8 fc, u8 bc, u8 sizey, u8 mode)
{
    while (*s != 0)
    {
        if (sizey == 12)      LCD_ShowChinese12x12(x, y, s, fc, bc, sizey, mode);
        else if (sizey == 16) LCD_ShowChinese16x16(x, y, s, fc, bc, sizey, mode);
        else if (sizey == 24) LCD_ShowChinese24x24(x, y, s, fc, bc, sizey, mode);
        else if (sizey == 32) LCD_ShowChinese32x32(x, y, s, fc, bc, sizey, mode);
        else return;
        s += 2;
        x += sizey;
    }
}

/******************************************************************************
 * 函数: LCD_ShowChinese12x12
 ******************************************************************************/
void LCD_ShowChinese12x12(u8 x, u8 y, u8 *s, u8 fc, u8 bc, u8 sizey, u8 mode)
{
    u8 i, j;
    u16 k;
    u16 HZnum;
    u16 TypefaceNum;
    u16 x0 = x;

    TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;  // 24 bytes
    HZnum = sizeof(tfont12) / sizeof(typFNT_GB12);

    for (k = 0; k < HZnum; k++)
    {
        if ((tfont12[k].Index[0] == *(s)) && (tfont12[k].Index[1] == *(s + 1)))
        {
            for (i = 0; i < TypefaceNum; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (!mode)  // 覆盖模式
                    {
                        if (tfont12[k].Msk[i] & (0x01 << j))
                            OLED_GRAM[x][y / 8] |= (1 << (y % 8));
                        else
                            OLED_GRAM[x][y / 8] &= ~(1 << (y % 8));
                        x++;
                        if ((x - x0) == sizey)
                        {
                            x = x0;
                            y++;
                            break;
                        }
                    }
                    else  // 叠加模式
                    {
                        if (tfont12[k].Msk[i] & (0x01 << j))
                            LCD_DrawPoint(x, y, fc);
                        x++;
                        if ((x - x0) == sizey)
                        {
                            x = x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }
        continue;
    }
}

/******************************************************************************
 * 函数: LCD_ShowChinese16x16
 ******************************************************************************/
void LCD_ShowChinese16x16(u8 x, u8 y, u8 *s, u8 fc, u8 bc, u8 sizey, u8 mode)
{
    u8 i, j;
    u16 k;
    u16 HZnum;
    u16 TypefaceNum;
    u16 x0 = x;

    TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
    HZnum = sizeof(tfont16) / sizeof(typFNT_GB16);

    for (k = 0; k < HZnum; k++)
    {
        if ((tfont16[k].Index[0] == *(s)) && (tfont16[k].Index[1] == *(s + 1)))
        {
            for (i = 0; i < TypefaceNum; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (!mode)
                    {
                        if (tfont16[k].Msk[i] & (0x01 << j))
                            OLED_GRAM[x][y / 8] |= (1 << (y % 8));
                        else
                            OLED_GRAM[x][y / 8] &= ~(1 << (y % 8));
                        x++;
                        if ((x - x0) == sizey)
                        {
                            x = x0;
                            y++;
                            break;
                        }
                    }
                    else
                    {
                        if (tfont16[k].Msk[i] & (0x01 << j))
                            LCD_DrawPoint(x, y, fc);
                        x++;
                        if ((x - x0) == sizey)
                        {
                            x = x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }
        continue;
    }
}

/******************************************************************************
 * 函数: LCD_ShowChinese24x24
 ******************************************************************************/
void LCD_ShowChinese24x24(u8 x, u8 y, u8 *s, u8 fc, u8 bc, u8 sizey, u8 mode)
{
    u8 i, j;
    u16 k;
    u16 HZnum;
    u16 TypefaceNum;
    u16 x0 = x;

    TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
    HZnum = sizeof(tfont24) / sizeof(typFNT_GB24);

    for (k = 0; k < HZnum; k++)
    {
        if ((tfont24[k].Index[0] == *(s)) && (tfont24[k].Index[1] == *(s + 1)))
        {
            for (i = 0; i < TypefaceNum; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (!mode)
                    {
                        if (tfont24[k].Msk[i] & (0x01 << j))
                            OLED_GRAM[x][y / 8] |= (1 << (y % 8));
                        else
                            OLED_GRAM[x][y / 8] &= ~(1 << (y % 8));
                        x++;
                        if ((x - x0) == sizey)
                        {
                            x = x0;
                            y++;
                            break;
                        }
                    }
                    else
                    {
                        if (tfont24[k].Msk[i] & (0x01 << j))
                            LCD_DrawPoint(x, y, fc);
                        x++;
                        if ((x - x0) == sizey)
                        {
                            x = x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }
        continue;
    }
}

/******************************************************************************
 * 函数: LCD_ShowChinese32x32
 ******************************************************************************/
void LCD_ShowChinese32x32(u8 x, u8 y, u8 *s, u8 fc, u8 bc, u8 sizey, u8 mode)
{
    u8 i, j;
    u16 k;
    u16 HZnum;
    u16 TypefaceNum;
    u16 x0 = x;

    TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
    HZnum = sizeof(tfont32) / sizeof(typFNT_GB32);

    for (k = 0; k < HZnum; k++)
    {
        if ((tfont32[k].Index[0] == *(s)) && (tfont32[k].Index[1] == *(s + 1)))
        {
            for (i = 0; i < TypefaceNum; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (!mode)
                    {
                        if (tfont32[k].Msk[i] & (0x01 << j))
                            OLED_GRAM[x][y / 8] |= (1 << (y % 8));
                        else
                            OLED_GRAM[x][y / 8] &= ~(1 << (y % 8));
                        x++;
                        if ((x - x0) == sizey)
                        {
                            x = x0;
                            y++;
                            break;
                        }
                    }
                    else
                    {
                        if (tfont32[k].Msk[i] & (0x01 << j))
                            LCD_DrawPoint(x, y, fc);
                        x++;
                        if ((x - x0) == sizey)
                        {
                            x = x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }
        continue;
    }
}

/******************************************************************************
 * 函数: mypow
 ******************************************************************************/
u32 mypow(u8 m, u8 n)
{
    u32 result = 1;
    while (n--) result *= m;
    return result;
}

/******************************************************************************
 * 函数: LCD_ShowIntNum
 * 说明: 在帧缓冲中显示整数
 ******************************************************************************/
void LCD_ShowIntNum(u8 x, u8 y, u16 num, u8 len, u8 fc, u8 bc, u8 sizey)
{
    u8 t, temp;
    u8 enshow = 0;
    u8 sizex = sizey / 2;

    for (t = 0; t < len; t++)
    {
        temp = (num / mypow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                LCD_ShowChar(x + t * sizex, y, ' ', fc, bc, sizey, 0);
                continue;
            }
            else enshow = 1;
        }
        LCD_ShowChar(x + t * sizex, y, temp + 48, fc, bc, sizey, 0);
    }
}

/******************************************************************************
 * 函数: LCD_ShowFloatNum1
 * 说明: 在帧缓冲中显示2位小数的浮点数
 ******************************************************************************/
void LCD_ShowFloatNum1(u8 x, u8 y, float num, u8 len, u8 fc, u8 bc, u8 sizey)
{
    u8 t, temp, sizex;
    u16 num1;
    sizex = sizey / 2;
    num1 = num * 100;

    for (t = 0; t < len; t++)
    {
        temp = (num1 / mypow(10, len - t - 1)) % 10;
        if (t == (len - 2))
        {
            LCD_ShowChar(x + (len - 2) * sizex, y, '.', fc, bc, sizey, 0);
            t++;
            len += 1;
        }
        LCD_ShowChar(x + t * sizex, y, temp + 48, fc, bc, sizey, 0);
    }
}

/******************************************************************************
 * 函数: LCD_ShowPicture
 * 说明: 在帧缓冲中显示图片 (RGB565格式 → 单色)
 *       pic[] 格式: 逐行扫描, 每像素2字节 RGB565 (高字节在前)
 *       非黑色像素 → 点亮, 黑色像素(0x0000) → 熄灭
 *       length: 图片宽度(像素), width: 图片高度(像素)
 ******************************************************************************/
void LCD_ShowPicture(u8 x, u8 y, u16 length, u16 width, const u8 pic[])
{
    u16 col, row;
    u32 k = 0;

    for (row = 0; row < width; row++)
    {
        for (col = 0; col < length; col++)
        {
            u8 px = x + col;
            u8 py = y + row;

            if (px >= OLED_W || py >= OLED_H)
            {
                k++;
                continue;  // 超出屏幕范围, 跳过
            }

            // RGB565: 每像素2字节, 高字节在前
            // 任一字节非零 → 非黑色 → 点亮像素
            if (pic[k * 2] || pic[k * 2 + 1])
                OLED_GRAM[px][py / 8] |= (1 << (py % 8));
            else
                OLED_GRAM[px][py / 8] &= ~(1 << (py % 8));

            k++;
        }
    }
}
