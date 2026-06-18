#ifndef __LCD_H
#define __LCD_H
#include "lcd_init.h"

#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif

// OLED 单色定义
#define OLED_BLACK  0
#define OLED_WHITE  1

// 保留旧颜色宏名以兼容旧代码, 全部映射到 OLED 单色
// 非黑色 -> 白色(点亮), 黑色 -> 黑色(熄灭)
#define WHITE          OLED_WHITE
#define BLACK          OLED_BLACK
#define RED            OLED_WHITE
#define BLUE           OLED_WHITE
#define BRED           OLED_WHITE
#define GRED           OLED_WHITE
#define GBLUE          OLED_WHITE
#define MAGENTA        OLED_WHITE
#define GREEN          OLED_WHITE
#define CYAN           OLED_WHITE
#define YELLOW         OLED_WHITE
#define BROWN          OLED_WHITE
#define BRRED          OLED_WHITE
#define GRAY           OLED_WHITE
#define DARKBLUE       OLED_WHITE
#define LIGHTBLUE      OLED_WHITE
#define GRAYBLUE       OLED_WHITE
#define LIGHTGREEN     OLED_WHITE
#define LGRAY          OLED_BLACK
#define LGRAYBLUE      OLED_WHITE
#define LBBLUE         OLED_WHITE

#define LCD_W       OLED_W
#define LCD_H       OLED_H

//-----------------基本绘图函数----------------
void LCD_Fill(u8 xsta, u8 ysta, u8 xend, u8 yend, u8 color);
void LCD_DrawPoint(u8 x, u8 y, u8 color);
void LCD_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 color);
void LCD_DrawRectangle(u8 x1, u8 y1, u8 x2, u8 y2, u8 color);
void Draw_Circle(u8 x0, u8 y0, u8 r, u8 color);

//-----------------字符显示函数（内置字库）----------------
void LCD_ShowChar(u8 x, u8 y, u8 num, u8 fc, u8 bc, u8 sizey, u8 mode);
void LCD_ShowString(u8 x, u8 y, const u8 *p, u8 fc, u8 bc, u8 sizey, u8 mode);
void LCD_ShowChinese(u8 x, u8 y, u8 *s, u8 fc, u8 bc, u8 sizey, u8 mode);
void LCD_ShowChinese12x12(u8 x, u8 y, u8 *s, u8 fc, u8 bc, u8 sizey, u8 mode);
void LCD_ShowChinese16x16(u8 x, u8 y, u8 *s, u8 fc, u8 bc, u8 sizey, u8 mode);
void LCD_ShowChinese24x24(u8 x, u8 y, u8 *s, u8 fc, u8 bc, u8 sizey, u8 mode);
void LCD_ShowChinese32x32(u8 x, u8 y, u8 *s, u8 fc, u8 bc, u8 sizey, u8 mode);

//-----------------数字显示函数----------------
u32 mypow(u8 m, u8 n);
void LCD_ShowIntNum(u8 x, u8 y, u16 num, u8 len, u8 fc, u8 bc, u8 sizey);
void LCD_ShowFloatNum1(u8 x, u8 y, float num, u8 len, u8 fc, u8 bc, u8 sizey);

//-----------------图片显示函数----------------
void LCD_ShowPicture(u8 x, u8 y, u16 length, u16 width, const u8 pic[]);

//-----------------字库芯片函数（需外接字库ROM）----------------
void ZK_command(u8 dat);
u8 get_data_from_ROM(void);
void get_n_bytes_data_from_ROM(u8 AddrHigh, u8 AddrMid, u8 AddrLow, u8 *pBuff, u8 DataLen);
void Display_GB2312(u8 x, u8 y, u8 zk_num, u8 fc, u8 bc);
void Display_GB2312_String(u8 x, u8 y, u8 zk_num, u8 text[], u8 fc, u8 bc);
void Display_Asc(u8 x, u8 y, u8 zk_num, u8 fc, u8 bc);
void Display_Asc_String(u8 x, u8 y, u16 zk_num, u8 text[], u8 fc, u8 bc);
void Display_Arial_TimesNewRoman(u8 x, u8 y, u8 zk_num, u8 fc, u8 bc);
void Display_Arial_String(u8 x, u8 y, u16 zk_num, u8 text[], u8 fc, u8 bc);
void Display_TimesNewRoman_String(u8 x, u8 y, u16 zk_num, u8 text[], u8 fc, u8 bc);

#endif
