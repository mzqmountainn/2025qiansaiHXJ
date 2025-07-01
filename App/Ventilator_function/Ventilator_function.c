#include "Ventilator_function.h"

void PID_SETTEMPERATURE(TemPID_TypeDef* pid,int32_t setpoint)
{
	//分段PID设置
	if(setpoint==1) //3C
		{PID_SET_Tem(pid,600,0,0,setpoint);}					
	if(setpoint==2) //6C
		{PID_SET_Tem(pid,300,0,0,setpoint);}							
}

// PID_Tem控制器初始化
void PID_SET_Tem(TemPID_TypeDef* pid, float Kp, float Ki, float Kd, float setpoint) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->Tem_target = setpoint;
    pid->integral = 0.0f;
    pid->last_error = 0.0f;
//    pid->pre_error = 0.0f;
//    pid->thrsod=7200;
}



// PID_Tem清除函数
void PID_CLEAR_Tem(TemPID_TypeDef* pid)
{
	pid->integral=0.0f; // 积分项
  pid->last_error=0.0f; // 上次误差
  pid->error_sum=0.0f; //误差之和
}




//位置式PID-Tem
float WPID_GetValue_Tem(TemPID_TypeDef* pid, float feedback)
//PID_3:kp=600,kd=
//PID_6:kp=300,kd=
{
	float cur_error,increment;
	uint32_t OUTPUT;
	cur_error=pid->Tem_target-feedback;
	//当前误差值=目标值-反馈值（当前温度），一般目标值>反馈值，得数为+。
	//cur_error->0~6C >> increment->0~2000 >> OUTPUT->10000~8000
//	if(cur_error<0)cur_error=cur_error*2;
	pid->error_sum+=cur_error;

	if((pid->Ki!=0)&&(pid->error_sum>(PID_Tem_LIMIT_MAX/pid->Ki))) pid->error_sum=PID_Tem_LIMIT_MAX/pid->Ki;	
	if((pid->Ki!=0)&&(pid->error_sum<(PID_Tem_LIMIT_MIN/pid->Ki))) pid->error_sum=PID_Tem_LIMIT_MIN/pid->Ki;	//积分限幅

	increment=pid->Kp*cur_error+pid->Ki*pid->error_sum+pid->Kd*(cur_error-pid->last_error);//计算返回值
	pid->last_error=cur_error;

	if(increment<PID_Tem_LIMIT_MIN)//PID->out限幅
	{
		increment=PID_Tem_LIMIT_MIN;//0，不加热
	}
	if(increment>PID_Tem_LIMIT_MAX)
	{
	increment=PID_Tem_LIMIT_MAX;//2000，加热最大值，80%
	}
	
	OUTPUT = 10000-increment;//cur_error越大，越要热，返回值越小

	return OUTPUT;//返回加热pwm，8000-10000
}



//卡尔曼滤波
void KF_Struct_Init(KF_Struct* KFS,float pressure)
{
	KFS->P_now	=0;
	KFS->P_last	=0.02;
	KFS->K		=0;
	KFS->Q_cov	=0.002;//过程激励噪声协方差,参数可调
	KFS->R_cov	=0.02;//测量噪声协方差，与仪器测量的性质有关，参数可调
	KFS->x_out	=pressure;
}


float KMFilter(KF_Struct* KFS,float z)
{
	KFS->P_now = KFS->P_last + KFS->Q_cov;
    KFS->K = KFS->P_now / (KFS->P_now + KFS->R_cov );
    KFS->x_out = KFS->x_out + KFS->K * (z - KFS->x_out);
    KFS->P_last = (1 - KFS->K)* KFS->P_now;
    return KFS->x_out;
}

// PID控制器初始化
void PID_SET(INCPID_TypeDef* pid, float Kp, float Ki, float Kd, float setpoint) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->setpoint = setpoint;
    pid->last_error = 0.0f;		
}

//位置式PID
float WPID_GetValue(INCPID_TypeDef* pid, float feedback)
{
     float cur_error,increment;
     cur_error=pid->setpoint-feedback;
		 if(cur_error<0)cur_error=cur_error*2;
     pid->error_sum+=cur_error;

     if((pid->Ki!=0)&&(pid->error_sum>(PID_LIMIT_MAX/pid->Ki))) pid->error_sum=PID_LIMIT_MAX/pid->Ki;
	   if((pid->Ki!=0)&&(pid->error_sum<(PID_LIMIT_MIN/pid->Ki))) pid->error_sum=PID_LIMIT_MIN/pid->Ki;//积分限幅

     increment=pid->Kp*cur_error+pid->Ki*pid->error_sum+pid->Kd*(cur_error-pid->last_error);
     pid->last_error=cur_error;

     if(increment<PID_LIMIT_MIN) {increment=PID_LIMIT_MIN;}
	   if(increment>PID_LIMIT_MAX) {increment=PID_LIMIT_MAX;} //PID->out限幅
     
     return increment;
}


// PID清除函数
void PID_CLEAR(INCPID_TypeDef* pid)
{
    pid->last_error=0.0f; // 上次误差
    pid->error_sum=0.0f; //误差之和
}

/*********** CPAP_PID *************/
void PID_SETPRESSURE(INCPID_TypeDef* pid,int32_t setpoint)
{
	//分段PID设置
	if(setpoint<600) 
		{PID_SET(pid,0.2,0.045,0.18,setpoint*16);}					
	if(setpoint>=600&&setpoint<1000) 
		{PID_SET(pid,0.2,0.04,0.08,setpoint*16);}						
	if(setpoint>=1000&&setpoint<1400) 
		{PID_SET(pid,0.2,0.025,0.05,setpoint*16);}			
	if(setpoint>=1400&&setpoint<1800) 
		{PID_SET(pid,0.2,0.02,0.03,setpoint*16);}				
	if(setpoint>=1800) 
		{PID_SET(pid,0.2,0.02,0.02,setpoint*16);}			
}

/*********** BiCPAP_PID *************/
//分段PID设置,下降
void PID_SETPRESSURE_F(INCPID_TypeDef* pid,int32_t setpoint)
{
	if(setpoint<1100)  {PID_SET(pid,0.2,0.02,0.10,setpoint*16);}
	if(setpoint>=1100) {PID_SET(pid,0.2,0.0185,0.08,setpoint*16);}		
}

//分段PID设置,上升
void PID_SETPRESSURE_R(INCPID_TypeDef* pid,int32_t setpoint)
{	
	
	if(setpoint<1100)  {PID_SET(pid,0.2,0.02,0.10,setpoint*16);}
	if(setpoint>=1100) {PID_SET(pid,0.2,0.0185,0.08,setpoint*16);}		
}

void PID_SETPRESSURE_RA(INCPID_TypeDef* pid,int32_t setpoint,uint32_t falling_pressure)
{	
	if(falling_pressure>=400)
	{
		if(setpoint<1100)  {PID_SET(pid,0.2,0.02,0.10,setpoint*16);}
		if(setpoint>=1100) {PID_SET(pid,0.2,0.0185,0.08,setpoint*16);}		
	}
	else if(falling_pressure<400)
	{
		if(setpoint<1100)  {PID_SET(pid,0.35,0.03,0.10,setpoint*16);}
		if(setpoint>=1100) {PID_SET(pid,0.2,0.0185,0.08,setpoint*16);}			
	}
	
}



