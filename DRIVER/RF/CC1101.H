#ifndef _CC1101_H_
#define _CC1101_H_

#include "STM8l10x_conf.h"
#include "mytypedef.h"
#include "CC1101_REG.h"

// 常量定义
#define TX              1       // 发送模式
#define RX              0       // 接收模式

#define RX_Address 0xff         // 接收端   设备地址

#define TX_Address 0x01         // 发送端   从机设备地址  
//#define TX_Address 0x02         
//#define TX_Address 0x03        
//#define TX_Address 0x04         
//#define TX_Address 0x05         
//#define TX_Address 0x06        
//#define TX_Address 0x07         

#define SEND_GAP         500    // 每间隔1s发送一次数据
#define RECV_TIMEOUT    1500    // 接收超时

#define ACK_LENGTH      5       // 应答信号长度        
#define SEND_LENGTH     6       // 发送数据每包的长度

/*===========================================================================
------------------------------Internal IMPORT functions----------------------
you must offer the following functions for this module
1. INT8U SPI_ExchangeByte(INT8U input); // SPI Send and Receive function
2. CC_CSN_LOW();                        // Pull down the CSN line
3. CC_CSN_HIGH();                       // Pull up the CSN Line
===========================================================================*/
// CC1101相关控制引脚定义， CSN(PB4), IRQ(PA2), GDO2(PA3) 
#define PORT_CC_CSN     GPIOB
#define PIN_CC_CSN      GPIO_Pin_4

#define PORT_CC_IRQ     GPIOB
#define PIN_CC_IRQ      GPIO_Pin_3
//#define PORT_CC_IRQ     GPIOA
//#define PIN_CC_IRQ      GPIO_Pin_2

#define PORT_CC_GDO2    GPIOA
#define PIN_CC_GDO2     GPIO_Pin_3

#define CC_CSN_LOW()    GPIO_ResetBits(PORT_CC_CSN, PIN_CC_CSN);\
                        while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)!=0);
#define CC_CSN_HIGH()   GPIO_SetBits(PORT_CC_CSN, PIN_CC_CSN)

#define CC_IRQ_READ()   GPIO_ReadInputDataBit(PORT_CC_IRQ, PIN_CC_IRQ)

/*===========================================================================
----------------------------------macro definitions--------------------------
============================================================================*/
typedef enum { TX_MODE, RX_MODE } TRMODE;
typedef enum { BROAD_ALL, BROAD_NO, BROAD_0, BROAD_0AND255 } ADDR_MODE;
typedef enum { BROADCAST, ADDRESS_CHECK} TX_DATA_MODE;

/*===========================================================================
-------------------------------------exported APIs---------------------------
============================================================================*/

/*read a byte from the specified register*/
INT8U CC1101ReadReg(INT8U addr);

/*Read a status register*/
INT8U CC1101ReadStatus(INT8U addr);

/*Set the device as TX mode or RX mode*/
void CC1101SetTRMode(TRMODE mode);

/*Write a command byte to the device*/
void CC1101WriteCmd(INT8U command);

/*Set the CC1101 into IDLE mode*/
void CC1101SetIdle(void);

/*Send a packet*/
void CC1101SendPacket(INT8U *txbuffer, INT8U size, TX_DATA_MODE mode);

/*Set the address and address mode of the CC1101*/
void CC1101SetAddress(INT8U address, ADDR_MODE AddressMode);

/*Set the SYNC bytes of the CC1101*/
void CC1101SetSYNC(INT16U sync);

/*Receive a packet*/
INT8U CC1101RecPacket(INT8U *rxBuffer);

/*Initialize the WOR function of CC1101*/
void  CC1101WORInit(void);

/*Initialize the CC1101, User can modify it*/
void CC1101Init(void);

#endif // _CC1101_H_

/*===========================================================================
-----------------------------------文件结束----------------------------------
===========================================================================*/
