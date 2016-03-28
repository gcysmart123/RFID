#include "com.h"
#include "ds1302.h"
#include "usart.h"

void mcu_to_computer(unsigned char call_type, unsigned char* number,unsigned char key_type)
{
	unsigned char temp1=0,temp2=0,key=0;

	unsigned char sec = 0, hour = 0, min = 0;
	Ds1302_Read_Time();

	Usart1_SendData(0xaa);
	Usart1_SendData(0x42);
	Usart1_SendData(0x01);
	Usart1_SendData(0x01);
	Usart1_SendData(0x01);
	Usart1_SendData(call_type);//呼叫或者取消  91 92
	Usart1_SendData(0xff);

	temp1 = *(number+1) << 4 | *(number + 2);
	temp2 = *(number+3) << 4 | *(number + 4);
	Usart1_SendData(temp1);//号码
	Usart1_SendData(temp2);//号码

	switch (key_type)
	{
	case 1:
		key = 0x01;
		Usart1_SendData(0x01);
		break;	
	case 2:
		key = 0x0d; 
		Usart1_SendData(0x0d);
		break;
	case 3:
		key = 0x03;
		Usart1_SendData(0x03);
		break;
	case 4:
		key = 0x0b; 
		Usart1_SendData(0x0b); 
		break;
	case 5:
		key = 0x05;
		Usart1_SendData(0x05);
		break;
	case 6:
		key = 0x06;
		Usart1_SendData(0x06);
		break;
	case 7:
		key = 0x07;
		Usart1_SendData(0x07);
		break;
	case 8:
		key = 0x0c;
		Usart1_SendData(0x0c); 
		break;
	case 9:
		key = 0x09;
		Usart1_SendData(0x09);
		break;
	case 10:
		key = 0x0A;
		Usart1_SendData(0x0A);
		break;
	case 11:
		key = 0x04;
		Usart1_SendData(0x04);
		break;
	case 12:
		key = 0x08;
		Usart1_SendData(0x08);
		break;
	case 13:
		key = 0x02;
		Usart1_SendData(0x02);
		break;
	case 14:
		key = 0x0e;
		Usart1_SendData(0x0e);
		break;
	case 15:
		key = 0x0f;
		Usart1_SendData(0x0f);
		break;
	default:
		key = 0x00;
		Usart1_SendData(0x00);
		break;
	}
	//Usart1_SendData(key_type);
	//键值
	

	Usart1_SendData(0);
	Usart1_SendData(0);

	hour  =  ((time_buf1[4] / 10)*6) + time_buf1[4];
	min =    ((time_buf1[5] / 10)*6) + time_buf1[5];
	sec  =   ((time_buf1[6] / 10)*6) + time_buf1[6];

	Usart1_SendData(hour);
	Usart1_SendData(min);
	Usart1_SendData(sec);

	Usart1_SendData(0x1EE + call_type + temp1 + temp2 + key + hour + min + sec);//校验


	Usart1_SendData(0x55);
}




