/*****************************************************************
2015 by quanzhou noraml university
******************************************************************/
#include "global.h"

/*********************************************
 initialization
*********************************************/
void printer58mm_initialization(void)
{
 //   Usart1_SendData(); /*switch to uart1*/
    SendData(0x1B);      /*send command to printer for initialization*/
    SendData(0x40);
}

/*********************************************
 set the printer mode before use the printer
*********************************************/
void printer58mm_setmode(unsigned char mode)
{
    SendData(0x1B);
    SendData(0x21);
    SendData(mode);
}

