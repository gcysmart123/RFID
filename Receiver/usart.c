/***********************************************************
**************************************************************/
#include "usart.h"
#ifdef COPY_TWO_MACHINE
#include "key.h"
#include "menu.h"
#include "tm1629.h"
#endif
bit busy;
uint8_t usart2_enable = 0;
uint8_t usart2_data[4] = { 0, 0, 0,'\0' };
uint8_t usart2_num = 0;
uint8_t usart2_right = 0;
uint8_t usart2_begin = 0;
static const unsigned char* OKSPI = "OKSPI";
static const unsigned char* SPIOK = "SPIOK";
static const unsigned char* OK = "OK";

#ifdef COPY_TWO_MACHINE
uint8_t usart1_begin = 0;
uint8_t usart1_num = 0;
#endif
/*----------------------------
UART 中断服务程序
-----------------------------*/
void Uart() interrupt 4 using 1
{
	unsigned char temp=0;
    if (RI)
    {
        RI = 0;                 //清除RI位
#ifdef COPY_TWO_MACHINE
		temp = SBUF;
		if (return_func_index() == TWO_MENU_FE_E1_SET && Two_Menu_FE_status == 1)
		{
			if (temp == 0xAA) //AA表示接收正确 写2 地址加8
			{
				Two_Menu_FE_status = 2;
			}
			else if (temp == 0x55) //55表示接收错误 写0重发
			{
				Two_Menu_FE_status = 0;
			}
		}
		else if (return_func_index() == TWO_MENU_FE_E2_SET)
		{
			if (temp == 0xAA && usart1_begin == 0)
			{
				Two_Menu_FE_E2_data[0] = 0xAA; //首位
				usart1_num = 1;//数组计数
				usart1_begin = 1;//开始位置
			}
			else if (usart1_begin == 1)
			{
				Two_Menu_FE_E2_data[usart1_num] = temp;
				usart1_num++;
				if (usart1_num == 10)
				{
					usart1_num = 0;
					usart1_begin = 0;
					if (Two_Menu_FE_E2_data[0] == 0xAA && Two_Menu_FE_E2_data[9] == 0x55) //判断正确
					{
						Two_Menu_FE_E2_table = 1;
					}
					else
					{
						Usart1_SendData(0x55);
					}

				}
			}
		}

#endif
      //  computer_send_data_to_mcu();
      //  P0 = SBUF;              //P0显示串口数据
      //  P22 = RB8;              //P2.2显示校验位
    }
    if (TI)
    {
        TI = 0;                 //清除TI位
        busy = 0;               //清忙标志
    }
}

/*----------------------------
UART2 中断服务程序
-----------------------------*/
void Uart2() interrupt 8 using 1
{
	uint8_t temp=0;
	if (S2CON & S2RI)
	{
		S2CON &= ~S2RI;         //清除S2RI位
		temp = S2BUF;
		if (usart2_enable == 1 )
		{
			if (usart2_begin == 1)
			{
				usart2_data[usart2_num++] = temp;
			}

			if (temp == 'O'  &&  usart2_begin == 0)
			{
				usart2_begin = 1;
				usart2_num = 1;
				usart2_data[0] = temp;
			}
		}
		if (usart2_num == 2)
		{
			usart2_enable = 0;
			usart2_begin = 0;
			if (strcmp(usart2_data, OK) == 0)
			{
				usart2_right = 1;
			}
		}

		//P0 = S2BUF;             //P0显示串口数据
		//P2 = (S2CON & S2RB8);   //P2.2显示校验位
	}
	if (S2CON & S2TI)
	{
		S2CON &= ~S2TI;         //清除S2TI位
		busy = 0;               //清忙标志
	}
}

/*----------------------------
发送串口数据with 8 bits
----------------------------*/
void Usart1_SendData(BYTE dat)
{
    while (busy);               //等待前面的数据发送完成
    ACC = dat;                  //获取校验位P (PSW.0)
    if (P)                      //根据P来设置校验位
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;                //设置校验位为0
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;                //设置校验位为1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;                //设置校验位为1
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;                //设置校验位为0
#endif
    }
    busy = 1;
    SBUF = ACC;                 //写数据到UART数据寄存器
}

/*----------------------------
发送字符串
----------------------------*/
void Usart1_SendString(char *s)
{
    while (*s)                  //检测字符串结束标志
    {
		Usart1_SendData(*s++);         //发送当前字符
    }
}

/*----------------------------
发送串口数据
----------------------------*/
void Usart2_SendData(BYTE dat)
{
	while (busy);               //等待前面的数据发送完成
	ACC = dat;                  //获取校验位P (PSW.0)
	if (P)                      //根据P来设置校验位
	{
#if (PARITYBIT == ODD_PARITY)
		S2CON &= ~S2TB8;        //设置校验位为0
#elif (PARITYBIT == EVEN_PARITY)
		S2CON |= S2TB8;         //设置校验位为1
#endif
	}
	else
	{
#if (PARITYBIT == ODD_PARITY)
		S2CON |= S2TB8;         //设置校验位为1
#elif (PARITYBIT == EVEN_PARITY)
		S2CON &= ~S2TB8;        //设置校验位为0
#endif
	}
	busy = 1;
	S2BUF = ACC;                //写数据到UART2数据寄存器
}

/*----------------------------
发送字符串
----------------------------*/
void Usart2_SendString(char *s)
{
	while (*s)                  //检测字符串结束标志
	{
		Usart2_SendData(*s++);         //发送当前字符
	}
}

#ifdef PRINTER
/*----------------------------
发送串口数据with 8 bits
----------------------------*/
void SendData(BYTE dat)
{
    while (busy);               //等待前面的数据发送完成
    ACC = dat;                  //获取校验位P (PSW.0)
    if (P)                      //根据P来设置校验位
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;                //设置校验位为0
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;                //设置校验位为1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;                //设置校验位为1
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;                //设置校验位为0
#endif
    }
    busy = 1;
    SBUF = ACC;                 //写数据到UART数据寄存器
}

/*----------------------------
发送字符串
----------------------------*/
void SendString(char *s)
{
    while (*s)                  //检测字符串结束标志
    {
        SendData(*s++);         //发送当前字符
    }
}
#endif
/*uart init with baud settings----transfer mode-----open serial port*/
void UsartInit(void)
{
    ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)
    
//  ACC = P_SW1;
//  ACC &= ~(S1_S0 | S1_S1);    //S1_S0=1 S1_S1=0
//  ACC |= S1_S0;               //(P3.6/RxD_2, P3.7/TxD_2)
//  P_SW1 = ACC;  
//  
//  ACC = P_SW1;
//  ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=1
//  ACC |= S1_S1;               //(P1.6/RxD_3, P1.7/TxD_3)
//  P_SW1 = ACC;  

	P_SW2 &= ~S2_S0;            //S2_S0=0 (P1.0/RxD2, P1.1/TxD2)
	//  P_SW2 |= S2_S0;             //S2_S0=1 (P4.6/RxD2_2, P4.7/TxD2_2)

#if (PARITYBIT == NONE_PARITY)
    SCON = 0x50;                //8位可变波特率
#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    SCON = 0xda;                //9位可变波特率,校验位初始为1
#elif (PARITYBIT == SPACE_PARITY)
    SCON = 0xd2;                //9位可变波特率,校验位初始为0
#endif

#if (PARITYBIT == NONE_PARITY)
	S2CON = 0x50;               //8位可变波特率
#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
	S2CON = 0xda;               //9位可变波特率,校验位初始为1
#elif (PARITYBIT == SPACE_PARITY)
	S2CON = 0xd2;               //9位可变波特率,校验位初始为0
#endif


    T2L = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
    T2H = (65536 - (FOSC/4/BAUD))>>8;
    AUXR = 0x14;                //T2为1T模式, 并启动定时器2
    AUXR |= 0x01;               //选择定时器2为串口1的波特率发生器
    ES = 1;                     //使能串口中断
	IE2 = 0x01;                 //使能串口2中断
    EA = 1;

	//Usart1_SendString("STC15F2K60S2\r\nUart Test !\r\n");
	//Usart2_SendString("STC15F2K60S2\r\nUart2 Test !\r\n");
	
}

void uart_printf(const char *fmt, ...)
{
	va_list ap;
	char xdata string[256];//访问内部拓展RAM，非访问外部RAM，不能超过内部拓展RAM大小(此处为1024)  

	va_start(ap, fmt);
	vsprintf(string, fmt, ap);//此处也可以使用sprintf函数，用法差不多，稍加修改即可，此处略去  
	Usart1_SendString(string);
	va_end(ap);
}

///*Switch to uart2 port by hardware*/
//void usart2_SendData(void)
//{
//  ACC = P_SW1;
//  ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=1
//  ACC |= S1_S1;               //(P1.6/RxD_3, P1.7/TxD_3)
//  P_SW1 = ACC; 
//}
//
///*Switch to uart1 port by hardware*/
//void usart1_SendData(void)
//{
//    ACC = P_SW1;
//    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
//    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)
//}

