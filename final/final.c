/*-----------------------------------------------
  �W�١GLCD1602
  �׾¡Gwww.doflye.net
  �s�g�Gshifang
  ����G2009.5
  �ק�G�L
  ���e�G�q�L�зǵ{�ǰʺA��ܦr��
  �޸}�w�q�p�U�G1-VSS 2-VDD 3-V0 4-RS 5-R/W 6-E 7-14 DB0-DB7 15-BLA 16-BLK
------------------------------------------------*/
#include<reg52.h> //�]�t�Y���A�@�뱡�p���ݭn��ʡA�Y���]�t�S��\��H�s�����w�q
#include<intrins.h>

sbit RS = P2^4;   //�w�q�ݤf 
sbit RW = P2^5;
sbit EN = P2^6;

#define RS_CLR RS=0 
#define RS_SET RS=1

#define RW_CLR RW=0 
#define RW_SET RW=1 

#define EN_CLR EN=0
#define EN_SET EN=1

#define DataPort P0
#define Limit    16 // ���Φ�C

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

sbit SPK=P3^2;    //�w�q��z�ݤf

unsigned char frq;

unsigned char recbuf[Limit];
unsigned char head = 0;
unsigned char tail = 0;

void InitUART  (void)
{

	SCON  = 0x50;		        // SCON: �Ҧ� 1, 8-bit UART, �ϯ౵��  
	TMOD |= 0x20;               // TMOD: timer 1, mode 2, 8-bit ����
	TH1   = 0xFD;               // TH1:  ���˭� 9600 �i�S�v ���� 11.0592MHz  
	TR1   = 1;                  // TR1:  timer 1 ���}                         
	EA    = 1;                  //���}�`���_
	ES    = 1;                  //���}��f���_
}    


void SendByte(unsigned char dat)
{
	SBUF = dat;
	while(!TI);
		TI = 0;
}
/*------------------------------------------------
                    �o�e�@�Ӧr�Ŧ�
------------------------------------------------*/
void SendStr(unsigned char *s)
{
	while(*s!='\0')// \0 ��ܦr�Ŧ굲���лx�A�q�L�˴��O�_�r�Ŧ꥽��
	{
		SendByte(*s);
		s++;
	}
}
/*------------------------------------------------
                     ��f���_�{��
------------------------------------------------*/
void UART_SER (void) interrupt 4 //��椤�_�A�ȵ{��
{
    unsigned char Temp;          //�w�q�{���ܶq 
   
	if(RI)                        //�P�_�O�������_����
	{
		RI=0;                      //�лx��M�s
		Temp=SBUF;                 //Ū�J�w�İϪ���
		recbuf[head] = Temp;                  
	
		head++;
		if (head == Limit)
			head = 0;                 
	}
	// if(TI)                        //�p�G�O�o�e�лx��A�M�s
		// TI=0;
} 

/*------------------------------------------------
 uS���ɨ�ơA�t����J�Ѽ� unsigned char t�A�L��^��
 unsigned char �O�w�q�L�Ÿ��r���ܶq�A��Ȫ��d��O
 0~255 �o�̨ϥδ���12M�A��T���ɽШϥηJ�s,�j�P����
 ���צp�U T=tx2+5 uS 
------------------------------------------------*/
void DelayUs2x(unsigned char t)
{   
	while(--t);
}
/*------------------------------------------------
 mS���ɨ�ơA�t����J�Ѽ� unsigned char t�A�L��^��
 unsigned char �O�w�q�L�Ÿ��r���ܶq�A��Ȫ��d��O
 0~255 �o�̨ϥδ���12M�A��T���ɽШϥηJ�s
------------------------------------------------*/
void DelayMs(unsigned char t)
{
     
	while(t--)
	{
		//�j�P����1mS
		DelayUs2x(245);
		DelayUs2x(245);
	}
}
/*------------------------------------------------
              �P�����
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
              �g�J�R�O���
------------------------------------------------*/
void LCD_Write_Com(unsigned char com) 
{
	// while(LCD_Check_Busy()); //���h����
	DelayMs(5);
	RS_CLR; 
	RW_CLR; 
	EN_SET; 
	DataPort= com; 
	_nop_(); 
	EN_CLR;
}
/*------------------------------------------------
              �g�J�ƾڨ��
------------------------------------------------*/
void LCD_Write_Data(unsigned char Data) 
{ 
	//while(LCD_Check_Busy()); //���h����
	DelayMs(5);
	RS_SET; 
	RW_CLR; 
	EN_SET; 
	DataPort= Data; 
	_nop_();
	EN_CLR;
}

/*------------------------------------------------
                �M�̨��
------------------------------------------------*/
void LCD_Clear(void) 
{ 
	LCD_Write_Com(0x01); 
	DelayMs(5);
}
/*------------------------------------------------
              �g�J�r�Ŧ���
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
              �g�J�r�Ũ��
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
              ��l�ƨ��
------------------------------------------------*/
void LCD_Init(void) 
{
	LCD_Write_Com(0x38);    /*��ܼҦ��]�m*/ 
	DelayMs(5); 
	LCD_Write_Com(0x38); 
	DelayMs(5); 
	LCD_Write_Com(0x38); 
	DelayMs(5); 
	LCD_Write_Com(0x38);  
	LCD_Write_Com(0x08);    /*�������*/ 
	LCD_Write_Com(0x01);    /*��ܲM��*/ 
	LCD_Write_Com(0x06);    /*��ܥ��в��ʳ]�m*/ 
	DelayMs(5); 
	LCD_Write_Com(0x0C);    /*��ܶ}�Υ��г]�m*/
}
   
/*------------------------------------------------
                    �w�ɾ���l�Ƥl�{��
------------------------------------------------*/
void Init_Timer0(void)
{
	TMOD |= 0x01;	  //�ϥμҦ�1�A16��w�ɾ��A�ϥ�"|"�Ÿ��i�H�b�ϥΦh�өw�ɾ��ɤ����v�T		     
	//TH0=0x00;	      //���w���
	//TL0=0x00;
	EA=1;            //�`���_���}
	ET0=1;           //�w�ɾ����_���}
	TR0=1;           //�w�ɾ��}�����}
}

/*------------------------------------------------
                 �w�ɾ����_�l�{��
------------------------------------------------*/
void Timer0_isr(void) interrupt 1 
{
	TH0=0xfe;		  //���s��� 
	TL0=frq;         //�C8��Ȧb�D�{�Ǥ����_�֥[
	
	SPK=!SPK;        //�ݤf�q������
}

/*------------------------------------------------
                    �D���
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
					DelayMs(1); //����1ms�A�֥[�W�v��
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


