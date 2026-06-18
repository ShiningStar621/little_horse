/*===========================================================================
 * M03507 (地猛星 MSPM0G3507) + CI1302 语音 + OLED 屏 + 舵机
 *
 * 屏幕: 1.3寸 SH1106 OLED (128x64 单色 SPI)
 *
 * 硬件引脚:
 *   PA14 = LED (板载)
 *   PA10 = UART0 TX (调试/printf)
 *   PA11 = UART0 RX
 *   PB2  = I2C SCL → CI1302 SCL
 *   PB3  = I2C SDA → CI1302 SDA
 *
 *   OLED SPI (软件模拟, 引脚避开舵机PA15/PA21):
 *     PA12 = SCL, PA13 = SDA, PA17 = RES, PA16 = DC, PA22 = CS
 *
 *   舵机 (4路独立PWM):
 *     PA15 = TIMA1 CCP0 = PWM_1 = 前左腿 (FL)
 *     PA0  = TIMA0 CCP0 = PWM_2 = 前右腿 (FR)
 *     PA21 = TIMG6 CCP0 = PWM_3 = 后左腿 (RL)
 *     PA23 = TIMG0 CCP0 = PWM_4 = 后右腿 (RR)
 *=========================================================================*/

#include "ti_msp_dl_config.h"
#include <stdint.h>
#include <stdio.h>
#include "board.h"
#include "LCD/lcd_init.h"
#include "LCD/lcd.h"

/* ---- 图片头文件 (用 Image2Lcd 取模后替换数组) ---- */
#include "LCD/img_idle.h"
#include "LCD/img_go.h"
#include "LCD/img_back.h"
#include "LCD/img_left.h"
#include "LCD/img_right.h"
#include "LCD/img_sit.h"
#include "LCD/img_walk.h"
#include "LCD/img_stop.h"
#include "LCD/img_dance.h"
#include "LCD/img_sing.h"
#include "LCD/img_sleep.h"



/*===========================================================================
 * 1. CI1302 语音模块 — 软件 I2C (PB2/PB3)
 *=========================================================================*/
#ifndef CI1302_SCL_IOMUX
#define CI1302_SCL_IOMUX  I2C_CI1302_SCL_IOMUX
#endif
#ifndef CI1302_SDA_IOMUX
#define CI1302_SDA_IOMUX  I2C_CI1302_SDA_IOMUX
#endif

#define CI1302_PORT       GPIOB
#define CI1302_SCL_PIN    DL_GPIO_PIN_2
#define CI1302_SDA_PIN    DL_GPIO_PIN_3
#define CI1302_ADDR       0x34
#define CI1302_RESULT_REG 0x64
#define CI1302_SPEAK_REG  0x6E
#define CI1302_CMD        0x00
#define CI1302_ANNOUNCE   0xFF

#define CI1302_SDA_OUT() {                                        \
    DL_GPIO_initDigitalOutput(CI1302_SDA_IOMUX);                  \
    DL_GPIO_setPins(CI1302_PORT, CI1302_SDA_PIN);                 \
    DL_GPIO_enableOutput(CI1302_PORT, CI1302_SDA_PIN);            \
}
#define CI1302_SDA_IN()  { DL_GPIO_initDigitalInput(CI1302_SDA_IOMUX); }
#define CI1302_SDA_GET() ( (DL_GPIO_readPins(CI1302_PORT,CI1302_SDA_PIN) \
                          & CI1302_SDA_PIN) ? 1 : 0 )
#define CI1302_SDA(x)   ((x) ? DL_GPIO_setPins(CI1302_PORT,CI1302_SDA_PIN)   \
                             : DL_GPIO_clearPins(CI1302_PORT,CI1302_SDA_PIN))
#define CI1302_SCL(x)   ((x) ? DL_GPIO_setPins(CI1302_PORT,CI1302_SCL_PIN)   \
                             : DL_GPIO_clearPins(CI1302_PORT,CI1302_SCL_PIN))

static void I2C_Start(void)
{
    CI1302_SDA_OUT();
    CI1302_SCL(0); CI1302_SDA(1); CI1302_SCL(1);
    delay_us(5);   CI1302_SDA(0);
    delay_us(5);   CI1302_SCL(0);
    delay_us(5);
}

static void I2C_Stop(void)
{
    CI1302_SDA_OUT();
    CI1302_SCL(0); CI1302_SDA(0); CI1302_SCL(1);
    delay_us(5);   CI1302_SDA(1);
    delay_us(5);
}

static uint8_t I2C_Wait_Ack(void)
{
    unsigned char ack_flag = 10;
    CI1302_SDA_IN(); CI1302_SDA(1); delay_us(5);
    CI1302_SCL(1);   delay_us(5);
    while ((CI1302_SDA_GET() == 1) && ack_flag) {
        ack_flag--; delay_us(5);
    }
    if (ack_flag <= 0) { I2C_Stop(); return 1; }
    CI1302_SCL(0); CI1302_SDA_OUT();
    return 0;
}

static void I2C_Send_Ack(uint8_t ack)
{
    CI1302_SDA_OUT(); CI1302_SCL(0); CI1302_SDA(0); delay_us(5);
    if (!ack) CI1302_SDA(0); else CI1302_SDA(1);
    CI1302_SCL(1); delay_us(5);
    CI1302_SCL(0); CI1302_SDA(1);
}

static void I2C_Write(uint8_t data)
{
    int i;
    CI1302_SDA_OUT(); CI1302_SCL(0);
    for (i = 0; i < 8; i++) {
        CI1302_SDA((data & 0x80) >> 7);
        delay_us(2); data <<= 1; delay_us(6);
        CI1302_SCL(1); delay_us(4);
        CI1302_SCL(0); delay_us(4);
    }
}

static uint8_t I2C_Read(void)
{
    unsigned char i, receive = 0;
    CI1302_SDA_IN();
    for (i = 0; i < 8; i++) {
        CI1302_SCL(0); delay_us(5);
        CI1302_SCL(1); delay_us(5);
        receive <<= 1;
        if (CI1302_SDA_GET()) receive |= 1;
        delay_us(5);
    }
    return receive;
}

uint8_t CI1302_Recognition(void)
{
    uint8_t result = 0;
    I2C_Start();
    I2C_Write(CI1302_ADDR << 1 | 0);
    if (I2C_Wait_Ack() == 1) goto err;
    I2C_Write(CI1302_RESULT_REG);
    if (I2C_Wait_Ack() == 1) goto err;
    I2C_Stop();

    I2C_Start();
    I2C_Write(CI1302_ADDR << 1 | 1);
    if (I2C_Wait_Ack() == 1) goto err;
    result = I2C_Read();
    I2C_Send_Ack(1);
    I2C_Stop();
    return result;

err:
    I2C_Stop();
    return 0;
}

void CI1302_Speak(uint8_t type, uint8_t id)
{
    if (type != CI1302_CMD && type != CI1302_ANNOUNCE) return;
    I2C_Start();
    I2C_Write(CI1302_ADDR << 1 | 0);
    if (I2C_Wait_Ack() == 1) goto err;
    I2C_Write(CI1302_SPEAK_REG);
    if (I2C_Wait_Ack() == 1) goto err;
    I2C_Write(type);
    if (I2C_Wait_Ack() == 1) goto err;
    I2C_Write(id);
    if (I2C_Wait_Ack() == 1) goto err;
    I2C_Stop();
    delay_ms(30);
    return;
err:
    I2C_Stop();
}

/*===========================================================================
 * 2. OLED 显示 — 取代 TJC 串口屏
 *=========================================================================*/

/* ---- 命令名称映射 ---- */
static const char* GetCmdName(uint8_t id)
{
    switch (id) {
    case 0x01: return "Forward";
    case 0x02: return "Backward";
    case 0x03: return "Left";
    case 0x04: return "Right";
    case 0x05: return "Sit";
    case 0x06: return "tand";
    case 0x07: return "Stop";
    case 0x08: return "Dance";
    case 0x09: return "Sing";
    default:   return "Idle";
    }
}

/* Image2Lcd 头: [scan][bpp][w_lo][w_hi][h_lo][h_hi], 6字节 */
#define IMG_W(p)  ((u16)((p)[2] | ((u16)((p)[3] << 8))))
#define IMG_H(p)  ((u16)((p)[4] | ((u16)((p)[5] << 8))))

/* 命令ID → 图片数组映射 */
static const u8* GetCmdImage(uint8_t cmdId, u8 *w, u8 *h)
{
    const u8 *pic = NULL;
    switch (cmdId) {
    case 0x01: pic = gImage_go;    break;
    case 0x02: pic = gImage_back;  break;
    case 0x03: pic = gImage_left;  break;  // 左转
    case 0x04: pic = gImage_right; break;  // 右转
    case 0x05: pic = gImage_sit;   break;
    case 0x06: pic = gImage_walk;  break;
    case 0x07: pic = gImage_sleep;  break;
    case 0x08: pic = gImage_dance;  break;  // 跳舞
    case 0x09: pic = gImage_sing; break;  // 唱歌
    default:   pic = gImage_idle;  break;
    }
    /* 从Image2Lcd头自动读取宽高 (头6字节有效则w>0) */
    *w = IMG_W(pic);
    *h = IMG_H(pic);
    return (*w > 0 && *w <= 128) ? pic : NULL;
}

/* OLED 显示更新 (自动匹配图片) */
static void OLED_DisplayCommand(uint8_t cmdId)
{
    u8 w, h;
    const u8 *pic = GetCmdImage(cmdId, &w, &h);

    OLED_Clear(0);

    if (pic) {
        /* 有图片: 图片居中显示 */
        OLED_ShowBMP((128 - w) / 2, 0, w, h, pic + 6, 1);
        /* 底部显示命令名 */
        LCD_ShowString(0, 52, (u8 *)GetCmdName(cmdId), RED, BLACK, 12, 0);
    } else {
        /* 无图片: 大字显示命令名 */
        LCD_ShowString(0, 8,  (u8 *)GetCmdName(cmdId), RED, BLACK, 16, 0);
        LCD_ShowString(0, 36, (u8 *)"CMD: 0x", RED, BLACK, 12, 0);
        {
            char hex[3];
            hex[0] = "0123456789ABCDEF"[cmdId >> 4];
            hex[1] = "0123456789ABCDEF"[cmdId & 0x0F];
            hex[2] = '\0';
            LCD_ShowString(48, 36, (u8 *)hex, RED, BLACK, 12, 0);
        }
    }

    OLED_Refresh();
}

/* 简化版: 纯文字显示 */
static void OLED_ShowText(const char *line1, const char *line2)
{
    OLED_Clear(0);
    if (line1) LCD_ShowString(0, 8,  (u8 *)line1, RED, BLACK, 16, 0);
    if (line2) LCD_ShowString(0, 32, (u8 *)line2, RED, BLACK, 12, 0);
    OLED_Refresh();
}

/*===========================================================================
 * 3. WT2003H4 MP3 模块 — UART1 (PA8/PA9, 115200)
 *
 *   RX ← PA8 (MCU TX), TX → PA9 (MCU RX, 可选)
 *   协议: 7E + 长度 + 命令 + 参数 + 校验和 + EF
 *=========================================================================*/

/* 发送一帧 */
static void MP3_SendCmd(uint8_t cmd, uint8_t *param, uint8_t paramLen)
{
    uint8_t len = 2 + paramLen;
    uint8_t checksum = len + cmd;
    uint8_t i;
    for (i = 0; i < paramLen; i++) checksum += param[i];

    DL_UART_Main_transmitDataBlocking(UART_1_INST, 0x7E);
    DL_UART_Main_transmitDataBlocking(UART_1_INST, len);
    DL_UART_Main_transmitDataBlocking(UART_1_INST, cmd);
    for (i = 0; i < paramLen; i++)
        DL_UART_Main_transmitDataBlocking(UART_1_INST, param[i]);
    DL_UART_Main_transmitDataBlocking(UART_1_INST, checksum);
    DL_UART_Main_transmitDataBlocking(UART_1_INST, 0xEF);
}

/* 播放第 N 首 (索引) */
#define MP3_Play(n) do { uint8_t _p[]={0x00,(n)}; MP3_SendCmd(0xA0,_p,2); } while(0)

/* 停止 */
#define MP3_Stop()      MP3_SendCmd(0xAB, NULL, 0)

/* 音量 0~31 */
void MP3_SetVolume(uint8_t vol)
{
    if (vol > 31) vol = 31;
    uint8_t ck = 0x04 + 0xAE + vol;
    const uint8_t cmd[] = {0x7E, 0x04, 0xAE, vol, ck, 0xEF};
    MP3_SendCmd(0xAE, &vol, 1);
}

/*===========================================================================
 * 4. 舵机 PWM — 4路独立PWM实例
 *=========================================================================*/
#define SERVO_CH_FL     0
#define SERVO_CH_FR     1
#define SERVO_CH_RL     2
#define SERVO_CH_RR     3

#define SERVO_MIN_CCR    0
#define SERVO_MAX_CCR    2500
#define SERVO_MID_CCR    750

static uint16_t g_servoCCR[4] = {SERVO_MID_CCR, SERVO_MID_CCR,
                                  SERVO_MID_CCR, SERVO_MID_CCR};

/* 单腿方向: 1=反相(1500-x), 0=正常  [FL, FR, RL, RR] */
static const int8_t g_legInv[4] = {0, 1, 0, 1};

static void Servo_WriteCCR(uint8_t ch, uint16_t ccr)
{
    switch (ch) {
    case 0: DL_TimerA_setCaptureCompareValue(PWM_1_INST, ccr, GPIO_PWM_1_C0_IDX); break;
    case 1: DL_TimerA_setCaptureCompareValue(PWM_2_INST, ccr, GPIO_PWM_2_C0_IDX); break;
    case 2: DL_TimerG_setCaptureCompareValue(PWM_3_INST, ccr, GPIO_PWM_3_C0_IDX); break;
    case 3: DL_TimerG_setCaptureCompareValue(PWM_4_INST, ccr, GPIO_PWM_4_C0_IDX); break;
    }
}

void Servo_SetCCR(uint8_t ch, uint16_t ccr)
{
    if (ccr < SERVO_MIN_CCR) ccr = SERVO_MIN_CCR;
    if (ccr > SERVO_MAX_CCR) ccr = SERVO_MAX_CCR;
    g_servoCCR[ch] = ccr;
    Servo_WriteCCR(ch, ccr);
}

void Servo_AllMid(void)
{
    Servo_SetCCR(0, SERVO_MID_CCR); Servo_SetCCR(1, SERVO_MID_CCR);
    Servo_SetCCR(2, SERVO_MID_CCR); Servo_SetCCR(3, SERVO_MID_CCR);
}

void Servo_Init(void)
{
    Servo_AllMid();
    printf("[SERVO] 4ch ready\r\n");
}

/* 双舵机同步移动 (阻塞, 用于左转/右转等一次性动作) */
void Servo_Move2CCR(uint8_t c0, uint16_t t0, uint8_t c1, uint16_t t1,
                    uint8_t stepMs, uint8_t stepCCR)
{
    int32_t p0 = g_servoCCR[c0], p1 = g_servoCCR[c1];
    uint8_t d0 = (p0 == t0), d1 = (p1 == t1);
    if (d0 && d1) return;
    while (!d0 || !d1) {
        if (!d0) { Servo_WriteCCR(c0, (uint16_t)p0);
            if (t0 > p0) { p0 += (1+stepCCR); if (p0 >= t0) {p0=t0; d0=1;} }
            else         { p0 -= (1+stepCCR); if (p0 <= t0) {p0=t0; d0=1;} }
        }
        if (!d1) { Servo_WriteCCR(c1, (uint16_t)p1);
            if (t1 > p1) { p1 += (1+stepCCR); if (p1 >= t1) {p1=t1; d1=1;} }
            else         { p1 -= (1+stepCCR); if (p1 <= t1) {p1=t1; d1=1;} }
        }
        if (!d0 || !d1) delay_ms(stepMs);
    }
    Servo_WriteCCR(c0, t0); Servo_WriteCCR(c1, t1);
    g_servoCCR[c0] = t0; g_servoCCR[c1] = t1;
}

/*===========================================================================
 * 数控振荡器 (NCO) 步态引擎 — 非阻塞, 平滑正弦轨迹
 *
 *  前进: MID → MID-1125 → MID+1125 → MID (750→1875→3000→1875)
 *  后退: MID → MID+1125 → MID-1125 → MID
 *
 *  参数说明:
 *    CYCLE_STEPS    = 周期步数 (越大越慢越平滑)
 *    CYCLE_DELAY_MS = 每步延时 (配合 STEP 控制周期时间)
 *    SMOOTH_STEP    = 每步 CCR 逼近量 (越大响应越快)
 *    LAG_STEPS      = 对侧腿滞后步数 (行走时转弯用)
 *=========================================================================*/
#define CYCLE_STEPS     100
#define CYCLE_DELAY_MS  5
#define SMOOTH_STEP     30
#define LAG_STEPS       (CYCLE_STEPS / 2)
#define PHASE_LEN       (CYCLE_STEPS / 3)

/* 前进: 快前摆10% → 慢后推90% */

static uint16_t Gait_ForwardCCR(uint16_t t)
{
    t %= CYCLE_STEPS;
		
    if(t < 10)
{
    return 1050 - t * 60;
}
else
{
    return 450 + (t - 10) * 6;
}
}
/* 后退: 快后摆10% → 慢前推80% */
static uint16_t Gait_BackwardCCR(uint16_t t)
{
    t %= 100;

    if(t < 10)
    {
        return 450 + t * 60;
    }
    else
    {
        return 1050 - (t - 10) * 6;
    }
}

typedef enum {
    GAIT_IDLE = 0,
    GAIT_FORWARD,
    GAIT_BACKWARD,
    GAIT_TURN_LEFT,
    GAIT_TURN_RIGHT,
    GAIT_DANCE,
} GaitState;

/*
 * 非阻塞步态更新 — 每次主循环调用一次
 *   计算目标 CCR → 平滑逼近 → 写入 PWM
 */
void Servo_GaitUpdate(GaitState gs, uint16_t step)
{
    uint16_t tL, tR, target[4];

    switch (gs) {
    case GAIT_FORWARD:
        tL = step;
        tR = (step + CYCLE_STEPS - LAG_STEPS) % CYCLE_STEPS;
        { uint16_t v0 = Gait_ForwardCCR(tL); target[0]=g_legInv[0]?(1500-v0):v0; }
        { uint16_t v1 = Gait_ForwardCCR(tR); target[1]=g_legInv[1]?(1500-v1):v1; }
        { uint16_t v2 = Gait_ForwardCCR(tR); target[2]=g_legInv[2]?(1500-v2):v2; }
        { uint16_t v3 = Gait_ForwardCCR(tL); target[3]=g_legInv[3]?(1500-v3):v3; }
        break;

    case GAIT_BACKWARD:
        tL = step;
        tR = (step + CYCLE_STEPS - LAG_STEPS) % CYCLE_STEPS;
        { uint16_t v0 = Gait_BackwardCCR(tL); target[0]=g_legInv[0]?(1500-v0):v0; }
        { uint16_t v1 = Gait_BackwardCCR(tR); target[1]=g_legInv[1]?(1500-v1):v1; }
        { uint16_t v2 = Gait_BackwardCCR(tR); target[2]=g_legInv[2]?(1500-v2):v2; }
        { uint16_t v3 = Gait_BackwardCCR(tL); target[3]=g_legInv[3]?(1500-v3):v3; }
        break;

    case GAIT_TURN_LEFT:
        /* 左转: 右半身前推+左半身后推 → 原地左旋 */
        /* 配对: FL+RR(左) vs FR+RL(右), 对角交替 */
        tL = step;
        tR = (step + CYCLE_STEPS - LAG_STEPS) % CYCLE_STEPS;
        { uint16_t v0 = Gait_BackwardCCR(tL); target[0]=g_legInv[0]?(1500-v0):v0; }
        { uint16_t v1 = Gait_ForwardCCR(tR);  target[1]=g_legInv[1]?(1500-v1):v1; }
        { uint16_t v2 = Gait_BackwardCCR(tR); target[2]=g_legInv[2]?(1500-v2):v2; }
        { uint16_t v3 = Gait_ForwardCCR(tL);  target[3]=g_legInv[3]?(1500-v3):v3; }
        break;

    case GAIT_TURN_RIGHT:
        /* 右转: 左半身前推+右半身后推 → 原地右旋 */
        tL = step;
        tR = (step + CYCLE_STEPS - LAG_STEPS) % CYCLE_STEPS;
        { uint16_t v0 = Gait_ForwardCCR(tL);  target[0]=g_legInv[0]?(1500-v0):v0; }
        { uint16_t v1 = Gait_BackwardCCR(tR); target[1]=g_legInv[1]?(1500-v1):v1; }
        { uint16_t v2 = Gait_ForwardCCR(tR);  target[2]=g_legInv[2]?(1500-v2):v2; }
        { uint16_t v3 = Gait_BackwardCCR(tL); target[3]=g_legInv[3]?(1500-v3):v3; }
        break;

    case GAIT_DANCE:
        /* 4相波浪: FL→FR→RL→RR 依次起伏, 每腿差1/4周期 */
        /* 前腿+后腿反相: 形成身体前后摇摆的跳舞效果 */
        { uint16_t v0 = Gait_ForwardCCR(step);                        target[0]=g_legInv[0]?(1500-v0):v0; }
        { uint16_t v1 = Gait_ForwardCCR(step + CYCLE_STEPS/4);        target[1]=g_legInv[1]?(1500-v1):v1; }
        { uint16_t v2 = Gait_BackwardCCR(step + CYCLE_STEPS*2/4);     target[2]=g_legInv[2]?(1500-v2):v2; }
        { uint16_t v3 = Gait_BackwardCCR(step + CYCLE_STEPS*3/4);     target[3]=g_legInv[3]?(1500-v3):v3; }
        break;

    default:
        return;
    }

    /* 平滑逼近 */
    for (int ch = 0; ch < 4; ch++) {
        uint16_t cur = g_servoCCR[ch];
        if (cur < target[ch])      { cur += SMOOTH_STEP; if (cur > target[ch]) cur = target[ch]; }
        else if (cur > target[ch]) { cur -= SMOOTH_STEP; if (cur < target[ch]) cur = target[ch]; }
        g_servoCCR[ch] = cur;
        Servo_WriteCCR(ch, cur);
    }
}

/* 坐下 (阻塞, 一次性动作)
 *   前腿 → 中立位 750
 *   后腿 → 相对中立位前弯 90° (RL=1250, RR=250, 因后装反相) */
void Servo_Sit(void)
{
    /* 目标: [FL, FR, RL, RR] */
    uint16_t target[4] = {750, 750, 1250, 250};
    int i, done;
    uint8_t step = 10;  /* 每步CCR增量 */

    for (i = 0; i < 80; i++) {
        done = 1;
        for (int ch = 0; ch < 4; ch++) {
            uint16_t cur = g_servoCCR[ch];
            uint16_t tgt = target[ch];
            if (cur < tgt) {
                cur += step; if (cur > tgt) cur = tgt;
                g_servoCCR[ch] = cur; Servo_WriteCCR(ch, cur);
                done = 0;
            } else if (cur > tgt) {
                if (cur < step) cur = 0;
                else cur -= step;
                if (cur < tgt) cur = tgt;
                g_servoCCR[ch] = cur; Servo_WriteCCR(ch, cur);
                done = 0;
            }
        }
        if (done) break;
        delay_ms(12);
    }
}

/* 卧倒/休眠 (阻塞, 一次性动作)
 *   前腿 → 中立位+90° 后弯 (FL=250, FR=1250, FR因前装反相)
 *   后腿 → 中立位+90° 前弯 (RL=1250, RR=250, RR因后装反相) */
void Servo_Sleep(void)
{
    /* 目标: [FL, FR, RL, RR] — 四条腿全部偏离中立90° */
    uint16_t target[4] = {250, 1250, 1250, 250};
    int i, done;
    uint8_t step = 10;  /* 每步CCR增量 */

    for (i = 0; i < 80; i++) {
        done = 1;
        for (int ch = 0; ch < 4; ch++) {
            uint16_t cur = g_servoCCR[ch];
            uint16_t tgt = target[ch];
            if (cur < tgt) {
                cur += step; if (cur > tgt) cur = tgt;
                g_servoCCR[ch] = cur; Servo_WriteCCR(ch, cur);
                done = 0;
            } else if (cur > tgt) {
                if (cur < step) cur = 0;
                else cur -= step;
                if (cur < tgt) cur = tgt;
                g_servoCCR[ch] = cur; Servo_WriteCCR(ch, cur);
                done = 0;
            }
        }
        if (done) break;
        delay_ms(12);
    }
}

/* 停止 (归中) */
void Servo_Stop(void)
{
    Servo_AllMid();
}

/* 跳舞: 四腿依次抬起 */
void Servo_Dance(void)
{
    Servo_SetCCR(0, 550); delay_ms(200);
    Servo_SetCCR(0, SERVO_MID_CCR); delay_ms(100);
    Servo_SetCCR(1, 950); delay_ms(200);
    Servo_SetCCR(1, SERVO_MID_CCR); delay_ms(100);
    Servo_SetCCR(2, 550); delay_ms(200);
    Servo_SetCCR(2, SERVO_MID_CCR); delay_ms(100);
    Servo_SetCCR(3, 950); delay_ms(200);
    Servo_SetCCR(3, SERVO_MID_CCR); delay_ms(100);
}


/*===========================================================================
 * 4. main — 主程序
 *=========================================================================*/
#define LED_ON()   DL_GPIO_clearPins(GPIOA, DL_GPIO_PIN_14)
#define LED_OFF()  DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_14)

typedef enum {
    STATE_IDLE = 0,
    STATE_WALK_FORWARD,
    STATE_WALK_BACKWARD,
    STATE_TURN_LEFT,
    STATE_TURN_RIGHT,
    STATE_SIT,
    STATE_DANCE,
} RobotState;

static RobotState g_state = STATE_IDLE;

/* 防抖: 缓存上次命令ID */
static uint8_t g_lastDisplayedCmd = 0xFF;

static void HandleCommand(uint8_t id)
{
    /* 相同命令不重复刷屏 */
    if (id == g_lastDisplayedCmd) return;
    g_lastDisplayedCmd = id;

    printf("[CMD] 0x%02X %s\r\n", id, GetCmdName(id));

    /* ---- OLED 显示 ---- */
    OLED_DisplayCommand(id);

    /* ---- 语音 + 舵机 ---- */
    switch (id) {
    case 0x01: // 前进
        LED_ON();
        CI1302_Speak(CI1302_CMD, 0x81);
        g_state = STATE_WALK_FORWARD;
        break;
    case 0x02: //后退
        LED_ON();
        CI1302_Speak(CI1302_CMD, 0x82);
        g_state = STATE_WALK_BACKWARD;
        break;
    case 0x03: // 左转 (原地左旋)
        LED_ON();
        CI1302_Speak(CI1302_CMD, 0x83);
        g_state = STATE_TURN_LEFT;
        break;
    case 0x04: // 右转 (原地右旋)
        LED_ON();

        CI1302_Speak(CI1302_CMD, 0x84);
        g_state = STATE_TURN_RIGHT;
        break;
    case 0x05: // 坐下
        
        CI1302_Speak(CI1302_CMD, 0x85);
        g_state = STATE_SIT;
        Servo_Sit();
        break;
    case 0x06: // 站立
        LED_ON();

        CI1302_Speak(CI1302_CMD, 0x86);
        Servo_Stop();
        break;
    case 0x07: // 停下 → 卧倒休眠
        LED_OFF();
        MP3_Play(2);
        CI1302_Speak(CI1302_CMD, 0x87);
        g_state = STATE_IDLE;
				Servo_Stop();
				delay_ms(200000);
        Servo_Sleep();
        break;
    case 0x08: // 跳舞
        MP3_Play(3);
				g_state = STATE_DANCE;
        CI1302_Speak(CI1302_CMD, 0x88);
        
        break;
    case 0x09: // 唱歌
        MP3_Play(1);
        CI1302_Speak(CI1302_CMD, 0x89);
        break;
    default:
        break;
    }
}

static uint16_t g_gaitStep = 0;

int main(void)
{
    uint8_t lastResult = 0;

    board_init();

    /* UART1 波特率改为 115200 (syscfg默认9600, MP3模块需要115200) */
    DL_UART_Main_setBaudRateDivisor(UART_1_INST, 2, 11);  // 4MHz→115200

    /* MP3 开机初始化 */
    MP3_SetVolume(10);   // 音量 25/31
    MP3_Stop();       // 确保停止状态

    /* OLED 初始化 (先于舵机, 避免GPIO操作干扰PWM) */
    OLED_Init();
    OLED_Clear(0);
    OLED_Refresh();

    Servo_Init();

    /* 开机自检: 舵机微动一下确认PWM正常 */
    Servo_SetCCR(0, 500);
    Servo_SetCCR(1, 1000);
    delay_ms(300);
    Servo_AllMid();
                //g_state = STATE_WALK_FORWARD;

    printf("\r\n================================\r\n");
    printf(" DMX + CI1302 + OLED + Servo\r\n");
    printf("================================\r\n\r\n");

    /* 开机显示 */
    OLED_DisplayCommand(0x00);   // 显示待机图 (gImage_idle)
    CI1302_Speak(CI1302_ANNOUNCE, 0x01);
    printf("[SYS] Ready.\r\n\r\n");
        //g_state = STATE_WALK_FORWARD;
    while (1) {
        uint8_t result = CI1302_Recognition();
        
        if (result != 0x00 && result != 0x80 && result != lastResult) {
            HandleCommand(result);
            lastResult = result;
        }

        /* 非阻塞步态更新 */
        printf("[DBG] g_state=%d\r\n", g_state);
        if (g_state == STATE_WALK_FORWARD) {
            Servo_GaitUpdate(GAIT_FORWARD, g_gaitStep);
            g_gaitStep = (g_gaitStep + 1) % CYCLE_STEPS;
        } else if (g_state == STATE_WALK_BACKWARD) {
            Servo_GaitUpdate(GAIT_BACKWARD, g_gaitStep);
            g_gaitStep = (g_gaitStep + 1) % CYCLE_STEPS;
        } else if (g_state == STATE_TURN_LEFT) {
            Servo_GaitUpdate(GAIT_TURN_LEFT, g_gaitStep);
            g_gaitStep = (g_gaitStep + 1) % CYCLE_STEPS;
        } else if (g_state == STATE_TURN_RIGHT) {
            Servo_GaitUpdate(GAIT_TURN_RIGHT, g_gaitStep);
            g_gaitStep = (g_gaitStep + 1) % CYCLE_STEPS;
        } else if (g_state == STATE_DANCE) {
            Servo_GaitUpdate(GAIT_DANCE, g_gaitStep);
            g_gaitStep = (g_gaitStep + 1) % CYCLE_STEPS;
        }
        delay_ms(CYCLE_DELAY_MS);
    }
}