#include "stdio.h" 
#include "string.h" 
#include "bsp.h" 
#include "CC1101.H"

#define LED_GPIO_PORT  GPIOA
#define LED_GPIO_PINS  GPIO_Pin_6

volatile u16  Cnt1ms = 0;    // 1ms����������ÿ1ms��һ 
int  RecvWaitTime = 0;        // ���յȴ�ʱ��                
u16  SendCnt = 0;             // �������͵����ݰ���                

                           // ֡ͷ  Դ��ַ  Ŀ���ַ  distance*10  ֡β
u8 SendBuffer[SEND_LENGTH] = {0x55,   0,    0xff,     15,    0x0d, 0x0a}; // �ӻ�����������
                           // ֡ͷ  Դ��ַ  Ŀ���ַ  ֡β
u8 AckBuffer[ACK_LENGTH]   = {0x55,  0xff,     0,     0x0d, 0x0a};                                             // ����Ӧ������

void TIM3_Set(u8 sta);                          // ����TIM3�Ŀ���   sta:0���ر�   1������
void USART1_SendStr(unsigned char *Str);       // USART�����ַ�������                         
void System_Initial(void);                     // ϵͳ��ʼ��
void Sleep_Initial(void);                      // AWU��ʱ���ѳ�ʼ�� 
u8   RF_SendPacket(u8 *Sendbuffer, u8 length);  // �ӻ��������ݰ�
void DelayMs(u16 x); 

// printf֧��
int putchar(int c)  
{  
  while(!USART_GetFlagStatus (USART_FLAG_TXE));//�ȴ��������
  USART_SendData8((uint8_t)c);
  return (c);  
}

void Delay(__IO uint16_t nCount)
{
    /* Decrement nCount value */
    while (nCount != 0)
    {
        nCount--;
    }
}

void main(void)
{
    volatile u8 res = 0;
    volatile u8 Timer_30s = 6;                        // �ϵ緢��
    
    System_Initial();                                 // ��ʼ��ϵͳ��������               
    CC1101Init();                                   // ��ʼ��CC1101Ϊ����ģʽ 
    SendBuffer[1] = TX_Address;                       // ���ݰ�Դ��ַ���ӻ���ַ��
    
    //Sleep_Initial();                                  // AWU��ʱ���ѳ�ʼ�� 
    
    // ͨ�Ų���
//    while(1)
//    {
//        LED_ON();                          // LED��˸������ָʾ���ͳɹ�
//        CC1101Init(); 
// send:        
//        res = RF_SendPacket(SendBuffer, SEND_LENGTH);
//        if(res != 0) 
//        {
//          printf("Send ERROR:%d\r\nRetry now...\r\n", (int)res);  // ����ʧ��
//          DelayMs(300);
//          goto send;
//        }
//        else  printf("Send OK!\r\n");              // ���ͳɹ�
//        LED_OFF();
//        DelayMs(1000);DelayMs(1000);DelayMs(1000);
//    }
    
    while(1)
    {
        printf("Timer_30s=%d\r\n", (int)Timer_30s);  
        if(Timer_30s++ == 6)                   // Լ 3 Min     30s * 6
        {
            SWITCH_ON();
            LED_ON();                          // LED��˸������ָʾ���ͳɹ�
            CC1101Init(); 
 send:            
            res = RF_SendPacket(SendBuffer, SEND_LENGTH);
            if(res != 0) 
            {
              printf("Send ERROR:%d\r\nRetry now...\r\n", (int)res);  // ����ʧ��
              DelayMs(10);
              //DelayMs(350);
              goto send;
            }
            else printf("Send OK!\r\n");              // ���ͳɹ�
            
            SWITCH_OFF();
            LED_OFF();
            Timer_30s = 1;
        }
        halt();//������͹���
    }
}

// ����TIM3�Ŀ���
// sta:0���ر�   1������
void TIM3_Set(u8 sta)
{
    if(sta)
    {  
        TIM3_SetCounter(0);     // ���������
        TIM3_ITConfig(TIM3_IT_Update,ENABLE);   // ʹ��TIM3�����ж�
        TIM3_Cmd(ENABLE);      // ʹ��TIM3	
    }
    else 
    {
        TIM3_Cmd(DISABLE);     // �ر�TIM3		   
        TIM3_ITConfig(TIM3_IT_Update,DISABLE);  // �ر�TIM3�����ж�
    }
}

/*===========================================================================
* ���� : DelayMs() => ��ʱ����(ms��)                                        *
* ���� ��x, ��Ҫ��ʱ����(0-65535)                                             *
============================================================================*/
void DelayMs(u16 x)
{
    volatile u16 timer_ms = x;
    
    Cnt1ms = 0;
    TIM3_Set(1);
    while(Cnt1ms <= timer_ms);
    TIM3_Set(0);
}

/*===========================================================================
* ���� ��TIM3_1MS_ISR() => ��ʱ��3������, ��ʱʱ���׼Ϊ1ms               *
============================================================================*/
void TIM3_1MS_ISR(void)
{
    Cnt1ms++;
    
    if(RecvWaitTime > 0) RecvWaitTime--;    // ���ݽ��ռ�ʱ
}

/*===========================================================================
* ����: System_Initial() => ��ʼ��ϵͳ��������                              *
============================================================================*/
void System_Initial(void)
{
    SClK_Initial();         // ��ʼ��ϵͳʱ�ӣ�16M     
    GPIO_Initial();         // ��ʼ��GPIO   LED
 
    USART1_Initial();       // ��ʼ������1  
    TIM3_Initial();         // ��ʼ����ʱ��3����׼1ms  
    SPI_Initial();          // ��ʼ��SPI               
    enableInterrupts();     // �����ж� 
    
    printf("Oil_Can_Drone\r\n");                      // �����ַ�����ĩβ����
}

/*===========================================================================
* ���� : BSP_RF_SendPacket() => ���߷������ݺ���                            *
* ���� ��Sendbufferָ������͵����ݣ�length�������ݳ���                      *
* ��� ��0�����ͳɹ�                                                      
         1���ȴ�Ӧ��ʱ
         2�����ݰ����ȴ���
         3�����ݰ�֡ͷ����
         4�����ݰ�Դ��ַ����
         5�����ݰ�Ŀ���ַ����
         6�����ݰ�֡β
============================================================================*/
INT8U RF_SendPacket(INT8U *Sendbuffer, INT8U length)
{
    uint8_t  i = 0, ack_len = 0, ack_buffer[15] = {0};
    RecvWaitTime = (int)RECV_TIMEOUT;           // �ȴ�Ӧ��ʱ����1500ms
    
    CC1101SendPacket(SendBuffer, length, ADDRESS_CHECK);    // �������� 
    //DelayMs(5);                       
    
    //CC1101Init();                               // ��ʼ��L01�Ĵ��� 
    CC1101SetTRMode(RX_MODE);                   // ׼������Ӧ��

    TIM3_Set(1);                                // ����TIM3
    printf("waiting for ack...\r\n");
    while(CC_IRQ_READ() != 0)                   // �ȴ��������ݰ�
    {
        if(RecvWaitTime <= 0)      
        {  
            TIM3_Set(0);                            // �ر�TIM3
            printf("RecvWaitTime0=%d\r\n", RecvWaitTime);
            return 1;                              // �ȴ�Ӧ��ʱ
        }
    }
    //TIM3_Set(0); 
    //printf("RecvWaitTime1=%d\r\n", RecvWaitTime);

    RecvWaitTime = 50;           // �ȴ�Ӧ��ʱ����50ms
    //TIM3_Set(1);                                // ����TIM3
    while(CC_IRQ_READ() == 0)
    {
        if(RecvWaitTime <= 0)      
        {  
            TIM3_Set(0);                            // �ر�TIM3
            printf("RecvWaitTime1=%d\r\n", RecvWaitTime);
            return 1;                              // �ȴ�Ӧ��ʱ
        }
    }
    printf("RecvWaitTime2=%d\r\n", RecvWaitTime);
    TIM3_Set(0);                                // �ر�TIM3
    ack_len = CC1101RecPacket(ack_buffer);      // ��ȡ�յ�������
    
//                        // ֡ͷ  Դ��ַ  Ŀ���ַ    ֡β
//AckBuffer[ACK_LENGTH]   = {0x55,  0xff,     0,     0x0d, 0x0a};                                             // ����Ӧ������
    
//    if((strlen((const char*)ack_buffer) <= 0) || (strlen((const char*)ack_buffer)) > 29)  
//    {
//        CC1101Init(); 
//        printf("ack_len0=%d\r\n", ack_len);
//        return 2;                                              // ���ݰ����ȴ���
//    }
    
    if(ack_len <= 0 || ack_len > 15)  
    {
        CC1101Init(); 
        printf("ack_len1=%d\r\n", ack_len);
        return 2;                                          // ���ݰ����ȴ���
    }
    if(ack_len != ACK_LENGTH) return 2;                                               // ���ݰ����ȴ���
    if(ack_buffer[0] != 0x55) return 3;                                               // ���ݰ�֡ͷ����
    if(ack_buffer[1] != 0xff) return 4;                                               // ���ݰ�Դ��ַ����       
    if(ack_buffer[2] == 0xff) return 5;                                               // ���ݰ�Ŀ���ַ����
    if((ack_buffer[ack_len-2] != 0x0d) || (ack_buffer[ack_len-1] != 0x0a)) return 6;  // ���ݰ�֡β

    // Ӧ����ȷ
    printf("ack_len=%d;ack_buffer:", (int)ack_len);
    for(i = 0; i < ack_len; i++)                     
    {
        printf("%d ", (int)ack_buffer[i]);
    }
    printf("\r\n");

    return 0;  
}

/*******************************************************************************
****��ڲ�������Ҫ���͵��ַ���
****���ڲ�������
****������ע��USART�����ַ�������
*******************************************************************************/
void USART1_SendStr(unsigned char *Str) 
{
    while(*Str!=0)//��Ϊ����
    {
        USART_SendData8(*Str);     //�������� 
        while(!USART_GetFlagStatus (USART_FLAG_TXE));//�ȴ��������
        Str++;//��һ������
    }
}

// AWU��ʱ���ѳ�ʼ��
void Sleep_Initial(void)
{
    // ����IOȫ������͵�ƽ    ���͹���
    GPIO_Init(GPIOA, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(GPIOB, GPIO_Pin_0|GPIO_Pin_1, GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(GPIOC, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(GPIOD, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);
    
    CLK_PeripheralClockConfig(CLK_Peripheral_AWU, ENABLE); // ʹ��AWU����ʱ��
    
    AWU_DeInit();                                          // AWU�ָ���ʼ״̬
    AWU_LSICalibrationConfig(12800);                       // AWU LSIУ׼����ΪAWU�Ǳ�LSI����������
    AWU_Init(AWU_Timebase_30s);                            // 30s��ʱ����
    AWU_Cmd(ENABLE);                                       // ʹ��AWU
}