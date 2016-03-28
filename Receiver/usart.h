#ifndef __USART_H_
#define __USART_H_
/*************************************************
this file will define all  usart  function 
************************************************/
#define USART 
//#define DEBUG

#include "STC15F2K60S2.h"
#include<stdio.h>  
#include<stdarg.h> 
#include "tpyedef.h"
#include "string.h"
#include "global.h"

#define  BYTE unsigned char 


#define FOSC 12000000L          //系统频率
#define BAUD 9600             //串口波特率

#define NONE_PARITY     0       //无校验
#define ODD_PARITY      1       //奇校验
#define EVEN_PARITY     2       //偶校验
#define MARK_PARITY     3       //标记校验
#define SPACE_PARITY    4       //空白校验

#define PARITYBIT NONE_PARITY   //定义校验位

#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
#define S2RB8 0x04              //S2CON.2
#define S2TB8 0x08              //S2CON.3

#define S2_S0 0x01              //P_SW2.0

#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7

extern uint8_t usart2_enable;
extern uint8_t usart2_data[4] ;
extern uint8_t usart2_num;
extern uint8_t usart2_right;
#ifdef COPY_TWO_MACHINE
extern uint8_t usart1_begin ;
#endif
void Usart1_SendData(BYTE dat);
void Usart1_SendString(char *s);
void Usart2_SendData(BYTE dat);
void Usart2_SendString(char *s);
void UsartInit(void);
void uart_printf(const char *fmt, ...);
#ifdef PRINTER
void SendData(BYTE dat);
void SendString(char *s);
#endif
//void usart1_SendData(void);
//void usart2_SendData(void);

#endif

