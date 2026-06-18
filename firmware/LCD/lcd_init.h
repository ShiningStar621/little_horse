#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "../ti_msp_dl_config.h"

// OLED分辨率 (SSD1306/SH1106: 128x64)
#define OLED_W  128
#define OLED_H  64
#define OLED_PAGES (OLED_H / 8)

// OLED 控制器: 0=SSD1306, 1=SH1106 (1.3寸常用SH1106)
#define OLED_IS_SH1106  1

#ifndef u8
#define u8 uint8_t
#endif
#ifndef u16
#define u16 uint16_t
#endif
#ifndef u32
#define u32 uint32_t
#endif

//-----------------OLED 引脚 (避开舵机PA15/PA21, 使用旧TFT的空闲引脚) -----------------
#define OLED_PORT        GPIOA
#define OLED_SCL_PIN     DL_GPIO_PIN_12   // PA12 = SCL (D0)
#define OLED_SDA_PIN     DL_GPIO_PIN_13   // PA13 = SDA (D1)
#define OLED_RES_PIN     DL_GPIO_PIN_17   // PA17 = RES (原BLK, 空闲)
#define OLED_DC_PIN      DL_GPIO_PIN_16   // PA16 = DC
#define OLED_CS_PIN      DL_GPIO_PIN_22   // PA22 = CS (原CS2/字库, 空闲)

#define GPIO_OLED_SCL_IOMUX  IOMUX_PINCM34   // PA12
#define GPIO_OLED_SDA_IOMUX  IOMUX_PINCM35   // PA13
#define GPIO_OLED_RES_IOMUX  IOMUX_PINCM39   // PA17
#define GPIO_OLED_DC_IOMUX   IOMUX_PINCM38   // PA16
#define GPIO_OLED_CS_IOMUX   IOMUX_PINCM47   // PA22

//-----------------OLED 引脚操作宏 -----------------
#define OLED_SCLK_Clr()  DL_GPIO_clearPins(OLED_PORT, OLED_SCL_PIN)
#define OLED_SCLK_Set()  DL_GPIO_setPins(OLED_PORT, OLED_SCL_PIN)
#define OLED_SDA_Clr()   DL_GPIO_clearPins(OLED_PORT, OLED_SDA_PIN)
#define OLED_SDA_Set()   DL_GPIO_setPins(OLED_PORT, OLED_SDA_PIN)
#define OLED_RES_Clr()   DL_GPIO_clearPins(OLED_PORT, OLED_RES_PIN)
#define OLED_RES_Set()   DL_GPIO_setPins(OLED_PORT, OLED_RES_PIN)
#define OLED_DC_Clr()    DL_GPIO_clearPins(OLED_PORT, OLED_DC_PIN)
#define OLED_DC_Set()    DL_GPIO_setPins(OLED_PORT, OLED_DC_PIN)
#define OLED_CS_Clr()    DL_GPIO_clearPins(OLED_PORT, OLED_CS_PIN)
#define OLED_CS_Set()    DL_GPIO_setPins(OLED_PORT, OLED_CS_PIN)

//-----------------OLED 命令/数据 模式----------------
#define OLED_CMD   0
#define OLED_DATA  1

//-----------------帧缓冲区----------------
extern u8 OLED_GRAM[OLED_W][OLED_PAGES];

//-----------------底层驱动函数----------------
void OLED_GPIO_Init(void);
void OLED_WR_Byte(u8 dat, u8 cmd);
void OLED_Init(void);
void OLED_Refresh(void);
void OLED_Clear(u8 dat);
void OLED_DrawPoint(u8 x, u8 y, u8 color);
u8  OLED_GetPoint(u8 x, u8 y);
void OLED_ShowBMP(u8 x, u8 y, u8 w, u8 h, const u8 *p, u8 reversebyte);

#endif
