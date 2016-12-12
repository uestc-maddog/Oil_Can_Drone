/*===========================================================================
* 网址 ：http://www.cdebyte.com/   http://yhmcu.taobao.com/                 *
* 作者 ：李勇  原 亿和电子工作室  现 亿佰特电子科技有限公司                 * 
* 邮件 ：yihe_liyong@126.com                                                *
* 电话 ：18615799380                                                        *
============================================================================*/

#include "bsp.h"

/*===========================================================================
* 函数 ：SClK_Initial() => 初始化系统时钟，系统时钟 = 4MHZ                  *
============================================================================*/
void SClK_Initial(void)
{
    CLK_MasterPrescalerConfig(CLK_MasterPrescaler_HSIDiv4); // 4M
}

/*===========================================================================
* 函数 ：GPIO_Initial() => 初始化通用IO端口                                 *
============================================================================*/
void GPIO_Initial(void)
{
    // 配置LED引脚 LED(PA6)  
    GPIO_Init(PORT_LED, PIN_LED, GPIO_Mode_Out_PP_High_Slow);
    GPIO_ResetBits(PORT_LED, PIN_LED);    // 点亮LED
     
    // 配置CC1101相关控制引脚 CSN(PB4), IRQ(PA2), GDO2(PA3)
    GPIO_Init(PORT_CC_IRQ, PIN_CC_IRQ, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(PORT_CC_GDO2, PIN_CC_GDO2, GPIO_Mode_In_PU_No_IT);
    
    GPIO_Init(PORT_CC_CSN, PIN_CC_CSN, GPIO_Mode_Out_PP_High_Fast);
    GPIO_SetBits(PORT_CC_CSN, PIN_CC_CSN);
}

/*===========================================================================
* 函数 USART1_Initial() => 初始化串口                                 *
============================================================================*/
void USART1_Initial(void)
{
    // 串口初始化
    CLK_PeripheralClockConfig (CLK_Peripheral_USART,ENABLE); //使能外设时钟，STM8L外设时钟默认关闭
    USART_Init(115200,USART_WordLength_8D,USART_StopBits_1,USART_Parity_No,USART_Mode_Tx|USART_Mode_Rx);//USART初始化，9600，8N1
    
    // USART_ITConfig (USART_IT_RXNE,ENABLE);//使能接收中断
    USART_Cmd (ENABLE);//使能USART 
}

/*===========================================================================
* 函数 ：SPI_Initial() => 初始化SPI                                         *
============================================================================*/
void SPI_Initial(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_SPI, ENABLE);
    
    SPI_DeInit();
    
    // 配置SPI相关参数,2分频（8MHZ）
    SPI_Init(SPI_FirstBit_MSB, SPI_BaudRatePrescaler_2,
         SPI_Mode_Master, SPI_CPOL_Low, SPI_CPHA_1Edge,
         SPI_Direction_2Lines_FullDuplex, SPI_NSS_Soft);

    SPI_Cmd(ENABLE);
    
    // SPI相关IO口配置
    GPIO_Init(PORT_SPI, PIN_MISO, GPIO_Mode_In_PU_No_IT);       // MISO (PB7)
    GPIO_Init(PORT_SPI, PIN_SCLK, GPIO_Mode_Out_PP_High_Fast);  // SCLK (PB5)
    GPIO_Init(PORT_SPI, PIN_MOSI, GPIO_Mode_Out_PP_High_Fast);  // MOSI (PB6)
}

/*===========================================================================
* 函数 ：TIM3_Initial() => 初始化定时器3，定时时间为1ms                     *
============================================================================*/
void TIM3_Initial(void)
{
    TIM3_DeInit();

    CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);

    // 配置Timer3相关参数，时钟为16/16 = 1MHZ，定时时间 = 1000/1000000 = 1ms
    TIM3_TimeBaseInit(TIM3_Prescaler_4, TIM3_CounterMode_Up, 1000);
    TIM3_Set(0);                      // 关闭TIM3
}

/*===========================================================================
* 函数 ：SPI_ExchangeByte() => 通过SPI进行数据交换                          * 
* 输入 ：需要写入SPI的值                                                    * 
* 输出 ：通过SPI读出的值                                                    * 
============================================================================*/
INT8U SPI_ExchangeByte(INT8U input)
{
    SPI_SendData(input);
    while (RESET == SPI_GetFlagStatus(SPI_FLAG_TXE));   // 等待数据传输完成	
    while (RESET == SPI_GetFlagStatus(SPI_FLAG_RXNE));  // 等待数据接收完成
    return (SPI_ReceiveData());
}

/*===========================================================================
-----------------------------------文件结束----------------------------------
===========================================================================*/
