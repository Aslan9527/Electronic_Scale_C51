#include<reg51.h>    //������Ƭ���Ĵ�����ͷ�ļ�
#include<intrins.h>  //����_nop_()���������ͷ�ļ�
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
unsigned char code string[ ]={"Weight:"};  	//��ʾ����
//unsigned char code string3[ ]={"error!"};  	//��ʾ����
unsigned char code digit[ ]={"0123456789"}; //�����ַ�������ʾ����

sbit RS=P2^6;    //�Ĵ���ѡ��λ����RSλ����ΪP2.0����
sbit RW=P2^5;    //��дѡ��λ����RWλ����ΪP2.1����
sbit E=P2^7;     //ʹ���ź�λ����Eλ����ΪP2.2����
sbit BF=P0^7;    //æµ��־λ����BFλ����ΪP0.7����
/*****************************************************
�������ܣ���ʱ1ms
(3j+2)*i=(3��33+2)��10=1010(΢��)��������Ϊ��1����
***************************************************/
void delay1ms()
{
   unsigned char i,j;	
	 for(i=0;i<10;i++)
	  for(j=0;j<33;j++)
	   ;		 
 }
 /*****************************************************
�������ܣ���ʱ���ɺ���
��ڲ�����n
***************************************************/
 void delay(unsigned char n)
 {
   unsigned char i;
	for(i=0;i<n;i++)
	   delay1ms();
 }
/*****************************************************
�������ܣ��ж�Һ��ģ���æµ״̬
����ֵ��result��result=1��æµ;result=0����æ
***************************************************/
 unsigned char BusyTest(void)
  {
   bit result;
	RS=0;       //���ݹ涨��RSΪ�͵�ƽ��RWΪ�ߵ�ƽʱ�����Զ�״̬
    RW=1;
    E=1;        //E=1���������д
    _nop_();   //�ղ���
    _nop_();
    _nop_(); 
    _nop_();   //�ղ����ĸ��������ڣ���Ӳ����Ӧʱ��	
   result=BF;  //��æµ��־��ƽ����result
	E=0;
    return result;
  }
/*****************************************************
�������ܣ���ģʽ����ָ�����ʾ��ַд��Һ��ģ��
��ڲ�����dictate
***************************************************/
void WriteInstruction (unsigned char dictate)
{   
    while(BusyTest()==1); //���æ�͵ȴ�
	 RS=0;                  //���ݹ涨��RS��R/WͬʱΪ�͵�ƽʱ������д��ָ��
	 RW=0;   
	 E=0;                   //E�õ͵�ƽ(���ݱ�8-6��дָ��ʱ��EΪ�����壬
                             // ������E��0��1���������䣬����Ӧ����"0"
	 _nop_();
	 _nop_();             //�ղ��������������ڣ���Ӳ����Ӧʱ��
	 P0=dictate;            //����������P0�ڣ���д��ָ����ַ
	 _nop_();
	 _nop_();
	 _nop_();
	 _nop_();               //�ղ����ĸ��������ڣ���Ӳ����Ӧʱ��
	 E=1;                   //E�øߵ�ƽ
	 _nop_();
	 _nop_();
	 _nop_();
	 _nop_();               //�ղ����ĸ��������ڣ���Ӳ����Ӧʱ��
	  E=0;                  //��E�ɸߵ�ƽ����ɵ͵�ƽʱ��Һ��ģ�鿪ʼִ������
 }
/*****************************************************
�������ܣ�ָ���ַ���ʾ��ʵ�ʵ�ַ
��ڲ�����x
***************************************************/
 void WriteAddress(unsigned char x)
 {
     WriteInstruction(x|0x80); //��ʾλ�õ�ȷ�������涨Ϊ"80H+��ַ��x"
 }
/*****************************************************
�������ܣ�������(�ַ��ı�׼ASCII��)д��Һ��ģ��
��ڲ�����y(Ϊ�ַ�����)
***************************************************/
 void WriteData(unsigned char y)
 {
    while(BusyTest()==1);  
	  RS=1;           //RSΪ�ߵ�ƽ��RWΪ�͵�ƽʱ������д������
	  RW=0;
	  E=0;            //E�õ͵�ƽ(���ݱ�8-6��дָ��ʱ��EΪ�����壬
                       // ������E��0��1���������䣬����Ӧ����"0"
	  P0=y;           //����������P0�ڣ���������д��Һ��ģ��
	  _nop_();
	  _nop_();
 	  _nop_();
     _nop_();       //�ղ����ĸ��������ڣ���Ӳ����Ӧʱ��
	  E=1;          //E�øߵ�ƽ
	  _nop_();
	  _nop_();
	  _nop_();
	 _nop_();        //�ղ����ĸ��������ڣ���Ӳ����Ӧʱ��
	 E=0;            //��E�ɸߵ�ƽ����ɵ͵�ƽʱ��Һ��ģ�鿪ʼִ������
 }
/*****************************************************
�������ܣ���LCD����ʾģʽ���г�ʼ������
***************************************************/
void LcdInitiate(void)
{
   delay(15);             //��ʱ15ms���״�дָ��ʱӦ��LCDһ�νϳ��ķ�Ӧʱ��
   WriteInstruction(0x38);  //��ʾģʽ���ã�16��2��ʾ��5��7����8λ���ݽӿ�
	delay(5);   //��ʱ5ms��
	WriteInstruction(0x38);
	delay(5);
	WriteInstruction(0x38);
	delay(5);
	WriteInstruction(0x0c);  //��ʾģʽ���ã���ʾ�����й�꣬�����˸
	delay(5);
	WriteInstruction(0x06);  //��ʾģʽ���ã�������ƣ��ַ�����
	delay(5);
	WriteInstruction(0x01);  //����Ļָ�����ǰ����ʾ�������
	delay(5);
 }

unsigned long HX711_Read(void)	//����128
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
    count=count^0x800000;//��25�������½�����ʱ��ת������
	Delay__hx711_us();
    HX711_SCK=0;
   return(count);	
}

//****************************************************
//��ȡëƤ����
//****************************************************
void Get_Maopi()
{
	Weight_Maopi = HX711_Read();	
} 

//****************************************************
//����
//****************************************************
void Get_Weight()
{
    unsigned char i;
	Weight_Shiwu = HX711_Read();
	Weight_Shiwu = Weight_Shiwu - Weight_Maopi;		//��ȡ����
	if(Weight_Shiwu > 0)			
	{	
	Weight_Shiwu = (unsigned int)((float)Weight_Shiwu/GapValue); 	//����ʵ���ʵ������	
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

void main(void)            //������
 {
    unsigned char i;
	LcdInitiate();         //����LCD��ʼ������ 
	WriteAddress(0x00); 	 //д��ַ���ӵ�1�е�1�п�ʼ��ʾ	
	 i=0;              //���ַ�����ĵ�1��Ԫ�ؿ�ʼ��ʾ
	 while(string[i]!='\0')   //ֻҪû����ʾ���ַ����Ľ�����־'\0'���ͼ���
	 {
		 WriteData(string[i]); //����i���ַ�����Ԫ��д��LCD
		 i++;                 //ָ����һ������Ԫ��
	 }
	 	WriteAddress(0x4D);     //д��ַ����M��ʾ�ڵ�2�е�16��
	    WriteData('g');	

	Get_Maopi();				//��ëƤ����

		while(1)
		{
		  EA=0;
		  Get_Weight();
		  EA=1;
		}		       
}