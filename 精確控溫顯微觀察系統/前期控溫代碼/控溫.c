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

sbit beep=P2^0;

double catch();
uint keyscan();
void display(double sum);


uchar DisplayData[8];

double datapros(int temp) 	 
{
   	float tp;
	double yad;
//	int bk;  
	if(temp< 0)				//���¶�ֵΪ����
  	{
		DisplayData[0] = 0x40; 
		temp=temp-1;
		temp=~temp;
		tp=temp;
		temp=tp*0.0625*100;//+0.5;	
  	}
 	else
  	{			
		DisplayData[0] = 0x00;
		tp=temp;
		temp=tp*0.0625*100;//+0.5;	
	}

	yad=(double)temp/100;
//	bk = (int)((yad - (int)yad) * 10 + 0.50000001);	  //�������뱣��һλС��
//	yad = (double)bk / 10 + (int)yad;
//	temp=yad*100;

	DisplayData[1] = table[temp / 10000];
	DisplayData[2] = table[temp % 10000 / 1000];
	DisplayData[3] = table[temp % 1000 / 100] | 0x80;
	DisplayData[4] = table[temp % 100 / 10];
	DisplayData[5] = table[temp % 10];

	if(temp<0)
		yad=-yad;

	return yad;
}

void DigDisplay()
{
	uchar i;
	for(i=0;i<6;i++)
	{
		switch(i)	 //λѡ��ѡ�����������ܣ�
		{
			case(0):
				LSA=0;LSB=0;LSC=0; break;//��ʾ��0λ
			case(1):
				LSA=1;LSB=0;LSC=0; break;//��ʾ��1λ
			case(2):
				LSA=0;LSB=1;LSC=0; break;//��ʾ��2λ
			case(3):
				LSA=1;LSB=1;LSC=0; break;//��ʾ��3λ
			case(4):
				LSA=0;LSB=0;LSC=1; break;//��ʾ��4λ
			case(5):
				LSA=1;LSB=0;LSC=1; break;//��ʾ��5λ	
		}
		P0=DisplayData[i];//��������
		delay(100); //���һ��ʱ��ɨ��	
		P0=0x00;//����
	}		
}

int main()
{
	int k;
	double sum,now;
	do{
	sum=catch();
	}while(sum>50||sum<-10);
	for(k=500;k>0;k--)
	{
		 beep=~beep;
		 delay(50);
	}
	display(sum);	
	while(1)
	{
		now=datapros(Ds18b20ReadTemp());//���ݴ�����
		DigDisplay();					//�������ʾ����		
	}
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