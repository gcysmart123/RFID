#include "timer.h"
#include "key.h"
#include "menu.h"
#include "ev1527.h"
#include "global.h"

unsigned char await_number = 0;      //待机时刻计算50MS次数变量
unsigned char await_number_table = 0;//0.5秒标志变量

unsigned char main_press_time_table = 0;//main计算时间变量跟标志位
unsigned char main_press_time = 0;

unsigned char again_and_again_time = 0;// 判断重复解码所需要的计算时间变量

unsigned char logout_cycle_number = 0; //销号 循环 计数变量以及标志位
unsigned char logout_cycle_table = 0;

unsigned char return_standby_time = 0;//一段时间未操作  返回待机界面

unsigned char second_times = 0;

unsigned char fd_table = 0;

unsigned char filter_main = 0;
unsigned char filter_other = 0;

unsigned char second_filter_time = 0;
unsigned char second_filter_table = 0;

void Init_Timer0(void)
{
	TMOD |= 0x01;	  //使用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响	
	TH0 = (65536 - TIMER50MS) >> 8; //重新赋值 5ms
	TL0 = (65536 - TIMER50MS) & 0xff;
	EA = 1;            //总中断打开
	ET0 = 1;           //定时器中断打开
	TR0 = 1;           //定时器开关打开
}

void Init_Timer1(void) 
{
	TMOD |= 0x10;	  //使用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响	
	TH1 = (65536 - 100) >> 8; //重新赋值 100us
	TL1 = (65536 - 100) & 0xff;
	EA = 1;            //总中断打开
	ET1 = 1;           //定时器中断打开
	//TR1=1;           //定时器开关打开
}

void Timer0_isr(void) interrupt 1  //定时器0中断服务程序
{
	unsigned char func_index_temp = 0;
	unsigned char Two_Menu_FC_E1_temp = 0;
	TF0 = 0;
	TH0 = (65536 - TIMER50MS) >> 8;		  //重新赋值 50ms
	TL0 = (65536 - TIMER50MS) & 0xff;
	func_index_temp = return_func_index();
	Two_Menu_FC_E1_temp = return_Two_Menu_FC_E1();
	//if (func_index_temp == MENU_STANDBY || func_index_temp == TWO_MENU_F0_YEAR || func_index_temp == TWO_MENU_F0_MOUTH 
	//	|| func_index_temp == TWO_MENU_F0_DAY || func_index_temp == TWO_MENU_F0_WEEK || func_index_temp == TWO_MENU_F0_HOUR
	//	|| func_index_temp == TWO_MENU_F0_MINUTE || func_index_temp == TWO_MENU_F1_E1_D1 || func_index_temp == TWO_MENU_F1_E1_D2
	//	|| func_index_temp == TWO_MENU_F1_E1_D3 || func_index_temp == TWO_MENU_F1_E1_D4 || func_index_temp == TWO_MENU_F1_E2_D1
	//	|| func_index_temp == TWO_MENU_F1_E2_D2 || func_index_temp == TWO_MENU_F1_E2_D3 || func_index_temp == TWO_MENU_F1_E2_D4	
	//	)
	if (1)
	{
		await_number++;
		if (await_number == 10)
		{
			await_number_table++;
			fd_table++;
			await_number = 0;
			if (filter_main != 0)
			{
				filter_main--;
			}

		}
		if (await_number_table >= 4)//确保程序正确延时
		{
			await_number_table = 0;
		}		
		if (fd_table >= 24)
		{
			fd_table = 0;
		}
		if (filter_other != 0)
		{
			filter_other--;
		}
	}

	if (main_press_time_table == 1) //菜单键按下时间标志
	{
		main_press_time++;			//计算菜单键按下时间长度变量
	}

	if (return_again_and_again_decoder_table() == 1)
	{
		again_and_again_time++;
		if (again_and_again_time >120)
		{
			clear_again_and_again_decoder_table();
			again_and_again_time = 0;
		}
	}

	if (second_filter_table == 1)
	{
		second_filter_time++;
		if (second_filter_time >120)
		{
			second_filter_table = 0;
			second_filter_time = 0;
		}
	}

	if (func_index_temp == DECODER_MENU)
	{
		logout_cycle_number++;
		if (logout_cycle_number == 20)
		{
			logout_cycle_table++;
			logout_cycle_number = 0;
		}

	}
#ifdef COPY_TWO_MACHINE
	if (func_index_temp != DECODER_MENU && func_index_temp != TWO_MENU_Fd_SET && func_index_temp != TWO_MENU_FE_E1_SET&& func_index_temp != TWO_MENU_FE_E2_SET)
#else
	if (func_index_temp != DECODER_MENU && func_index_temp != TWO_MENU_Fd_SET)
#endif
	{
		return_standby_time++;
		if (return_standby_time >200)
		{
			return_standby_time = 0;
			set_func_index(MENU_STANDBY);
		}
	}

	if (func_index_temp == MENU_STANDBY && Two_Menu_FC_E1_temp == 2)
	{
		second_times++;
		if (second_times >= 10)
		{
			P55 = ~P55;
			second_times = 0;
		}
	}

}

void Timer1_isr(void) interrupt 3  //定时器1中断服务程序
{
	unsigned char func_index_temp = 0;
	func_index_temp = return_func_index();
	TF1 = 0;
	TH1 = (65536 - 100) >> 8;		  //重新赋值 100us
	TL1 = (65536 - 100) & 0xff;
	if (func_index_temp == TWO_MENU_FA_SET)
	{
		RF_decode_main_sjz_test();
	}
	else
	{
		RF_decode_main();
	}
	
}

unsigned char return_await_number_table(void)	//返回await_number_table变量的值
{
	unsigned char await_number_table_temp=0;
	await_number_table_temp = await_number_table;
	return await_number_table_temp;
}

void set_await_number_table(unsigned char temp)	//设置await_number_table变量的值
{
	await_number_table = temp;
}

void set_main_press_time_table(unsigned char temp) //设置main_press_time_table变量的值
{
	main_press_time_table = temp;
}

unsigned char return_main_press_time(void)
{
	unsigned char temp = 0;
	temp = main_press_time;
	return temp;
}

void clear_main_press_time(void)
{
	main_press_time = 0;
}

unsigned char return_logout_cycle_table(void)
{
	unsigned char temp = 0;
	temp = logout_cycle_table;
	return temp;
}

void set_logout_cycle_table(unsigned char temp) //设置logout_cycle_table变量的值
{
	logout_cycle_table = temp;
}

void clear_return_standby_time(void)
{
	return_standby_time = 0;
}

void set_main_press_time(unsigned char temp)
{
	main_press_time = temp;
}

unsigned char return_fd_table(void)	//返回fd_table变量的值
{
	unsigned char fd_table_temp = 0;
	fd_table_temp = fd_table;
	return fd_table_temp;
}

void set_fd_table(unsigned char temp) //设置fd_table变量的值
{
	fd_table = temp;
	await_number = 0;
}

unsigned char return_filter_main(void)	//返回filter_main变量的值
{
	unsigned char filter_main_temp = 0;
	filter_main_temp = filter_main;
	return filter_main_temp;
}

void set_filter_main(unsigned char temp) //设置filter_main变量的值
{
	filter_main = temp;
	await_number = 0;
}

unsigned char return_filter_other(void)	//返回filter_main变量的值
{
	unsigned char filter_other_temp = 0;
	filter_other_temp = filter_other;
	return filter_other_temp;
}

void set_filter_other(unsigned char temp) //设置filter_main变量的值
{
	filter_other = temp;
}

void clear_again_and_again_time(void)
{
	again_and_again_time = 0;
}

void clear_second_filter_time(void)
{
	second_filter_time = 0;
}

void set_second_filter_table(unsigned char temp) //设置second_filter_table变量的值
{
	second_filter_table = temp;
}

unsigned char return_second_filter_table(void)	//返回second_filter_table变量的值
{
	unsigned char second_filter_table_temp = 0;
	second_filter_table_temp = second_filter_table;
	return second_filter_table_temp;
}

