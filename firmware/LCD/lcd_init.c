/**
 * @file    lcd_init.c
 * @brief   SSD1306/SH1106 OLED 驱动层 (128x64, 软件SPI, 帧缓冲)
 * @note    适配 MSPM0G3507 地猛星主控, 硬编码引脚 PA12/13/15/16/21
 */

#include "lcd_init.h"
#include "board.h"   // delay_ms()

/* 帧缓冲区 */
u8 OLED_GRAM[OLED_W][OLED_PAGES];

/******************************************************************************
 * OLED_GPIO_Init — 手动初始化OLED引脚 (不依赖syscfg)
 ******************************************************************************/
void OLED_GPIO_Init(void)
{
    DL_GPIO_initDigitalOutput(GPIO_OLED_SCL_IOMUX);
    DL_GPIO_initDigitalOutput(GPIO_OLED_SDA_IOMUX);
    DL_GPIO_initDigitalOutput(GPIO_OLED_RES_IOMUX);
    DL_GPIO_initDigitalOutput(GPIO_OLED_DC_IOMUX);
    DL_GPIO_initDigitalOutput(GPIO_OLED_CS_IOMUX);

    DL_GPIO_clearPins(OLED_PORT, OLED_SCL_PIN | OLED_SDA_PIN);
    DL_GPIO_setPins(OLED_PORT, OLED_RES_PIN | OLED_DC_PIN | OLED_CS_PIN);
    DL_GPIO_enableOutput(OLED_PORT,
        OLED_SCL_PIN | OLED_SDA_PIN | OLED_RES_PIN | OLED_DC_PIN | OLED_CS_PIN);
}

/******************************************************************************
 * OLED_SPI_WriteByte — 软件SPI写入一个字节
 ******************************************************************************/
static void OLED_SPI_WriteByte(u8 dat)
{
    u8 i;
    for (i = 0; i < 8; i++) {
        OLED_SCLK_Clr();
        if (dat & 0x80) OLED_SDA_Set();
        else            OLED_SDA_Clr();
        OLED_SCLK_Set();
        dat <<= 1;
    }
}

/******************************************************************************
 * OLED_WR_Byte — 向OLED写命令或数据
 ******************************************************************************/
void OLED_WR_Byte(u8 dat, u8 cmd)
{
    if (cmd) OLED_DC_Set();
    else     OLED_DC_Clr();
    OLED_CS_Clr();
    OLED_SPI_WriteByte(dat);
    OLED_CS_Set();
}

/******************************************************************************
 * OLED_Reset — 硬件复位
 ******************************************************************************/
static void OLED_Reset(void)
{
    OLED_RES_Set(); delay_ms(100);
    OLED_RES_Clr(); delay_ms(100);
    OLED_RES_Set(); delay_ms(100);
}

/******************************************************************************
 * OLED_Init — SSD1306 / SH1106 初始化
 ******************************************************************************/
void OLED_Init(void)
{
    OLED_GPIO_Init();
    delay_ms(200);
    OLED_Reset();

#if OLED_IS_SH1106
    /************** SH1106 初始化 (1.3寸OLED) *****************/
    OLED_WR_Byte(0xAE, OLED_CMD);  // Display OFF
    OLED_WR_Byte(0x02, OLED_CMD);  // Lower Column = 2
    OLED_WR_Byte(0x10, OLED_CMD);  // Higher Column
    OLED_WR_Byte(0x40, OLED_CMD);  // Start Line = 0
    OLED_WR_Byte(0xB0, OLED_CMD);  // Page = 0
    OLED_WR_Byte(0x81, OLED_CMD); OLED_WR_Byte(0xFF, OLED_CMD);  // Contrast max
    OLED_WR_Byte(0xA1, OLED_CMD);  // Segment Re-map
    OLED_WR_Byte(0xA6, OLED_CMD);  // Normal Display
    OLED_WR_Byte(0xA8, OLED_CMD); OLED_WR_Byte(0x3F, OLED_CMD);  // MUX 64
    OLED_WR_Byte(0xC8, OLED_CMD);  // COM Scan
    OLED_WR_Byte(0xD3, OLED_CMD); OLED_WR_Byte(0x00, OLED_CMD);  // Offset 0
    OLED_WR_Byte(0xD5, OLED_CMD); OLED_WR_Byte(0x80, OLED_CMD);  // Clock
    OLED_WR_Byte(0xD9, OLED_CMD); OLED_WR_Byte(0x1F, OLED_CMD);  // Pre-charge
    OLED_WR_Byte(0xDA, OLED_CMD); OLED_WR_Byte(0x12, OLED_CMD);  // COM Pins
    OLED_WR_Byte(0xDB, OLED_CMD); OLED_WR_Byte(0x40, OLED_CMD);  // VCOMH
    OLED_WR_Byte(0xAD, OLED_CMD); OLED_WR_Byte(0x8A, OLED_CMD);  // Charge Pump
    OLED_WR_Byte(0xAF, OLED_CMD);  // Display ON
#else
    /************** SSD1306 初始化 (0.96寸OLED) *****************/
    OLED_WR_Byte(0xAE, OLED_CMD);  // Display OFF
    OLED_WR_Byte(0x00, OLED_CMD); OLED_WR_Byte(0x10, OLED_CMD);  // Column
    OLED_WR_Byte(0x40, OLED_CMD);  // Start Line
    OLED_WR_Byte(0x81, OLED_CMD); OLED_WR_Byte(0xFF, OLED_CMD);
    OLED_WR_Byte(0xA1, OLED_CMD);
    OLED_WR_Byte(0xA6, OLED_CMD);
    OLED_WR_Byte(0xA8, OLED_CMD); OLED_WR_Byte(0x3F, OLED_CMD);
    OLED_WR_Byte(0xC8, OLED_CMD);
    OLED_WR_Byte(0xD3, OLED_CMD); OLED_WR_Byte(0x00, OLED_CMD);
    OLED_WR_Byte(0xD5, OLED_CMD); OLED_WR_Byte(0x80, OLED_CMD);
    OLED_WR_Byte(0xD9, OLED_CMD); OLED_WR_Byte(0xF1, OLED_CMD);
    OLED_WR_Byte(0xDA, OLED_CMD); OLED_WR_Byte(0x12, OLED_CMD);
    OLED_WR_Byte(0xDB, OLED_CMD); OLED_WR_Byte(0x30, OLED_CMD);
    OLED_WR_Byte(0x8D, OLED_CMD); OLED_WR_Byte(0x14, OLED_CMD);
    OLED_WR_Byte(0xAF, OLED_CMD);
#endif
}

/******************************************************************************
 * OLED_Refresh — 刷新帧缓冲到屏幕 (SH1106用页寻址, SSD1306用水平寻址)
 ******************************************************************************/
void OLED_Refresh(void)
{
    u8 page, col;

#if OLED_IS_SH1106
    for (page = 0; page < OLED_PAGES; page++) {
        OLED_WR_Byte(0xB0 + page, OLED_CMD);
        OLED_WR_Byte(0x10, OLED_CMD);
        OLED_WR_Byte(0x02, OLED_CMD);
        for (col = 0; col < OLED_W; col++)
            OLED_WR_Byte(OLED_GRAM[col][page], OLED_DATA);
    }
#else
    OLED_WR_Byte(0x20, OLED_CMD); OLED_WR_Byte(0x00, OLED_CMD);
    OLED_WR_Byte(0x21, OLED_CMD); OLED_WR_Byte(0x00, OLED_CMD); OLED_WR_Byte(127, OLED_CMD);
    OLED_WR_Byte(0x22, OLED_CMD); OLED_WR_Byte(0x00, OLED_CMD); OLED_WR_Byte(7, OLED_CMD);
    for (page = 0; page < OLED_PAGES; page++)
        for (col = 0; col < OLED_W; col++)
            OLED_WR_Byte(OLED_GRAM[col][page], OLED_DATA);
    OLED_WR_Byte(0x20, OLED_CMD); OLED_WR_Byte(0x02, OLED_CMD);
#endif
}

/******************************************************************************
 * OLED_Clear — 清帧缓冲
 ******************************************************************************/
void OLED_Clear(u8 dat)
{
    u8 page, col;
    u8 fill = (dat) ? 0xFF : 0x00;
    for (page = 0; page < OLED_PAGES; page++)
        for (col = 0; col < OLED_W; col++)
            OLED_GRAM[col][page] = fill;
}

/******************************************************************************
 * OLED_DrawPoint — 在帧缓冲画点
 ******************************************************************************/
void OLED_DrawPoint(u8 x, u8 y, u8 color)
{
    if (x >= OLED_W || y >= OLED_H) return;
    if (color)
        OLED_GRAM[x][y / 8] |=  (1 << (y % 8));
    else
        OLED_GRAM[x][y / 8] &= ~(1 << (y % 8));
}

/******************************************************************************
 * OLED_GetPoint — 读帧缓冲像素
 ******************************************************************************/
u8 OLED_GetPoint(u8 x, u8 y)
{
    if (x >= OLED_W || y >= OLED_H) return 0;
    return (OLED_GRAM[x][y / 8] >> (y % 8)) & 0x01;
}

/******************************************************************************
 * OLED_ShowBMP — 显示单色位图 (垂直扫描格式)
 ******************************************************************************/
void OLED_ShowBMP(u8 x, u8 y, u8 w, u8 h, const u8 *p, u8 reversebyte)
{
    u8 col, page;
    u8 pages = (h + 7) / 8;
    for (col = 0; col < w; col++) {
        for (page = 0; page < pages; page++) {
            u8 px = x + col;
            u8 pg = y / 8 + page;
            u8 b = p[col * pages + page];
            if (reversebyte) {
                b = (u8)(((b & 0x80) >> 7) | ((b & 0x40) >> 5) |
                         ((b & 0x20) >> 3) | ((b & 0x10) >> 1) |
                         ((b & 0x08) << 1) | ((b & 0x04) << 3) |
                         ((b & 0x02) << 5) | ((b & 0x01) << 7));
            }
            if (px < OLED_W && pg < OLED_PAGES)
                OLED_GRAM[px][pg] = b;
        }
    }
}
