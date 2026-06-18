# 地猛星 (Di Meng Xing) — MSPM0G3507 四足语音机器人

> 哈尔滨工业大学（深圳）电子工艺实习项目  |  2026年4月–6月

基于 TI MSPM0G3507 的四足机器人，集成离线语音识别、OLED 状态显示、MP3 音频
反馈和 4 路舵机步态控制。支持 9 条语音口令非接触控制，5 种步态模式（前进、后退、
左转、右转、跳舞）。

---

## 硬件架构

| 模块 | 型号 | 接口 | 说明 |
|------|------|------|------|
| MCU | MSPM0G3507 (Cortex-M0+, 32MHz) | — | TI LaunchPad 开发板 |
| 语音识别 | CI1302 | 软件 I2C (PB2/PB3) | 离线非特定人，9条口令 |
| 显示屏 | SH1106 OLED 1.3" 128×64 | 软件 SPI (PA12/13/16/17/22) | 单色，页寻址 |
| MP3 播放 | WT2003H4 M09 | UART1 (PA8, 115200) | TF卡，3W功放 |
| 舵机 ×4 | SG90 | 独立 PWM 50Hz | 外部 5V/3A 供电 |
| 3D 外壳 | PLA 打印 | — | 立式屏幕支架+紧凑布局 |

## 引脚映射

| 引脚 | 功能 | 引脚 | 功能 |
|------|------|------|------|
| PA0 | 舵机 FR (前右) | PA15 | 舵机 FL (前左) |
| PA8 | MP3 UART TX | PA16 | OLED DC |
| PA9 | MP3 UART RX | PA17 | OLED RES |
| PA10 | 调试 UART TX | PA21 | 舵机 RL (后左) |
| PA11 | 调试 UART RX | PA22 | OLED CS |
| PA12 | OLED SCL | PA23 | 舵机 RR (后右) |
| PA13 | OLED SDA | PB2 | CI1302 SCL |
| PA14 | 板载 LED | PB3 | CI1302 SDA |

## 语音命令

| 口令 | 命令ID | 步态 | 口令 | 命令ID | 步态 |
|------|--------|------|------|--------|------|
| 前进 | 0x01 | 对角小跑 | 行走 | 0x06 | 归中站立 |
| 后退 | 0x02 | 后退小跑 | 停止 | 0x07 | 卧倒休眠 |
| 左转 | 0x03 | 原地左旋 | 跳舞 | 0x08 | 四相波浪 |
| 右转 | 0x04 | 原地右旋 | 唱歌 | 0x09 | 仅MP3 |
| 坐下 | 0x05 | 坐下姿态 | | | |

## 目录结构

```
├── README.md                  # 本文件
├── docs/                      # 文档
│   ├── 地猛星机器人发布说明书.md / .docx
│   ├── 硬件安装说明书.md
│   └── 屏幕硬件图.pdf
├── firmware/                  # 源代码
│   ├── empty.c                # 主程序 (~740行)
│   ├── board.c / board.h      # 板级初始化+延时
│   ├── ti_msp_dl_config.c/.h  # SysConfig 生成
│   ├── empty.syscfg           # SysConfig 工程
│   ├── LCD/                   # OLED 驱动
│   │   ├── lcd_init.c/.h      # SH1106 底层
│   │   ├── lcd.c/.h           # 上层绘图
│   │   ├── lcdfont.h          # ASCII 字库
│   │   └── img_*.h (×11)      # Image2Lcd 图片
│   └── keil/                  # Keil 工程
├── hardware/                  # 硬件设计
│   ├── pcb文件.epro           # 立创EDA PCB
│   └── 屏幕尺寸.png
├── model/                     # 3D 打印模型 (SolidWorks)
├── voice-module/              # CI1302 语音模块
│   ├── 固件/
│   ├── 烧录工具/
│   └── 文档
├── mp3-module/                # WT2003H4 MP3 模块
├── images/                    # OLED 图片资源
│   ├── 原图/                  # BMP/PNG 原图
│   └── 取模数组/              # Image2Lcd 生成的 .c
└── tools/                     # 工具
    └── Image2Lcd/             # 图片取模工具
```

## 编译与烧录

- **编译器**: ARMCLANG V6.21 (Keil MDK)
- **工程文件**: `firmware/keil/empty_LP_MSPM0G3507_nortos_keil.uvprojx`
- **Include 路径**: `..` (firmware), `../../..` (SDK根), `../LCD`
- **预编译**: 需禁用 Keil 预编译步骤（SysConfig 权限问题）
- **烧录**: Keil F8 或使用 TI Uniflash

## 注意事项

1. **舵机必须独立供电**（外部 5V/3A），绝不能接板卡 5V 排针
2. SH1106 与 SSD1306 驱动不兼容——列偏移 +2、页寻址、电荷泵 0xAD
3. CI1302 识别结果 ID 为 0x01-0x09（播报编号为 0x81-0x89，两者不同）
4. OLED_Init() 必须在 Servo_Init() 之前（避免 GPIO 干扰 PWM）
5. MP3 模块 UART 需手动设 115200（SysConfig 默认 9600）

## 已知问题

- [ ] 前进/后退步态需上机调参以获得地面推力
- [ ] 跳舞/左转/右转步态未经实物验证
- [ ] img_sleep/dance/sing.h 为全黑占位图，待替换
- [ ] SMOOTH_STEP=30 偏大，建议降至 3~10

## 许可

MIT License
