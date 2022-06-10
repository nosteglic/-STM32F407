#include "calendar.h" 	      						  
#include "math.h"
#include "alarm.h"
//----------------------------------------------------------------
//                 calendar��ʱ��
//----------------------------------------------------------------
//**********************************************����***********************************************************
//����
short temp;						//�����¶�ֵ
u8*const week_table[8]={(u8 *)" ",(u8 *)"һ",(u8 *)"��",(u8 *)"��",(u8 *)"��",(u8 *)"��",(u8 *)"��",(u8 *)"��"};

//����ṹ��
RTC_TimeTypeDef RTC_TimeStruct;     //ʱ��ṹ�壺ʱ���֡��롢ampm
RTC_DateTypeDef RTC_DateStruct;			//���ڽṹ�壺�ꡢ�¡��ա�����
//**********************************************����***********************************************************
//*******************************************************************
//���ݵ�ǰ������,������ʾ����Ļ�ϵ�������Ϣ
//�������ͣ�void
//ʹ�ò�������
//*******************************************************************
void calendar_date_refresh(void) 
{
	
 	//��ʾ����������
	POINT_COLOR=WHITE;//��ɫ����
	gui_show_num(DATE_TOPX,DATE_TOPY,2,WHITE,36,((RTC_DateStruct.RTC_Year+2000)/100)%100,0X80);		//��ʾ��  20/19 
	gui_show_num(DATE_TOPX+36,DATE_TOPY,2,WHITE,36,(RTC_DateStruct.RTC_Year+2000)%100,0X80);		 	//��ʾ�� 
	gui_show_ptchar(DATE_TOPX+72,DATE_TOPY,lcddev.width,lcddev.height,0,WHITE,36,'-',0);					//"-"
	gui_show_num(DATE_TOPX+90,DATE_TOPY,2,WHITE,36,RTC_DateStruct.RTC_Month,0X80);		 						//��ʾ��
	gui_show_ptchar(DATE_TOPX+126,DATE_TOPY,lcddev.width,lcddev.height,0,WHITE,36,'-',0);					//"-"   
	gui_show_num(DATE_TOPX+144,DATE_TOPY,2,WHITE,36,RTC_DateStruct.RTC_Date,0X80);	 							//��ʾ��
	//��ʾ���ڼ�
	POINT_COLOR=RED;	
	Show_Str(DATE_TOPX+24,DATE_TOPY+50,lcddev.width,lcddev.height,(u8 *)"����",24,0);//��ʾ���֡����ڡ�
	Show_Str(DATE_TOPX+72,DATE_TOPY+50,lcddev.width,lcddev.height,week_table[RTC_DateStruct.RTC_WeekDay],24,0);//��ʾ����
}
//*******************************************************************
//��Բ��ָ�����
//�������ͣ�void
//ʹ�ò�����x,y:�������ĵ�
//          size:���̴�С(ֱ��)
//          d:���̷ָ�,���ӵĸ߶�
//*******************************************************************
void calendar_circle_clock_drawpanel(u16 x,u16 y,u16 size,u16 d)
{
	u16 r=size/2;				//�õ��뾶 
	u16 sx=x-r;					//��߽�
	u16 sy=y-r;					//�ϱ߽�
	u16 px0,px1;				//���ӡ�ʱ�Ӹ���������
	u16 py0,py1; 				//���ӡ�ʱ�Ӹ��յ������
	u16 i; 
	gui_fill_circle(x,y,r,RED);		//����Ȧ
	gui_fill_circle(x,y,r-6,BLACK);		//����Ȧ
	for(i=0;i<60;i++)//�����Ӹ�
	{ 
		px0=sx+r+(r-7)*sin((PI/30)*i); 
		py0=sy+r-(r-7)*cos((PI/30)*i); 
		px1=sx+r+(r-d)*sin((PI/30)*i); 
		py1=sy+r-(r-d)*cos((PI/30)*i);  
		gui_draw_bline1(px0,py0,px1,py1,0,WHITE);		//��һ������
	}
	for(i=0;i<12;i++)//��Сʱ��
	{ 
		px0=sx+r+(r-7)*sin((PI/6)*i); 
		py0=sy+r-(r-7)*cos((PI/6)*i); 
		px1=sx+r+(r-d-2)*sin((PI/6)*i); 
		py1=sy+r-(r-d-2)*cos((PI/6)*i);  
		gui_draw_bline1(px0,py0,px1,py1,2,YELLOW);	//��һ������
		//��ʾ���̶̿�
		px1=sx+r+(r-2*d-8)*sin((PI/6)*i)-d-2; 
		py1=sy+r-(r-2*d-8)*cos((PI/6)*i)-d-2;  
		if(i==0) LCD_ShowNum(px1,py1,12,2,24);
		else LCD_ShowNum(px1,py1,i,2,24); 
	}
	for(i=0;i<4;i++)//��3Сʱ��
	{ 
		px0=sx+r+(r-7)*sin((PI/2)*i); 
		py0=sy+r-(r-7)*cos((PI/2)*i); 
		px1=sx+r+(r-d-7)*sin((PI/2)*i); 
		py1=sy+r-(r-d-7)*cos((PI/2)*i);  
		gui_draw_bline1(px0,py0,px1,py1,2,YELLOW);	//��һ������	
	}
	gui_fill_circle(x,y,d/2,RED);		//������Ȧ
}
//*******************************************************************
//��ʾ�ӱ����ϵ�ʱ�롢���롢����
//�������ͣ�void
//ʹ�ò�����x,y:�������ĵ�
//          size:���̴�С(ֱ��)
//          d:���̷ָ�,���ӵĸ߶�
//          hour:ʱ�ӡ�min:���ӡ�sec:����
//*******************************************************************
void calendar_circle_clock_showtime(u16 x,u16 y,u16 size,u16 d,u8 hour,u8 min,u8 sec)
{
	static u8 oldhour=0;	//���һ�ν���ú�����ʱ������Ϣ
	static u8 oldmin=0;
	static u8 oldsec=0;
	float temp;
	u16 r=size/2;		//�õ��뾶 
	u16 sx=x-r;			//��߽�
	u16 sy=y-r;			//�ϱ߽�
	u16 px0,px1;		//���
	u16 py0,py1;  	//�յ�
	u8 r1=d/2+3; 		//ָ���յ�
	if(hour>11)hour-=12;//����12Сʱʱ����ѭ��
  ////////////////�����һ�ε�����////////////////////
	//���Сʱ
	temp=(float)oldmin/60;
	temp+=oldhour;
	px0=sx+r+(r-6*d)*sin((PI/6)*temp); 
	py0=sy+r-(r-6*d)*cos((PI/6)*temp); 
	px1=sx+r+r1*sin((PI/6)*temp); 
	py1=sy+r-r1*cos((PI/6)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,2,BLACK);
	//�������
	temp=(float)oldsec/60;
	temp+=oldmin;
	px0=sx+r+(r-5*d)*sin((PI/30)*temp); 
	py0=sy+r-(r-5*d)*cos((PI/30)*temp); 
	px1=sx+r+r1*sin((PI/30)*temp); 
	py1=sy+r-r1*cos((PI/30)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,1,BLACK); 
	//������� 
	px0=sx+r+(r-4*d)*sin((PI/30)*oldsec); 
	py0=sy+r-(r-4*d)*cos((PI/30)*oldsec); 
	px1=sx+r+r1*sin((PI/30)*oldsec); 
	py1=sy+r-r1*cos((PI/30)*oldsec); 
	gui_draw_bline1(px0,py0,px1,py1,0,BLACK); 
  //////////////////��ʾ////////////////////////
	//��ʾ�µ�ʱ��
	temp=(float)min/60;
	temp+=hour;
	px0=sx+r+(r-6*d)*sin((PI/6)*temp); 
	py0=sy+r-(r-6*d)*cos((PI/6)*temp); 
	px1=sx+r+r1*sin((PI/6)*temp); 
	py1=sy+r-r1*cos((PI/6)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,2,YELLOW); 
	//��ʾ�µķ���
	temp=(float)sec/60;
	temp+=min;
	px0=sx+r+(r-5*d)*sin((PI/30)*temp); 
	py0=sy+r-(r-5*d)*cos((PI/30)*temp); 
	px1=sx+r+r1*sin((PI/30)*temp); 
	py1=sy+r-r1*cos((PI/30)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,1,GREEN); 	 
	//��ʾ�µ�����
	px0=sx+r+(r-4*d)*sin((PI/30)*sec); 
	py0=sy+r-(r-4*d)*cos((PI/30)*sec); 
	px1=sx+r+r1*sin((PI/30)*sec); 
	py1=sy+r-r1*cos((PI/30)*sec); 
	gui_draw_bline1(px0,py0,px1,py1,0,GBLUE); 
	oldhour=hour;	//����ʱ
	oldmin=min;		//�����
	oldsec=sec;		//������
}	 
//*******************************************************************
//����ʱ�ӣ����ϸ���ʱ��(���� ���ӵ���/УʱУ��/�������� �ĵ���)
//�������ͣ�void
//ʹ�ò�������
//*******************************************************************
u8 calendar_play(u8 tag)
{
	u8 rval=HOLD_CLOCK_SETTING;
	u8 second=0;							//��ʱ����
	u8 minute=0;							//��ʱ������
	u8 hour=0;								//��ʱСʱ��
	u8 tempdate=0;						//��ʱ����ֵ
	u8 tempmonth=0;						//��ʱ�·�
	u8 tempyear=0;						//��ʱ���
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);		//��ȡʱ��  
	RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);		//��ȡ����
	calendar_date_refresh();						//��������
	POINT_COLOR=WHITE;									//��ɫ����
	calendar_circle_clock_drawpanel(center_x,center_y,size,d);														//��ʾָ��ʱ�ӱ��� 
	gui_show_ptchar(TIME_TOPX+80,TIME_TOPY,lcddev.width,lcddev.height,0,GBLUE,60,':',0);	//":"
	gui_show_ptchar(TIME_TOPX+170,TIME_TOPY,lcddev.width,lcddev.height,0,GBLUE,60,':',0);	//":" 
	second=RTC_TimeStruct.RTC_Seconds;			//�ݴ浱ǰ����
	minute=30;															//��������30minʱ��ʱ
	hour=RTC_TimeStruct.RTC_Hours;					//�ݴ�Сʱ��
	tempdate=RTC_DateStruct.RTC_Date;				//�ݴ浱ǰ����
	tempmonth=RTC_DateStruct.RTC_Month;			//�ݴ浱ǰ�·�
	tempyear=RTC_DateStruct.RTC_Year;				//�ݴ浱ǰ���
	while(1)
	{	
		//�ж�TPAD���ذ���
		if(TPAD_Scan(0)){
			rval=SWITCH_CLOCK_SETTING;
			break;
		}
		/////////////////////ʱ������///////////////////////////////
		RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);//��ȡʱ��  
		if(second!=RTC_TimeStruct.RTC_Seconds||tag==2)//��һ�������͵�ǰ��ͬ(�������ж�tag ==2 �ﵽ���ý���ʱ��Ҳ��ʾ��Ч��)
		{
			second=RTC_TimeStruct.RTC_Seconds;//��������
			
			calendar_circle_clock_showtime(center_x,center_y,size,d,RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);//��ʾʱ����ָ��    
			gui_show_num(TIME_TOPX+20,TIME_TOPY,2,GBLUE,60,RTC_TimeStruct.RTC_Hours,0X80);	//��ʾʱ
			gui_show_num(TIME_TOPX+110,TIME_TOPY,2,GBLUE,60,RTC_TimeStruct.RTC_Minutes,0X80);	//��ʾ��
			gui_show_num(TIME_TOPX+200,TIME_TOPY,2,GBLUE,60,RTC_TimeStruct.RTC_Seconds,0X80);	//��ʾ��	
			temperature_show();
			
			RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);	//����ʱ��ṹ��
			RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);	//�������ڽṹ��
			//��Сʱ��ʱ
			if(RTC_TimeStruct.RTC_Seconds == 0)
				if(minute == RTC_TimeStruct.RTC_Minutes)	//��30��
				{
					calendar_ring(1);												
				}
			//���㱨ʱ
			if(hour!=RTC_TimeStruct.RTC_Hours)							//��һ��Сʱ�����ڲ�ͬ
			{
				calendar_ring(3);												
				hour=RTC_TimeStruct.RTC_Hours;								//��ֹ�ظ�����
			}
			//���ꡢ�¡�������һ�����ı䶼����������Ļ�ϵ�����
			if((RTC_DateStruct.RTC_Date!=tempdate)|(tempmonth!=RTC_DateStruct.RTC_Month)|(tempyear!=RTC_DateStruct.RTC_Year))
			{
				calendar_date_refresh();	//  ��������
				tempdate=RTC_DateStruct.RTC_Date;	//��ֹ�ظ�����
				tempmonth=RTC_DateStruct.RTC_Date;	//
	      tempyear=RTC_DateStruct.RTC_Date;	//
			}
		}
		if(tag==2)
			break;
		////////////////���ӵ��������///////////////////////////
		alarm_schedule();	//���ӵ���	
		if(Alarm_FLAG == 1)//��������Ҫ��ʱ
			break;
 	}
	return rval;
}

//*******************************************************************
//��ʾ�¶ȵ���Ļָ��λ��
//�������ͣ�void
//ʹ�ò�������
//*******************************************************************
void temperature_show(void)
{
	POINT_COLOR=WHITE;				//��ɫ����
	Show_Str(TEMP_TOPX,TEMP_TOPY+20,lcddev.width,lcddev.height,(u8 *)"�¶�",24,0);  									//��ʾ���֡��¶ȡ�
	gui_show_ptchar(TEMP_TOPX+48,TEMP_TOPY+20,lcddev.width,lcddev.height,0,WHITE,24,':',0);						//":" 
	temp=Get_Temprate();	//�õ��¶�ֵ 
		if(temp<0)						//���¶�Ϊ��ʱ
		{
			temp=-temp;					//ȡ��
			gui_show_ptchar(TEMP_TOPX+72,TEMP_TOPY,lcddev.width,lcddev.height,0,GBLUE,60,'-',0);					//��ʾ����"-"
		}
		else 
			gui_show_ptchar(TEMP_TOPX+72,TEMP_TOPY,lcddev.width,lcddev.height,0,BRRED,60,' ',0);					//�޷���
		gui_show_num(TEMP_TOPX+92,TEMP_TOPY,2,BRRED,60,temp/100,0X80);																	//��������			   
		gui_show_ptchar(TEMP_TOPX+152,TEMP_TOPY,lcddev.width,lcddev.height,0,BRRED,60,'.',0);						//"." 
		gui_show_num(TEMP_TOPX+182,TEMP_TOPY,2,BRRED,60,temp%100,0X80);																	//С������
		gui_show_ptchar(TEMP_TOPX+252,TEMP_TOPY,lcddev.width,lcddev.height,0,BRRED,60,95+' ',0);				//��λ�����϶ȡ�
}
//*******************************************************************
//���㱨ʱ
//�������ͣ�void
//ʹ�ò����type:��������
//0,��.
//1,��.��.
//2,��.��.��
//4,��.��.��.��
//*******************************************************************
void calendar_ring(u8 type)
{
	u8 i;	 										//��ʱ����
	for(i=0;i<(type+1);i++)		//ѭ��
	{
		BEEP=1;									//��������
		delay_ms(50);						
		BEEP=0;									//������ͣ
		delay_ms(70);
	}	 
}













