#include "tm1629.h"
#include "timer.h"
#include "usart.h"
#include "ds1302.h"
#include "key.h"
#include "menu.h"
#include "gd5800.h"
#include "at24c256.h"
#include "ev1527.h"
#include "decoder.h"
//#include "global.h"

unsigned char const CODE[] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x76, 0x38, 0x5c, 0x73, 0x3e };//0-9 abcdef 显示器码数组
unsigned char const INIT_CODE[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };//逐段点亮数码管数组
unsigned char const SHANGSHUO[] = { 0x40, 0x00 }; // 点亮数码管中间段以及灭
unsigned char buf_display[6][8] = { 0 }; //3个TM1629显存数组
unsigned char display_ram[800] = { 0 }; //程序运行时记录显示数据的内存 
unsigned char await_time_table= 0 ;//用于记录待机显示横杠数码管次数 
unsigned char Two_Menu_F8_E1_save =0;

unsigned char single_key[16]   = { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 };//单键位设置存储数组
unsigned char multiple_key[16] = { 0x01, QUXIAO-QUXIAO, JIEZHANG-QUXIAO, DIANDANG-QUXIAO, JIUSHUI-QUXIAO, 0X01, 0x01, JIASHUI-QUXIAO, HUJIAO - QUXIAO, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 };//多键位设置存储数组
#ifdef COPY_TWO_MACHINE
//11111111111111
uint16_t Two_Menu_FE_E1_addr = 0;
uint8_t  Two_Menu_FE_status = 0;
uint16_t Two_Menu_FE_E2_addr = 0;
uint8_t  Two_Menu_FE_E2_table = 0;
uint8_t  Two_Menu_FE_E2_data[10];
#endif
void writeDataTo1629(unsigned char p) //写数据给第一个TM1629
{
	unsigned int i;
	TM1629_STB = 0;
	for (i = 0; i<8; i++)
	{
		TM1629_CLK = 0;
		if (p & 0x01)
			TM1629_DIO = 1;
		else
			TM1629_DIO = 0;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		TM1629_CLK = 1;
		p = p >> 1;
	}
	TM1629_CLK = 0;
	TM1629_DIO = 0;
}

void writeDataTo1629_2(unsigned char p) //写数据给第二个TM1629
{
	unsigned int i;
	TM1629_STB2 = 0;
	for (i = 0; i<8; i++)
	{
		TM1629_CLK = 0;
		if (p & 0x01)
			TM1629_DIO = 1;
		else
			TM1629_DIO = 0;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		TM1629_CLK = 1;
		p = p >> 1;
	}
	TM1629_CLK = 0;
	TM1629_DIO = 0;
}

void writeDataTo1629_3(unsigned char p) //写数据给第三个TM1629
{
	unsigned int i;
	TM1629_STB3 = 0;
	for (i = 0; i<8; i++)
	{
		TM1629_CLK = 0;
		if (p & 0x01)
			TM1629_DIO = 1;
		else
			TM1629_DIO = 0;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		TM1629_CLK = 1;
		p = p >> 1;
	}
	TM1629_CLK = 0;
	TM1629_DIO = 0;
}

void send_command(unsigned char word) //写命令给第一个TM1629
{
	TM1629_STB = 1;
	nop;
	TM1629_STB = 0;
	writeDataTo1629(word);
}

void send_command_2(unsigned char word) //写命令给第二个TM1629
{
	TM1629_STB2 = 1;
	nop;
	TM1629_STB2 = 0;
	writeDataTo1629_2(word);
}

void send_command_3(unsigned char word) //写命令给第三个TM1629
{
	TM1629_STB3 = 1;
	nop;
	TM1629_STB3 = 0;
	writeDataTo1629_3(word);
}

void display(void) //3个TM1629显示函数
{
	unsigned char i,temp;
#ifdef PRODUCT_4_15INCH
	unsigned char buf_temp;
#endif
	temp = return_Two_Menu_F6_E5();
	send_command(0x40);	//设置数据命令:普通模式、地址自增1，写数据到显存
	send_command(0xc0);	//设置显示地址命令：从00H开始
#ifdef PRODUCT_4_15INCH
	buf_display[1][7] = buf_display[1][3];
	buf_display[1][6] = buf_display[1][2];
	buf_display[1][5] = buf_display[1][1];
	buf_temp =	buf_display[1][0];
	buf_display[1][0] = buf_display[1][4];
	buf_display[1][4] = buf_temp;
#endif 	
#ifdef PRODUCT_4_30INCH
		buf_display[0][4]=buf_display[0][0];
		buf_display[0][5]=buf_display[0][1];
		buf_display[0][6]=buf_display[0][2];
		buf_display[0][7]=buf_display[0][3];
#endif
	for (i = 0; i<8; i++)	//发送16字节的显存数据
	{
		writeDataTo1629(buf_display[0][i]);
		writeDataTo1629(buf_display[1][i]);
	}
	send_command(0x88 + temp);	//设置显示控制命令：打开显示，并设置为11/16.
	TM1629_STB = 1;

	send_command_2(0x40);	//设置数据命令:普通模式、地址自增1，写数据到显存
	send_command_2(0xc0);	//设置显示地址命令：从00H开始
	for (i = 0; i<8; i++)	//发送16字节的显存数据
	{
		writeDataTo1629_2(buf_display[2][i]);
		writeDataTo1629_2(buf_display[3][i]);
	}
	send_command_2(0x88 + temp);	//设置显示控制命令：打开显示，并设置为11/16.
	TM1629_STB2 = 1;

	send_command_3(0x40);	//设置数据命令:普通模式、地址自增1，写数据到显存
	send_command_3(0xc0);	//设置显示地址命令：从00H开始
	for (i = 0; i<8; i++)	//发送16字节的显存数据
	{
		writeDataTo1629_3(buf_display[4][i]);
		writeDataTo1629_3(buf_display[5][i]);
	}
	send_command_3(0x88 + temp);	//设置显示控制命令：打开显示，并设置为11/16.
	TM1629_STB3 = 1;
}

void display_light(unsigned char temp) //3个TM1629显示函数
{
	unsigned char i;
	send_command(0x40);	//设置数据命令:普通模式、地址自增1，写数据到显存
	send_command(0xc0);	//设置显示地址命令：从00H开始
	for (i = 0; i<8; i++)	//发送16字节的显存数据
	{
		writeDataTo1629(temp);
		writeDataTo1629(temp);
	}
	send_command(0x8f);	//设置显示控制命令：打开显示，并设置为11/16.
	TM1629_STB = 1;

	send_command_2(0x40);	//设置数据命令:普通模式、地址自增1，写数据到显存
	send_command_2(0xc0);	//设置显示地址命令：从00H开始
	for (i = 0; i<8; i++)	//发送16字节的显存数据
	{
		writeDataTo1629_2(temp);
		writeDataTo1629_2(temp);
	}
	send_command_2(0x8f);	//设置显示控制命令：打开显示，并设置为11/16.
	TM1629_STB2 = 1;

	send_command_3(0x40);	//设置数据命令:普通模式、地址自增1，写数据到显存
	send_command_3(0xc0);	//设置显示地址命令：从00H开始
	for (i = 0; i<8; i++)	//发送16字节的显存数据
	{
		writeDataTo1629_3(temp);
		writeDataTo1629_3(temp);
	}
	send_command_3(0x8f);	//设置显示控制命令：打开显示，并设置为11/16.
	TM1629_STB3 = 1;
}

void tm1629_init(void) //TM1629开机初始化函数
{
	unsigned char i, j, k;		//k控制显示的具体数字，i和j控制buf_display的刷新
	for (k = 0; k<8; k++)
	{
		for (i = 0; i<6; i++)
		{
			for (j = 0; j<8; j++)
			{
				buf_display[i][j] = INIT_CODE[k];
			}
		}
		display();
		if (k == 1 )
		GD5800_select_chapter(CHUSHIHUA);
		Tm1629_delay(30);
	}
}


void Tm1629_delay(unsigned char k) //延时函数
{
	unsigned char i, j;
	for (; k>0; k--)
	{
		for (i = 255; i>0; i--)
		{
			for (j = 255; j>0; j--)
			{
				;

			}
		}
	}
}

void tm1629_clear(void)//全部归零
{
	unsigned char i, j;
	for (i = 0; i<6; i++)
	{
		for (j = 0; j<8; j++)
		{
			buf_display[i][j] = 0x00;
		}
	}
}

void tm1629_display_buff_clear(void)//清除队列缓存
{
	unsigned char i;
	for (i = 0; i<120; i++)
		display_ram[i] = 0;
}

void tm1629_await(void)
{
	unsigned char i;	//k控制显示的具体数字，i和j控制buf_display的刷新
	unsigned char await_number_table_temp = 0;
	await_number_table_temp = return_await_number_table();
	if (await_number_table_temp == 1)
	{
		//GD5800_select_chapter(0x0008)	;
		tm1629_clear();
		i = await_time_table & 0x03;
		buf_display[0][i] = 0x40;
		display();
		set_await_number_table(0);
		await_time_table++;
		if (await_time_table == 4)
			await_time_table = 0;
	}
}

void Display_time(void)
{
	if (return_Two_Menu_F6_E7() == 0)
	{
		tm1629_clear();
		Ds1302_Read_Time();
		buf_display[0][7] = CODE[2];				//年
		buf_display[0][6] = CODE[0];
		buf_display[0][5] = CODE[time_buf1[1] / 10];
		buf_display[0][4] = CODE[time_buf1[1] % 10];


		buf_display[1][1] = CODE[time_buf1[2] / 10]; //月
		buf_display[1][0] = CODE[time_buf1[2] % 10];


		buf_display[1][3] = CODE[time_buf1[3] / 10];
		buf_display[1][2] = CODE[time_buf1[3] % 10];//日

		buf_display[1][4] = CODE[time_buf1[7]];		//星期

		buf_display[0][3] = CODE[time_buf1[4] / 10]; //小时
		buf_display[0][2] = CODE[time_buf1[4] % 10];
		buf_display[0][1] = CODE[time_buf1[5] / 10]; //小时
		buf_display[0][0] = CODE[time_buf1[5] % 10];

		display();

	}
	else
	{
		tm1629_clear();
		Ds1302_Read_Time();
		buf_display[1][3] = CODE[2];				//年
		buf_display[1][2] = CODE[0];
		buf_display[1][1] = CODE[time_buf1[1] / 10];
		buf_display[1][0] = CODE[time_buf1[1] % 10];


		buf_display[0][7] = CODE[time_buf1[2] / 10]; //月
		buf_display[0][6] = CODE[time_buf1[2] % 10];


		buf_display[0][5] = CODE[time_buf1[3] / 10];
		buf_display[0][4] = CODE[time_buf1[3] % 10];//日

		buf_display[1][4] = CODE[time_buf1[7]];		//星期

		buf_display[0][3] = CODE[time_buf1[4] / 10]; //小时
		buf_display[0][2] = CODE[time_buf1[4] % 10];
		buf_display[0][1] = CODE[time_buf1[5] / 10]; //小时
		buf_display[0][0] = CODE[time_buf1[5] % 10];

		display();
	}
}

void tm1629_f(unsigned char f_number) //第一行倒数二个显示F,倒数第一个显示f_number
{
	tm1629_clear();
#ifdef PRODUCT_6_32INCH
	buf_display[0][2] = CODE[0x0f];
	buf_display[0][1] = CODE[f_number];
#endif
#ifdef  OLD_FUNTION
	buf_display[0][1] = CODE[0x0f];
	buf_display[0][0] = CODE[f_number];
#endif
	display();
}

void tm1629_E(unsigned char f_number) //第一行倒数二个显示F,倒数第一个显示f_number
{
	tm1629_clear();
#ifdef PRODUCT_6_32INCH
	buf_display[0][2] = CODE[0x0E];
	buf_display[0][1] = CODE[f_number];
#endif
#ifdef  OLD_FUNTION
	buf_display[0][1] = CODE[0x0E];
	buf_display[0][0] = CODE[f_number];
#endif
	display();
}
void Show_on(void)
{
	tm1629_clear();
#ifdef PRODUCT_6_32INCH
	buf_display[0][2] = CODE[0];
	buf_display[0][1] = 0X37;
#endif
#ifdef OLD_FUNTION
	buf_display[0][1] = CODE[0];
	buf_display[0][0] = 0x37;
#endif
	display();
}

void Show_off(void)
{
	tm1629_clear();
#ifdef PRODUCT_6_32INCH
	buf_display[0][2] = CODE[0x00];
	buf_display[0][1] = CODE[0X0f];
#endif
#ifdef OLD_FUNTION
	buf_display[0][1] = CODE[0x00];
	buf_display[0][0] = CODE[0x0f];
#endif
	display();
}

void Show_one_number(unsigned char f_number) //显示一个数字
{
	tm1629_clear();
	//buf_display[0][1] = CODE[0x0E];
#ifdef PRODUCT_6_32INCH
	buf_display[0][1] = CODE[f_number];
#endif
#ifdef OLD_FUNTION
	buf_display[0][0] = CODE[f_number];
#endif
	display();
}

void Show_two_number(unsigned char f_number) //显示两个数字
{
	tm1629_clear();
#ifdef PRODUCT_6_32INCH
	buf_display[0][2] = CODE[f_number / 10];
	buf_display[0][1] = CODE[f_number % 10];
#endif
#ifdef OLD_FUNTION
	buf_display[0][1] = CODE[f_number / 10];
	buf_display[0][0] = CODE[f_number % 10];
#endif
	display();
}


void Show_three_number(unsigned int f_number) //显示三个数字
{
	tm1629_clear();
#ifdef PRODUCT_6_32INCH
	buf_display[0][3] = CODE[f_number / 100];
	buf_display[0][2] = CODE[(f_number / 10) %10];
	buf_display[0][1] = CODE[f_number % 10];
#endif
#ifdef OLD_FUNTION
	buf_display[0][2] = CODE[f_number / 100];
	buf_display[0][1] = CODE[(f_number / 10) %10];
	buf_display[0][0] = CODE[f_number % 10];
#endif
	display();
}

void Show_four_number(unsigned char* temp)
{
	tm1629_clear();
#ifdef PRODUCT_6_32INCH
	buf_display[0][3] = CODE[*(temp+1)];
	buf_display[0][2] = CODE[*(temp+2)];
	buf_display[0][1] = CODE[*(temp+3)];
	//buf_display[0][0] = CODE[*(temp+3)];
#endif
#ifdef OLD_FUNTION
	buf_display[0][3] = CODE[*(temp)];
	buf_display[0][2] = CODE[*(temp+1)];
	buf_display[0][1] = CODE[*(temp+2)];
	buf_display[0][0] = CODE[*(temp+3)];
#endif
	display();
}

void tm1629_load(void)
{
	tm1629_clear();
	if (*(display_ram + 88) != 0)
		mcuram_to_displayram(buf_display[5] + 7, display_ram + 88);
	if (*(display_ram + 80) != 0)
		mcuram_to_displayram(buf_display[5] + 3, display_ram + 80);
	if (*(display_ram + 72) != 0)
		mcuram_to_displayram(buf_display[4] + 7, display_ram + 72);
	if (*(display_ram + 64) != 0)
		mcuram_to_displayram(buf_display[4] + 3, display_ram + 64);
	if (*(display_ram + 56) != 0)
		mcuram_to_displayram(buf_display[3] + 7, display_ram + 56);
	if (*(display_ram + 48) != 0)
		mcuram_to_displayram(buf_display[3] + 3, display_ram + 48);
	if (*(display_ram + 40) != 0)
		mcuram_to_displayram(buf_display[2] + 7, display_ram + 40);
	if (*(display_ram + 32) != 0)
		mcuram_to_displayram(buf_display[2] + 3, display_ram + 32);
	if (*(display_ram + 24) != 0)
		mcuram_to_displayram(buf_display[1] + 7, display_ram + 24);
	if (*(display_ram + 16) != 0)
		mcuram_to_displayram(buf_display[1] + 3, display_ram + 16);
	if (*(display_ram + 8) != 0)
		mcuram_to_displayram(buf_display[0] + 7, display_ram + 8);
	if (*(display_ram + 0) != 0)
		mcuram_to_displayram(buf_display[0] + 3, display_ram);

	//buf_display[0][5] = CODE[decoder_num / 10];
	//buf_display[0][4] = CODE[decoder_num % 10];
}

void mcuram_to_displayram(unsigned char a[48], unsigned char* b)  //从RAM区域移到显存区域,顺带翻译成数码管显示的码
{
	/*
	A// 类别、区号、接收号
	B// 类别、区号、接收号
	*/
#ifdef PRODUCT_6_32INCH
	unsigned char temp = 0;
	temp = return_Two_Menu_FC_E1();
	if(temp == 1)
		{
			*(a) = CODE[*(b + 2)];
			*(a - 1) = CODE[*(b + 3)];
			*(a - 2) = CODE[*(b + 4)];
			//*(a - 3) = CODE[*(b + 4)];   //个位
			switch (*(b + 5) & 0x0f)//显示呼叫类型
			{
			case 2:
				*(a - 3) |= CODE[0X0B];
				break;//7键
			case 3:
				*(a - 3) |= CODE[0X0E];
				break;//78键
			case 4:
				*(a - 3) |= CODE[0X0D];
				break;//6键
			case 7:
				*(a - 3) |= CODE[0X0A];
				break;//678键
			case 8:
				*(a - 3) |= CODE[0x0C];
				break;//5键
			default:break;
			}
			
		}
	else
		{
			*(a) = CODE[*(b + 2)];
			*(a - 1) = CODE[*(b + 3)];
			*(a - 2) = CODE[*(b + 4)];  //个位

			switch (*(b + 5) & 0x0f)//显示呼叫类型
			{
			case 2:
				*(a - 2) |= 0x80;
				*(a - 1) |= 0x80;

				break;//7键
			case 3:
				*(a - 0) |= 0x80;
				*(a - 2) |= 0x80;
				*(a - 1) |= 0x80;
				*(a) |= 0x80;
				break;//78键
			case 4:
				*(a - 0) |= 0x80;
				*(a - 2) |= 0x80;
				*(a - 1) |= 0x80;
				break;//6键
			case 7:
				*(a - 1) |= 0x80;
				break;//678键
			case 8:
				*(a - 2) |= 0x80;
				break;//5键
			default:break;
			}
		}
#endif
#ifdef OLD_FUNTION
	*(a) = CODE[*(b + 1)];
	*(a - 1) = CODE[*(b + 2)];
	*(a - 2) = CODE[*(b + 3)];
	*(a - 3) = CODE[*(b + 4)];

	switch (*(b + 5) & 0x0f)//显示呼叫类型
	{
	case 2:
		*(a - 3) |= 0x80;
		*(a - 2) |= 0x80;

		break;//7键
	case 3:
		*(a - 3) |= 0x80;
		*(a - 2) |= 0x80;
		*(a - 1) |= 0x80;
		*(a) |= 0x80;
		break;//78键
	case 4:
		*(a - 3) |= 0x80;
		*(a - 2) |= 0x80;
		*(a - 1) |= 0x80;
		break;//6键
	case 7:
		*(a - 2) |= 0x80;
		break;//678键
	case 8:
		*(a - 3) |= 0x80;
		break;//5键
	default:break;
	}
#endif

}

void decoder_temp_to_mcuram(unsigned char* a, unsigned char* index)//a为MCU缓存区   index为解码后取出的8字节临时数组
{
	*(a + 0) = *(index);
	*(a + 1) = *(index + 1);
	*(a + 2) = *(index + 2);
	*(a + 3) = *(index + 3);
	*(a + 4) = *(index + 4);
	*(a + 5) = *(index + 7);//保存按键的键值
}

void mcuram_to_mcuram_down(unsigned char* a) //a为MCURAM缓存区区   向下压一组数据
{
	*(a + 8) = *(a);		//ram区元素下移6个
	*(a + 9) = *(a + 1);
	*(a + 10) = *(a + 2);
	*(a + 11) = *(a + 3);
	*(a + 12) = *(a + 4);
	*(a + 13) = *(a + 5);

}

void mcuram_to_mcuram_up(unsigned char* a) //a为MCURAM缓存区区   向上压一组数据
{
	*(a) = *(a + 8);   //ram区元素上移6个
	*(a + 1) = *(a + 9);
	*(a + 2) = *(a + 10);
	*(a + 3) = *(a + 11);
	*(a + 4) = *(a + 12);
	*(a + 5) = *(a + 13);
}

void CycleProcess(void)
{
	unsigned char i, index;
	unsigned char f5_xunhuan, f4_xiaohao, logout_cycle_table_temp;
	unsigned char temp[8] = { 0 };
	unsigned char Two_Menu_F6_E3_temp = 0;
	unsigned char Two_Menu_F6_E1_temp = 0;

	Two_Menu_F6_E3_temp = return_Two_Menu_F6_E3();
	Two_Menu_F6_E1_temp = return_Two_Menu_F6_E1();

	f5_xunhuan = return_Two_Menu_F5_E1();
	f4_xiaohao = return_Two_Menu_F4_E1();
	logout_cycle_table_temp = return_logout_cycle_table();
	/*循环处理函数*/
	if ((f5_xunhuan<f4_xiaohao && logout_cycle_table_temp>f5_xunhuan && f4_xiaohao && f5_xunhuan) || (f4_xiaohao == 0 && f5_xunhuan &&logout_cycle_table_temp>f5_xunhuan))
	{
		for (i = 0; i<10; i++)
		{
			if (display_ram[i<<3] == 0)
			{
				index = i;
				break;
			}
		}

		for (i = 0; i<6; i++)
		{
			temp[i] = display_ram[((index - 1)<<3) + i];
		}

		for (i = index; i>1; i--)
		{
			mcuram_to_mcuram_down(display_ram + ((i - 2)<<3));
		}
		for (i = 0; i<6; i++)
		{
			display_ram[i] = temp[i];
		}
		tm1629_load();
		display();
		set_logout_cycle_table(0);
		if (Two_Menu_F6_E3_temp == 1)
		{
				submenuf6_1(Two_Menu_F6_E1_temp, display_ram, display_ram[0], display_ram[5] & 0x0f);
		}
	}
}

void LogoutProcess(void)
{
	unsigned int i;
	unsigned char f5_xunhuan, f4_xiaohao, logout_cycle_table_temp, Two_Menu_F3_E2_temp, Two_Menu_F3_E1_temp;
	unsigned char Two_Menu_F6_E1_temp = 0;
	unsigned char Two_Menu_F6_E2_temp = 0;

	f5_xunhuan = return_Two_Menu_F5_E1();
	f4_xiaohao = return_Two_Menu_F4_E1();
	logout_cycle_table_temp = return_logout_cycle_table();
	Two_Menu_F3_E2_temp = return_Two_Menu_F3_E2();
	Two_Menu_F3_E1_temp = return_Two_Menu_F3_E1();
	Two_Menu_F6_E2_temp = return_Two_Menu_F6_E2();
	/*销号处理函数*/
	if ((f4_xiaohao<f5_xunhuan&& logout_cycle_table_temp>f4_xiaohao && f5_xunhuan && f4_xiaohao) || (f5_xunhuan == 0 && f4_xiaohao && logout_cycle_table_temp>f4_xiaohao))
	{
		for (i = 0; i<(Two_Menu_F3_E2_temp - 1); i++)
		{
			mcuram_to_mcuram_up(display_ram + (i<<3));
		}
		for (i = ((Two_Menu_F3_E2_temp - 1)<<3); i<(Two_Menu_F3_E2_temp<<3); i++)
		{
			display_ram[i] = 0;
		}
		tm1629_load();
		display();
		set_logout_cycle_table(0);
		if (display_ram[0] == 0)
		{
			set_func_index(MENU_STANDBY);
		}
		else if (Two_Menu_F3_E1_temp == 2)
		{
//#ifndef AWAIT_VOICE_PATCH
#ifdef REPLAY_PATCH/*sjz add 2015 11 30*/
//                          repeat_times = Two_Menu_F6_E2_temp;//返回所需要的外部变量
                          if(Two_Menu_F6_E2_temp > 1)  /*repeat mode*/
                         {repeat_times = Two_Menu_F6_E2_temp;//返回所需要的外部变量
                          rep_saved_parameter_when_dec.call_type = display_ram[0];
                          rep_saved_parameter_when_dec.key_value =display_ram[5] & 0x0f;
                          rep_saved_parameter_when_dec.report_mode= Two_Menu_F6_E1_temp;
                          for(i = 0; i < 8;i++)
                          {
                               rep_saved_parameter_when_dec.report_number[i]=*(display_ram+i);
                          }
                         }
                         else   /*single play mode*/
                         {
                             submenuf6_1(Two_Menu_F6_E1_temp, display_ram, display_ram[0], display_ram[5] & 0x0f); 
                         }
/*sjz add end*/
#else
			for (i = 0; i < Two_Menu_F6_E2_temp; i++)
			{
				submenuf6_1(Two_Menu_F6_E1_temp, display_ram, display_ram[0], display_ram[5] & 0x0f);
			}
#endif
//#endif
		}
	}
}





void Logout(void)
{
	unsigned int i;
	unsigned char Two_Menu_F3_E2_temp= 0;
	Two_Menu_F3_E2_temp = return_Two_Menu_F3_E2();
	/*销号处理函数*/
	for (i = 0; i<(Two_Menu_F3_E2_temp - 1); i++)
	{
		mcuram_to_mcuram_up(display_ram + (i<<3));
	}
	for (i = ((Two_Menu_F3_E2_temp - 1) <<3); i<(Two_Menu_F3_E2_temp<<3); i++)
	{
		display_ram[i] = 0;
	}
	decoder_num--;
	tm1629_load();
	display();
	if (display_ram[0] == 0)
	{
		set_func_index(MENU_STANDBY);
	}
}

void CycleUp(void)
{
	unsigned char i, index, Two_Menu_F3_E2_temp;
	unsigned char temp[6] = { 0 };
	index = return_Two_Menu_F3_E2();
	Two_Menu_F3_E2_temp = return_Two_Menu_F3_E2();
	/*循环处理函数*/
	for (i = 0; i<Two_Menu_F3_E2_temp; i++)
	{
		if (display_ram[i<<3] == 0)
		{
			index = i;
			break;
		}
	}
	for (i = 0; i<6; i++)
	{
		temp[i] = display_ram[i];
	}
	for (i = 0; i<index; i++)
	{
		mcuram_to_mcuram_up(display_ram + (i<<3));
	}
	for (i = 0; i<6; i++)
	{
		display_ram[((index - 1)<<3) + i] = temp[i];
	}
	tm1629_load();
	display();
}

void CycleDown(void)
{
	unsigned char i, index, Two_Menu_F3_E2_temp;
	unsigned char temp[6] = { 0 };
	index = return_Two_Menu_F3_E2();
	Two_Menu_F3_E2_temp = return_Two_Menu_F3_E2();
	/*循环处理函数*/
	for (i = 0; i<Two_Menu_F3_E2_temp; i++)
	{
		if (display_ram[i<<3] == 0)
		{
			index = i;
			break;
		}
	}

	for (i = 0; i<6; i++)
	{
		temp[i] = display_ram[((index - 1)<<3) + i];
	}

	for (i = index; i>1; i--)
	{
		mcuram_to_mcuram_down(display_ram + ((i - 2)<<3));
	}
	for (i = 0; i<6; i++)
	{
		display_ram[i] = temp[i];
	}
	tm1629_load();
	display();
}

void Cancel_funtion(unsigned char* id_number, unsigned char* buff)//id_number为解码的temp数组，buff为显示缓存
{
	/*
	id_number  //  序号、接收号*3、ID*3、区号
	buff	//		  序号、类别、区号、接收号
	*/

	unsigned int i, index, Two_Menu_F3_E2_temp;
	unsigned char temp[6] = { 0 };
	index = return_Two_Menu_F3_E2();
	Two_Menu_F3_E2_temp = return_Two_Menu_F3_E2();

	for (i = 0; i<Two_Menu_F3_E2_temp; i++)
	{
		if (*(id_number + 1) == *(buff + (i << 3) + 1) && *(id_number + 2) == *(buff + (i << 3) + 2) && *(id_number + 3) == *(buff + (i << 3) + 3) && *(id_number + 4) == *(buff + (i << 3) + 4))
		{
			decoder_num--;
			index = i;
			break;
		}
	}
	if (index == 0)
	{
		GD5800_stop_music();
//#ifndef AWAIT_VOICE_PATCH
#ifdef REPLAY_PATCH
		repeat_times = 0;
#endif
//#endif
	}
	for (i = index; i<Two_Menu_F3_E2_temp; i++)
	{
		mcuram_to_mcuram_up(buff + (i << 3));
	}
	if (index != Two_Menu_F3_E2_temp)
	{
		for (i = 0; i<6; i++)
		{
			*(buff + ((Two_Menu_F3_E2_temp - 1)<<3) + i) = 0;
		}
	}
	if (*(buff) == 0)
	{
		set_func_index(MENU_STANDBY);
	}
}

/*
if call number is  001. 002, 003,all three number is in call array, now the number 002 is call again, 
then we should move the number 002 to the first position at current array
now the array is changed to 002 001 003
sjz 2015 1221
*/
void Search_funtion(unsigned char* id_number, unsigned char* buff)//id_number为解码的temp数组，buff为显示缓存
{
	/*
	id_number  //  序号、接收号*3、ID*3、区号
	buff	//		  序号、类别、区号、接收号
	*/

	unsigned char i, index, Two_Menu_F3_E2_temp;
	unsigned char temp[6] = { 0 };
	index = return_Two_Menu_F3_E2();
	Two_Menu_F3_E2_temp = return_Two_Menu_F3_E2();

	for (i = 0; i<Two_Menu_F3_E2_temp; i++)
	{
		if (*(id_number + 1) == *(buff + (i << 3) + 1) && *(id_number + 2) == *(buff + (i << 3) + 2) && *(id_number + 3) == *(buff + (i << 3) + 3) && *(id_number + 4) == *(buff + (i << 3) + 4))
		{
			decoder_num--;
			index = i;
			break;
		}
	}

	for (i = index; i<Two_Menu_F3_E2_temp; i++)
	{
		mcuram_to_mcuram_up(buff + (i << 3));
	}
	if (index != Two_Menu_F3_E2_temp)
	{
		for (i = 0; i<6; i++)
		{
			*(buff + ((Two_Menu_F3_E2_temp - 1)<<3) + i) = 0;
		}
	}
}



void fun0(void) //待机显示函数
{
 #ifdef OLD_FUNTION
	unsigned char temp = 0;
	temp = return_Two_Menu_FC_E1();
	decoder_num = 0;
	if (temp == 1)
	{
		P55 = 1;
		tm1629_await();
	}
	else
	{
		Display_time();
	}
#endif
#ifdef PRODUCT_6_32INCH
	tm1629_await();
#endif

	if (return_Two_Menu_Fb_E1() == 1)
	{
		tm1629_display_buff_clear();
	}
	P17 = 0;
	
}

void fun1(void) //一级菜单F0
{
	tm1629_f(0x00);
}

void fun2(void) //一级菜单F1
{
	tm1629_f(0x01);
	if (return_sound_table() == 1 && return_Two_Menu_F6_E6() == 1)
	{
		GD5800_select_chapter(CUNCHUSHEZHI);
		set_sound_table(0);
	}
}

void fun3(void) //一级菜单F2
{
	tm1629_f(0x02);
	if (return_sound_table() == 1 && return_Two_Menu_F6_E6() == 1)
	{
		GD5800_select_chapter_new(SHANGCHUSHEZHI);
		set_sound_table(0);
	}
}

void fun4(void) //一级菜单F3
{
	tm1629_f(0x03);
	if (return_sound_table() == 1 && return_Two_Menu_F6_E6() == 1)
	{
		GD5800_select_chapter_new(XIANSHISHEZHI);
		set_sound_table(0);
	}
}

void fun5(void) //一级菜单F4
{
	tm1629_f(0x04);
	if (return_sound_table() == 1 && return_Two_Menu_F6_E6() == 1)
	{
		GD5800_select_chapter_new(XIAOHAOSHEZHI);
		set_sound_table(0);
	}
}

void fun6(void) //一级菜单F5
{
	tm1629_f(0x05);
	if (return_sound_table() == 1 && return_Two_Menu_F6_E6() == 1)
	{
		GD5800_select_chapter_new(XUNHUANSHEZHI);
		set_sound_table(0);
	}
}

void fun7(void) //一级菜单F6
{
	tm1629_f(0x06);
	if (return_sound_table() == 1 && return_Two_Menu_F6_E6() == 1)
	{
		GD5800_select_chapter_new(YUYINSHEZHI);
		set_sound_table(0);
	}
}

void fun8(void) //一级菜单F7
{
	tm1629_f(0x07);
	if (return_sound_table() == 1 && return_Two_Menu_F6_E6() == 1)
	{
		GD5800_select_chapter_new(JIANPANSHEZHI);
		set_sound_table(0);
	}
}

void fun9(void) //一级菜单F8
{
	tm1629_f(0x08);
	if (return_sound_table() == 1 && return_Two_Menu_F6_E6() == 1)
	{
		GD5800_select_chapter_new(ANJIANSHEZHI);
		set_sound_table(0);
	}
}

void fun10(void) //一级菜单F9
{
	tm1629_f(0x09);
	if(return_sound_table() == 1 && return_Two_Menu_F6_E6() == 1)
	{
		GD5800_select_chapter_new(HUIFUSHEZHI);
		set_sound_table(0);
	}
}

void fun11(void) //一级菜单FA
{
	tm1629_f(0x0a);
}

void fun12(void) //一级菜单Fb
{
	tm1629_f(0x0b);
}

void fun13(void) //一级菜单FC
{
	tm1629_f(0x0c);
}

void fun14(void) //一级菜单Fd
{
	tm1629_f(0x0d);
	set_fd_table(0);
	P55 = 1;
}

void fun15(void) //设置年份
{
		if (return_await_number_table() == 1)
		{
			Display_time();
			set_await_number_table(2);
		}
		if (return_await_number_table() == 3)
		{
			if (return_Two_Menu_F6_E7() == 0)
			{
				buf_display[0][7] = 0;
				buf_display[0][6] = 0;
				buf_display[0][5] = 0;
				buf_display[0][4] = 0;
			}
			else
			{
				buf_display[1][3] = 0;
				buf_display[1][2] = 0;
				buf_display[1][1] = 0;
				buf_display[1][0] = 0;
			}
			display();
			set_await_number_table(0);
		}
		Ds1302_Write_Time();
}

void fun16(void) //设置月份
{
	if (return_await_number_table() == 1)
	{
		Display_time();
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		if (return_Two_Menu_F6_E7() == 0)
		{
			buf_display[1][1] = 0;
			buf_display[1][0] = 0;
		}
		else
		{
			buf_display[0][7] = 0;
			buf_display[0][6] = 0;
		}
		display();
		set_await_number_table(0);
	}
	Ds1302_Write_Time();
}

void fun17(void) //设置日期
{
	if (return_await_number_table() == 1)
	{
		Display_time();
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		if (return_Two_Menu_F6_E7() == 0)
		{
			buf_display[1][3] = 0;
			buf_display[1][2] = 0;
		}
		else
		{
			buf_display[0][5] = 0;
			buf_display[0][4] = 0;
		}
		display();
		set_await_number_table(0);
	}
	Ds1302_Write_Time();
}

void fun18(void) //设置星期
{
	if (return_await_number_table() == 1)
	{
		Display_time();
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[1][4] = 0;
		display();
		set_await_number_table(0);
	}
	Ds1302_Write_Time();
}

void fun19(void) //设置小时
{
	if (return_await_number_table() == 1)
	{
		Display_time();
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
		buf_display[0][3] = 0;
		buf_display[0][2] = 0;
		display();
		set_await_number_table(0);
	}
	Ds1302_Write_Time();
}

void fun20(void) //设置分钟
{
	unsigned char temp;
	temp = return_Two_Menu_FC_E1();
	if (temp == 1)//如果为----  则不让进入F0
	{
		set_func_index(ONE_MENU_F0);
	}
	else
	{
		if (return_await_number_table() == 1)
		{
			Display_time();
			set_await_number_table(2);
		}
		if (return_await_number_table() == 3)
		{
			buf_display[0][1] = 0;
			buf_display[0][0] = 0;
			display();
			set_await_number_table(0);
		}
		Ds1302_Write_Time();
	}
}

void fun21(void) //二级菜单F1-E1
{
	tm1629_E(0x01);
#ifdef OLD_FUNTION
	Two_Menu_F1_E1[0] = 0;
	Two_Menu_F1_E1[1] = 0;
	Two_Menu_F1_E1[2] = 0;
	Two_Menu_F1_E1[3] = 1;
#endif
#ifdef PRODUCT_6_32INCH
	Two_Menu_F1_E1[0] = 0;
	Two_Menu_F1_E1[1] = 0;
	Two_Menu_F1_E1[2] = 0;
	Two_Menu_F1_E1[3] = 1;
#endif
}

void fun22(void) //二级菜单F1-E2
{
	tm1629_E(0x02);
	Two_Menu_F1_E2[0] = 0;
	Two_Menu_F1_E2[1] = 0;
	Two_Menu_F1_E2[2] = 0;
	Two_Menu_F1_E2[3] = 1;
}

void fun23(void) //二级菜单F1-E3
{
	tm1629_E(0x03);
	Two_Menu_F1_E3[0] = 0;
	Two_Menu_F1_E3[1] = 0;
	Two_Menu_F1_E3[2] = 0;
	Two_Menu_F1_E3[3] = 1;
}

void fun24(void) //二级菜单F1-E4
{
	tm1629_E(0x04);
	Two_Menu_F1_E4[0] = 0;
	Two_Menu_F1_E4[1] = 0;
	Two_Menu_F1_E4[2] = 0;
	Two_Menu_F1_E4[3] = 1;
}

void fun25(void) //二级菜单F2-E1
{
	tm1629_E(0x01);
	Two_Menu_F2_E1[0] = 0;
	Two_Menu_F2_E1[1] = 0;
	Two_Menu_F2_E1[2] = 0;
	Two_Menu_F2_E1[3] = 1;
}

void fun26(void) //二级菜单F2-E2
{
	tm1629_E(0x02);
	Two_Menu_F2_E2[0] = 0;
	Two_Menu_F2_E2[1] = 0;
	Two_Menu_F2_E2[2] = 0;
	Two_Menu_F2_E2[3] = 1;
}

void fun27(void) //二级菜单F2-E3
{
	tm1629_E(0x03);
	Two_Menu_F2_E3[0] = 0;
	Two_Menu_F2_E3[1] = 0;
	Two_Menu_F2_E3[2] = 0;
	Two_Menu_F2_E3[3] = 1;
}

void fun28(void) //二级菜单F2-E4
{
	tm1629_E(0x04);
	Two_Menu_F2_E4[0] = 0;
	Two_Menu_F2_E4[1] = 0;
	Two_Menu_F2_E4[2] = 0;
	Two_Menu_F2_E4[3] = 1;
}

void fun29(void) //二级菜单F3-E1
{
	tm1629_E(0x01);
}

void fun30(void) //二级菜单F3-E2
{
	tm1629_E(0x02);
}

void fun31(void) //二级菜单F4
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F4_E1();
	Show_two_number(temp);
}

void fun32(void) //二级菜单F5
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F5_E1();
	Show_two_number(temp);
}

void fun33(void) //二级菜单F6-E1
{
	tm1629_E(0x01);
}

void fun34(void) //二级菜单F6-E2
{
	tm1629_E(0x02);
}

void fun35(void) //二级菜单F6-E3
{
	tm1629_E(0x03);
}

void fun36(void) //二级菜单F6-E4
{
	tm1629_E(0x04);
}

void fun37(void) //二级菜单F6-E5
{
	tm1629_E(0x05);
}

void fun38(void) //二级菜单F7-E1
{
	tm1629_E(0x01);
}

void fun39(void) //二级菜单F7-E2
{
	tm1629_E(0x02);
}

void fun40(void) //二级菜单F7-E3
{
	tm1629_E(0x03);
}

void fun41(void) //二级菜单F7-E4
{
	tm1629_E(0x04);
}

void fun42(void) //二级菜单F8-E1
{
	tm1629_E(0x01);
	Two_Menu_F8_E1_save = return_Two_Menu_F8_E1();
}

void fun43(void) //二级菜单F8-E2
{
	tm1629_E(0x02);
}

void fun44(void) //二级菜单F9-E1
{
	tm1629_E(0x01);
}

void fun45(void) //二级菜单F9-E2
{
	tm1629_E(0x02);
}

void fun46(void) //二级菜单FA
{
	unsigned int measure_sync_saved_temp = 0;
	unsigned int show_temp = 0;

		measure_sync_saved_temp = return_RF_trans_count();
		show_temp = 10000 / measure_sync_saved_temp;
		Show_three_number(show_temp);

//		uart_printf("measure_sync_saved_temp为 %d ! \n\r", measure_sync_saved_temp);
//		uart_printf("show_temp为 %d ! \n\r", show_temp);

}


void fun47(void) //二级菜单Fb
{
	unsigned char temp = 0;
	temp = return_Two_Menu_Fb_E1();
	Show_one_number(temp);
}

void fun48(void) //二级菜单FC
{
	unsigned char temp = 0;
	temp = return_Two_Menu_FC_E1();
	Show_one_number(temp);
}

void fun49(void) //二级菜单Fd
{
	unsigned char temp;
	temp = return_fd_table();
	if (temp <= 20)
	{
		if (temp < 4)
		{
			display_light(0xff);
		}
		else if (temp <= 18)
		{
			display_light(INIT_CODE[temp / 2 - 2]);
		}
		else
		{
			display_light(0xff);
		}
		if (P24 == 1)
		{
			GD5800_select_chapter(YINGYUEYI);
		}
	}
	if (temp >20)
	{
		tm1629_clear();
		display();
	}
	P55 = 0;
}

void fun50(void) //F1_E1 千位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E1);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][3] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][0] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun51(void) //F1_E1 百位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E1);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][2] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][3] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun52(void)  //F1_E1 十位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E1);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][1] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][2] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun53(void) //F1_E1 个位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E1);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][0] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][1] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun54(void) //F1_E2 千位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E2);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][3] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][0] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun55(void) //F1_E2 百位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E2);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][2] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][3] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun56(void) //F1_E2 十位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E2);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][1] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][2] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun57(void) //F1_E2 个位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E2);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][0] = 0;
#endif

#ifdef PRODUCT_6_32INCH
		buf_display[0][1] = 0;
#endif	
		display();
		set_await_number_table(0);
	}
}

void fun58(void) //F1_E3 千位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E3);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][3] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][0] = 0;
#endif		
		display();
		set_await_number_table(0);
	}
}

void fun59(void) //F1_E3 百位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E3);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][2] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][3] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun60(void) //F1_E3 十位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E3);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][1] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][2] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun61(void) //F1_E3 个位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E3);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][0] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][1] = 0;
#endif		
		display();
		set_await_number_table(0);
	}
}

void fun62(void) //F1_E4 千位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E4);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][3] = 0;
#endif

#ifdef PRODUCT_6_32INCH
		buf_display[0][0] = 0;
#endif	
	display();
		set_await_number_table(0);
	}
}

void fun63(void) //F1_E4 百位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E4);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][2] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][3] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun64(void) //F1_E4 十位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E4);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][1] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][2] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun65(void) //F1_E4 个位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F1_E4);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][0] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][1] = 0;
#endif	
	display();
		set_await_number_table(0);
	}
}

void fun66(void) //F2_E1 千位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E1);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][3] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][0] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun67(void) //F2_E1 百位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E1);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][2] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][3] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun68(void) //F2_E1 十位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E1);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][1] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][2] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun69(void) //F2_E1 个位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E1);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][0] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][1] = 0;
#endif		
display();
		set_await_number_table(0);
	}
}

void fun70(void) //F2_E2 千位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E2);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][3] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][0] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun71(void) //F2_E2 百位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E2);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][2] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][3] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun72(void) //F2_E2 十位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E2);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][1] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][2] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun73(void) //F2_E2 个位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E2);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][0] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][1] = 0;
#endif	
	display();
		set_await_number_table(0);
	}
}

void fun74(void) //F2_E3 千位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E3);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][3] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][0] = 0;
#endif	
		display();
		set_await_number_table(0);
	}
}

void fun75(void) //F2_E3 百位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E3);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][2] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][3] = 0;
#endif	
	display();
		set_await_number_table(0);
	}
}

void fun76(void) //F2_E3 十位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E3);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][1] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][2] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun77(void) //F2_E3 个位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E3);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][0] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][1] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun78(void) //F2_E4 千位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E4);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][3] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][0] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun79(void) //F2_E4 百位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E4);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][2] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][3] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun80(void) //F2_E4 十位闪烁
{
	if (return_await_number_table() == 1)
	{

		Show_four_number(Two_Menu_F2_E4);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][1] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][2] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun81(void) //F2_E4 个位闪烁
{
	if (return_await_number_table() == 1)
	{
		Show_four_number(Two_Menu_F2_E4);
		set_await_number_table(2);
	}
	if (return_await_number_table() == 3)
	{
#ifdef OLD_FUNTION
		buf_display[0][0] = 0;
#endif
#ifdef PRODUCT_6_32INCH
		buf_display[0][1] = 0;
#endif
		display();
		set_await_number_table(0);
	}
}

void fun82(void) //显示即时显示 还是循环显示
{
	unsigned char temp=0;
	temp = return_Two_Menu_F3_E1();
	Show_one_number(temp);
}

void fun83(void) //队列存储个数
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F3_E2();
	Show_two_number(temp);
}

void fun84(void) //F6_E1语音选择
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F6_E1();
	Show_one_number(temp);
}

void fun85(void) //F6_E2语音报读次数
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F6_E2();
	Show_one_number(temp);
}

void fun86(void) //F6_E3循环时候是否报读
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F6_E3();
	Show_one_number(temp);
}

void fun87(void) //F6_E4音量大小调整
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F6_E4();
	GD5800_sound_volume(temp * 3);
	Show_one_number(temp);
}

void fun88(void) //F6_E5屏幕亮度调整
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F6_E5();
	Show_one_number(temp);
}

void fun89(void) //F7_E1 999*9
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F7_E1();
	if (temp < 10)
	{
		Show_one_number(temp);
	}
	else
	{
		Show_off();
	}
}

void fun90(void) //F7_E2 9999*9
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F7_E2();
	Show_one_number(temp);
}

void fun91(void) //F7_E3 999*99
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F7_E3();
	Show_one_number(temp);
}

void fun92(void) //F7_E4 9999*99
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F7_E4();
	Show_one_number(temp);
}

void fun93(void) //F8_E1单键位跟多键位切换
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F8_E1();
	Show_one_number(temp);
	if (temp != Two_Menu_F8_E1_save)
	{
		Delete_all_data();
		Two_Menu_F8_E1_save = temp;
		if (return_sound_table() == 1)
		{
			GD5800_select_chapter(CHENGGONG);

			set_sound_table(0);
		}
	}
}

void fun94(void) //F8_E2键位设置
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F8_E2();
	Show_two_number(temp);
	if (return_sound_table() == 1 )
	{
		GD5800_select_chapter_new(temp + QUXIAO);
	}
	if (return_Two_menu_set_success() == 1)
	{
		GD5800_select_chapter_new(CHENGGONG);
		set_Two_menu_set_success(0);
	}
}

void fun95(void) //解码菜单
{
	unsigned char Two_Menu_F6_E1_temp = 0;
	unsigned char Two_Menu_F6_E8_temp = 0;
	Two_Menu_F6_E1_temp = return_Two_Menu_F6_E1();
	Two_Menu_F6_E8_temp = return_Two_Menu_F6_E8();
	//tm1629_load();
	//display();

	if (  display_ram[0] == BAOJING_1)
	{
		if (P24 == 1)
		{
			GD5800_select_chapter(BAOJING);
		}	
	}
	P55 = 1;//秒针灭
	if (Two_Menu_F6_E8_temp == 0)
	{
		P17 = 1;
	}
	else
	{
		if (return_await_number_table() == 1)
		{
			P17 = 1;
			set_await_number_table(2);
		}
		if (return_await_number_table() == 3)
		{
			P17 = 0;
			set_await_number_table(0);
		}
	}
	
	
}

void fun96(void) //二级菜单F6-E6
{
	tm1629_E(0x06);
}

void fun97(void) //二级菜单F6-E6
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F6_E6();
	Show_one_number(temp);
}

void fun98(void) //二级菜单F6-E7
{
	tm1629_E(0x07);
}

void fun99(void) //二级菜单F6-E7
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F6_E7();
	Show_one_number(temp);
}

void fun100(void) //二级菜单F6-E8
{
	tm1629_E(0x08);
}

void fun101(void) //二级菜单F6-E8
{
	unsigned char temp = 0;
	temp = return_Two_Menu_F6_E8();
	Show_one_number(temp);
}
#ifdef COPY_TWO_MACHINE
void fun102(void) //FE菜单
{
	tm1629_f(0x0E);
}

void fun103(void) //FE E1菜单
{
	tm1629_E(0x01);
	Two_Menu_FE_E1_addr = 0;
    Two_Menu_FE_status = 0;
	usart1_begin = 0;
}

void fun104(void) //FE E2菜单
{
	tm1629_E(0x02);
	Two_Menu_FE_E2_addr = 0;
	Two_Menu_FE_E2_table = 0;
}

void fun105(void) //FE E1菜单 设置菜单
{
	uint8_t eeprom_buf[8];
	uint8_t i;
	if (Two_Menu_FE_status == 0) //状态0 表示 发送或者重发  状态1 表示等待对方接收  状态2表示对方接收正确 状态3表示接收完成
	{
		if (return_await_number_table() == 1)
		{
			tm1629_clear();
			buf_display[0][1] = 0x80;
			buf_display[0][2] = 0x80;
			display();
			set_await_number_table(2);
		}
		if (return_await_number_table() == 3)
		{
			tm1629_clear();
			display();
			set_await_number_table(0);
		}
		IRcvStr(I2C_ADDRESS, Two_Menu_FE_E1_addr, eeprom_buf, 8);
		//delay10ms();
		Usart1_SendData(0XAA);
		for (i = 0; i < 8; i++)
		{
			Usart1_SendData(eeprom_buf[i]);
		}
		Usart1_SendData(0X55);
		Two_Menu_FE_status = 1;
	}
	else if (Two_Menu_FE_status == 2)
	{
		Two_Menu_FE_E1_addr = Two_Menu_FE_E1_addr + 8;
		if (Two_Menu_FE_E1_addr == (BACK+0x20))
		{
			Two_Menu_FE_status = 3;
		}
		else
		{
			Two_Menu_FE_status = 0;
		}
		
	}
	else if (Two_Menu_FE_status == 3)
	{
		tm1629_clear();
		buf_display[0][0] = 0x80;
		buf_display[0][1] = 0x80;
		buf_display[0][2] = 0x80;
		buf_display[0][3] = 0x80;
		display();
	}
	
}

void fun106(void) //FE E2菜单 设置菜单接收
{
//	unsigned char i;
//	unsigned char eeprom[8];
	if (Two_Menu_FE_E2_table == 1)
	{
		ISendStr(I2C_ADDRESS, Two_Menu_FE_E2_addr, Two_Menu_FE_E2_data + 1, 8);
		//delay10ms();
		Two_Menu_FE_E2_addr = Two_Menu_FE_E2_addr + 8;
		Two_Menu_FE_E2_table = 0;
		Usart1_SendData(0xaa);
#if 0
		for (i = 0; i < 8; i++)
		{
			Usart1_SendData(Two_Menu_FE_E2_data[i+1]);
		}	
		Usart1_SendData(0x55);
		IRcvStr(I2C_ADDRESS, 0x1500, eeprom, 8);
		delay10ms();
		delay10ms();
		for (i = 0; i < 8; i++)
		{
			Usart1_SendData(eeprom[i]);
		}

#endif
	}
	if (Two_Menu_FE_E2_addr == 0) //等待传输
	{
		tm1629_clear();
		buf_display[0][0] = 0x80;
		display();
	}
	else if (Two_Menu_FE_E2_addr == (BACK + 0x20)) //传输完毕
	{
		tm1629_clear();
		buf_display[0][0] = 0x80;
		buf_display[0][1] = 0x80;
		buf_display[0][2] = 0x80;
		buf_display[0][3] = 0x80;
		display();
	}
	else //传输中
	{
		if (return_await_number_table() == 1)
		{
			tm1629_clear();
			buf_display[0][1] = 0x80;
			buf_display[0][2] = 0x80;
			display();
			set_await_number_table(2);
		}
		if (return_await_number_table() == 3)
		{
			tm1629_clear();
			display();
			set_await_number_table(0);
		}
	}
	
}
#endif