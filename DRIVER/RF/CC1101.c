/*
================================================================================
Copyright   : Ebyte electronic co.,LTD
Website     : http://yh-ebyte.taobao.com
              http://yiheliyong.cn.alibaba.com
Description : This module contains the low level operations for CC1101
================================================================================
*/
#include "CC1101.H"
#include "STM8l10x_conf.h"
#include "stdio.h"

////10, 7, 5, 0, -5, -10, -15, -20, dbm output power, 0x12 == -30dbm
//INT8U PaTabel[] = { 0xc0, 0xC8, 0x84, 0x60, 0x68, 0x34, 0x1D, 0x0E};

INT8U PaTabel[] = {0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};   // 915MHz   10dBm

// Sync word qualifier mode = 30/32 sync word bits detected 
// CRC autoflush = false 
// Channel spacing = 199.951172 
// Data format = Normal mode 
// Data rate = 2.00224 
// RX filter BW = 58.035714 
// PA ramping = false 
// Preamble count = 4 
// Whitening = false 
// Address config = No address check 
// Carrier frequency = 400.199890 
// Device address = 0 
// TX power = 10 
// Manchester enable = false 
// CRC enable = true 
// Deviation = 5.157471 
// Packet length mode = Variable packet length mode. Packet length configured by the first byte after sync word 
// Packet length = 255 
// Modulation format = GFSK 
// Base frequency = 399.999939 
// Modulated = true 
// Channel number = 1 
// PA table 
#define PA_TABLE {0xc2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,}

// RF = 915MHz
static const uint8_t CC1101InitData[23][2]= 
{
  {CC1101_IOCFG0,   0x06},
  {CC1101_FIFOTHR,  0x47},
  {CC1101_PKTCTRL0, 0x05},
  {CC1101_CHANNR,   0x01},   // í¨μà1  Channel number = 1
  {CC1101_FSCTRL1,  0x06},
  {CC1101_FREQ2,    0x23},   // ?ù?μ  915.000000       ??2¨?μ?ê=?ù?μ+2???￡¨0.2MHz￡?* í¨μào?  915.199951MHz
  {CC1101_FREQ1,    0x31},
  {CC1101_FREQ0,    0x3B},
  {CC1101_MDMCFG4,  0xFA},   // êy?Y?ù?ê￡o49.9878kBaud
  {CC1101_MDMCFG3,  0xF8},
  {CC1101_MDMCFG2,  0x13},
  {CC1101_DEVIATN,  0x15},
  {CC1101_MCSM2,    0x07},   // none
  {CC1101_MCSM1,    0x30},   // 0x3B
  {CC1101_MCSM0,    0x18},
  {CC1101_FOCCFG,   0x16},
  {CC1101_WORCTRL,  0xFB},
  {CC1101_FSCAL3,   0xE9},
  {CC1101_FSCAL2,   0x2A},
  {CC1101_FSCAL1,   0x00},
  {CC1101_FSCAL0,   0x1F},
  {CC1101_TEST2,    0x81},
  {CC1101_TEST1,    0x35},
};

//// RF = 400MHz
//static const uint8_t CC1101InitData[22][2]= 
//{
//  {CC1101_IOCFG0,   0x06},
//  {CC1101_FIFOTHR,  0x47},
//  {CC1101_PKTCTRL0, 0x05},
//  {CC1101_CHANNR,   0x01},   // í¨μà1  Channel number = 1
//  {CC1101_FSCTRL1,  0x06},
//  {CC1101_FREQ2,    0x0F}, // ?ù?μ  399.999939MHz    ??2¨?μ?ê=?ù?μ+2???￡¨0.2MHz￡?* í¨μào?  400.199890MHz
//  {CC1101_FREQ1,    0x62},
//  {CC1101_FREQ0,    0x76},
//  {CC1101_MDMCFG4,  0xF6},
//  {CC1101_MDMCFG3,  0x43},
//  {CC1101_MDMCFG2,  0x13},
//  {CC1101_DEVIATN,  0x15},
//  {CC1101_MCSM0,    0x18},
//  {CC1101_FOCCFG,   0x16},
//  {CC1101_WORCTRL,  0xFB},
//  {CC1101_FSCAL3,   0xE9},
//  {CC1101_FSCAL2,   0x2A},
//  {CC1101_FSCAL1,   0x00},
//  {CC1101_FSCAL0,   0x1F},
//  {CC1101_TEST2,    0x81},
//  {CC1101_TEST1,    0x35},
//  {CC1101_MCSM1,    0x3B},
//};

/*read a byte from the specified register*/
INT8U CC1101ReadReg( INT8U addr );

/*Read some bytes from the rigisters continously*/
void CC1101ReadMultiReg( INT8U addr, INT8U *buff, INT8U size );

/*Write a byte to the specified register*/
void CC1101WriteReg( INT8U addr, INT8U value );

/*Flush the TX buffer of CC1101*/
void CC1101ClrTXBuff( void );

/*Flush the RX buffer of CC1101*/
void CC1101ClrRXBuff( void );

/*Get received count of CC1101*/
INT8U CC1101GetRXCnt( void );

/*Reset the CC1101 device*/
void CC1101Reset( void );

/*Write some bytes to the specified register*/
void CC1101WriteMultiReg( INT8U addr, INT8U *buff, INT8U size );

extern INT8U SPI_ExchangeByte(INT8U input); // 通过SPI进行数据交换,见bsp.c
extern void DelayMs(u16 x); 

/*
================================================================================
Function : CC1101WORInit( )
    Initialize the WOR function of CC1101
INPUT    : None
OUTPUT   : None
================================================================================
*/
void  CC1101WORInit( void )
{

    CC1101WriteReg(CC1101_MCSM0,0x18);
    CC1101WriteReg(CC1101_WORCTRL,0x78); //Wake On Radio Control
    CC1101WriteReg(CC1101_MCSM2,0x00);
    CC1101WriteReg(CC1101_WOREVT1,0x8C);
    CC1101WriteReg(CC1101_WOREVT0,0xA0);
	
    CC1101WriteCmd( CC1101_SWORRST );
}
/*
================================================================================
Function : CC1101ReadReg( )
    read a byte from the specified register
INPUT    : addr, The address of the register
OUTPUT   : the byte read from the rigister
================================================================================
*/
INT8U CC1101ReadReg( INT8U addr )
{
    INT8U i;
    CC_CSN_LOW( );
    SPI_ExchangeByte( addr | READ_SINGLE);
    i = SPI_ExchangeByte( 0xFF );
    CC_CSN_HIGH( );
    return i;
}
/*
================================================================================
Function : CC1101ReadMultiReg( )
    Read some bytes from the rigisters continously
INPUT    : addr, The address of the register
           buff, The buffer stores the data
           size, How many bytes should be read
OUTPUT   : None
================================================================================
*/
void CC1101ReadMultiReg( INT8U addr, INT8U *buff, INT8U size )
{
    INT8U i, j;
    CC_CSN_LOW( );
    SPI_ExchangeByte( addr | READ_BURST);
    for( i = 0; i < size; i ++ )
    {
        for( j = 0; j < 20; j ++ );
        *( buff + i ) = SPI_ExchangeByte( 0xFF );
    }
    CC_CSN_HIGH( );
}
/*
================================================================================
Function : CC1101ReadStatus( )
    Read a status register
INPUT    : addr, The address of the register
OUTPUT   : the value read from the status register
================================================================================
*/
INT8U CC1101ReadStatus( INT8U addr )
{
    INT8U i;
    CC_CSN_LOW( );
    SPI_ExchangeByte( addr | READ_BURST);
    i = SPI_ExchangeByte( 0xFF );
    CC_CSN_HIGH( );
    return i;
}
/*
================================================================================
Function : CC1101SetTRMode( )
    Set the device as TX mode or RX mode
INPUT    : mode selection
OUTPUT   : None
================================================================================
*/
void CC1101SetTRMode( TRMODE mode )
{
    if( mode == TX_MODE )
    {
        CC1101WriteReg(CC1101_IOCFG0,0x46);
        CC1101WriteCmd( CC1101_STX );
    }
    else if( mode == RX_MODE )
    {
        CC1101WriteReg(CC1101_IOCFG0,0x46);
        CC1101WriteCmd( CC1101_SRX );
    }
}
/*
================================================================================
Function : CC1101WriteReg( )
    Write a byte to the specified register
INPUT    : addr, The address of the register
           value, the byte you want to write
OUTPUT   : None
================================================================================
*/
void CC1101WriteReg( INT8U addr, INT8U value )
{
    CC_CSN_LOW( );
    SPI_ExchangeByte( addr );
    SPI_ExchangeByte( value );
    CC_CSN_HIGH( );
}
/*
================================================================================
Function : CC1101WriteMultiReg( )
    Write some bytes to the specified register
INPUT    : addr, The address of the register
           buff, a buffer stores the values
           size, How many byte should be written
OUTPUT   : None
================================================================================
*/
void CC1101WriteMultiReg( INT8U addr, INT8U *buff, INT8U size )
{
    INT8U i;
    CC_CSN_LOW( );
    SPI_ExchangeByte( addr | WRITE_BURST );
    for( i = 0; i < size; i ++ )
    {
        SPI_ExchangeByte( *( buff + i ) );
    }
    CC_CSN_HIGH( );
}
/*
================================================================================
Function : CC1101WriteCmd( )
    Write a command byte to the device
INPUT    : command, the byte you want to write
OUTPUT   : None
================================================================================
*/
void CC1101WriteCmd( INT8U command )
{
    CC_CSN_LOW( );
    SPI_ExchangeByte( command );
    CC_CSN_HIGH( );
}
/*
================================================================================
Function : CC1101Reset( )
    Reset the CC1101 device
INPUT    : None
OUTPUT   : None
================================================================================
*/
void CC1101Reset( void )
{
    INT8U x;

    CC_CSN_HIGH( );
    CC_CSN_LOW( );
    CC_CSN_HIGH( );
    for( x = 0; x < 100; x ++ );        // 至少40us
    CC1101WriteCmd(CC1101_SRES);
}
/*
================================================================================
Function : CC1101SetIdle( )
    Set the CC1101 into IDLE mode
INPUT    : None
OUTPUT   : None
================================================================================
*/
void CC1101SetIdle( void )
{
    CC1101WriteCmd(CC1101_SIDLE);
}
/*
================================================================================
Function : CC1101ClrTXBuff( )
    Flush the TX buffer of CC1101
INPUT    : None
OUTPUT   : None
================================================================================
*/
void CC1101ClrTXBuff( void )
{
    CC1101SetIdle();//MUST BE IDLE MODE
    CC1101WriteCmd( CC1101_SFTX );
}
/*
================================================================================
Function : CC1101ClrRXBuff( )
    Flush the RX buffer of CC1101
INPUT    : None
OUTPUT   : None
================================================================================
*/
void CC1101ClrRXBuff( void )
{
    CC1101SetIdle();//MUST BE IDLE MODE
    CC1101WriteCmd( CC1101_SFRX );
}
/*
================================================================================
Function : CC1101SendPacket( )
    Send a packet
INPUT    : txbuffer, The buffer stores data to be sent
           size, How many bytes should be sent
           mode, Broadcast or address check packet
OUTPUT   : None
================================================================================
*/
void CC1101SendPacket( INT8U *txbuffer, INT8U size, TX_DATA_MODE mode )
{
    uint8_t address;
    static uint8_t flag = 0;
	
    if(mode == BROADCAST)          address = 0;
    else if(mode == ADDRESS_CHECK) address = CC1101ReadReg(CC1101_ADDR);  // 本机地址

    if(flag == 0)    // 仅第一次打印
    {
        printf("local_address:%d\r\n", (int)address);
        flag = 1;
    }
    CC1101ClrTXBuff();
    
    if((CC1101ReadReg(CC1101_PKTCTRL1)& ~0x03)!= 0)
    { 
        address = RX_Address;
        CC1101WriteReg(CC1101_TXFIFO, size + 1);
        CC1101WriteReg(CC1101_TXFIFO, address);
    }
    else
    {
        CC1101WriteReg(CC1101_TXFIFO, size);
    }

    CC1101WriteMultiReg(CC1101_TXFIFO, txbuffer, size);
    CC1101SetTRMode(TX_MODE);
    while(CC_IRQ_READ()!= 0);
    while(CC_IRQ_READ()== 0);

    CC1101ClrTXBuff();
}
/*
================================================================================
Function : CC1101GetRXCnt( )
    Get received count of CC1101
INPUT    : None
OUTPUT   : How many bytes hae been received
================================================================================
*/
INT8U CC1101GetRXCnt( void )
{
    return ( CC1101ReadStatus( CC1101_RXBYTES )  & BYTES_IN_RXFIFO );
}
/*
================================================================================
Function : CC1101SetAddress( )
    Set the address and address mode of the CC1101
INPUT    : address, The address byte
           AddressMode, the address check mode
OUTPUT   : None
================================================================================
*/
void CC1101SetAddress( INT8U address, ADDR_MODE AddressMode)
{
    INT8U btmp = CC1101ReadReg( CC1101_PKTCTRL1 ) & ~0x03;
    CC1101WriteReg(CC1101_ADDR, address);
    if     ( AddressMode == BROAD_ALL )     {}
    else if( AddressMode == BROAD_NO  )     { btmp |= 0x01; }
    else if( AddressMode == BROAD_0   )     { btmp |= 0x02; }
    else if( AddressMode == BROAD_0AND255 ) { btmp |= 0x03; }   
}
/*
================================================================================
Function : CC1101SetSYNC( )
    Set the SYNC bytes of the CC1101
INPUT    : sync, 16bit sync 
OUTPUT   : None
================================================================================
*/
void CC1101SetSYNC( INT16U sync )
{
    CC1101WriteReg(CC1101_SYNC1, 0xFF & ( sync>>8 ) );
    CC1101WriteReg(CC1101_SYNC0, 0xFF & sync ); 
}
/*
================================================================================
Function : CC1101RecPacket( )
    Receive a packet
INPUT    : rxBuffer, A buffer store the received data
OUTPUT   : 1:received count, 0:no data
================================================================================
*/
INT8U CC1101RecPacket(INT8U *rxBuffer)
{
    uint8_t status[2], pktLen;

    if(CC1101GetRXCnt()!= 0)
    {
        pktLen = CC1101ReadReg(CC1101_RXFIFO) & 0xff;        // Read length byte
        if((CC1101ReadReg(CC1101_PKTCTRL1) & ~0x03)!= 0)
        {
            CC1101ReadReg(CC1101_RXFIFO);
        }
        if(pktLen <= 0 || pktLen > 15) return 0;
        else                           pktLen --;
        CC1101ReadMultiReg(CC1101_RXFIFO, rxBuffer, pktLen); // Pull data
        CC1101ReadMultiReg(CC1101_RXFIFO, status, 2);        // Read  status bytes

        CC1101ClrRXBuff();

        if(status[1] & CRC_OK) return pktLen; 
        else                   return 0; 
    }
    else return 0;                               // Error
}
/*
================================================================================
Function : CC1101Init( )
    Initialize the CC1101, User can modify it
INPUT    : None
OUTPUT   : None
================================================================================
*/
void CC1101Init( void )
{
    volatile INT8U i, j;
    CC1101Reset();    
    
    for(i = 0; i < 23; i++)
    {
        CC1101WriteReg(CC1101InitData[i][0], CC1101InitData[i][1]);
    }
 
    for(i = 0; i < 23; i++)
    {
        j = CC1101ReadReg(CC1101InitData[i][0]);
        printf("%d  ", (int)j);
    }
    
    CC1101SetAddress(TX_Address, BROAD_0AND255);  // 从机地址
    CC1101SetSYNC(0xD391);                        // 8799
    CC1101WriteReg(CC1101_MDMCFG1, 0x22);         // Modem Configuration      
    CC1101WriteReg(CC1101_MDMCFG0, 0xF8);

//    CC1101WriteMultiReg(CC1101_PATABLE, PaTabel+1, 1);  // 发射功率
    CC1101WriteMultiReg(CC1101_PATABLE, PaTabel, 8);

//    i = CC1101ReadStatus(CC1101_PARTNUM);//for test, must be 0x80
//    i = CC1101ReadStatus(CC1101_VERSION);//for test, refer to the datasheet
//    CC1101SetTRMode(TX_MODE);                      // 发送模式 
    printf("Mode:TX\r\n");
}


/*
================================================================================
------------------------------------THE END-------------------------------------
================================================================================
*/
