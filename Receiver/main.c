#include "STC15F2K60S2.H"
#include "usart.h"
#include "key.h"
#include "tm1629.h"
#include "timer.h"
#include "ds1302.h"
#include "ev1527.h"
#include "decoder.h"
#include "gd5800.h"
#include "menu.h"
#include "printer.h"
#include "global.h"

void main()
{	
	//WDT_CONTR = 0x00;
	UsartInit();	//串口初始化
#ifdef DEBUG
	uart_printf("UsartInit Complete! \r\n");
#endif
	
#ifdef DEBUG
	uart_printf("tm1629_init Complete! \r\n");
#endif
	Init_Timer0();
#ifdef DEBUG
	uart_printf("Init_Timer0 Complete! \r\n");
#endif
	Init_Timer1();
#ifdef DEBUG
	uart_printf("Init_Timer1 Complete! \r\n");
#endif
	exint0_init();
#ifdef DEBUG
	uart_printf("exint0_init Complete! \r\n");
#endif
	Ds1302_Init();
#ifdef DEBUG
	uart_printf("Ds1302_Init Complete! \r\n");
#endif
	GD5800_initialization();
	delay10ms();
#ifdef DEBUG
	uart_printf("GD5800_initialization Complete! \r\n");
#endif
	//Ds1302_Write_Time();
#if 0 //def PRINTER
     printer58mm_initialization();
#endif
	P3M1 = 0X0C; //IO口设置为输入 必须添加 否则无法解码
	P3M0 = 0X00;

	tm1629_init();
	//GD5800_select_chapter(CHUSHIHUA);
	env_init();
	
#ifdef WDT_FUNC
       WDT_CONTR = 0x3e; /*进行分频，时间为4s，在空闲状态也计数，启动*/
#endif

	while (1)
	{
#ifdef WDT_FUNC
       WDT_CONTR = 0x3e;  /* 4 seconds to 喂狗*/
#endif
		KeyProcess();
		DecoderProcess();

#ifdef REPLAY_PATCH /*sjz add 2015 11 30*/

        replay_process();
/*sjz add 2015 11 30 end*/
#endif



	}
}



