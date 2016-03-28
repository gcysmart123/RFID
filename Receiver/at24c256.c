#include "at24c256.h"
#include "key.h"
#include "usart.h"
#include "menu.h"
#include "gd5800.h"

bit ack;	              //应答标志位
Env_t	EEPROM;
void DelayUs2x(unsigned char t)
{
	while (--t);
}
/*------------------------------------------------
mS延时函数，含有输入参数 unsigned char t，无返回值
unsigned char 是定义无符号字符变量，其值的范围是
0~255 这里使用晶振12M，精确延时请使用汇编
------------------------------------------------*/
void DelayMs(unsigned char t)
{
	while (t--)
	{
		//大致延时1mS
		DelayUs2x(245);
		DelayUs2x(245);
	}
}


/*------------------------------------------------
启动总线
------------------------------------------------*/
void Start_I2c()
{
	SDA = 1;   //发送起始条件的数据信号
	_Nop();
	SCL = 1;
	_Nop();    //起始条件建立时间大于4.7us,延时
	_Nop();
	_Nop();
	_Nop();
	_Nop();
	SDA = 0;     //发送起始信号
	_Nop();    //起始条件锁定时间大于4μ
	_Nop();
	_Nop();
	_Nop();
	_Nop();
	SCL = 0;    //钳住I2C总线，准备发送或接收数据
	_Nop();
	_Nop();
}
/*------------------------------------------------
结束总线
------------------------------------------------*/
void Stop_I2c()
{
	SDA = 0;    //发送结束条件的数据信号
	_Nop();   //发送结束条件的时钟信号
	SCL = 1;    //结束条件建立时间大于4μ
	_Nop();
	_Nop();
	_Nop();
	_Nop();
	_Nop();
	SDA = 1;    //发送I2C总线结束信号
	_Nop();
	_Nop();
	_Nop();
	_Nop();
}

/*----------------------------------------------------------------
字节数据传送函数
函数原型: void  SendByte(unsigned char c);
功能:  将数据c发送出去,可以是地址,也可以是数据,发完后等待应答,并对
此状态位进行操作.(不应答或非应答都使ack=0 假)
发送数据正常，ack=1; ack=0表示被控器无应答或损坏。
------------------------------------------------------------------*/
void  SendByte(unsigned char c)
{
	unsigned char BitCnt;
	for (BitCnt = 0; BitCnt<8; BitCnt++)  //要传送的数据长度为8位
	{
		if ((c << BitCnt) & 0x80)
			SDA = 1;   //判断发送位
		else
			SDA = 0;
		_Nop();
		SCL = 1;               //置时钟线为高，通知被控器开始接收数据位
		_Nop();
		_Nop();             //保证时钟高电平周期大于4μ
		_Nop();
		_Nop();
		_Nop();
		SCL = 0;
	}
	_Nop();
	_Nop();
	SDA = 1;               //8位发送完后释放数据线，准备接收应答位
	_Nop();
	_Nop();
	SCL = 1;
	_Nop();
	_Nop();
	_Nop();
	if (SDA == 1)ack = 0;
	else ack = 1;        //判断是否接收到应答信号
	SCL = 0;
	_Nop();
	_Nop();
}

/*----------------------------------------------------------------
字节数据传送函数
函数原型: unsigned char  RcvByte();
功能:  用来接收从器件传来的数据,并判断总线错误(不发应答信号)，
发完后请用应答函数。
------------------------------------------------------------------*/
unsigned char  RcvByte()
{
	unsigned char retc;
	unsigned char BitCnt;
	retc = 0;
	SDA = 1;             //置数据线为输入方式
	for (BitCnt = 0; BitCnt<8; BitCnt++)
	{
		_Nop();
		SCL = 0;       //置时钟线为低，准备接收数据位
		_Nop();
		_Nop();      //时钟低电平周期大于4.7us
		_Nop();
		_Nop();
		_Nop();
		SCL = 1;       //置时钟线为高使数据线上数据有效
		_Nop();
		_Nop();
		retc = retc << 1;
		if (SDA == 1)retc = retc + 1; //读数据位,接收的数据位放入retc中
		_Nop();
		_Nop();
	}
	SCL = 0;
	_Nop();
	_Nop();
	return(retc);
}


/*----------------------------------------------------------------
应答子函数
原型:  void Ack_I2c(void);

----------------------------------------------------------------*/
void Ack_I2c(void)
{
	SDA = 0;
	_Nop();
	_Nop();
	_Nop();
	SCL = 1;
	_Nop();
	_Nop();              //时钟低电平周期大于4μ
	_Nop();
	_Nop();
	_Nop();
	SCL = 0;               //清时钟线，钳住I2C总线以便继续接收
	_Nop();
	_Nop();
}

/*----------------------------------------------------------------
非应答子函数
原型:  void NoAck_I2c(void);

----------------------------------------------------------------*/
void NoAck_I2c(void)
{
	SDA = 1;
	_Nop();
	_Nop();
	_Nop();
	SCL = 1;
	_Nop();
	_Nop();              //时钟低电平周期大于4μ
	_Nop();
	_Nop();
	_Nop();
	SCL = 0;                //清时钟线，钳住I2C总线以便继续接收
	_Nop();
	_Nop();
}

/*----------------------------------------------------------------
向有子地址器件发送多字节数据函数
函数原型: bit  ISendStr(unsigned char sla,unsigned char suba,ucahr *s,unsigned char no);
功能:     从启动总线到发送地址，子地址,数据，结束总线的全过程,从器件
地址sla，子地址suba，发送内容是s指向的内容，发送no个字节。
如果返回1表示操作成功，否则操作有误。
注意：    使用前必须已结束总线。
----------------------------------------------------------------*/
bit ISendStr(unsigned char sla, unsigned int suba, unsigned char *s, unsigned char no)
{
	unsigned char i;
	Start_I2c();               //启动总线
	SendByte(sla);             //发送器件地址
	if (ack == 0)return(0);
	SendByte(suba >> 8);            //发送器件子地址
	if (ack == 0)return(0);
	SendByte(suba);
	if (ack == 0)return(0);
	for (i = 0; i<no; i++)
	{
		SendByte(*s);            //发送数据
		if (ack == 0)return(0);
		s++;
	}
	Stop_I2c();                  //结束总线
	return(1);
}


/*----------------------------------------------------------------
向有子地址器件读取多字节数据函数
函数原型: bit  ISendStr(unsigned char sla,unsigned char suba,ucahr *s,unsigned char no);
功能:     从启动总线到发送地址，子地址,读数据，结束总线的全过程,从器件
地址sla，子地址suba，读出的内容放入s指向的存储区，读no个字节。
如果返回1表示操作成功，否则操作有误。
注意：    使用前必须已结束总线。
----------------------------------------------------------------*/
bit IRcvStr(unsigned char sla, unsigned int suba, unsigned char *s, unsigned char no)
{
	unsigned char i;
	Start_I2c();               //启动总线
	SendByte(sla);             //发送器件地址
	if (ack == 0)return(0);
	SendByte(suba >> 8);            //发送器件子地址
	if (ack == 0)return(0);
	SendByte(suba);            //发送器件子地址
	if (ack == 0)return(0);
	Start_I2c();
	SendByte(sla + 1);
	if (ack == 0)return(0);
	for (i = 0; i<no - 1; i++)
	{
		*s = RcvByte();              //发送数据
		Ack_I2c();                //发送就答位 
		s++;
	}
	*s = RcvByte();
	NoAck_I2c();                 //发送非应位
	Stop_I2c();                    //结束总线
	return(1);
}

bit register_call_function(RF_def *pRF)
{
	uint16_t addr;
	uint16_t i;
	RF_def RFtmp;
	uint16_t tmp = 0;
	addr = CALL_DATA_START;

	for (i = 0; i < CALL_NUMBER; i++)
	{
		IRcvStr(I2C_ADDRESS, addr, (uint8_t *)&RFtmp, sizeof(RF_def));

		if (!(RFtmp.rf & 0xff000000))
		{

			if (return_Two_Menu_F8_E1() == 2)      //多键模式低位为0
			{
				RFtmp.rf &= 0x00fffff0;
				pRF->rf &= 0x00fffff0;
			}

			if (RFtmp.rf == pRF->rf)    //有相等RF在里面，重新覆盖
			{
				memcpy(Two_Menu_F1_E1, RFtmp.region, 4);
				Two_Menu_F1_E1[4] = 0;
				GD5800_select_chapter_new(SHIBAI);
				return 1;
			}
		}
		else
		{
			if (!tmp) tmp = addr;
		}
		addr += sizeof(RF_def);
	}
	ISendStr(I2C_ADDRESS, tmp, (uint8_t *)pRF, sizeof(RF_def));
	if (tmp != 0)
	{
		GD5800_select_chapter_new(CHENGGONG);
	}
	else
	{
		GD5800_select_chapter_new(SHIBAI);
		return 1;
	}
	//EEP_WriteBytes(tmp, (uint8_t *)pRF, sizeof(RF_def));
	return 0;
}


bit register_host_function(RF_def *pRF)
{
	uint16_t addr;
	uint16_t i;
	RF_def RFtmp;
	uint16_t tmp = 0;
	addr = HOST_DATA_START;

	for (i = 0; i < HOST_NUMBER; i++)
	{
		IRcvStr(I2C_ADDRESS, addr, (uint8_t *)&RFtmp, sizeof(RF_def));
		//EEP_ReadBytes(addr, (uint8_t *)&RFtmp, sizeof(RF_def));

		if (!(RFtmp.rf & 0xff000000))
		{

			if (return_Two_Menu_F8_E1() == 2)      //多键模式低位为0
			{
				RFtmp.rf &= 0x00fffff0;
				pRF->rf &= 0x00fffff0;
			}

			if (RFtmp.rf == pRF->rf)    //有相等RF在里面，重新覆盖
			{
				memcpy(Two_Menu_F1_E2, RFtmp.region, 4);
				Two_Menu_F1_E2[4] = 0;
				GD5800_select_chapter(SHIBAI);
				return 1;
			}
		}
		else
		{
			if (!tmp) tmp = addr;
		}
		addr += sizeof(RF_def);
	}
	ISendStr(I2C_ADDRESS, tmp, (uint8_t *)pRF, sizeof(RF_def));
	//EEP_WriteBytes(tmp, (uint8_t *)pRF, sizeof(RF_def));
	if (tmp != 0)
	{
		GD5800_select_chapter_new(CHENGGONG);
	}
	else
	{
		GD5800_select_chapter_new(SHIBAI);
		return 1;
	}
	return 0;
}

bit register_alarm_function(RF_def *pRF)
{
	uint16_t addr;
	uint16_t i;
	RF_def RFtmp;
	uint16_t tmp = 0;
	addr = ALARM_DATA_START;

	for (i = 0; i < ALARM_NUMBER; i++)
	{
		IRcvStr(I2C_ADDRESS, addr, (uint8_t *)&RFtmp, sizeof(RF_def));
		//EEP_ReadBytes(addr, (uint8_t *)&RFtmp, sizeof(RF_def));

		if (!(RFtmp.rf & 0xff000000))
		{

			if (return_Two_Menu_F8_E1() == 2)      //多键模式低位为0
			{
				RFtmp.rf &= 0x00fffff0;
				pRF->rf &= 0x00fffff0;
			}

			if (RFtmp.rf == pRF->rf)    //有相等RF在里面，重新覆盖
			{
				memcpy(Two_Menu_F1_E3, RFtmp.region, 4);
				Two_Menu_F1_E3[4] = 0;
				GD5800_select_chapter(SHIBAI);
				return 1;
			}
		}
		else
		{
			if (!tmp) tmp = addr;
		}
		addr += sizeof(RF_def);
	}
	ISendStr(I2C_ADDRESS, tmp, (uint8_t *)pRF, sizeof(RF_def));
	//EEP_WriteBytes(tmp, (uint8_t *)pRF, sizeof(RF_def));
	if (tmp != 0)
	{
		GD5800_select_chapter_new(CHENGGONG);
	}
	else
	{
		GD5800_select_chapter_new(SHIBAI);
		return 1;
	}
	return 0;
}

bit register_cancel_function(RF_def *pRF)
{
	uint16_t addr;
	uint16_t i;
	RF_def RFtmp;
	uint16_t tmp = 0;
	addr = CANCEL_DATA_START;

	for (i = 0; i < CANCEL_NUMBER; i++)
	{
		IRcvStr(I2C_ADDRESS, addr, (uint8_t *)&RFtmp, sizeof(RF_def));
		//EEP_ReadBytes(addr, (uint8_t *)&RFtmp, sizeof(RF_def));

		if (!(RFtmp.rf & 0xff000000))
		{

			if (return_Two_Menu_F8_E1() == 2)      //多键模式低位为0
			{
				RFtmp.rf &= 0x00fffff0;
				pRF->rf &= 0x00fffff0;
			}

			if (RFtmp.rf == pRF->rf)    //有相等RF在里面，重新覆盖
			{
				memcpy(Two_Menu_F1_E4, RFtmp.region, 4);
				Two_Menu_F1_E4[4] = 0;
				GD5800_select_chapter(SHIBAI);
				return 1;
			}
		}
		else
		{
			if (!tmp) tmp = addr;
		}
		addr += sizeof(RF_def);
	}
	ISendStr(I2C_ADDRESS, tmp, (uint8_t *)pRF, sizeof(RF_def));
	//EEP_WriteBytes(tmp, (uint8_t *)pRF, sizeof(RF_def));
	if (tmp != 0)
	{
		GD5800_select_chapter_new(CHENGGONG);
	}
	else
	{
		GD5800_select_chapter_new(SHIBAI);
		return 1;
	}
	return 0;
}

bit delete_call_function(unsigned char *buf)//buf为组码数组的指针
{
	RF_def RFtmp;
	uint16_t addr = CALL_DATA_START;
	uint16_t i;
	unsigned char dofly[32] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	if (*(volatile uint32_t*)buf == 0x00000000ul)
	{
		for (i = 0; i<128; i++)
		{
			ISendStr(I2C_ADDRESS, addr + (i << 5), dofly, 32);                   //写入24c02
			delay10ms();
		}

	}
	else
	{
		for (i = 0; i < CALL_NUMBER; i++)
		{
			IRcvStr(I2C_ADDRESS, addr, (uint8_t *)&RFtmp, sizeof(RF_def));
			//EEP_ReadBytes(addr, (uint8_t *)&RFtmp, sizeof(RF_def));
			if (!(RFtmp.rf & 0xff000000ul))
			{
				if (*(volatile uint32_t*)buf == *(volatile uint32_t*)(RFtmp.region))
				{
					memset(&RFtmp, 0xff, sizeof(RF_def));
					ISendStr(I2C_ADDRESS, addr, (uint8_t *)&RFtmp, sizeof(RF_def));
					//EEP_WriteBytes(addr, (uint8_t *)&RFtmp, sizeof(RF_def));
					break;
				}
			}
			addr += sizeof(RF_def);
		}
	}
	return 0;
}

bit delete_host_function(unsigned char *buf)//buf为组码数组的指针
{
	RF_def RFtmp;
	uint16_t addr = HOST_DATA_START;
	uint16_t i;
	unsigned char dofly[32] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	if (*(volatile uint32_t*)buf == 0x00000000ul)
	{
		for (i = 0; i<8; i++)
		{
			ISendStr(I2C_ADDRESS, addr+( i << 5), dofly, 32);                   //写入24c02
			delay10ms();
		}

	}
	else
	{
		for (i = 0; i < HOST_NUMBER; i++)
		{
			IRcvStr(I2C_ADDRESS, addr, (uint8_t *)&RFtmp, sizeof(RF_def));
			//EEP_ReadBytes(addr, (uint8_t *)&RFtmp, sizeof(RF_def));
			if (!(RFtmp.rf & 0xff000000ul))
			{
				if (*(volatile uint32_t*)buf == *(volatile uint32_t*)(RFtmp.region))
				{
					memset(&RFtmp, 0xff, sizeof(RF_def));
					ISendStr(I2C_ADDRESS, addr, (uint8_t *)&RFtmp, sizeof(RF_def));
					//EEP_WriteBytes(addr, (uint8_t *)&RFtmp, sizeof(RF_def));
					break;
				}
			}
			addr += sizeof(RF_def);
		}
	}
	return 0;
}

bit delete_alarm_function(unsigned char *buf)//buf为组码数组的指针
{
	RF_def RFtmp;
	uint16_t addr = ALARM_DATA_START;
	uint16_t i;
	unsigned char dofly[32] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	if (*(volatile uint32_t*)buf == 0x00000000ul)
	{
		for (i = 0; i<16; i++)
		{
			ISendStr(I2C_ADDRESS, addr + (i << 5), dofly, 32);                   //写入24c02
			delay10ms();
		}

	}
	else
	{
		for (i = 0; i < ALARM_NUMBER; i++)
		{
			IRcvStr(I2C_ADDRESS, addr, (uint8_t *)&RFtmp, sizeof(RF_def));
			//	EEP_ReadBytes(addr, (uint8_t *)&RFtmp, sizeof(RF_def));
			if (!(RFtmp.rf & 0xff000000ul))
			{
				if (*(volatile uint32_t*)buf == *(volatile uint32_t*)(RFtmp.region))
				{
					memset(&RFtmp, 0xff, sizeof(RF_def));
					ISendStr(I2C_ADDRESS, addr, (uint8_t *)&RFtmp, sizeof(RF_def));
					//EEP_WriteBytes(addr, (uint8_t *)&RFtmp, sizeof(RF_def));
					break;
				}
			}
			addr += sizeof(RF_def);
		}
	}
	return 0;
}

bit delete_cancel_function(unsigned char *buf)//buf为组码数组的指针
{
	RF_def RFtmp;
	uint16_t addr = CANCEL_DATA_START;
	uint16_t i;
	unsigned char dofly[32] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	if (*(volatile uint32_t*)buf == 0x00000000ul)
	{
		for (i = 0; i<16; i++)
		{
			ISendStr(I2C_ADDRESS, addr + (i << 5), dofly, 32);                   //写入24c02
			delay10ms();
		}

	}
	else
	{
		for (i = 0; i < CANCEL_NUMBER; i++)
		{
			IRcvStr(I2C_ADDRESS, addr, (uint8_t *)&RFtmp, sizeof(RF_def));
			//EEP_ReadBytes(addr, (uint8_t *)&RFtmp, sizeof(RF_def));
			if (!(RFtmp.rf & 0xff000000ul))
			{
				if (*(volatile uint32_t*)buf == *(volatile uint32_t*)(RFtmp.region))
				{
					memset(&RFtmp, 0xff, sizeof(RF_def));
					ISendStr(I2C_ADDRESS, addr, (uint8_t *)&RFtmp, sizeof(RF_def));
					//EEP_WriteBytes(addr, (uint8_t *)&RFtmp, sizeof(RF_def));
					break;
				}
			}
			addr += sizeof(RF_def);
		}
	}
	return 0;
}

void Delete_all_data(void)
{
//	uint8_t i,j;
//	int8_t buf[32] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
//	uint16_t addr = 0;
//	int8_t buf1[32];
////	memset(buf, 0xff, sizeof(buf));
//	addr = 0;
//	for (i = 0; i<168; i++)
//	{	
//		addr = i << 5;
//		ISendStr(I2C_ADDRESS, addr, buf, 32);
//		delay10ms();
//		//EEP_WriteBytes(addr, buf, sizeof(buf));
//		//addr += 32;
//	}
//	uart_printf("24c read:");
//	for (i = 0; i<168; i++)
//	{
//		IRcvStr(I2C_ADDRESS, addr, buf, 32);
//		delay10ms();
//		for (j = 0; j < 32; j++)
//		{
//			uart_printf("%02x ", (unsigned int)buf[j]);
//		}
//		uart_printf("\n\r");
//		//EEP_WriteBytes(addr, buf, sizeof(buf));
//		addr += sizeof(buf);
//	}

	unsigned char dofly[32] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	unsigned int k;
	for (k = 0; k<168; k++)
	{
		ISendStr(I2C_ADDRESS, 0 + (k << 5), dofly, 32);                   //写入24c02
		delay10ms();
	}
}


int8_t Find_RF_EEPROM_Call(RF_def *p, uint32_t dat)
{
	RF_def RFtmp;
	uint16_t addr;
	uint16_t i;
	addr = CALL_DATA_START;

	for (i = 0; i<CALL_NUMBER; i++)
	{
		IRcvStr(I2C_ADDRESS, addr, (uint8_t *)&RFtmp, sizeof(RF_def));
		//EEP_ReadBytes(addr, (uint8_t *)&RFtmp, sizeof(RF_def));

		if (return_Two_Menu_F8_E1() == 2)      //多键模式低位为0
		{
			RFtmp.rf &= 0x00fffff0;
			dat &= 0x00fffff0;
		}

		if (RFtmp.rf == dat)
		{
			memcpy(p, &RFtmp, sizeof(RF_def));
			return 1;
		}
		addr += sizeof(RF_def);
	}
	return 0;
}

int8_t Find_RF_EEPROM_Host(RF_def *p, uint32_t dat)
{
	RF_def RFtmp;
	uint16_t addr;
	uint16_t i;
	addr = HOST_DATA_START;

	for (i = 0; i<HOST_NUMBER; i++)
	{
		IRcvStr(I2C_ADDRESS, addr, (uint8_t *)&RFtmp, sizeof(RF_def));
		//EEP_ReadBytes(addr, (uint8_t *)&RFtmp, sizeof(RF_def));

		if (return_Two_Menu_F8_E1() == 2)      //多键模式低位为0
		{
			RFtmp.rf &= 0x00fffff0;
			dat &= 0x00fffff0;
		}

		if (RFtmp.rf == dat)
		{
			memcpy(p, &RFtmp, sizeof(RF_def));
			return 1;
		}
		addr += sizeof(RF_def);
	}
	return 0;
}

int8_t Find_RF_EEPROM_Alarm(RF_def *p, uint32_t dat)
{
	RF_def RFtmp;
	uint16_t addr;
	uint16_t i;
	addr = ALARM_DATA_START;

	for (i = 0; i<ALARM_NUMBER; i++)
	{
		IRcvStr(I2C_ADDRESS, addr, (uint8_t *)&RFtmp, sizeof(RF_def));
		//EEP_ReadBytes(addr, (uint8_t *)&RFtmp, sizeof(RF_def));

		if (return_Two_Menu_F8_E1() == 2)      //多键模式低位为0
		{
			RFtmp.rf &= 0x00fffff0;
			dat &= 0x00fffff0;
		}

		if (RFtmp.rf == dat)
		{
			memcpy(p, &RFtmp, sizeof(RF_def));
			return 1;
		}
		addr += sizeof(RF_def);
	}
	return 0;
}

int8_t Find_RF_EEPROM_Cancel(RF_def *p, uint32_t dat)
{
	RF_def RFtmp;
	uint16_t addr;
	uint16_t i;
	addr = CANCEL_DATA_START;

	for (i = 0; i<CANCEL_NUMBER; i++)
	{
		IRcvStr(I2C_ADDRESS, addr, (uint8_t *)&RFtmp, sizeof(RF_def));
		//EEP_ReadBytes(addr, (uint8_t *)&RFtmp, sizeof(RF_def));

		if (return_Two_Menu_F8_E1() == 2)      //多键模式低位为0
		{
			RFtmp.rf &= 0x00fffff0;
			dat &= 0x00fffff0;
		}

		if (RFtmp.rf == dat)
		{
			memcpy(p, &RFtmp, sizeof(RF_def));
			return 1;
		}
		addr += sizeof(RF_def);
	}
	return 0;
}

int8_t Find_RF_EEPROM(RF_def *p, uint32_t dat)
{
	if (Find_RF_EEPROM_Cancel(p, dat))
	{
		return 1;
	}
	else if (Find_RF_EEPROM_Alarm(p, dat))
	{
		return 2;
	}
	else if (Find_RF_EEPROM_Call(p, dat))
	{
		return 3;
	}
	else
	{
		return 0;
	}
}