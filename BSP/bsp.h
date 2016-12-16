/*===========================================================================
网址 ：http://yhmcu.taobao.com/
作者 ：李勇  原 亿和电子工作室  现 亿佰特电子科技有限公司 
邮件 ：yihe_liyong@126.com
电话 ：18615799380
===========================================================================*/

#ifndef _BSP_H_
#define _BSP_H_

#include "STM8l10x_conf.h"
#include "CC1101.h"
#include "mytypedef.h"
#include "OLED.h"

// SPI引脚定义 SCLK(PB5), MOSI(PB6), MISO(PB7)
#define PORT_SPI        GPIOB
#define PIN_SCLK        GPIO_Pin_5        
#define PIN_MOSI        GPIO_Pin_6
#define PIN_MISO        GPIO_Pin_7

// LED 和 SWITCH引脚定义，LED(PC6), SWITCH(PD1), SMG_EN(PD2)
#define PORT_LED        GPIOA
#define PIN_LED         GPIO_Pin_6

#define PORT_SWITCH     GPIOD
#define PIN_SWITCH      GPIO_Pin_1

#define PORT_SMGEN      GPIOD
#define PIN_SMGEN       GPIO_Pin_2    // SMG_EN

// LED SWITCH操作函数，(ON)打开, (OFF)关闭，(TOG)翻转
#define LED_ON()        GPIO_ResetBits(PORT_LED, PIN_LED)        
#define LED_OFF()       GPIO_SetBits(PORT_LED, PIN_LED)
#define LED_TOG()       GPIO_ToggleBits(PORT_LED, PIN_LED)

#define SWITCH_ON()     GPIO_ResetBits(PORT_SMGEN, PIN_SMGEN);GPIO_ResetBits(PORT_SWITCH, PIN_SWITCH)        
#define SWITCH_OFF()    GPIO_SetBits(PORT_SWITCH, PIN_SWITCH);GPIO_SetBits(PORT_SMGEN, PIN_SMGEN)

void SClK_Initial(void);                // 初始化系统时钟，系统时钟 = 16MHZ
void GPIO_Initial(void);                // 初始化通用IO端口 LED KEY
void USART1_Initial(void);              // 初始化USART1
void SPI_Initial(void);                 // 初始化SPI
void TIM3_Initial(void);                // 初始化定时器3，定时时间为1ms
INT8U SPI_ExchangeByte(INT8U input);     // 通过SPI进行数据交换 

#endif //_BSP_H_

/*===========================================================================
-----------------------------------文件结束----------------------------------
===========================================================================*/
