#include<reg51.h>    //包含单片机寄存器的头文件
#include<intrins.h>  //包含_nop_()函数定义的头文件
#define GapValue 400

typedef unsigned int u16;
typedef unsigned char u8;
sbit bz= P1^5;

 unsigned long HX711_Buffer = 0;
 sbit HX711_SCK=P2^1;
 sbit HX711_DOUT=P2^0;
 void Delay__hx711_us()
{
	_nop_();
	_nop_();
}

long Weight_Shiwu = 0;
unsigned long Weight_Maopi = 0;
unsigned int ge=0,shi=0,bai=0,qian=0;
unsigned char code string[ ]={"Weight:"};  	//显示数组
//unsigned char code string3[ ]={"error!"};  	//显示数组
unsigned char code digit[ ]={"0123456789"}; //定义字符数组显示数字

sbit RS=P2^6;    //寄存器选择位，将RS位定义为P2.0引脚
sbit RW=P2^5;    //读写选择位，将RW位定义为P2.1引脚
sbit E=P2^7;     //使能信号位，将E位定义为P2.2引脚
sbit BF=P0^7;    //忙碌标志位，将BF位定义为P0.7引脚
/*****************************************************
函数功能：延时1ms
(3j+2)*i=(3×33+2)×10=1010(微秒)，可以认为是1毫秒
***************************************************/
void delay1ms()
{
   unsigned char i,j;	
	 for(i=0;i<10;i++)
	  for(j=0;j<33;j++)
	   ;		 
 }
 /*****************************************************
函数功能：延时若干毫秒
入口参数：n
***************************************************/
 void delay(unsigned char n)
 {
   unsigned char i;
	for(i=0;i<n;i++)
	   delay1ms();
 }
/*****************************************************
函数功能：判断液晶模块的忙碌状态
返回值：result。result=1，忙碌;result=0，不忙
***************************************************/
 unsigned char BusyTest(void)
  {
   bit result;
	RS=0;       //根据规定，RS为低电平，RW为高电平时，可以读状态
    RW=1;
    E=1;        //E=1，才允许读写
    _nop_();   //空操作
    _nop_();
    _nop_(); 
    _nop_();   //空操作四个机器周期，给硬件反应时间	
   result=BF;  //将忙碌标志电平赋给result
	E=0;
    return result;
  }
/*****************************************************
函数功能：将模式设置指令或显示地址写入液晶模块
入口参数：dictate
***************************************************/
void WriteInstruction (unsigned char dictate)
{   
    while(BusyTest()==1); //如果忙就等待
	 RS=0;                  //根据规定，RS和R/W同时为低电平时，可以写入指令
	 RW=0;   
	 E=0;                   //E置低电平(根据表8-6，写指令时，E为高脉冲，
                             // 就是让E从0到1发生正跳变，所以应先置"0"
	 _nop_();
	 _nop_();             //空操作两个机器周期，给硬件反应时间
	 P0=dictate;            //将数据送入P0口，即写入指令或地址
	 _nop_();
	 _nop_();
	 _nop_();
	 _nop_();               //空操作四个机器周期，给硬件反应时间
	 E=1;                   //E置高电平
	 _nop_();
	 _nop_();
	 _nop_();
	 _nop_();               //空操作四个机器周期，给硬件反应时间
	  E=0;                  //当E由高电平跳变成低电平时，液晶模块开始执行命令
 }
/*****************************************************
函数功能：指定字符显示的实际地址
入口参数：x
***************************************************/
 void WriteAddress(unsigned char x)
 {
     WriteInstruction(x|0x80); //显示位置的确定方法规定为"80H+地址码x"
 }
/*****************************************************
函数功能：将数据(字符的标准ASCII码)写入液晶模块
入口参数：y(为字符常量)
***************************************************/
 void WriteData(unsigned char y)
 {
    while(BusyTest()==1);  
	  RS=1;           //RS为高电平，RW为低电平时，可以写入数据
	  RW=0;
	  E=0;            //E置低电平(根据表8-6，写指令时，E为高脉冲，
                       // 就是让E从0到1发生正跳变，所以应先置"0"
	  P0=y;           //将数据送入P0口，即将数据写入液晶模块
	  _nop_();
	  _nop_();
 	  _nop_();
     _nop_();       //空操作四个机器周期，给硬件反应时间
	  E=1;          //E置高电平
	  _nop_();
	  _nop_();
	  _nop_();
	 _nop_();        //空操作四个机器周期，给硬件反应时间
	 E=0;            //当E由高电平跳变成低电平时，液晶模块开始执行命令
 }
/*****************************************************
函数功能：对LCD的显示模式进行初始化设置
***************************************************/
void LcdInitiate(void)
{
   delay(15);             //延时15ms，首次写指令时应给LCD一段较长的反应时间
   WriteInstruction(0x38);  //显示模式设置：16×2显示，5×7点阵，8位数据接口
	delay(5);   //延时5ms　
	WriteInstruction(0x38);
	delay(5);
	WriteInstruction(0x38);
	delay(5);
	WriteInstruction(0x0c);  //显示模式设置：显示开，有光标，光标闪烁
	delay(5);
	WriteInstruction(0x06);  //显示模式设置：光标右移，字符不移
	delay(5);
	WriteInstruction(0x01);  //清屏幕指令，将以前的显示内容清除
	delay(5);
 }

unsigned long HX711_Read(void)	//增益128
{
	unsigned long count; 
	unsigned char i; 
  	HX711_DOUT=1; 
		Delay__hx711_us();
  	HX711_SCK=0; 
  	count=0; 
  	while(HX711_DOUT); 
  	for(i=0;i<24;i++)				
	{ 
	  	HX711_SCK=1; 
	  	count=count<<1; 
			HX711_SCK=0; 
	  	if(HX711_DOUT)
			count++; 
	} 
 	HX711_SCK=1;
    count=count^0x800000;//第25个脉冲下降沿来时，转换数据
	Delay__hx711_us();
    HX711_SCK=0;
   return(count);	
}

//****************************************************
//获取毛皮重量
//****************************************************
void Get_Maopi()
{
	Weight_Maopi = HX711_Read();	
} 

//****************************************************
//称重
//****************************************************
void Get_Weight()
{
    unsigned char i;
	Weight_Shiwu = HX711_Read();
	Weight_Shiwu = Weight_Shiwu - Weight_Maopi;		//获取净重
	if(Weight_Shiwu > 0)			
	{	
	Weight_Shiwu = (unsigned int)((float)Weight_Shiwu/GapValue); 	//计算实物的实际重量	
   ge=Weight_Shiwu%10; 
   WriteAddress(0x4c);  
   WriteData(digit[ge]); 
   shi=Weight_Shiwu/10%10; 
   WriteAddress(0x4b);  
   WriteData(digit[shi]); 
   bai=Weight_Shiwu/100%10; 
   WriteAddress(0x4a);  
   WriteData(digit[bai]);
   qian=Weight_Shiwu/1000; 
   WriteAddress(0x49);  
   WriteData(digit[qian]); 

	if(Weight_Shiwu > 5000)
	{
	  while(1)
	  {
	    bz=~bz;
		delay(1);
	  }	
	}
	  else
	  {
	    ;
	  }
	}	
	else
	{
      ;
	}	
}

void main(void)            //主函数
 {
    unsigned char i;
	LcdInitiate();         //调用LCD初始化函数 
	WriteAddress(0x00); 	 //写地址，从第1行第1列开始显示	
	 i=0;              //从字符数组的第1个元素开始显示
	 while(string[i]!='\0')   //只要没有显示到字符串的结束标志'\0'，就继续
	 {
		 WriteData(string[i]); //将第i个字符数组元素写入LCD
		 i++;                 //指向下一个数组元素
	 }
	 	WriteAddress(0x4D);     //写地址，将M显示在第2行第16列
	    WriteData('g');	

	Get_Maopi();				//称毛皮重量

		while(1)
		{
		  EA=0;
		  Get_Weight();
		  EA=1;
		}		       
}