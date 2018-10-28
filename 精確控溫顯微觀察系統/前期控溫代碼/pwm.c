#include <reg52.h>
#include <math.h>
#include"HEADFILE.H"
#define uint unsigned int
#define uchar unsigned char

void delay(uint i)
{
	while(i--);	
}

uint code table[13]={0x3f,0x06,0x5b,0x4f,
					 0x66,0x6d,0x7d,0x07,
			         0x7f,0x6f,0x80,0x40,0x00};

//���a��λ�x�c���x
sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;
//���a�ܶ��x��P0��
//��ꇰ��IP1
sbit PWM=P2^1;

sbit beep=P2^0;

double catch();
uint keyscan();
void display(double sum);
void boil(uint rate);
void Timer1Init();

uint timer1;

double datapros(int temp) 	 
{
   	float tp;
	double yad;
//	int bk;  
	if(temp< 0)				//���¶�ֵΪ����
  	{ 
		temp=temp-1;
		temp=~temp;
		tp=temp;
		temp=tp*0.0625*100;//+0.5;	
  	}
 	else
  	{
		tp=temp;
		temp=tp*0.0625*100;//+0.5;	
	}

	yad=(double)temp/100;

	if(temp<0)
		yad=-yad;

	return yad;
}

void redisplay(double sum)
{
	uchar i;
	uint tt=500;
	uint smg[4];
	if(sum<0)
	{	smg[0]=table[11]; sum=-sum;	}
	else
		smg[0]=table[12];
	smg[1]=table[(int)sum/10];
	smg[2]=table[(int)sum%10]|0x80;
	smg[3]=table[(int)(10*sum)%10];
	while(tt--)
	{
	for(i=0;i<4;i++)
	{
		switch(i)
		{
			case(0):
				LSA=0;LSB=0;LSC=0; break;
			case(1):
				LSA=1;LSB=0;LSC=0; break;
			case(2):
				LSA=0;LSB=1;LSC=0; break;
			case(3):
				LSA=1;LSB=1;LSC=0; break;
		}
		P0=smg[i];
		delay(10);
		P0=0;
	}
	}		
}

int main()
{
	int k;
	double sum,now;
	do{
	sum=catch();
	}while(sum>50);
	for(k=500;k>0;k--)
	{
		 beep=~beep;
		 delay(50);
	}
	display(sum);	
	while(1)
	{
		now=datapros(Ds18b20ReadTemp());//���ݴ�����
		redisplay(now);
		if(now>=sum)
			boil(0);			
		else if(sum-now>12)
			boil(100);
		else if(sum-now>8)
			boil(80);
		else if(sum-now<=4)
			boil(5);
	};
	return 0;
}

//�I�P�����@ʾ���
double catch()
{
	double sum=0;
	int ak;
	uint get,i=0,j=0,flag=0;
	uint ic=0,ll=0,cast;
	uint cao[8]={12,12,12,12,12,12,12,12};
	while(1)
	{	
		get=keyscan();
		if(j==8||get==13)	
		{	P0=0;break;  }
		if(get==11)
			{
				flag++;
				for(cast=ll;cast>=1;cast--)
				{
					cao[cast]=cao[cast-1];
				}
				cao[0]=11;
				j--;
			}
	    if(get==10)
		    { i=j; j--; }
		if(get<10)
		{
			sum=10*sum+(double)get;
		}
		if(get<12)
		{
			if(get!=11)
				cao[ll]=get;
			j++;
			ll++;
		}
		switch(ic)	 //λѡ��ѡ�����������ܣ�
		{
			case 0:
				LSA=0;LSB=0;LSC=0;P0=table[cao[0]]; break;//��ʾ��0λ			
			case 1:
				LSA=1;LSB=0;LSC=0;P0=table[cao[1]]; break;//��ʾ��1λ
			case 2:
				LSA=0;LSB=1;LSC=0;P0=table[cao[2]]; break;//��ʾ��2λ
			case 3:
				LSA=1;LSB=1;LSC=0;P0=table[cao[3]]; break;//��ʾ��3λ
			case 4:
				LSA=0;LSB=0;LSC=1;P0=table[cao[4]]; break;//��ʾ��4λ
			case 5:
				LSA=1;LSB=0;LSC=1;P0=table[cao[5]]; break;//��ʾ��5λ
			case 6:
				LSA=0;LSB=1;LSC=1;P0=table[cao[6]]; break;//��ʾ��6λ
			case 7:
				LSA=1;LSB=1;LSC=1;P0=table[cao[7]]; break;//��ʾ��7λ	
		}
		delay(100);	
		ic++;
		if(ic==8)
		ic=0;
	}
	if(i)
		sum=sum/pow(10,j-i);  //

	ak = (int)((sum - (int)sum) * 10 + 0.50000001);	  //�������뱣��һλС��
	sum = (double)ak / 10 + (int)sum;
		
	if(flag%2==1)
		sum=-sum;

	return sum;
}

//�z�y��ꇰ��I
uint keyscan()
{
	uint num=12;
	uint temp;
	P1=0xfe;   //first line
	temp=P1;
	temp=temp&0xf0;
	temp=temp&0xf0;
	while(temp!=0xf0)
	{
		delay(1000);
		temp=P1;
		temp&=0xf0;
		while(temp!=0xf0)
		{
			temp=P1;
			switch(temp)
			{
				case 0xee:num=1;
					break;
				case 0xde:num=2;
					break;
				case 0xbe:num=3;
					break;
				case 0x7e:num=4;
					break;
			}
			while(temp!=0xf0)
			{
				temp=P1;
				temp=temp&0xf0;
			}
		}
	}
	P1=0xfd;	   //second line
	temp=P1;
	temp=temp&0xf0;
	temp=temp&0xf0;
	while(temp!=0xf0)
	{
		delay(1000);
		temp=P1;
		temp&=0xf0;
		while(temp!=0xf0)
		{
			temp=P1;
			switch(temp)
			{
				case 0xed:num=5;
					break;
				case 0xdd:num=6;
					break;
				case 0xbd:num=7;
					break;
				case 0x7d:num=8;
					break;
			}
			while(temp!=0xf0)
			{
				temp=P1;
				temp=temp&0xf0;
			}
		}
	}
	P1=0xfb;		//third line
	temp=P1;
	temp=temp&0xf0;
	temp=temp&0xf0;
	while(temp!=0xf0)
	{
		delay(1000);
		temp=P1;
		temp&=0xf0;
		while(temp!=0xf0)
		{
			temp=P1;
			switch(temp)
			{
				case 0xeb:num=9;
					break;
				case 0xdb:num=0;
					break;
				case 0xbb:num=10; //С���c
					break;
				case 0x7b:num=11;	  //̖ؓ
					break;
			}
			while(temp!=0xf0)
			{
				temp=P1;
				temp=temp&0xf0;
			}
		}
	}
	P1=0xf7;		//third line
	temp=P1;
	temp=temp&0xf0;
	temp=temp&0xf0;
	while(temp!=0xf0)
	{
		delay(1000);
		temp=P1;
		temp&=0xf0;
		while(temp!=0xf0)
		{
			temp=P1;
			switch(temp)
			{
				case 0xe7:num=13;  //�_�����I
					break;
			}
			while(temp!=0xf0)
			{
				temp=P1;
				temp=temp&0xf0;
			}
		}
	}
	return num;
}


void display(double sum)
{
	uchar i;
	uint tt=500;
	uint smg[4];
	if(sum<0)
	{	smg[0]=table[11]; sum=-sum;	}
	else
		smg[0]=table[12];
	smg[1]=table[(int)sum/10];
	smg[2]=table[(int)sum%10]|0x80;
	smg[3]=table[(int)(10*sum)%10];
	while(tt--)
	{
	for(i=0;i<4;i++)
	{
		switch(i)
		{
			case(0):
				LSA=0;LSB=0;LSC=0; break;
			case(1):
				LSA=1;LSB=0;LSC=0; break;
			case(2):
				LSA=0;LSB=1;LSC=0; break;
			case(3):
				LSA=1;LSB=1;LSC=0; break;
		}
		P0=smg[i];
		delay(100);
		P0=0;
	}
	}		
}

void boil(uint rate)
{	
	uint value=0;
	uint p=1;
	//uint k;
	Timer1Init();  //��ʱ��1��ʼ��
	value=1000*rate/100;
	
	while(1)
	{	
		/*
		for(k=500;k>0;k--)
		{
			beep=~beep;
			delay(50);
		}*/
		if(timer1 >= 1000)  //PWM����Ϊ1000*1us
		{
			timer1=0;
			p++;
			if(p >= 1000)
				break;
		}
		if(timer1 < value)	
			PWM=1;
		else
			PWM=0;	
	}		
}

void Timer1Init()
{
	TMOD|=0X10;//ѡ��Ϊ��ʱ��1ģʽ��������ʽ1������TR1��������

	TH1 = 0xFF; 
	TL1 = 0xff;   //1us
		
	ET1=1;//�򿪶�ʱ��1�ж�����
	EA=1;//�����ж�
	TR1=1;//�򿪶�ʱ��			
}

void Time1(void) interrupt 3    //3 Ϊ��ʱ��1���жϺ�  1 ��ʱ��0���жϺ� 0 �ⲿ�ж�1 2 �ⲿ�ж�2  4 �����ж�
{
	TH1 = 0xFF; 
	TL1 = 0xff;   //1us
	timer1++; 
}