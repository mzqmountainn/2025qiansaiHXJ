#include "board.h"
#include "rtthread.h"

/************************ VARIABLE *************************/
extern const unsigned char gImage_121[45000];

//tempreture
float tempre_value=0;		//����Ƭ�¶�
float tempre_value_pre=0;	//��һʱ�̼���Ƭ�¶�
uint8_t Tem_flag=0;			//0-�����ȣ�1-����3C��2-����6C
uint8_t Tem_flag_yn = 0;	//�Ƿ����

//��ѹ
int32_t preassure=0;	
int32_t preassure_really=0;	
uint32_t cs_flag=0;	

/* thread control block */
static rt_thread_t  Feature_thread=RT_NULL;			       //�������񣬹ķ��
static rt_thread_t  Alarm_thread=RT_NULL;		           //�쳣��������
static rt_thread_t  humidification_thread=RT_NULL;	   //��ʪ������
static rt_thread_t  ci_thread=RT_NULL;	               //���ܻ�������
static rt_thread_t  data_acquisition_thread = RT_NULL; //���ݲɼ�����
static rt_thread_t  thread_lcd= RT_NULL;							 //lcd����

/* event control block*/
static rt_event_t ketctrl_event=RT_NULL;            
static rt_event_t Alarm_event=RT_NULL;							  //�����¼����ƿ�
static rt_event_t ketctrl_tem_event=RT_NULL;	
static rt_event_t data_acquisition_event=RT_NULL;	

/* timer control block*/
static rt_timer_t Alarm_timer=RT_NULL;                //�����ʱ�����ƿ�

static uint8_t Alarm_flag=1;
static uint8_t Alarm_cnt=0;

/* Define global variables */
#define CPAP_START_EVENT 			 (0x01 << 0)
#define Alarm_EVENT      			 (0x01 << 0)
#define Tem_START_EVENT  			 (0x01 << 0)
#define data_acquisition_EVENT (0x01 << 0)

#define Tem_Threshold_1	30		//΢����
#define Tem_Threshold_2	40		//����
#define Tem_Threshold	80		//����

/*device begin*/
#define UART8266     "uart2"
static struct rt_semaphore rx_sem;
static rt_device_t serial;

/*************************  FUNCATION *********************/
static void Feature_thread_entry(void* parameter); 
static void Alarm_thread_entry(void* parameter); 
static void humidification_thread_entry(void* parameter);
static void rtc_thread_entry(void* parameter);
static void Alarm_timer_callback(void* parameter);
static void data_acquisition_entry(void* parameter);
static void thread_lcd_entry(void* parameter);
static void CI_entry(void *parameter);

void Debug_USART_Config8266(void);//����2��ʼ������

//flashд�뺯��ԭ��
void flash_writeInfo(void);
void flash_readInfo(void);

int lastui, nowui;
unsigned char uiSelect = 0;
unsigned char signalUIselectChange = 0;
unsigned char signalUIchange = 0;
unsigned char signalValueChange = 0;

void key_valueChange(int key);
void ui_clear(int last);
void key_sonSelect(int nowui,int t);
void key_UIchange(int t, unsigned char son);
void (*now_running_ui)(void);  //ָ��void���͵ĺ���ָ��
void ui_welcome(void);
void ui_0(void);
void ui_1(void);
void ui_2(void);
void ui_3(void);
void ui_4(void);
void ui_5(void);

//ʱ�����ò˵�
key_table table[KEY_TABLE_NUM] = {
    {0, 0, {0}, 1, (*ui_0)},
    {1, 3, { 3, 4, 1 , 1}, 2, (*ui_1)},
    {2, 6, {0,0,0,0,0,1}, 3, (*ui_2)},
    {3, 11, {0,0,0,0,0,0,0,0,0,0,1}, 3, (*ui_3)},
    {4, 8, {0,0,0,0,0,0,4,1}, 3, (*ui_4)},
    {5, 5, {4,4,4,4,4},4,(*ui_5)}, 
};


/*�ҵ���Ϣ�����ݴ洢*/
myInfosFlashStruct myInfosFlash[30] = {0};

/*�Ƿ��һ�δ�*/
char IsFirstTimeUse = 0;

/*�Ƿ����thread_control�ź�*/
char stateSuspendthread_control = 1; //1 ���� 0�ͷ�

//RTC
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;

/*�˵���ر���*/
myInfosStrust        myInfos = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0, 0.0, 0.0, 0.0};
breathSettingStruct  breathSetting = {0, 300, 30, 8, 5, 2.0, 0.5, 2, 2,15};
breathSettingStruct  breathSettingBuff = {0, 300, 30, 8, 5, 2.0, 0.5, 2, 2,15};
breathSettingStruct  *breathSettingP[10] = {&breathSetting};
runningSettingStruct runningSetting = {0, 1, 1, 0, 3, 3};
runningSettingStruct runningSettingBuff = {0, 1, 1, 0, 3, 3};
runningSettingStruct *runningSettingP[6] = {&runningSetting};

/**
 * @brief ������
 * @param ��
 * @retval ��
 */
int main(void)
{
	LCD_Init();
  LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
  flash_readInfo();
	
	/* ����ϵͳ�еĴ����豸 */
	serial = rt_device_find(UART8266);
	if (!serial)
	{
			Usart_SendString(USART2,"find  failed!\n");
	}

	/* �����¼� */
	ketctrl_event = rt_event_create("ketctrl_event", RT_IPC_FLAG_FIFO);	
	Alarm_event = rt_event_create("Alarm_event", RT_IPC_FLAG_FIFO);	
	ketctrl_tem_event = rt_event_create("ketctrl_tem_event", RT_IPC_FLAG_FIFO);
	data_acquisition_event = rt_event_create("data_acquisition_event", RT_IPC_FLAG_FIFO);
	
	/* ����һ�������ʱ�� */
	Alarm_timer=rt_timer_create("Alarm_timer_callback",
															Alarm_timer_callback,
															0,
															2000,
															RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
	
	if(Alarm_timer!=RT_NULL)
	{
		rt_timer_stop(Alarm_timer);		
	}

	/* ��ʪ������ */
	humidification_thread=rt_thread_create("humidification_thread",
																	humidification_thread_entry,
																	RT_NULL,
																	512,
																	3,
																	20); 
	rt_thread_startup(humidification_thread);
  	
	/* �����̶߳��� */
	Feature_thread = rt_thread_create("Feature_thread",
														Feature_thread_entry,
														RT_NULL,
														512,
														2,
														20); 
	rt_thread_startup(Feature_thread);
	
	/* �����̶߳��� */
	Alarm_thread = rt_thread_create("Alarm_thread",
														Alarm_thread_entry,
														RT_NULL,
														128,
														1,
														20); 
	rt_thread_startup(Alarm_thread);

	/* LCD�̶߳��� */
	thread_lcd = rt_thread_create("thread_lcd",
												 thread_lcd_entry,
												 RT_NULL,
												 512,
												 3,
												 30);
	rt_thread_startup(thread_lcd);
	
	/* ���ݲɼ��̶߳��� */
	data_acquisition_thread = rt_thread_create("data_acquisition_thread",
												data_acquisition_entry,
												RT_NULL,
												256,
												2,
												20);	
	rt_thread_startup(data_acquisition_thread);
	
	/* RTC�̶߳��� */
	ci_thread = rt_thread_create("ci_thread",
												CI_entry,
												RT_NULL,
												128,
												3,
												20);	
	rt_thread_startup(ci_thread);
	
}

/*
*************************************************************************
*                             thread definition
*************************************************************************
*/

//��������
static void Alarm_thread_entry(void* parameter){
	rt_thread_delay(10);
	rt_err_t rt_Alarm_event=RT_NULL;
	rt_uint32_t recved;
		
	while(1)
	{
		//�������ƿ����¼�
		rt_Alarm_event=rt_event_recv(Alarm_event,
									   Alarm_EVENT,					            
									   RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,  
									   RT_WAITING_FOREVER,			          				   
									   &recved);				            
		if(rt_Alarm_event==RT_EOK)
		{		
				if(recved&Alarm_EVENT)
				{				
					BUZZER=1;
				}			
		}	
		rt_thread_delay(3000);
		BUZZER=0;
	}
}

/**
 * @brief ����ʵ���߳�
 * @param ��
 * @retval ���̵߳���Ҫ�ǲɼ����ݣ��������ݽ��й���ʵ�֣���CPAP��APAPģʽ��
 */
static void Feature_thread_entry(void* parameter){		
	//�����¼�����
	rt_err_t rt_ketctrl_event=RT_NULL;
	uint8_t AIR_BLOWER_ON_OFF_FLAG=0;	
	rt_uint32_t recved;
	uint8_t cpap_flag=0;

	//���Ʋ���
	uint32_t CPAP_pressure;
	
	//BiPAP����
	uint32_t Inspiratory_pressure=800; 
	uint32_t Inspiratory_pressure_F=0; //������ѹ      
	uint32_t Inspiratory_time_ms=2000; //����ʱ��,��λ����

	uint32_t Expiratory_pressure=400;  
	uint32_t Expiratory_pressure_F=0;  //������ѹ
	uint32_t Expiratory_time_ms=2000;  //����ʱ��,��λ����
	
	uint32_t Interval_time_ms=300; 	   //���ʱ��,��λ����

	uint32_t all_time=0;						   //��ʱʱ��
	uint8_t bipap_flag=0;			 			   //����������ת̬ת����־

	//״̬�쳣����
	uint8_t al_flag=0;
	
	//��ֵ����
	int32_t size=6;
	int32_t pressure_avr=0;
	int32_t pressure_last_avr=0;
	uint8_t avr_cnt=0;
	
	int32_t pressure_avr_R=0;
		
	//PID����
	float PID_CTR;
 	INCPID_TypeDef PID1;
	
	rt_thread_delay(1000);
	
  while (1)
  {
		//�������ƿ����¼�
		rt_ketctrl_event=rt_event_recv(ketctrl_event,
									   CPAP_START_EVENT,					            
									   RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,  
									   1,			          				   
									   &recved);				            
		if(rt_ketctrl_event==RT_EOK)
		{		
				if(recved==CPAP_START_EVENT)
				{
					cpap_flag=(cpap_flag+1)%2;
					
					if(1==cpap_flag)
					{										
						AIR_BLOWER_ON_OFF_FLAG=AIR_BLOWER_ON;
						TIM_Cmd(GTIM,ENABLE);
						AIR_BLOWER_PIN_HIGH();
					}
					else
					{											
						AIR_BLOWER_ON_OFF_FLAG=AIR_BLOWER_OFF;
						TIM_Cmd(GTIM,DISABLE);
						AIR_BLOWER_PIN_LOW();
					}
				}		
		}		
		
		//CPAPģʽ
		if((AIR_BLOWER_ON_OFF_FLAG==AIR_BLOWER_ON)&&(breathSetting.mode==0))
		{				
			CPAP_pressure=breathSetting.delayBoostPressure;
			PID_SETPRESSURE(&PID1,CPAP_pressure);	
									
			switch(cs_flag)
			{
				case 0:
					    TIM3->CCR2=(uint32_t)(10000*24/100);
							cs_flag=1;				
							if(preassure_really>120)
							{
								cs_flag=1;
							}						
					break;
				case 1:
							PID_CTR=WPID_GetValue(&PID1,(float)preassure);  
							TIM3->CCR2=(uint16_t)(PID_CTR);							
					break; 
				
				default:
					break;
			}	
			rt_thread_delay(20);
		}
		else
		{
			rt_thread_delay(200);
		}
		
		//CMV_BiPAPģʽ
		if((AIR_BLOWER_ON_OFF_FLAG==AIR_BLOWER_ON)&&(breathSetting.mode==1))
		{					
			//�����仯
			Inspiratory_pressure=breathSetting.suctionPressure*100;            //����ѹ��
			Expiratory_pressure=breathSetting.expiratoryPressure*100;					 //����ѹ��
			
			Inspiratory_time_ms=(uint32_t)(breathSetting.maxSuctionTime*1000); //����ʱ��
			Expiratory_time_ms=(uint32_t)(breathSetting.minSuctionTime*1000);	 //����ʱ��
					
			//��ֵ
			if(avr_cnt<size-1)
			{
				avr_cnt++;
				pressure_avr=pressure_last_avr+(preassure_really-pressure_last_avr)/avr_cnt;
			}
			else
			{
				pressure_avr=pressure_last_avr+(preassure_really-pressure_last_avr)/size;
				//rt_kprintf("pressure_avr=%d\r\n",pressure_avr);						
				avr_cnt=0;
				pressure_last_avr=0;
			}
			pressure_last_avr=preassure_really;
			
			//�����¼�����
			//��ֵ�����趨������ѹ
			if(pressure_avr>(Inspiratory_pressure+100))   
			{				
				if(1==Alarm_flag)
				{
					Alarm_flag=0;		
					rt_timer_start(Alarm_timer);	
				}		
			}
					
			//CMV_BiPAP
			switch (bipap_flag)
			{
				/* ��ʱ��ʼ�� */				
				case Keep_time_Init_flag:							
					BiPAP_clear_keep_time();											//���¼�ʱ
					bipap_flag=Inspiratory_flag;									//�л�״̬					
					break;
											
				/* ������ */
				case Inspiratory_flag:						
					PID_SETPRESSURE_R(&PID1,Inspiratory_pressure);	//��ѹ����
					PID_CTR=WPID_GetValue(&PID1,(float)preassure);      
					TIM3->CCR2=(uint16_t)(PID_CTR);				
					all_time=TIM6->CNT/10+time_cnt*1000;			    //��ʱ						
					if(pressure_avr>pressure_avr_R)
					{
						pressure_avr_R=pressure_avr;
					}
													
					//�ж��Ƿ�ʱ
					if(all_time>=Inspiratory_time_ms+50)
					{				
						if(pressure_avr_R>(Inspiratory_pressure+30))
						
						BiPAP_clear_keep_time();											//���¼�ʱ	
						bipap_flag=Expiratory_flag;							  //�л�״̬																		
					}
					break;
				
				/* ������ */
				case Expiratory_flag:				
					PID_SETPRESSURE_F(&PID1,Expiratory_pressure);
				
					PID_CTR=WPID_GetValue(&PID1,(float)preassure);      
					TIM3->CCR2=(uint16_t)(PID_CTR);			
					all_time=TIM6->CNT/10+time_cnt*1000;			    //��ʱ			
				
					if(all_time>=Expiratory_time_ms+50)
					{												
						BiPAP_clear_keep_time();											//���¼�ʱ						
						bipap_flag=Inspiratory_flag;
					}															
					break;	
					
				case Free_time_flag:	
					TIM3->CCR2=(uint16_t)(10000*20/100);										
					all_time=TIM6->CNT/10+time_cnt*1000;			    //��ʱ	
				
					if(all_time>=Interval_time_ms)
					{																				
						BiPAP_clear_keep_time();											//���¼�ʱ						
						bipap_flag=Inspiratory_flag;
					}					
					break;									
				default:
					break;
			}		
			rt_thread_delay(20);
		}
		else
		{
			rt_thread_delay(200);
		}			
	}		
}
/**
 * @brief ��ʪ�ȿ���ʵ���߳�
 * @param ��
 * @retval ���̵߳���Ҫ�ǲɼ��¶����ݣ������¶����ݽ��п��£�����ˮ���е�ˮ�����������¶�
 */
static void humidification_thread_entry(void* parameter){
	rt_thread_delay(10);
	//�����¼�����
	rt_err_t rt_ketctrl_tem_event=RT_NULL;
	uint8_t Tem_Heat_ON_OFF_FLAG=0;	
	rt_uint32_t recved;
	
	int8_t P[8];
	float temperature=0; 
	float Humidity = 0;
	float Temperature=0; 
	int32_t Tem = 0;
  int32_t preassure=0;	
	
	//Tem��Hum
	uint8_t Hum_Room;		//����ʪ��
	uint8_t Tem_Room;		//�����¶�
	
	SDP810.SDP8XX_ReadData(&SDP810,0X3615,P);		//�¶Ȳ���				
	temperature=(float)(((P[3]*256)+P[4])/200.0);	//�¶�����
	
	Tem_Room = temperature;					//�����¶�
	Hum_Room = Humidity;					//����ʪ��
	
	Tem_flag = runningSetting.humidifier;
	Tem_flag_yn = runningSetting.flexMode;	//�Ƿ�������
	
	while(1)
	{
		tempre_value = DS18B20_Get_Temp();	//���µ�ǰʱ���¶�
		//��ds18b20��׼ȷ��ʹ����һʱ�̵��¶�ֵ
		if(tempre_value == 85||tempre_value == 15)tempre_value = tempre_value_pre;		
		tempre_value_pre = tempre_value;	//������һʱ���¶�
		
		Tem_flag_yn = runningSetting.flexMode;	//�����Ƿ�������
		//rt_kprintf("%.2f ,%d ,%d\r\n",tempre_value,Tem_flag_yn,Tem_flag);		
		

		if(Tem_flag_yn == 0)
		{
			Tem_flag = 0;		//�Ƿ������ȣ�0-�����ȣ�1-����
		}
		else
		{
			Tem_flag = runningSetting.humidifier;			//1-΢���ȣ�2-����
		}			

		SDP810.SDP8XX_ReadData(&SDP810,0X3615,P);			//�¶Ȳ���				 
		temperature=(float)(((P[3]*256)+P[4])/200.0);		//�¶�����

		//�������ƿ����¼�
		rt_ketctrl_tem_event=rt_event_recv(ketctrl_tem_event,
										   Tem_START_EVENT,					            
										   RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,  
										   1,			          				   
										   &recved);
		
		if(rt_ketctrl_tem_event==RT_EOK)
		{		
			if(recved==Tem_START_EVENT)
			{
				if(Tem_flag==0)				//������
				{
					rt_kprintf("Stop Heating!\r\n");
					Tem_Heat_ON_OFF_FLAG=Tem_Heat_OFF;
					TIM_Cmd(TM_TIM,ENABLE);
				}
				if(Tem_flag==1)				//΢���£�������40
				{
					rt_kprintf("Start Slight Heating!\r\n");
					Tem_Heat_ON_OFF_FLAG=Tem_Heat_ON;
					TIM_Cmd(TM_TIM,ENABLE);
				}
				if(Tem_flag==2)				//���£�������60
				{
					rt_kprintf("Start Heating!\r\n");
					Tem_Heat_ON_OFF_FLAG=Tem_Heat_ON;
					TIM_Cmd(TM_TIM,ENABLE);
				}
			}		
		}	 
		
		if(Tem_Heat_ON_OFF_FLAG==Tem_Heat_ON)		//��������
		{			
			switch(Tem_flag)
			{
				case 1:		//΢����
					if(tempre_value>Tem_Threshold_1-3)	//ֹͣ���Ⱥ���������3������
					{
						TIM_SetCompare1(TIM2, 11000);		//100��������
					}
					else 
					{
						TIM_SetCompare1(TIM2, 10200*0.9);	//90%����ֵԽСԽ��
					}
				break;
				case 2:		//����
					if(tempre_value>Tem_Threshold_2-3)
					{
						TIM_SetCompare1(TIM2, 11000);		//100��������
					}
					else 
					{
						TIM_SetCompare1(TIM2, 10200*0.8);	//80%
					}
				break;
				default:
					TIM_SetCompare1(TIM2, 11000);		//100��������
				break;
			}								
		}
		else			//������
		{
			rt_thread_delay(10);
			TIM_SetCompare1(TIM2, 11000);		//100
		}
		rt_thread_delay(500);
	}
}

//���ܻ���
static void CI_entry(void* parameter){	
  char buf[64];
  int test;
  static char count = 0;
  static char ifFirstEnter = 1;
  char a = 0;
  char b = 0;
	//rt_device_write(serial, 0, buf, len);  
	//rt_kprintf("CI\r\n");		
  /*
        ��һ������wifiʱʹ�õĴ��룡����
  rt_device_write(serial, 0, "AT+CWMODE=1\r\n", 13); 
  rt_device_write(serial, 0, "AT+CWJAP=\"zq\'s Galaxy S21 5G\",\"mzqmountain\"\r\n", 50);
  */
	rt_thread_delay(1000);
	while(1)
	{

    count++;
    if(ifFirstEnter && count>=8){
      ifFirstEnter = 0;
      // Usart_SendString8266(USART2, "AT+CIPSTART=\"TCP\",\"192.168.218.60\",1111\r\n");
      rt_device_write(serial, 0, "AT+CIPSTART=\"TCP\",\"192.168.240.60\",1111\r\n", 41); 
      a = 1;
      count = 0;

    }
    if(a ==1 &&count>=1){
      // Usart_SendString8266(USART2, "AT+CIPMODE=1\r\n");
      rt_device_write(serial, 0, "AT+CIPMODE=1\r\n", 15);
      b = 1;
      a = 0;
      count = 0;
    }
    if(b ==1 &&count>=1){
      // Usart_SendString8266(USART2, "AT+CIPSEND\r\n");
      rt_device_write(serial, 0, "AT+CIPSEND\r\n", 12);
      b = 0;
    }
    if(!ifFirstEnter&&a==0&&b==0){
          // IntToStrLite(RTC_TimeStruct.RTC_Seconds, string);
      // Usart_SendString8266(USART2, string);
    }


		rt_thread_delay(1000); 
	}
}

//���ݲɼ�
void data_acquisition_entry(void* parameter){
	
	//�����¼�����
	rt_err_t rt_data_acquisition_event=RT_NULL;
	uint8_t DA_ON_OFF_FLAG=0;	
	rt_uint32_t recved;
	uint8_t data_acquisition_flag=0;
	
	char buf[32];
	rt_thread_delay(1000);
	
  while (1)
  {
		//�������ƿ����¼�
		rt_data_acquisition_event=rt_event_recv(data_acquisition_event,
																						data_acquisition_EVENT,					            
																						RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,  
																						1,			          				   
																						&recved);				            
		if(rt_data_acquisition_event==RT_EOK)
		{		
				if(recved==data_acquisition_EVENT)
				{
					data_acquisition_flag=(data_acquisition_flag+1)%2;
					
					if(1==data_acquisition_flag)
					{										
						DA_ON_OFF_FLAG=AIR_BLOWER_ON;
					}
					else
					{											
						DA_ON_OFF_FLAG=AIR_BLOWER_OFF;
					}
				}		
		}		
		if(DA_ON_OFF_FLAG==AIR_BLOWER_ON)
		{
			GZP6859D_ReadPressureModeData(&preassure);          //���ݲɼ�45ms���Ҳɼ�һ��			
			preassure_really=	preassure/16;	
			int len = rt_sprintf(buf, "%d\r\n", preassure_really);
			rt_device_write(serial, 0, buf, len);  // ��������
			rt_thread_delay(20);
		}
		else
		{
			rt_thread_delay(200);
		}
  }
}

/****************************LCD****************************/
void thread_lcd_entry(void* parameter){	
  char keyGet = 0;				//���յ���ֵ
  int i=0;
  int t =0;								//���յ���ֵ	
  nowui = 0;							
  lastui = 0;						 //�����Ƿ����仯
  ui_welcome();
	
	//RTC_Set_Date(25,7,1,2);
	//RTC_Set_Time(12,48,00,00);
	
	char buf[32];
	// int len = rt_sprintf(buf, "hello!\r\n");
	// rt_device_write(serial, 0, buf, len);  
	
  rt_thread_delay(1000); //�ֶ���ʱ���ó� CPU �����߳��л��������ڶ����߳�
	
  while (1)
  {
		RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
		RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);
		
		keyGet=0;
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_13) == 0){rt_thread_delay(20);while(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_13) == 0) keyGet=1; rt_thread_delay(20);}  	
    if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_14) == 0){rt_thread_delay(20);while(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_14) == 0) keyGet=2; rt_thread_delay(20);}  	
    if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_15) == 0){rt_thread_delay(20);while(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_15) == 0) keyGet=3; rt_thread_delay(20);}  	
    if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0){rt_thread_delay(20);while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0) keyGet=4; rt_thread_delay(20);}  
    if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) == 0){rt_thread_delay(20);while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) == 0) keyGet=5; rt_thread_delay(20);} 
		
		//1�� 2�� 3�� 4�� 5OK   PE13�ϣ�14�£�15��PB10�ң�11ȷ��
		t = keyGet;
		lastui = nowui;

		key_UIchange(t, uiSelect);   //����ı亯��,t����λ�ã�uiselectΪ����λ��

		if (nowui == lastui)         
		{				
			if (t == 1 || t == 2)      //����ֵΪ�ϻ���
			{
				key_sonSelect(nowui, t); //�˵�����������1��ǰ����ֵ������2������ֵ
			}
			else
			{
				key_valueChange(t);			 //�ı������ֵ
			}
			
		}
		else
		{
			ui_clear(lastui);    //����������л�����
		}

		t = 0;
		keyGet = 0;
		
		//����
    for (i = 0; i < KEY_TABLE_NUM; i++)
		{
			//nowui��ǰ���ڽ����Ƿ����
      if(nowui == table[i].current){
        now_running_ui = table[i].current_operation;
        break;
      }
    }
    (*now_running_ui)();		
    rt_thread_delay(50);
  } 
}

//�����ʱ��
static void Alarm_timer_callback(void* parameter){
	Alarm_cnt++;
	Alarm_flag=1;
	if(Alarm_cnt==3)
	{
		Alarm_cnt=0;	
		rt_event_send(Alarm_event,Alarm_EVENT);			
	}
	rt_timer_stop(Alarm_timer);			
}

/*�˵�����*/
void key_sonSelect(int nowui,int t){
	//����1
	//uiSelectΪ��ÿһ���������һ��
  if(nowui == 1){	
    if(t == 1)		//���ϼ�
		{
      uiSelect--;
    }
	else			//���¼�
		{
      uiSelect++;
    }	
		
    if(uiSelect>3)
		{
      uiSelect = 2;
    }
	else if(uiSelect == 3)
		{
      uiSelect = 0;
    }
  }
	
  if(nowui == 2)		//����2
  {
    if(t == 1)
		{
      uiSelect--;
    }
	else
		{
      uiSelect++;
    }
		
    if(uiSelect>6)
		{
      uiSelect = 5;
    }else if(uiSelect == 6)
		{
      uiSelect = 0;
    }
  }
		
  if(nowui == 3)		//����3
  {
    breathSettingBuff = breathSetting;
    signalUIselectChange = 1;
    signalValueChange = 1;		//!!!
    if(t == 1)
	{
		uiSelect--;
		if(uiSelect==9)
		{
			uiSelect=6;
		}
    }else
	{
		uiSelect++;
		if(uiSelect==7)
		{
			uiSelect=10;
		}
    }
	
	
    if(uiSelect>11)
		{
      uiSelect = 10;
    }else if(uiSelect == 11)
	  {
      uiSelect = 0;
    }
  }
	
  if(nowui == 4)
	{
    runningSettingBuff = runningSetting;
    signalUIselectChange = 1;
	signalValueChange = 1;		//!!!
    if(t == 1)
		{
      uiSelect--;
    }
	else
		{
      uiSelect++;
    }
		
    if(uiSelect>8)
		{
      uiSelect = 7;
    }
		else if(uiSelect == 8)
		{
      uiSelect = 0;
    }
  }
	
  if(nowui == 5){ // �˴���ʵΪvalueChange���ܣ���Ϊʱ������а���������֮ǰ�෴
    
    signalValueChange = 1;
    if(uiSelect == 0){
      if(t == 1){
        RTC_DateStruct.RTC_Year -= 1;
      }else if(t == 2){
        RTC_DateStruct.RTC_Year += 1;
      }
      if(RTC_DateStruct.RTC_Year>100){
        RTC_DateStruct.RTC_Year = 99;
      }else if(RTC_DateStruct.RTC_Year == 100){
        RTC_DateStruct.RTC_Year = 0;
      }
    }
    if(uiSelect == 1){
      if(t == 1){
        RTC_DateStruct.RTC_Month -= 1;
      }else if(t == 2){
        RTC_DateStruct.RTC_Month += 1;
      }
      if(RTC_DateStruct.RTC_Month>12){ //�·�Ϊbcd�룬��д��ʱ��Ҫע��
        RTC_DateStruct.RTC_Month = 0;
      }else if(RTC_DateStruct.RTC_Month == 0){
        RTC_DateStruct.RTC_Month = 12;
      }
    }
    if(uiSelect == 2){
      if(t == 1){
        RTC_DateStruct.RTC_Date -= 1;
      }else if(t == 2){
        RTC_DateStruct.RTC_Date += 1;
      }
      if(RTC_DateStruct.RTC_Date>31){
        RTC_DateStruct.RTC_Date = 1;
      }else if(RTC_DateStruct.RTC_Date == 0){
        RTC_DateStruct.RTC_Date = 31;  
      }//�����·ݵ��жϺ����ټӰ�
    }
    if(uiSelect == 3){
      if(t == 1){
        RTC_TimeStruct.RTC_Hours -= 1;
      }else if(t == 2){
        RTC_TimeStruct.RTC_Hours += 1;
      }
      if(RTC_TimeStruct.RTC_Hours>24){
        RTC_TimeStruct.RTC_Hours = 23;
      }else if(RTC_TimeStruct.RTC_Hours == 24){
        RTC_TimeStruct.RTC_Hours = 0;
      }
    }
    if(uiSelect == 4){
      if(t == 1){
        RTC_TimeStruct.RTC_Minutes -= 1;
      }else if(t == 2){
        RTC_TimeStruct.RTC_Minutes += 1;
      }
      if(RTC_TimeStruct.RTC_Minutes>60){
        RTC_TimeStruct.RTC_Minutes = 59;
      }else if(RTC_TimeStruct.RTC_Minutes == 60){
        RTC_TimeStruct.RTC_Minutes = 0;
      }
    }
  }
}

void key_UIchange(int t, unsigned char son){
	int i = 0;	//sonΪuiselect
	int j = 0;	
  for (i = 0; !(table[i].current == nowui);i++);  //���ҵ�ǰ����λ��	
	//��һ�㣬UI1
  if(table[i].level == 1)
	{
		//OK������ȷ�ϣ������0��Ϊ1
    if(t == 5)
		{
      nowui = 1;
      uiSelect = 0;
      signalUIchange = 1;
    }                                                                                                                                                                                                                        	
  }	
	//�ڶ���
  if(table[i].level == 2)
	{
    if(t==5)
		{
      if(nowui ==1&&uiSelect == 2)			
			{								
				//����/�ͷſ����߳�												
				rt_event_send(ketctrl_event, CPAP_START_EVENT); // ���͹��ܿ�ʼ�¼� 
				rt_event_send(data_acquisition_event, data_acquisition_EVENT); // ���͹��ܿ�ʼ�¼� 
			}
			
      nowui = (table[i]).sonIndex[son];  //����仯
      uiSelect = 0;
      signalUIchange = 1;
      signalUIselectChange = 1; // �����ת���һ�������ʾ��ͷ������
      signalValueChange = 1;
    }
  }
	//������
  if(table[i].level == 3)
	{
		if(t==5)
		{
			if(nowui == 2)
			{
				nowui = 1;
				uiSelect = 0;
				signalUIchange = 1;
				return;
		 }		
				
		if(nowui == 4 && son == 6)
		{
			nowui = 5;
			signalUIchange = 1;
			signalUIselectChange = 1;
				uiSelect = 0;
			return;
		 }		
		if(son == (table[i].sonNum)-1)
			{
		nowui = (table[i]).sonIndex[son];
		uiSelect = 0;
		signalUIchange = 1;
		}
    }
  }
	//���Ĳ�
  if(table[i].level == 4){
    if(nowui==5)
	{
      if(t==5)
	  {
			nowui = (table[i]).sonIndex[son];
			uiSelect = 0;
			signalUIchange = 1;
			signalUIselectChange = 1; // �����ת���һ�������ʾ��ͷ������
			signalValueChange = 1;
      }
    }
  }	
}

void ui_welcome(void){
  LCD_ShowChinese(90,100,"��ӭʹ��",BLACK,WHITE,32,0);
  rt_thread_delay(1000);
  LCD_ShowString(120, 200, "        ", WHITE, WHITE, 32, 0);
}

void ui_0(void){
	//LCD_ShowIntNum(20,300,0,2,BLACK,WHITE,16);
}

void ui_1(void){	
	uint8_t buf[20];
	uint8_t i=0;
	LCD_ShowChinese(64+24*3, 24, "��", BLACK, WHITE, 24, 0);
	LCD_ShowChinese(64+24*5, 24, "��", BLACK, WHITE, 24, 0);
	LCD_ShowChinese(64+24*7, 24, "��", BLACK, WHITE, 24, 0);
	LCD_ShowIntNum (64+16  , 24, 20, 2, BLACK, WHITE, 24);	
	LCD_ShowChinese(64+24*4+8,24+24+8, "����", BLACK, WHITE, 24, 0);
	i=RTC_DateStruct.RTC_WeekDay;
	switch(i)
	{
		case 1:
			LCD_ShowChinese(64+24*6+8,24+24+8, "һ", BLACK, WHITE, 24, 0);
			break;
		case 2:
			LCD_ShowChinese(64+24*6+8,24+24+8, "��", BLACK, WHITE, 24, 0);
			break;
		case 3:
			LCD_ShowChinese(64+24*6+8,24+24+8, "��", BLACK, WHITE, 24, 0);
			break;
		case 4:
			LCD_ShowChinese(64+24*6+8,24+24+8, "��", BLACK, WHITE, 24, 0);
			break;
		case 5:
			LCD_ShowChinese(64+24*6+8,24+24+8, "��", BLACK, WHITE, 24, 0);
			break;
		case 6:
			LCD_ShowChinese(64+24*6+8,24+24+8, "��", BLACK, WHITE, 24, 0);
			break;
		case 7:
			LCD_ShowChinese(64+24*6+8,24+24+8, "��", BLACK, WHITE, 24, 0);
			break;
		default:			
			break;
	}
	
	sprintf((char*)buf,"%02d",RTC_DateStruct.RTC_Year);
	LCD_ShowString (64+16+16+8, 24, buf , BLACK , WHITE, 24, 0);
	sprintf((char*)buf,"%02d",RTC_DateStruct.RTC_Month);
	LCD_ShowString (64+24*4, 24, buf , BLACK , WHITE, 24, 0);
	sprintf((char*)buf,"%02d",RTC_DateStruct.RTC_Date);
	LCD_ShowString (64+24*6, 24, buf , BLACK , WHITE, 24, 0);

	sprintf((char*)buf,"%02d",RTC_TimeStruct.RTC_Hours);
	LCD_ShowString (64+24  ,24+24+8, buf , BLACK , WHITE, 24, 0);
	LCD_ShowString (64+24*2+4,24+24+8, ":" , BLACK , WHITE, 24, 0);
	sprintf((char*)buf,"%02d",RTC_TimeStruct.RTC_Minutes);
	LCD_ShowString (64+24*3  ,24+24+8, buf , BLACK , WHITE, 24, 0);
	
  if(signalUIchange == 1)
	{
		// LCD_ShowChinese(64, 3*16+8*3, "�ҵ���Ϣ", BLACK, WHITE, 16, 0);
		LCD_ShowChinese(64, 3*24+8*3, "��������", BLACK, WHITE, 24, 0);
		LCD_ShowChinese(64, 4*24+8*4, "��������", BLACK, WHITE, 24, 0);
		LCD_ShowChinese(64, 5*24+8*5, "��ʼ����", BLACK, WHITE, 24, 0);	
		//LCD_ShowChinese(64, 7*16+8*7, "ģʽ", BLACK, WHITE, 16, 0);		
		signalUIchange = 0;
  }
	
	LCD_ShowString(40, 3*24+8*3, "  ", BLACK, WHITE, 24, 0);
	LCD_ShowString(40, 4*24+8*4, "  ", BLACK, WHITE, 24, 0);
	LCD_ShowString(40, 5*24+8*5, "  ", BLACK, WHITE, 24, 0);
	// LCD_ShowString(40, 6*16+8*6, "  ", BLACK, WHITE, 16, 0);
	//LCD_ShowString(40, 7*16+8*7, "  ", BLACK, WHITE, 16, 0);	
	Draw_Circle(50, (uiSelect+3) * 24 + 8 * (uiSelect+3) + 10, 5, BLACK);
	// LCD_Fill(0,7*16+8*6,LCD_W,LCD_H,WHITE);
}

void ui_2(void){
	LCD_ShowIntNum(0,0,2,2,BLACK,WHITE,16);
		
  if(signalUIchange){
      LCD_ShowIntNum(120, 20, 1, 1, BLACK, WHITE, 32);
      LCD_ShowIntNum(120+50, 20, 7, 1, BLACK, WHITE, 32);
      LCD_ShowIntNum(120+100, 20, 30, 2, BLACK, WHITE, 32);

      LCD_ShowChinese(20, 60, "����ʱ��", BLACK, WHITE, 16, 0);
      LCD_ShowString(20+ 16 * 4, 60, "/", BLACK, WHITE, 16, 0);
      LCD_ShowChinese(20 + 16 * 5, 60, "Сʱ", BLACK, WHITE, 16, 0);
      LCD_ShowFloatNum1(120, 60, myInfos.treatTimeD1, 3, BLACK, WHITE, 16);
      LCD_ShowFloatNum1(120+50, 60, myInfos.treatTimeD7, 3, BLACK, WHITE, 16);
      LCD_ShowFloatNum1(120+100, 60, myInfos.treatTimeD30, 3, BLACK, WHITE, 16);

      LCD_ShowString(20, 60+1*16+8*1, "AHI", BLACK, WHITE, 16, 0);
      LCD_ShowFloatNum1(120    , 60+1*16+8*1, myInfos.ahiD1, 3, BLACK, WHITE, 16);
      LCD_ShowFloatNum1(120+50 , 60+1*16+8*1, myInfos.ahiD7, 3, BLACK, WHITE, 16);
      LCD_ShowFloatNum1(120+100, 60+1*16+8*1, myInfos.ahiD30, 3, BLACK, WHITE, 16);

      LCD_ShowChinese(20, 60+2*16+8*2, "�������", BLACK, WHITE, 16, 0);
      LCD_ShowIntNum(120    ,60+2*16+8*2,myInfos.maskD1 ,3,BLACK,WHITE,16 );
      LCD_ShowIntNum(120+50 ,60+2*16+8*2,myInfos.maskD7 ,3,BLACK,WHITE,16 );
      LCD_ShowIntNum(120+100,60+2*16+8*2,myInfos.maskD30,3,BLACK,WHITE,16 );

      LCD_ShowString(20, 60+3*16+8*3, "P90", BLACK, WHITE, 16, 0);
      LCD_ShowChinese(20 + 16 * 3, 60+3*16+8*3, "ѹ��", BLACK, WHITE, 16, 0);
      LCD_ShowFloatNum1(120    ,60+3*16+8*3,myInfos.p90PressureD1 ,3,BLACK,WHITE,16);
      LCD_ShowFloatNum1(120+50 ,60+3*16+8*3,myInfos.p90PressureD7 ,3,BLACK,WHITE,16);
      LCD_ShowFloatNum1(120+100,60+3*16+8*3,myInfos.p90PressureD30,3,BLACK,WHITE,16);

      LCD_ShowChinese(20, 60+4*16+8*4, "�����Ժ���", BLACK, WHITE, 16, 0);
      LCD_ShowString(120    +16*2,60+4*16+8*4, "%", BLACK, WHITE, 16,0);
      LCD_ShowString(120+50 +16*2,60+4*16+8*4, "%", BLACK, WHITE, 16,0);
      LCD_ShowString(120+100+16*2,60+4*16+8*4, "%", BLACK, WHITE, 16,0);
      LCD_ShowIntNum(120 ,    60+4*16+8*4, myInfos.periodicBreathD1, 3, BLACK, WHITE, 16);
      LCD_ShowIntNum(120+50 , 60+4*16+8*4, myInfos.periodicBreathD7, 3, BLACK, WHITE, 16);
      LCD_ShowIntNum(120+100, 60+4*16+8*4, myInfos.periodicBreathD30, 3, BLACK, WHITE, 16);
      signalUIchange = 0;
    }
}

void ui_3(void){
	int i = 0;
	LCD_ShowIntNum(0, 0, 3, 2, BLACK, WHITE, 16);	
  for ( i = 0; i < 10; i++)
  {
    breathSettingP[i] = &breathSetting;
  }
  if(uiSelect < 10){
    breathSettingP[uiSelect] = &breathSettingBuff;
  }
  if(signalUIchange){
    LCD_ShowChinese(16*4, 30+0,         "ģʽѡ��",BLACK,WHITE,24,0 );
		LCD_ShowString (16*4, 30+1*10 +12*1,  "CPAP",BLACK,WHITE,24,0);
    LCD_ShowChinese(16*4, 30+1*10 +12*1,  "         ѹ��",BLACK,WHITE,24,0 );		
    LCD_ShowChinese(16*4, 30+2*10 +12*2,  "��ʱ��ѹʱ��",BLACK,WHITE,24,0 );		
    LCD_ShowChinese(16*4, 30+3*10 +12*3,  "������ѹ",BLACK,WHITE,24,0 );
    LCD_ShowChinese(16*4, 30+4*10 +12*4,  "����ѹ��",BLACK,WHITE,24,0 );
    LCD_ShowChinese(16*4, 30+5*10 +12*5,  "����ʱ��",BLACK,WHITE,24,0 );
    LCD_ShowChinese(16*4, 30+6*10 +12*6,  "����ʱ��",BLACK,WHITE,24,0 );
    LCD_ShowChinese(16*4, 30+9*10+12*7,  "����",BLACK,WHITE,24,0 );
    signalUIchange = 0;
  }
	
  if(signalUIselectChange){
    LCD_ShowString( 20,30+0*10+12*0,"   ",BLACK,WHITE,24,0);
    LCD_ShowString( 20,30+1*10+12*1,"   ",BLACK,WHITE,24,0);
    LCD_ShowString( 20,30+2*10+12*2,"   ",BLACK,WHITE,24,0);
    LCD_ShowString( 20,30+3*10+12*3,"   ",BLACK,WHITE,24,0);
    LCD_ShowString( 20,30+4*10+12*4,"   ",BLACK,WHITE,24,0);
    LCD_ShowString( 20,30+5*10+12*5,"   ",BLACK,WHITE,24,0);
    LCD_ShowString( 20,30+6*10+12*6,"   ",BLACK,WHITE,24,0);
    LCD_ShowString( 20,30+7*10+12*7,"   ",BLACK,WHITE,24,0);
		LCD_ShowString( 20,30+9*10+12*7,"   ",BLACK,WHITE,24,0);
    if(uiSelect<=6){
    Draw_Circle(25, 30 + uiSelect * 10 + 12 * uiSelect + 9, 5, BLACK);}
    else {
      Draw_Circle(25, 30 + 9 * 10 + 12 * 7, 5, BLACK);
    }
    signalUIselectChange = 0;
  }
	
  if(signalValueChange)
	{
    if(breathSettingP[0]->mode == 0){
    LCD_ShowString(220, 30,  "CPAP     ", BLACK, WHITE, 24, 0);
    }
		else if(breathSettingP[0]->mode == 1)
		{
      LCD_ShowString(220, 30,"BiPAP    ", BLACK, WHITE, 24, 0);
    }
		else
		{
      LCD_ShowString(220, 30,"Auto-CPAP", BLACK, WHITE, 24, 0);
    }
		
    LCD_ShowIntNum(220, 	 30+1*10+12*1, breathSettingP[1]->delayBoostPressure, 4, BLACK, WHITE, 24);	
    LCD_ShowIntNum(220,      30+2*10+12*2, breathSettingP[2]->delayBoostTime,3, BLACK, WHITE, 24);
    LCD_ShowString(220+16*3, 30+2*10+12*2, "min", BLACK, WHITE, 24,0);
    LCD_ShowIntNum(220,      30+3*10+12*3, breathSettingP[3]->suctionPressure,2, BLACK, WHITE, 24);
    LCD_ShowString(220+16*2, 30+3*10+12*3, "cmH2O", BLACK, WHITE, 24,0);
    LCD_ShowIntNum(220,      30+4*10+12*4,  breathSettingP[4]->expiratoryPressure,2, BLACK, WHITE, 24);
    LCD_ShowString(220+16*2, 30+4*10+12*4, "cmH2O", BLACK, WHITE, 24,0);
    LCD_ShowFloatNum1(220,   30+5*10+12*5, breathSettingP[5]->maxSuctionTime,3, BLACK, WHITE, 24);
    LCD_ShowString(220+16*3, 30+5*10+12*5, "S", BLACK, WHITE, 24,0);
    LCD_ShowFloatNum1(220,   30+6*10+12*6, breathSettingP[6]->minSuctionTime,3, BLACK, WHITE, 24);
    LCD_ShowString(220+16*3, 30+6*10+12*6, "S", BLACK, WHITE, 24,0);
    signalValueChange = 0;
  }
	
}

void ui_4(void){
	int i = 0;
	LCD_ShowIntNum(0, 0, 4, 2, BLACK, WHITE, 16);
  for ( i = 0; i < 6; i++)
  {
    runningSettingP[i] = &runningSetting;
  }
  if(uiSelect < 6){
    runningSettingP[uiSelect] = &runningSettingBuff;
  }
  if(signalUIchange){
    LCD_ShowString(16*4, 30+0,          "FLEX",BLACK,WHITE,24,0 );
    LCD_ShowChinese(16*4, 30+1*10+12*1,  "��������",BLACK,WHITE,24,0 );
    LCD_ShowChinese(16*4, 30+2*10+12*2,  "��·����",BLACK,WHITE,24,0 );
    LCD_ShowChinese(16*4, 30+3*10+12*3,  "��ʪ",BLACK,WHITE,24,0 );
    LCD_ShowChinese(16*4, 30+4*10+12*4,  "��ʪ��",BLACK,WHITE,24,0 );
    LCD_ShowChinese(16*4, 30+5*10+12*5,  "��·�¶�",BLACK,WHITE,24,0 );
    LCD_ShowChinese(16*4, 30+6*10+12*6,  "ʱ������",BLACK,WHITE,24,0 );
    LCD_ShowChinese(16*4, 30+7*10+12*7,  "�������˵�",BLACK,WHITE,24,0 );
    signalUIchange = 0;
  }
  if(signalUIselectChange){
    LCD_ShowString( 20,30+0*10+12*0,"   ",BLACK,WHITE,24,0);
    LCD_ShowString( 20,30+1*10+12*1,"   ",BLACK,WHITE,24,0);
    LCD_ShowString( 20,30+2*10+12*2,"   ",BLACK,WHITE,24,0);
    LCD_ShowString( 20,30+3*10+12*3,"   ",BLACK,WHITE,24,0);
    LCD_ShowString( 20,30+4*10+12*4,"   ",BLACK,WHITE,24,0);
    LCD_ShowString( 20,30+5*10+12*5,"   ",BLACK,WHITE,24,0);
    LCD_ShowString( 20,30+6*10+12*6,"   ",BLACK,WHITE,24,0);
    LCD_ShowString( 20,30+7*10+12*7,"   ",BLACK,WHITE,24,0);
    LCD_ShowString( 20,30+8*10+12*8,"   ",BLACK,WHITE,24,0);
    Draw_Circle(25, 30 + uiSelect * 10 + 12 * uiSelect + 9, 5, BLACK);
    signalUIselectChange = 0;
  }
  if(signalValueChange){
    // LCD_ShowFloatNum1(220, 30 + 1 * 16 + 8 * 1, breathSettingP[1]->delayBoostPressure, 3, BLACK, WHITE, 16);
    if(runningSettingP[0]->flexMode == 0){
      LCD_ShowChinese(220, 30,  "��", BLACK, WHITE, 24,0);
    }
	else
	{
      LCD_ShowChinese(220, 30,  "��", BLACK, WHITE, 24,0);
    }
    LCD_ShowIntNum(220, 30+1*10+12*1, runningSettingP[1]->maskType,1, BLACK, WHITE, 24);
    if(runningSettingP[2]->pipeType == 1){
      LCD_ShowIntNum(220, 30+2*10+12*2, 15,2, BLACK, WHITE, 24);
    }else{
      LCD_ShowIntNum(220, 30+2*10+12*2, 22,2, BLACK, WHITE, 24);
      }
    if(!(runningSettingP[3]->wetMode)){
      LCD_ShowString(220 + 24 * 2, 30+3*10+12*3, "     ", BLACK, WHITE, 24, 0);
      LCD_ShowChinese(220, 30+3*10+12*3, "�̶�", BLACK, WHITE, 24, 0);

    }else{
      LCD_ShowChinese(220, 30+3*10+12*3, "����Ӧ", BLACK, WHITE, 24, 0);
    }
    LCD_ShowIntNum(220, 30+4*10+12*4, runningSettingP[4]->humidifier,1, BLACK, WHITE, 24);
    LCD_ShowIntNum(220, 30+5*10+12*5, runningSettingP[5]->pipelineTemperature,1, BLACK, WHITE, 24);
  }
}

void ui_5(void){
	LCD_ShowIntNum(0,0,5,2,BLACK,WHITE,16);	
	
  if(signalUIchange){
//    RTC_DateGlobal_temp = RTC_DateGlobal;
//    RTC_TimeGlobal_temp = RTC_TimeGlobal;
    LCD_ShowChinese16x16(90, 40, "ʱ", BLACK, WHITE, 16, 0);
    LCD_ShowChinese16x16(90+16, 40, "��", BLACK, WHITE, 16, 0);
    LCD_ShowChinese16x16(90+16+16, 40, "��", BLACK, WHITE, 16, 0);
    LCD_ShowChinese16x16(90+16+16+16, 40, "��", BLACK, WHITE, 16, 0);
    LCD_ShowChinese16x16(60, 80, "��", BLACK, WHITE, 16, 0);
    LCD_ShowChinese16x16(60+25*1+16*1, 80, "��", BLACK, WHITE, 16, 0);
    LCD_ShowChinese16x16(60+25*2+16*2, 80, "��", BLACK, WHITE, 16, 0);
    LCD_ShowChinese16x16(60+25*3+16*3, 80, "ʱ", BLACK, WHITE, 16, 0);
    LCD_ShowChinese16x16(60+25*4+16*4, 80, "��", BLACK, WHITE, 16, 0);
    // LCD_ShowChar(50, 50, uiSelect, BLACK, WHITE, 16, 0);
    signalUIchange = 0;
  }
  if(signalUIselectChange){
    LCD_ShowString(60, 140, "                             ", WHITE, WHITE, 16, 0);
    Draw_Circle(66 + 25 * uiSelect + 16 * uiSelect, 145, 5, BLACK);

    signalUIselectChange = 0;
  }
  if(signalValueChange){
    LCD_ShowString(60, 120, "    ", WHITE, WHITE, 16, 0);
    LCD_ShowString(60+25*1+16*1, 120, "    ", WHITE, WHITE, 16, 0);
    LCD_ShowString(60+25*2+16*2, 120, "    ", WHITE, WHITE, 16, 0);
    LCD_ShowString(60+25*3+16*3, 120, "    ", WHITE, WHITE, 16, 0);
    LCD_ShowString(60+25*4+16*4, 120, "    ", WHITE, WHITE, 16, 0);
  //  LCD_ShowIntNum(60          , 120,RTC_DateGlobal_temp.RTC_Year , 2, BLACK, WHITE, 16);
  //  LCD_ShowIntNum(60+25*1+16*1, 120,RTC_DateGlobal_temp.RTC_Month , 2, BLACK, WHITE, 16);
  //  LCD_ShowIntNum(60+25*2+16*2, 120,RTC_DateGlobal_temp.RTC_Date , 2, BLACK, WHITE, 16);
  //  LCD_ShowIntNum(60+25*3+16*3, 120,RTC_TimeGlobal_temp.RTC_Hours , 2, BLACK, WHITE, 16);
  //  LCD_ShowIntNum(60+25*4+16*4, 120,RTC_TimeGlobal_temp.RTC_Minutes , 2, BLACK, WHITE, 16);
     LCD_ShowIntNum(60          , 120,RTC_DateStruct.RTC_Year , 2, BLACK, WHITE, 16);
   LCD_ShowIntNum(60+25*1+16*1, 120,RTC_DateStruct.RTC_Month , 2, BLACK, WHITE, 16);
   LCD_ShowIntNum(60+25*2+16*2, 120,RTC_DateStruct.RTC_Date , 2, BLACK, WHITE, 16);
   LCD_ShowIntNum(60+25*3+16*3, 120,RTC_TimeStruct.RTC_Hours , 2, BLACK, WHITE, 16);
   LCD_ShowIntNum(60+25*4+16*4, 120,RTC_TimeStruct.RTC_Minutes , 2, BLACK, WHITE, 16);
    signalValueChange = 0;
  }
}

void flash_writeInfo(void){
    FLASH_Unlock();   
    // ��������
    FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3); // �޸�Ϊ��Ҫ�������͵�ѹ��Χ
    FLASH_ProgramWord(0x080E0000, 0x12345678);//д���־����
    // д������
    for (size_t i = 0; i < sizeof(breathSettingBuff); i += 4) {
        uint32_t data_word = *((uint32_t*)((uint8_t*)(&breathSetting) + i));
        FLASH_ProgramWord(0x080E0010 + i, data_word);
    }
    for (size_t i = 0; i < sizeof(runningSettingStruct); i += 4) {
        uint32_t data_word = *((uint32_t*)((uint8_t*)(&runningSetting) + i));
        FLASH_ProgramWord(0x080E0040 + i, data_word);
    }
    FLASH_Lock();
}

void flash_readInfo(void){
  for (size_t i = 0; i < sizeof(breathSettingBuff); i += 4) {
        *((uint32_t*)((uint8_t*)(&breathSetting) + i)) = *(__IO uint32_t*)(0x080E0010 + i);
  }
  for (size_t i = 0; i < sizeof(runningSettingStruct); i += 4) {
        *((uint32_t*)((uint8_t*)(&runningSetting) + i)) = *(__IO uint32_t*)(0x080E0040 + i);
  }
  if(breathSetting.mode == 255){ //�Ƿ���Ҫ����ֵ�ж�
    IsFirstTimeUse = 1;
    breathSetting = breathSettingBuff;
    runningSetting = runningSettingBuff;
  }
    breathSettingBuff  =breathSetting;//����ϵ�ʱ��һ��ѡ����ʾ�������ݵ�����
  runningSettingBuff = runningSetting;
}

void ui_clear(int last){	
  LCD_Fill(0, 0, LCD_W, 30+10*16+8*10+30, WHITE);//30+10*16+8*10+10�ȶ�
}


void key_valueChange(int key){		//��ֵ
	
	//����ڽ���3����������
  if(nowui == 3)
	{
		signalValueChange = 1;
			
			//��һ������
		if(uiSelect == 0)
		{
			if(key == 3)
			{
				breathSettingBuff.mode -= 1;
			}
			else if(key == 4)
			{
				breathSettingBuff.mode += 1;
			}

			if(breathSettingBuff.mode>4)
			{
				breathSettingBuff.mode = 2;
			}
			else if(breathSettingBuff.mode == 3)
			{
				breathSettingBuff.mode = 0;
			}	
			if(key == 5)
			{
				breathSetting.mode = breathSettingBuff.mode;
			}
		}
		
    if(uiSelect == 1){
      if(key == 3){
        breathSettingBuff.delayBoostPressure -= 50;
      }else if(key == 4){
        breathSettingBuff.delayBoostPressure += 50; 
      }
      if(breathSettingBuff.delayBoostPressure==2050){
        breathSettingBuff.delayBoostPressure = 0;
      }else if(breathSettingBuff.delayBoostPressure >2050){
        breathSettingBuff.delayBoostPressure = 2000;
      }
      if(key == 5){
        breathSetting.delayBoostPressure = breathSettingBuff.delayBoostPressure;
      }
    }
    if(uiSelect == 2){
      if(key == 3){
        breathSettingBuff.delayBoostTime -= 5;
      }else if(key == 4){
        breathSettingBuff.delayBoostTime += 5;
      }
      if(breathSettingBuff.delayBoostTime>110){
        breathSettingBuff.delayBoostTime = 100;
      }else if(breathSettingBuff.delayBoostTime == 110){
        breathSettingBuff.delayBoostTime = 0;
      }
      if(key == 5){
        breathSetting.delayBoostTime = breathSettingBuff.delayBoostTime;
      }
    }
    if(uiSelect == 3){
      if(key == 3){
        breathSettingBuff.suctionPressure -= 1;
      }else if(key == 4){
        breathSettingBuff.suctionPressure += 1;
      }
      if(breathSettingBuff.suctionPressure>31){
        breathSettingBuff.suctionPressure = 30;
      }else if(breathSettingBuff.suctionPressure == 31){
        breathSettingBuff.suctionPressure = 0;
      }
      if(key == 5){
        breathSetting.suctionPressure = breathSettingBuff.suctionPressure;
      }
    }
    if(uiSelect == 4){
      if(key == 3)
			{
        breathSettingBuff.expiratoryPressure -= 1;
      }
			else if(key == 4)
			{
        breathSettingBuff.expiratoryPressure += 1;
      }
      if(breathSettingBuff.expiratoryPressure>31)
		  {
        breathSettingBuff.expiratoryPressure = 30;
      }
			else if(breathSettingBuff.expiratoryPressure == 31)
			{
        breathSettingBuff.expiratoryPressure = 0;
      }
      if(key == 5){
        breathSetting.expiratoryPressure = breathSettingBuff.expiratoryPressure;
      }
    }
    if(uiSelect == 5){
      if(key == 3){
        breathSettingBuff.maxSuctionTime -= 0.1f;
      }else if(key == 4){
        breathSettingBuff.maxSuctionTime += 0.1f;
      }
      if(breathSettingBuff.maxSuctionTime>=10){
        breathSettingBuff.maxSuctionTime = 0;
      }else if(breathSettingBuff.maxSuctionTime < 0){
        breathSettingBuff.maxSuctionTime = 9.9;
      }
      if(key == 5){
        breathSetting.maxSuctionTime = breathSettingBuff.maxSuctionTime;
      }
    }
    if(uiSelect == 6){
      if(key == 3){
        breathSettingBuff.minSuctionTime -= 0.1f;
      }else if(key == 4){
        breathSettingBuff.minSuctionTime += 0.1f;
      }
      if(breathSettingBuff.minSuctionTime>=10){
        breathSettingBuff.minSuctionTime = 0;
      }else if(breathSettingBuff.minSuctionTime < 0){
        breathSettingBuff.minSuctionTime = 9.9;
      }
      if(key == 5){
        breathSetting.minSuctionTime = breathSettingBuff.minSuctionTime;
      }   
    }
  }
	
	//����3����
  if(nowui == 4){
    signalValueChange = 1;
    if(uiSelect == 0){
      if(key == 3)							//FLEX����/�ر�
	  {
        if(--runningSettingBuff.flexMode == 255)
			runningSettingBuff.flexMode = 1;
      }
	  else if(key == 4){
		  if(++runningSettingBuff.flexMode >= 2)
			runningSettingBuff.flexMode = 0;
      }
      if(key == 5){
        runningSetting.flexMode = runningSettingBuff.flexMode;
		  if(runningSetting.flexMode == 1)
		  {
			  rt_event_send(ketctrl_tem_event, Tem_START_EVENT); 	// �����¿ؿ�ʼ�¼� 
//				rt_kprintf("Start Heating!\r\n");
		  }
		  else if(runningSetting.flexMode == 2)
			  rt_kprintf("Stop Heating!\r\n");
      }
    }
    if(uiSelect == 1){						//��������
      if(key == 3){
        runningSettingBuff.maskType -= 1;
      }else if(key == 4){
        runningSettingBuff.maskType += 1;
      }
      if(runningSettingBuff.maskType>3){
        runningSettingBuff.maskType = 1;
      }else if(runningSettingBuff.maskType == 0){
        runningSettingBuff.maskType = 3;
      }
      if(key == 5){
        runningSetting.maskType = runningSettingBuff.maskType;
      }
    }
    if(uiSelect == 2){						//��·����
      if(key == 3){
        runningSettingBuff.pipeType  -= 1;
      }else if(key == 4){
        runningSettingBuff.pipeType  += 1;
      }
      if(runningSettingBuff.pipeType >2){
        runningSettingBuff.pipeType  = 1;
      }else if(runningSettingBuff.pipeType  == 0){
        runningSettingBuff.pipeType  =2;
      }
      if(key == 5){
        runningSetting.pipeType  = runningSettingBuff.pipeType ;
      }
    }
    if(uiSelect == 3){						//��ʪ
      if(key == 3){
        runningSettingBuff.wetMode -= 1;
      }else if(key == 4){
        runningSettingBuff.wetMode += 1;
      }
      if(runningSettingBuff.wetMode>3){
        runningSettingBuff.wetMode = 1;
      }else if(runningSettingBuff.wetMode == 2){
        runningSettingBuff.wetMode = 0;
      }
      if(key == 5){
        runningSetting.wetMode = runningSettingBuff.wetMode;
      }
    }
    if(uiSelect == 4){						//��ʪ����1-΢���ȣ�2-����
      if(key == 3){
		  if(--runningSettingBuff.humidifier == 0)
			  runningSettingBuff.humidifier = 2;
      }else if(key == 4){
		  if(++runningSettingBuff.humidifier == 3)
			  runningSettingBuff.humidifier = 1;
      }
      if(key == 5){
        runningSetting.humidifier = runningSettingBuff.humidifier;
//		  Tem_flag = runningSetting.humidifier;		//1-΢���ȣ�2-����
      }
    }
    if(uiSelect == 5){						//��·�¶�
      if(key == 3){
        runningSettingBuff.pipelineTemperature -= 1;
      }else if(key == 4){
        runningSettingBuff.pipelineTemperature += 1;
      }
      if(runningSettingBuff.pipelineTemperature>3){
        runningSettingBuff.pipelineTemperature = 1;
      }else if(runningSettingBuff.pipelineTemperature == 0){
        runningSettingBuff.pipelineTemperature = 3;
      }
      if(key == 5){
        runningSetting.pipelineTemperature = runningSettingBuff.pipelineTemperature;
      }
    }
  }	
  if(nowui == 5){
    signalUIselectChange=1;
    if(key == 3){
      uiSelect--;
    }else if(key == 4){
      uiSelect++;
    }
    if(uiSelect>5){
      uiSelect = 4;
    }else if(uiSelect == 5){
      uiSelect = 0;
    }
  }
  if(key ==5)
	{
    flash_writeInfo();
	}
}
/* �������ݻص����� */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size){
    /* ���ڽ��յ����ݺ�����жϣ����ô˻ص�������Ȼ���ͽ����ź��� */
    rt_sem_release(&rx_sem);

    return RT_EOK;
}

void Debug_USART_Config8266(void){
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
		
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

  /* ʹ�� USART ʱ�� */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
  /* GPIO��ʼ�� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* ����Tx����Ϊ���ù���  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2  ;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* ����Rx����Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
 /* ���� PXx �� USARTx_Tx*/
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);

  /*  ���� PXx �� USARTx__Rx*/
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_USART2);
  
  /* ���ô�DEBUG_USART ģʽ */
  /* ���������ã�DEBUG_USART_BAUDRATE */
  USART_InitStructure.USART_BaudRate = 115200;
  /* �ֳ�(����λ+У��λ)��8 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  /* ֹͣλ��1��ֹͣλ */
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  /* У��λѡ�񣺲�ʹ��У�� */
  USART_InitStructure.USART_Parity = USART_Parity_No;
  /* Ӳ�������ƣ���ʹ��Ӳ���� */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  /* USARTģʽ���ƣ�ͬʱʹ�ܽ��պͷ��� */
  USART_InitStructure.USART_Mode =/* USART_Mode_Rx | */USART_Mode_Tx;
  /* ���USART��ʼ������ */
  USART_Init(USART2, &USART_InitStructure); 
	
  /* Ƕ�������жϿ�����NVIC���� */
  // NVIC_InitTypeDef NVIC_InitStructure;
  
  // /* Ƕ�������жϿ�������ѡ�� */
  // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  // /* ����USARTΪ�ж�Դ */
  // NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  // /* �������ȼ�Ϊ1 */
  // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  // /* �����ȼ�Ϊ1 */
  // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  // /* ʹ���ж� */
  // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  // /* ��ʼ������NVIC */
  // NVIC_Init(&NVIC_InitStructure);
  
	/* ʹ�ܴ��ڽ����ж� */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
  /* ʹ�ܴ��� */
  USART_Cmd(USART2, ENABLE);
}
