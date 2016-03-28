#ifndef __MENU_H_
#define __MENU_H_

#include "global.h"

//以下define为菜单数字定义为英文字母，方便识别

#define MENU_STANDBY 0   //待机
#define ONE_MENU_F0  1   //一级子菜单F0
#define ONE_MENU_F1  2
#define ONE_MENU_F2  3
#define ONE_MENU_F3  4
#define ONE_MENU_F4  5
#define ONE_MENU_F5  6
#define ONE_MENU_F6  7
#define ONE_MENU_F7  8
#define ONE_MENU_F8  9
#define ONE_MENU_F9  10
#define ONE_MENU_FA  11  //一级子菜单FA
#define ONE_MENU_Fb  12  //一级子菜单Fb
#define ONE_MENU_FC  13  //一级子菜单FC
#define ONE_MENU_Fd  14  //一级子菜单Fd

//万年历菜单定义
#define TWO_MENU_F0_YEAR    15  //设置年
#define TWO_MENU_F0_MOUTH   16	//设置月
#define TWO_MENU_F0_DAY     17	//设置日
#define TWO_MENU_F0_WEEK    18	//设置星期
#define TWO_MENU_F0_HOUR    19	//设置小时
#define TWO_MENU_F0_MINUTE  20	//设置分钟

//F1二级菜单
#define TWO_MENU_F1_E1      21  //E1注册呼叫器
#define TWO_MENU_F1_E2      22  //E2注册管理器
#define TWO_MENU_F1_E3      23  //E3注册报警器
#define TWO_MENU_F1_E4      24  //E4注册取消器

//F2二级菜单
#define TWO_MENU_F2_E1      25  //E1删除呼叫器
#define TWO_MENU_F2_E2      26  //E2删除管理器
#define TWO_MENU_F2_E3      27  //E3删除报警器
#define TWO_MENU_F2_E4      28  //E4删除取消器

//F3二级菜单
#define TWO_MENU_F3_E1      29  //E1删除呼叫器
#define TWO_MENU_F3_E2      30  //E2删除管理器

//F4 F5二级菜单
#define TWO_MENU_F4_SET     31  //F4功能设置
#define TWO_MENU_F5_SET     32  //F5功能设置

//F6二级子菜单
#define TWO_MENU_F6_E1      33  //E1语音选择
#define TWO_MENU_F6_E2      34  //E2语音报读次数
#define TWO_MENU_F6_E3      35  //循环时是否报读
#define TWO_MENU_F6_E4		36  //音量大小调整
#define TWO_MENU_F6_E5      37  //亮度调整
#define TWO_MENU_F6_E6      96  //语音导航调整
#define TWO_MENU_F6_E7      98  //中英文切换调整
#define TWO_MENU_F6_E8      100  //中英文切换调整

//F7二级子菜单
#define TWO_MENU_F7_E1      38  //E1 原始规则
#define TWO_MENU_F7_E2      39  //E2 原始规则
#define TWO_MENU_F7_E3      40  //E3 原始规则
#define TWO_MENU_F7_E4      41  //E4 原始规则

//F8二级子菜单
#define TWO_MENU_F8_E1      42   //E1 单键多键切换
#define TWO_MENU_F8_E2      43   //E2键位设置

//F9二级菜单
#define TWO_MENU_F9_E1      44  //E1恢复功能和号码全部出厂
#define TWO_MENU_F9_E2      45  //E2恢复功能出厂

//FA Fb FC二级菜单
#define TWO_MENU_FA_SET     46  //FA功能设置
#define TWO_MENU_Fb_SET     47  //FB功能设置
#define TWO_MENU_FC_SET     48  //FC功能设置
#define TWO_MENU_Fd_SET     49  //Fd功能设置

//F1_E1子菜单设置
#define TWO_MENU_F1_E1_D1   50  //注册呼叫器千位
#define TWO_MENU_F1_E1_D2   51  //注册呼叫器百位
#define TWO_MENU_F1_E1_D3   52  //注册呼叫器十位
#define TWO_MENU_F1_E1_D4   53  //注册呼叫器个位

//F1_E2子菜单设置
#define TWO_MENU_F1_E2_D1   54  //注册管理器千位
#define TWO_MENU_F1_E2_D2   55  //注册管理器百位
#define TWO_MENU_F1_E2_D3   56  //注册管理器十位
#define TWO_MENU_F1_E2_D4   57  //注册管理器个位

//F1_E3子菜单设置
#define TWO_MENU_F1_E3_D1   58  //注册报警器千位
#define TWO_MENU_F1_E3_D2   59  //注册报警器百位
#define TWO_MENU_F1_E3_D3   60  //注册报警器十位
#define TWO_MENU_F1_E3_D4   61  //注册报警器个位

//F1_E4子菜单设置
#define TWO_MENU_F1_E4_D1   62  //注册取消器千位
#define TWO_MENU_F1_E4_D2   63  //注册取消器百位
#define TWO_MENU_F1_E4_D3   64  //注册取消器十位
#define TWO_MENU_F1_E4_D4   65  //注册取消器个位

//F2_E1子菜单设置
#define TWO_MENU_F2_E1_D1   66  //删除呼叫器千位
#define TWO_MENU_F2_E1_D2   67  //删除呼叫器百位
#define TWO_MENU_F2_E1_D3   68  //删除呼叫器十位
#define TWO_MENU_F2_E1_D4   69  //删除呼叫器个位

//F2_E2子菜单设置
#define TWO_MENU_F2_E2_D1   70  //删除管理器千位
#define TWO_MENU_F2_E2_D2   71  //删除管理器百位
#define TWO_MENU_F2_E2_D3   72  //删除管理器十位
#define TWO_MENU_F2_E2_D4   73  //删除管理器个位

//F2_E3子菜单设置
#define TWO_MENU_F2_E3_D1   74  //删除报警器千位
#define TWO_MENU_F2_E3_D2   75  //删除报警器百位
#define TWO_MENU_F2_E3_D3   76  //删除报警器十位
#define TWO_MENU_F2_E3_D4   77  //删除报警器个位

//F2_E4子菜单设置
#define TWO_MENU_F2_E4_D1   78  //删除取消器千位
#define TWO_MENU_F2_E4_D2   79  //删除取消器百位
#define TWO_MENU_F2_E4_D3   80  //删除取消器十位
#define TWO_MENU_F2_E4_D4   81  //删除取消器个位

//F3子菜单设置
#define TWO_MENU_F3_E1_SET  82 //F3_E1设置选择
#define TWO_MENU_F3_E2_SET  83 //F3_E1设置选择

//F6子菜单设置
#define TWO_MENU_F6_E1_SET  84 //F6_E1语音选择
#define TWO_MENU_F6_E2_SET  85 //F6_E2语音报读次数
#define TWO_MENU_F6_E3_SET  86 //F6_E3循环是否报读
#define TWO_MENU_F6_E4_SET  87 //F6_E4音量大小调整
#define TWO_MENU_F6_E5_SET  88 //F6_E5显示屏亮度调整
#define TWO_MENU_F6_E6_SET  97 //F6_E6语音导航调整
#define TWO_MENU_F6_E7_SET  99 //F6_E6语音导航调整
#define TWO_MENU_F6_E8_SET  101 //F6_E6语音导航调整
//F7子菜单设置
#define TWO_MENU_F7_E1_SET  89 //F7_E1 默认规则999*9
#define TWO_MENU_F7_E2_SET  90 //F7_E2 附加规则9999*9
#define TWO_MENU_F7_E3_SET  91 //F7_E3 附加规则999*99
#define TWO_MENU_F7_E4_SET  92 //F7_E4 附加规则9999*99

//F8子菜单设置
#define TWO_MENU_F8_E1_SET  93 //F8_E1 单键盘还是多键盘
#define TWO_MENU_F8_E2_SET  94 //F8_E2 键位更改设置

//解码菜单
#define DECODER_MENU        95 //解码显示菜单

#ifdef COPY_TWO_MACHINE
#define ONE_MENU_FE  102    //有线传输24C64 主菜单
#define TWO_MENU_FE_E1  103 //有线传输24C64 发送菜单
#define TWO_MENU_FE_E2  104 //有线传输24C64 接收菜单
#define TWO_MENU_FE_E1_SET  105 //有线传输24C64 发送菜单设置子菜单
#define TWO_MENU_FE_E2_SET  106 //有线传输24C64 接收菜单设置子菜单
#endif
#define MENU_END                       107
#define MENU_SIZE   MENU_END
//以下为声音定义
#define ZERO      1
#define ONE       2
#define TWO       3
#define THREE     4
#define FOUR      5
#define FIVE      6
#define SIX       7
#define SEVEN     8
#define EIGHT     9
#define NINE      10
#define TEN       11
#define ELEVEN    12
#define TWELVE    13
#define THIRTEEN  14
#define FOURTEEN  15
#define FIFTEEN   16
#define SIXTEEN   17
#define SEVENTEEN 18
#define EIGHTEEN  19
#define NINETEEN  20
#define TWENTY    21
#define THIRTY    22
#define FORTY     23
#define FIFTY     24
#define SIXTY     25
#define SEVENTY   26
#define EIGHTY    27
#define NINETTY   28
#define HUNDRED   29
#define AND       30
#define THOUSAND  31

#define ZIMU_A   33
#define ZIMU_B   34
#define ZIMU_C   35
#define ZIMU_D   36
#define ZIMU_E   37
#define ZIMU_F   38
#define ZIMU_G   39
#define ZIMU_H   40
#define ZIMU_I   41
#define ZIMU_J   42
#define ZIMU_K   43
#define ZIMU_L   44
#define ZIMU_M   45
#define ZIMU_N   46
#define ZIMU_O   47
#define ZIMU_P   48
#define ZIMU_Q   49
#define ZIMU_R   50
#define ZIMU_S   51
#define ZIMU_T   52
#define ZIMU_U   53
#define ZIMU_V   54
#define ZIMU_W   55
#define ZIMU_X   56
#define ZIMU_Y   57
#define ZIMU_Z   58

//#define BAOJING   59
#define YINGYUEYI 60
#define YINGYUEER 61
#define DINGDONG  62
#define QING      63
#define DAO       64
#define GUITAI    65
#define QUXIAO    66
#define HUJIAO    67
#define JIUSHUI   68
#define JIEZHANG  69
#define MAIDANG   70
#define DIANDANG  71
#define JIASHUI   72
#define TIANDIAN  73
#define SHIYING   74
#define FUWU      75
#define PIJIU     76
#define JIU       77
#define CUFANG    78
#define CUSHI     79
#define BATAI     80
#define BANGZHU   81
#define YISHENG   82
#define HUSHI     83
#define ANMO      84
#define ZHILIAO   85
#define QIANTAI   86
#define BAOJIE    87
#define BANGONG   88
#define BAZHEN    89
#define HUANYAO   90
#define SHUYE     91
#define JINJI     92
#define JIASHI    93
#define QINGJIE   94
#define GUANLI    95
#define WEIXIU    96
#define GUZHANG   97
#define ZHUXI     98
#define ZHUREN    99
#define LICAI     100
#define SHOUQUAN  101
#define ANQUAN    102
#define BAOAN     103
#define BAOJING   104
#define HAO       105
#define FANG      106
#define CHUANG    107
#define ZHUO      108
#define LOU       109
#define DENGHOU   110
#define CUNCHUSHEZHI   111
#define SHANGCHUSHEZHI 112  /*删除设置*/
#define XIANSHISHEZHI  113
#define XIAOHAOSHEZHI  114
#define XUNHUANSHEZHI  115
#define YUYINSHEZHI    116
#define JIANPANSHEZHI 117
#define ANJIANSHEZHI   118
#define HUIFUSHEZHI    119
#define DI			   120
#define CHENGGONG      121
#define SHIBAI         122
#define CHUSHIHUA      123

#define QUXIAO_1  51
#define	BAOJING_1 52


#endif