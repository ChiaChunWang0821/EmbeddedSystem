/*-----------------------------------------------
  名稱：LCD1602
  論壇：www.doflye.net
  編寫：shifang
  日期：2009.5
  修改：無
  內容：通過標準程序動態顯示字符
  引腳定義如下：1-VSS 2-VDD 3-V0 4-RS 5-R/W 6-E 7-14 DB0-DB7 15-BLA 16-BLK
------------------------------------------------*/
#include<reg52.h> //包含頭文件，一般情況不需要改動，頭文件包含特殊功能寄存器的定義
#include<intrins.h>

sbit RS = P2^4;   //定義端口 
sbit RW = P2^5;
sbit EN = P2^6;

#define RS_CLR RS=0 
#define RS_SET RS=1

#define RW_CLR RW=0 
#define RW_SET RW=1 

#define EN_CLR EN=0
#define EN_SET EN=1

#define DataPort P0
#define Limit    16 // 環形佇列

sbit LED0 = P1^0;
sbit LED1 = P1^1;
sbit LED2 = P1^2;
sbit LED3 = P1^3;
sbit LED4 = P1^4;
sbit LED5 = P1^5;
sbit LED6 = P1^6;
sbit LED7 = P1^7;
#define ON 0
#define OFF 1

sbit BOT0 = P0^0;
sbit BOT1 = P0^1;
sbit BOT2 = P0^2;
sbit BOT3 = P0^3;
sbit BOT4 = P0^4;
sbit BOT5 = P0^5;
sbit BOT6 = P0^6;
sbit BOT7 = P0^7;

sbit SPK=P3^2;    //定義喇叭端口

unsigned char frq;

unsigned char recbuf[Limit];
unsigned char head = 0;
unsigned char tail = 0;

void InitUART  (void)
{

	SCON  = 0x50;		        // SCON: 模式 1, 8-bit UART, 使能接收  
	TMOD |= 0x20;               // TMOD: timer 1, mode 2, 8-bit 重裝
	TH1   = 0xFD;               // TH1:  重裝值 9600 波特率 晶振 11.0592MHz  
	TR1   = 1;                  // TR1:  timer 1 打開                         
	EA    = 1;                  //打開總中斷
	ES    = 1;                  //打開串口中斷
}    


void SendByte(unsigned char dat)
{
	SBUF = dat;
	while(!TI);
		TI = 0;
}
/*------------------------------------------------
                    發送一個字符串
------------------------------------------------*/
void SendStr(unsigned char *s)
{
	while(*s!='\0')// \0 表示字符串結束標誌，通過檢測是否字符串末尾
	{
		SendByte(*s);
		s++;
	}
}
/*------------------------------------------------
                     串口中斷程序
------------------------------------------------*/
void UART_SER (void) interrupt 4 //串行中斷服務程序
{
    unsigned char Temp;          //定義臨時變量 
   
	if(RI)                        //判斷是接收中斷產生
	{
		RI=0;                      //標誌位清零
		Temp=SBUF;                 //讀入緩衝區的值
		recbuf[head] = Temp;                  
	
		head++;
		if (head == Limit)
			head = 0;                 
	}
	// if(TI)                        //如果是發送標誌位，清零
		// TI=0;
} 

/*------------------------------------------------
 uS延時函數，含有輸入參數 unsigned char t，無返回值
 unsigned char 是定義無符號字符變量，其值的範圍是
 0~255 這裡使用晶振12M，精確延時請使用彙編,大致延時
 長度如下 T=tx2+5 uS 
------------------------------------------------*/
void DelayUs2x(unsigned char t)
{   
	while(--t);
}
/*------------------------------------------------
 mS延時函數，含有輸入參數 unsigned char t，無返回值
 unsigned char 是定義無符號字符變量，其值的範圍是
 0~255 這裡使用晶振12M，精確延時請使用彙編
------------------------------------------------*/
void DelayMs(unsigned char t)
{
     
	while(t--)
	{
		//大致延時1mS
		DelayUs2x(245);
		DelayUs2x(245);
	}
}
/*------------------------------------------------
              判忙函數
------------------------------------------------*/
bit LCD_Check_Busy(void) 
{ 
	DataPort= 0xFF; 
	RS_CLR; 
	RW_SET; 
	EN_CLR; 
	_nop_(); 
	EN_SET;
	return (bit)(DataPort & 0x80);
}
/*------------------------------------------------
              寫入命令函數
------------------------------------------------*/
void LCD_Write_Com(unsigned char com) 
{
	// while(LCD_Check_Busy()); //忙則等待
	DelayMs(5);
	RS_CLR; 
	RW_CLR; 
	EN_SET; 
	DataPort= com; 
	_nop_(); 
	EN_CLR;
}
/*------------------------------------------------
              寫入數據函數
------------------------------------------------*/
void LCD_Write_Data(unsigned char Data) 
{ 
	//while(LCD_Check_Busy()); //忙則等待
	DelayMs(5);
	RS_SET; 
	RW_CLR; 
	EN_SET; 
	DataPort= Data; 
	_nop_();
	EN_CLR;
}

/*------------------------------------------------
                清屏函數
------------------------------------------------*/
void LCD_Clear(void) 
{ 
	LCD_Write_Com(0x01); 
	DelayMs(5);
}
/*------------------------------------------------
              寫入字符串函數
------------------------------------------------*/
void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s) 
{     
	if (y == 0) 
	{     
		LCD_Write_Com(0x80 + x);     
	}
	else 
	{     
		LCD_Write_Com(0xC0 + x);     
	}        
	while (*s) 
	{     
		LCD_Write_Data( *s);     
		s++;     
	}
}
/*------------------------------------------------
              寫入字符函數
------------------------------------------------*/
void LCD_Write_Char(unsigned char x,unsigned char y,unsigned char Data) 
{     
	if (y == 0) 
	{     
		LCD_Write_Com(0x80 + x);     
	}    
	else 
	{     
		LCD_Write_Com(0xC0 + x);     
	}        
	LCD_Write_Data( Data);  
}
/*------------------------------------------------
              初始化函數
------------------------------------------------*/
void LCD_Init(void) 
{
	LCD_Write_Com(0x38);    /*顯示模式設置*/ 
	DelayMs(5); 
	LCD_Write_Com(0x38); 
	DelayMs(5); 
	LCD_Write_Com(0x38); 
	DelayMs(5); 
	LCD_Write_Com(0x38);  
	LCD_Write_Com(0x08);    /*顯示關閉*/ 
	LCD_Write_Com(0x01);    /*顯示清屏*/ 
	LCD_Write_Com(0x06);    /*顯示光標移動設置*/ 
	DelayMs(5); 
	LCD_Write_Com(0x0C);    /*顯示開及光標設置*/
}
   
/*------------------------------------------------
                    定時器初始化子程序
------------------------------------------------*/
void Init_Timer0(void)
{
	TMOD |= 0x01;	  //使用模式1，16位定時器，使用"|"符號可以在使用多個定時器時不受影響		     
	//TH0=0x00;	      //給定初值
	//TL0=0x00;
	EA=1;            //總中斷打開
	ET0=1;           //定時器中斷打開
	TR0=1;           //定時器開關打開
}

/*------------------------------------------------
                 定時器中斷子程序
------------------------------------------------*/
void Timer0_isr(void) interrupt 1 
{
	TH0=0xfe;		  //重新賦值 
	TL0=frq;         //低8位值在主程序中不斷累加
	
	SPK=!SPK;        //端口電平取反
}

/*------------------------------------------------
                    主函數
------------------------------------------------*/ 
void main(void) 
{ 
	unsigned char i; 
	unsigned char flag = 0;
	unsigned int count = 0;

	InitUART();

	i=0;
	while (1) 
	{  		
		if (head == 1)
		{
			head = 0;
			switch(recbuf[0] + 1)
			{
				case 1:
					LED0 = ON;
					LED1 = OFF;
					LED2 = OFF;
					LED3 = OFF;
					LED4 = OFF;
					LED5 = OFF;
					LED6 = OFF;
					LED7 = OFF;
					flag = 1;
					break;
				case 2:LED0 = ON;
					LED1 = ON;
					LED2 = OFF;
					LED3 = OFF;
					LED4 = OFF;
					LED5 = OFF;
					LED6 = OFF;
					LED7 = OFF;
					flag = 0;
					break;
				case 3:LED0 = ON;
					LED1 = ON;
					LED2 = ON;
					LED3 = OFF;
					LED4 = OFF;
					LED5 = OFF;
					LED6 = OFF;
					LED7 = OFF;
					flag = 0;
					break;
				case 4:LED0 = ON;
					LED1 = ON;
					LED2 = ON;
					LED3 = ON;
					LED4 = OFF;
					LED5 = OFF;
					LED6 = OFF;
					LED7 = OFF;
					flag = 0;
					break;
				case 5:LED0 = ON;
					LED1 = ON;
					LED2 = ON;
					LED3 = ON;
					LED4 = ON;
					LED5 = OFF;
					LED6 = OFF;
					LED7 = OFF;
					flag = 0;
					break;
				case 6:LED0 = ON;
					LED1 = ON;
					LED2 = ON;
					LED3 = ON;
					LED4 = ON;
					LED5 = ON;
					LED6 = OFF;
					LED7 = OFF;
					flag = 0;
					break;
				case 7:LED0 = ON;
					LED1 = ON;
					LED2 = ON;
					LED3 = ON;
					LED4 = ON;
					LED5 = ON;
					LED6 = ON;
					LED7 = OFF;
					flag = 0;
					break;
				case 8:LED0 = ON;
					LED1 = ON;
					LED2 = ON;
					LED3 = ON;
					LED4 = ON;
					LED5 = ON;
					LED6 = ON;
					LED7 = ON;
					flag = 0;
					break;
			}
			
			if(flag == 1)
			{
				Init_Timer0();
				while(1){
					DelayMs(1); //延時1ms，累加頻率值
					frq++;
					count++;
					if(count > 5000) break;
				}
				EA = 0;
				ET0 = 0;
				TR0 = 0;
				SPK = 0;
				flag = 0;
			}	          
		}
		

		if (BOT0 == ON)
        {
			SBUF = 21;
			while(TI == 0);
			TI = 0;
			while(BOT0 == ON);
			
			SBUF = 0x0;
			while(TI == 0);
			TI = 0;
        }
        if (BOT1 == ON)
		{
			SBUF = 22;
			while(TI == 0);
			TI = 0;
			while(BOT1 == ON);
		
			SBUF = 0x0;
			while(TI == 0);
			TI = 0;
		}
        if (BOT2 == ON)
		{
			SBUF = 23;
			while(TI == 0);
			TI = 0;
			while(BOT2 == ON);
		
			SBUF = 0x0;
			while(TI == 0);
			TI = 0;
		}
        if (BOT3 == ON)
		{
			SBUF = 24;
			while(TI == 0);
			TI = 0;
			while(BOT3 == ON);
		
			SBUF = 0x0;
			while(TI == 0);
			TI = 0;
		}
        if (BOT4 == ON)
		{
			SBUF = 25;
			while(TI == 0);
			TI = 0;
			while(BOT4 == ON);
		
			SBUF = 0x0;
			while(TI == 0);
			TI = 0;
		}
		if (BOT5 == ON)
		{
			SBUF = 26;
			while(TI == 0);
			TI = 0;
			while(BOT5 == ON);
		
			SBUF = 0x0;
			while(TI == 0);
			TI = 0;
		}
        if (BOT6 == ON)
		{
			SBUF = 27;
			while(TI == 0);
			TI = 0;
			while(BOT6 == ON);
		
			SBUF = 0x0;
			while(TI == 0);
			TI = 0;
		}
        if (BOT7 == ON)
		{
			SBUF = 28;
			while(TI == 0);
			TI = 0;
			while(BOT7 == ON);
		
			SBUF = 0x0;
			while(TI == 0);
			TI = 0;
		}
	}
}


