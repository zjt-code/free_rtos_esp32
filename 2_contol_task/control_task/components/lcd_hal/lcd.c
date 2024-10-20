#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
// #include "sys.h"
#include "string.h"
#include <stdint.h>
#include "driver/gpio.h"
#include <rom/ets_sys.h>
// #include "esp_rom_sys.h"

//LCD�Ļ�����ɫ�ͱ���ɫ	   
u16 POINT_COLOR=0x0000;	//������ɫ
u16 BACK_COLOR=0xFFFF;  //����ɫ 

//����LCD��Ҫ����
//Ĭ��Ϊ����
_lcd_dev lcddev;


void opt_delay(u8 i)
{
	while(i--);
}

	 					    
//д�Ĵ�������
void LCD_WR_REG(u16 REG)
{
		u8 i;	

		//SPI_CS = 0;		
		//SPI_DC = 0;//RS=0 ����
		lcd_cs_set(0);opt_delay(5);
		lcd_dc_set(0);opt_delay(5);
		
		for(i=0; i<8; i++)
		{
			if (REG & 0x80)
			 //SPI_SDI = 1;
			
			{
				lcd_mosi_set(1);
				//opt_delay(5);
				}
			else
			//  SPI_SDI = 0;
			{
				lcd_mosi_set(0);
				//opt_delay(5);
				}

			
			REG <<= 1;		 
			// SPI_SCK = 0;		  
			// SPI_SCK = 1;
			lcd_sclk_set(0);//opt_delay(5);
			lcd_sclk_set(1);//opt_delay(5);
		}
		// SPI_CS=1;opt_delay(5);			
		lcd_cs_set(1);	
}
//д���ݺ���
//data:�Ĵ���ֵ
void LCD_WR_DATA(u16 DATA)
{
		u8 i;
	
		// SPI_CS = 0;		
		// SPI_DC = 1;//RS=1 ����
		lcd_cs_set(0);//opt_delay(5);
		lcd_dc_set(1);//opt_delay(5);
		for(i=0; i<8; i++)
		{
			if (DATA & 0x80)
			//  SPI_SDI = 1;
			{
				lcd_mosi_set(1);//opt_delay(5);
				}
			else
			//  SPI_SDI = 0;
			{
				lcd_mosi_set(0);//opt_delay(5);
				}
			
			DATA <<= 1;		 
			// SPI_SCK = 0;		  
			// SPI_SCK = 1;
			lcd_sclk_set(0);//opt_delay(5);
			lcd_sclk_set(1);//opt_delay(5);
		}
		// SPI_CS=1;			
		lcd_cs_set(1);//opt_delay(5);
}

//д�Ĵ���
//LCD_Reg:�Ĵ������?
//LCD_RegValue:Ҫд����?
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WriteRAM(LCD_RegValue);	    		 
}   

//��ʼдGRAM
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
} 
//LCDдGRAM
//RGB_Code:��ɫֵ
void LCD_WriteRAM(u16 RGB_Code)
{							    
//дʮ��λGRAM
  u8 i;
	// SPI_CS = 0;
	// SPI_DC = 1;
	lcd_cs_set(0);
	lcd_dc_set(1);
	
	for(i=0; i<16; i++)
	{
		if (RGB_Code & 0x8000)
		//  SPI_SDI = 1;
			lcd_mosi_set(1);
		else
		//  SPI_SDI = 0;
			lcd_mosi_set(0);
		
		RGB_Code <<= 1;		 
		// SPI_SCK = 0;		  
		// SPI_SCK = 1;
		lcd_sclk_set(0);
		lcd_sclk_set(1);
	} 
	// SPI_CS=1;	
	lcd_cs_set(1);
}

//��mdk -O1ʱ���Ż�ʱ��Ҫ����
//��ʱi


void delay_ms(uint32_t t1)
{
	for(int32_t t=0; t<t1; t++)
	{
		ets_delay_us(1000);
	}

}

//LCD������ʾ
void LCD_DisplayOn(void)
{					   
	LCD_WR_REG(0X29);	//������ʾ
}	 
//LCD�ر���ʾ
void LCD_DisplayOff(void)
{	   
	LCD_WR_REG(0X28);	//�ر���ʾ
}   
//���ù��λ��?
//Xpos:������
//Ypos:������
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	 	    
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF); 			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8);LCD_WR_DATA(Ypos&0XFF); 		 
} 		 
   
//����
//x,y:����
//POINT_COLOR:�˵�����?
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);		//���ù��λ��? 
	LCD_WriteRAM_Prepare();	//��ʼд��GRAM
	LCD_WriteRAM(POINT_COLOR); 
}	 
//���ٻ���
//x,y:����
//color:��ɫ
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{	   
		//���ù��λ��?
		LCD_SetCursor(x,y); 	 
		//д����ɫ
		LCD_WriteReg(lcddev.wramcmd,color);
}


//dir:����ѡ�� 	0-0����ת��1-180����ת��2-270����ת��3-90����ת
void LCD_Display_Dir(u8 dir)
{
	if(dir==0||dir==1)			//����
	{
			lcddev.dir=0;	//����
			lcddev.width=128;
			lcddev.height=160;

			lcddev.wramcmd=0X2C;
			lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;
		
		if(dir==0)        //0-0����ת
		{
			LCD_WR_REG(0x36); 
			LCD_WR_DATA((0<<3)|(1<<7)|(1<<6)|(0<<5));
		}else							//1-180����ת
		{
			LCD_WR_REG(0x36); 
			LCD_WR_DATA((0<<3)|(0<<7)|(0<<6)|(0<<5));		
		}
		
	}else if(dir==2||dir==3)
	{
		
			lcddev.dir=1;	//����
			lcddev.width=160;
			lcddev.height=128;

			lcddev.wramcmd=0X2C;
			lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B; 

				if(dir==2)				//2-270����ת
				{
					LCD_WR_REG(0x36); 
					LCD_WR_DATA((0<<3)|(0<<7)|(1<<6)|(1<<5));

				}else							//3-90����ת
				{
					LCD_WR_REG(0x36); 
					LCD_WR_DATA((0<<3)|(1<<7)|(0<<6)|(1<<5));
				}		
	}	


		//������ʾ����	
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(0);LCD_WR_DATA(0);
		LCD_WR_DATA((lcddev.width-1)>>8);LCD_WR_DATA((lcddev.width-1)&0XFF);
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(0);LCD_WR_DATA(0);
		LCD_WR_DATA((lcddev.height-1)>>8);LCD_WR_DATA((lcddev.height-1)&0XFF);  
}	 
//���ô���,���Զ����û������굽�������Ͻ�(sx,sy).
//sx,sy:������ʼ����(���Ͻ�)
//width,height:���ڿ��Ⱥ͸߶�,�������?0!!
//������?:width*height. 
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{    
	u16 twidth,theight;
	twidth=sx+width-1;
	theight=sy+height-1;

		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(sx>>8); 
		LCD_WR_DATA(sx&0XFF);	 
		LCD_WR_DATA(twidth>>8); 
		LCD_WR_DATA(twidth&0XFF);  
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(sy>>8); 
		LCD_WR_DATA(sy&0XFF); 
		LCD_WR_DATA(theight>>8); 
		LCD_WR_DATA(theight&0XFF); 

}

//�˿�����
void Set_GPIO(void)
{
	gpio_config_t lcd_gpio_conf;
	lcd_gpio_conf.mode = GPIO_MODE_OUTPUT;
	lcd_gpio_conf.intr_type = GPIO_INTR_DISABLE;
	lcd_gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	lcd_gpio_conf.pull_up_en = GPIO_PULLUP_ENABLE;
	lcd_gpio_conf.pin_bit_mask = (1<<LCD_CS_PIN_NUM)|(1<<LCD_SCLK_PIN_NUM)
									|(1<<LCD_DC_PIN_NUM)|(1<<LCD_RST_PIN_NUM);

esp_err_t err=	gpio_config(&lcd_gpio_conf);

printf("config gpio error =%d\r\n", err);

	//// config sda pin
	lcd_gpio_conf.mode = GPIO_MODE_INPUT_OUTPUT;
	lcd_gpio_conf.intr_type = GPIO_INTR_DISABLE;
	lcd_gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	lcd_gpio_conf.pull_up_en = GPIO_PULLUP_ENABLE;
	lcd_gpio_conf.pin_bit_mask = (1<<LCD_MOSI_PIN_NUM);

err = 	gpio_config(&lcd_gpio_conf);

printf("config SDA error =%d\r\n", err);

}	



//��ʼ��lcd
void LCD_Init(void)
{
	Set_GPIO();

	// SPI_RST=1;
	lcd_rst_set(1);
	delay_ms(1);
	// SPI_RST=0;
	lcd_rst_set(0);
	delay_ms(10);
	// SPI_RST=1;
	lcd_rst_set(1);
	delay_ms(120);  

//************* Start Initial Sequence **********//

LCD_WR_REG(0x11);     //Sleep out

delay_ms(120);                //delay_ms 120ms

LCD_WR_REG(0xB1);     //In normal mode
LCD_WR_DATA(0x00);   //frame rate=85.3Hz
LCD_WR_DATA(0x2C);   
LCD_WR_DATA(0x2B);   

LCD_WR_REG(0xB2);     //In Idle mode
LCD_WR_DATA(0x00);   
LCD_WR_DATA(0x01);   
LCD_WR_DATA(0x01);   

LCD_WR_REG(0xB3);     //In partial mode
LCD_WR_DATA(0x00);   
LCD_WR_DATA(0x01);   
LCD_WR_DATA(0x01);   
LCD_WR_DATA(0x00);   
LCD_WR_DATA(0x01);   
LCD_WR_DATA(0x01);   

LCD_WR_REG(0xB4);     //DOT inversion Display Inversion Control
LCD_WR_DATA(0x03);   

LCD_WR_REG(0xB9);     //In normal mode
LCD_WR_DATA(0xFF);   
LCD_WR_DATA(0x83);   
LCD_WR_DATA(0x47);   

LCD_WR_REG(0xC0);     //VRH: Set the GVDD
LCD_WR_DATA(0xA2);   
LCD_WR_DATA(0x02);   
LCD_WR_DATA(0x84);   

LCD_WR_REG(0xC1);     //set VGH/ VGL
LCD_WR_DATA(0x02);   //??02 VGH=16.6 VGL=-7.5  00 VGH=11.6 VGL=-7.5  06 VGH=16.6  VGL=-10

LCD_WR_REG(0xC2);     //APA: adjust the operational amplifier DCA: adjust the booster Voltage
LCD_WR_DATA(0x0A);   
LCD_WR_DATA(0x00);   

LCD_WR_REG(0xC3);     //In Idle mode (8-colors)
LCD_WR_DATA(0x8A);   
LCD_WR_DATA(0x2A);   

LCD_WR_REG(0xC4);     //In partial mode + Full color
LCD_WR_DATA(0x8A);   
LCD_WR_DATA(0xEE);   

LCD_WR_REG(0xC5);     //VCOM
LCD_WR_DATA(0x09);   

LCD_WR_REG(0x20);     //Display inversion

LCD_WR_REG(0xC7);     
LCD_WR_DATA(0x10);   

LCD_WR_REG(0x36);     //MX, MY, RGB mode
LCD_WR_DATA(0xC0); //08  

LCD_WR_REG(0xE0);     
LCD_WR_DATA(0x0C);   
LCD_WR_DATA(0x1C);   
LCD_WR_DATA(0x1B);   
LCD_WR_DATA(0x1A);   
LCD_WR_DATA(0x2F);   
LCD_WR_DATA(0x28);   
LCD_WR_DATA(0x20);   
LCD_WR_DATA(0x24);   
LCD_WR_DATA(0x23);   
LCD_WR_DATA(0x22);   
LCD_WR_DATA(0x2A);   
LCD_WR_DATA(0x36);   
LCD_WR_DATA(0x00);   
LCD_WR_DATA(0x05);   
LCD_WR_DATA(0x00);   
LCD_WR_DATA(0x10);   

LCD_WR_REG(0xE1);     
LCD_WR_DATA(0x0C);   
LCD_WR_DATA(0x1A);   
LCD_WR_DATA(0x1A);   
LCD_WR_DATA(0x1A);   
LCD_WR_DATA(0x2E);   
LCD_WR_DATA(0x27);   
LCD_WR_DATA(0x21);   
LCD_WR_DATA(0x24);   
LCD_WR_DATA(0x24);   
LCD_WR_DATA(0x22);   
LCD_WR_DATA(0x2A);   
LCD_WR_DATA(0x35);   
LCD_WR_DATA(0x00);   
LCD_WR_DATA(0x05);   
LCD_WR_DATA(0x00);   
LCD_WR_DATA(0x10);   

LCD_WR_REG(0x35);     //65k mode
LCD_WR_DATA(0x00);   

LCD_WR_REG(0x3A);     //65k mode
LCD_WR_DATA(0x05);   

LCD_WR_REG(0x29);     //Display on

} 

//������
//����ֵ:������ֵ
u16 LCD_RD_DATA(void)
{	
  u8 i;	
	u16 t=0;
		for(i=0; i<8; i++)
		{
			// SPI_SCK =0;
			lcd_sclk_set(0);
			//opt_delay(10);//��ʱ	
			// t=t << 1 | SPI_SDI_RD; 
			t=t << 1 | lcd_rd_get(); 

			// SPI_SCK = 1;	
			lcd_sclk_set(1);
      //opt_delay(10);			
		}	
	return t;  
	

}

//����ʾ�� ID
u16 LCD_Read_ID(u8 reg)
{
	
		u8 i;	

		//SPI_CS = 0;		
		//SPI_DC = 0;//RS=0 ����
		lcd_cs_set(0);
		lcd_dc_set(0);

		for(i=0; i<8; i++)
		{
			if (reg & 0x80)
			 //SPI_SDI = 1;
			 lcd_mosi_set(1);
			else
			//  SPI_SDI = 0;
			lcd_mosi_set(0);
			
			reg <<= 1;		 
			// SPI_SCK = 0;		  
			// SPI_SCK = 1;
			lcd_sclk_set(0);
			lcd_sclk_set(1);

		}
	
	// SPI_SCK =0;
	// SPI_SCK =1;	
	lcd_sclk_set(0);
	lcd_sclk_set(1);


	LCD_RD_DATA(); //7C
	
	lcddev.id=LCD_RD_DATA();  //89  									   
	lcddev.id<<=8;
	lcddev.id|=LCD_RD_DATA(); //F0 	
			
		// SPI_CS=1;
		lcd_cs_set(1);
	
		
	return lcddev.id;
}


//��ȡ��ĳ�����ɫ�?	 
//x,y:����
//����ֵ:�˵�����?
u16 LCD_ReadPoint(u16 x,u16 y)
{
	u8 i=0,r=0,g=0,b=0,reg=0x2e;
 	u16 color=0;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//�����˷�Χ,ֱ�ӷ���		   
	LCD_SetCursor(x,y);
	// SPI_CS = 0;		
	// SPI_DC = 0;
	lcd_cs_set(0);
	lcd_dc_set(0);

		for(i=0; i<8; i++)
		{
			if (reg & 0x80)
			//  SPI_SDI = 1;
			lcd_mosi_set(1);
			else
			//  SPI_SDI = 0;
			lcd_mosi_set(0);
			
			reg <<= 1;		 
			// SPI_SCK = 0;  
			// SPI_SCK = 1;	
			lcd_sclk_set(0);
			lcd_sclk_set(1);
		}
		

		for(i=0; i<9; i++)							//��һ�οն� �����ηֱ�ΪR G B
		{
			// SPI_SCK = 0;		 	
			// SPI_SCK = 1;
			lcd_sclk_set(0);
			lcd_sclk_set(1);

		}	

		for(i=0; i<8; i++)
		{
			// SPI_SCK = 0;	
			lcd_sclk_set(0);

			// r=r << 1 | SPI_SDI_RD; 	
			r=r << 1 | lcd_rd_get(); 	

			lcd_sclk_set(1);
			// SPI_SCK = 1;			
		}


		for(i=0; i<8; i++)
		{
			lcd_sclk_set(0);
			// SPI_SCK = 0;		
			// g=g << 1 | SPI_SDI_RD; 	
			g=g << 1 | lcd_rd_get(); 	

			lcd_sclk_set(1);
			// SPI_SCK = 1;			
		}


		for(i=0; i<8; i++)
		{
			lcd_sclk_set(0);

			// SPI_SCK = 0;
					// b=b << 1 | SPI_SDI_RD; 	
					b=b << 1 | lcd_rd_get(); 	
					
			// SPI_SCK = 1;
			lcd_sclk_set(1);

	
		}		
		
		color = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
		
		// SPI_CS=1;	
		lcd_cs_set(1);
		
return color;
}	
  
//��������
//color:Ҫ����������?
void LCD_Clear(u16 color)
{
	u32 index=0;      
	u32 totalpoint;
	
	totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//�õ��ܵ���	
	LCD_SetCursor(0,0);	            //���ù��λ��? 
	LCD_WriteRAM_Prepare();     		//��ʼд��GRAM	  	  
	for(index=0;index<totalpoint;index++)LCD_WriteRAM(color);
}  
//��ָ�����������ָ�����?
//������?:(xend-xsta+1)*(yend-ysta+1)
//xsta
//color:Ҫ������ɫ
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
	
		xlen=ex-sx+1;	 
		for(i=sy;i<=ey;i++)
		{
		 	LCD_SetCursor(sx,i);      				//���ù��λ��? 
			LCD_WriteRAM_Prepare();     			//��ʼд��GRAM	  
			for(j=0;j<xlen;j++)LCD_WriteRAM(color);	//���ù��λ��? 	    
		}
	
}  

//����
//x1,y1:�������?
//x2,y2:�յ�����  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//�������? 
	{  
		LCD_DrawPoint(uRow,uCol);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    
//������	  
//(x1,y1),(x2,y2):���εĶԽ�����
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
//��ָ��λ�û�һ��ָ����С��Բ
//(x,y):���ĵ�
//r    :�뾶
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //�ж��¸���λ�õı�־
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b);             //5
 		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-a,y0+b);             //1       
 		LCD_DrawPoint(x0-b,y0+a);             
		LCD_DrawPoint(x0-a,y0-b);             //2             
  	LCD_DrawPoint(x0-b,y0-a);             //7     	         
		a++;
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 									  
//��ָ��λ����ʾһ���ַ�
//x,y:��ʼ����
//num:Ҫ��ʾ���ַ�:" "--->"~"
//size:������? 12/16/24
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���	
 	num=num-' ';//�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//����1206����
		else if(size==16)temp=asc2_1608[num][t];	//����1608����
		else if(size==24)temp=asc2_2412[num][t];	//����2412����
		else return;								//û�е��ֿ�
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;		//��������
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//��������
				break;
			}
		}  	 
	}  	    	   	 	  
}   
//m^n����
//����ֵ:m^n�η�.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 
//��ʾ����,��λΪ0,����ʾ
//x,y :�������?	 
//len :���ֵ�λ��
//size:������?
//color:��ɫ 
//num:��ֵ(0~4294967295);	 
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 
////��ʾ����,��λΪ0,������ʾ
////x,y:�������?
////num:��ֵ(0~999999999);	 
////len:����(��Ҫ��ʾ��λ��)
////size:������?
////mode:
////[7]:0,�����?;1,���?0.
////[6:1]:����
////[0]:0,�ǵ�����ʾ;1,������ʾ.
//void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
//{  
//	u8 t,temp;
//	u8 enshow=0;						   
//	for(t=0;t<len;t++)
//	{
//		temp=(num/LCD_Pow(10,len-t-1))%10;
//		if(enshow==0&&t<(len-1))
//		{
//			if(temp==0)
//			{
//				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
//				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
// 				continue;
//			}else enshow=1; 
//		 	 
//		}
//	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
//	}
//} 
//��ʾ�ַ���
//x,y:�������?
//width,height:������?  
//size:������?
//*p:�ַ�����ʼ��ַ		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//�˳�
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }
}


// //���� 16*16
// void GUI_DrawFont16(u16 x, u16 y,u8 *s,u8 mode)
// {
// 	u8 i,j;
// 	u16 k;
// 	u16 HZnum;
// 	u16 x0=x;
// 	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//�Զ�ͳ�ƺ�����Ŀ
	
			
// 	for (k=0;k<HZnum;k++) 
// 	{
// 	  if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
// 	  { 	LCD_Set_Window(x,y,16,16);
// 				LCD_WriteRAM_Prepare();
// 		    for(i=0;i<16*2;i++)
// 		    {
// 				for(j=0;j<8;j++)
// 		    	{	
// 					if(!mode) //�ǵ��ӷ�ʽ
// 					{
// 						if(tfont16[k].Msk[i]&(0x80>>j))	LCD_WriteRAM(POINT_COLOR);
// 						else LCD_WriteRAM(BACK_COLOR);
// 					}
// 					else
// 					{
// 						//POINT_COLOR=fc;
// 						if(tfont16[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//��һ����
// 						x++;
// 						if((x-x0)==16)
// 						{
// 							x=x0;
// 							y++;
// 							break;
// 						}
// 					}

// 				}
				
// 			}
			
			
// 		}				  	
// 		continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��?
// 	}

// 	LCD_Set_Window(0,0,lcddev.width,lcddev.height);//�ָ�����Ϊȫ��  
// } 

// //���� 24*24
// void GUI_DrawFont24(u16 x, u16 y, u8 *s,u8 mode)
// {
// 	u8 i,j;
// 	u16 k;
// 	u16 HZnum;
// 	u16 x0=x;
// 	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//�Զ�ͳ�ƺ�����Ŀ
		
// 			for (k=0;k<HZnum;k++) 
// 			{
// 			  if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
// 			  { 	LCD_Set_Window(x,y,24,24);
// 						LCD_WriteRAM_Prepare();
// 				    for(i=0;i<24*3;i++)
// 				    {
// 							for(j=0;j<8;j++)
// 							{
// 								if(!mode) //�ǵ��ӷ�ʽ
// 								{
// 									if(tfont24[k].Msk[i]&(0x80>>j))	LCD_WriteRAM(POINT_COLOR);
// 									else LCD_WriteRAM(BACK_COLOR);
// 								}
// 							else
// 							{
// 								//POINT_COLOR=fc;
// 								if(tfont24[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//��һ����
// 								x++;
// 								if((x-x0)==24)
// 								{
// 									x=x0;
// 									y++;
// 									break;
// 								}
// 							}
// 						}
// 					}
					
					
// 				}				  	
// 				continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��?
// 			}

// 	LCD_Set_Window(0,0,lcddev.width,lcddev.height);//�ָ�����Ϊȫ��  
// }

// //���� 32*32
// void GUI_DrawFont32(u16 x, u16 y, u8 *s,u8 mode)
// {
// 	u8 i,j;
// 	u16 k;
// 	u16 HZnum;
// 	u16 x0=x;
// 	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//�Զ�ͳ�ƺ�����Ŀ
// 	for (k=0;k<HZnum;k++) 
// 			{
// 			  if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
// 			  { 	LCD_Set_Window(x,y,32,32);
// 						LCD_WriteRAM_Prepare();
// 				    for(i=0;i<32*4;i++)
// 				    {
// 						for(j=0;j<8;j++)
// 				    	{
// 							if(!mode) //�ǵ��ӷ�ʽ
// 							{
// 								if(tfont32[k].Msk[i]&(0x80>>j))	LCD_WriteRAM(POINT_COLOR);
// 								else LCD_WriteRAM(BACK_COLOR);
// 							}
// 							else
// 							{
// 								//POINT_COLOR=fc;
// 								if(tfont32[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//��һ����
// 								x++;
// 								if((x-x0)==32)
// 								{
// 									x=x0;
// 									y++;
// 									break;
// 								}
// 							}
// 						}
// 					}
					
					
// 				}				  	
// 				continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��?
// 			}
	
// 	LCD_Set_Window(0,0,lcddev.width,lcddev.height);//�ָ�����Ϊȫ��  
// } 



//��ʾ���ֻ����ַ���
void Show_Str(u16 x, u16 y,u8 *str,u8 size,u8 mode)
{			
	u16 x0=x;							  	  
  	u8 bHz=0;     //�ַ��������� 
    while(*str!=0)//����δ����
    { 
        if(!bHz)
        {
			if(x>(lcddev.width-size/2)||y>(lcddev.height-size)) 
			return; 
	        if(*str>0x80)bHz=1;//���� 
	        else              //�ַ�
	        {          
		        if(*str==0x0D)//���з���
		        {         
		            y+=size;
		            x=x0;
		            str++; 
		        }  
		        else
				{
					if(size>=24)//�ֿ���û�м���12X24 16X32��Ӣ������,��8X16����
					{ 						
					LCD_ShowChar(x,y,*str,24,mode);
					x+=12; //�ַ�,Ϊȫ�ֵ�һ�� 
					}
					else
					{
					LCD_ShowChar(x,y,*str,size,mode);
					x+=size/2; //�ַ�,Ϊȫ�ֵ�һ�� 
					}
				} 
				str++; 
		        
	        }
        }else//���� 
        {   
				if(x>(lcddev.width-size)||y>(lcddev.height-size)) 
				return;  

				bHz=0;//�к��ֿ�    
				// if(size==32)
				// 	GUI_DrawFont32(x,y,str,mode);	 	
				// else if(size==24)
				// 	GUI_DrawFont24(x,y,str,mode);	
				// else
				// 	GUI_DrawFont16(x,y,str,mode);
					
				str+=2; 
				x+=size;//��һ������ƫ��	    
        }						 
    }   
}


//��ʾ40*40ͼƬ
void Gui_Drawbmp16(u16 x,u16 y,const unsigned char *p) //��ʾ40*40ͼƬ
{
  	int i; 
	unsigned char picH,picL; 
	LCD_Set_Window(x,y,40,40);
	LCD_WriteRAM_Prepare();	
	
    for(i=0;i<40*40;i++)
	{	
	 	picL=*(p+i*2);	//���ݵ�λ��ǰ
		picH=*(p+i*2+1);				
		LCD_WriteRAM(picH<<8|picL);  						
	}	
	LCD_Set_Window(0,0,lcddev.width,lcddev.height);//�ָ���ʾ����Ϊȫ��	

}

//������ʾ
void Gui_StrCenter(u16 x, u16 y, u8 *str,u8 size,u8 mode)
{
	u16 x1;
	u16 len=strlen((const char *)str);
	if(size>16)
	{
		x1=(lcddev.width-len*(size/2))/2;
	}else
	{
		x1=(lcddev.width-len*8)/2;
	}
	
	Show_Str(x+x1,y,str,size,mode);
} 


void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);//����   
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	BACK_COLOR=WHITE;
	LCD_ShowString(lcddev.width-24,0,200,16,16,(u8 *)"RST");//��ʾ��������
 	 POINT_COLOR=RED;//���û�����ɫ 
}
////////////////////////////////////////////////////////////////////////////////
//���ݴ�����ר�в���
//��ˮƽ��
//x0,y0:����
//len:�߳���
//color:��ɫ
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}
//��ʵ��Բ
//x0,y0:����
//r:�뾶
//color:��ɫ
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}  

//��һ������
//(x1,y1),(x2,y2):��������ʼ����
//size�������Ĵ�ϸ�̶�
//color����������ɫ
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)return; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//�������? 
	{  
		gui_fill_circle(uRow,uCol,size,color);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}
