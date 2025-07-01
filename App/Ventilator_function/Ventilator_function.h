#ifndef __Ventilator_function_H__
#define __Ventilator_function_H__

#include "stm32f4xx.h"
#include "air_blower.h"

#define  ERR  0
#define  OK   1

#define PID_LIMIT_MAX 10000
#define PID_LIMIT_MIN 0

#define I_LIMIT_MAX 4000
#define I_LIMIT_MIN 0

#define PID_Tem_LIMIT_MAX 2000	//8000
#define PID_Tem_LIMIT_MIN 0	//10000

// 定义PID结构体
typedef struct {
    float Kp; // 比例系数
    float Ki; // 积分系数
    float Kd; // 微分系数
    float setpoint; // 目标压力
    float last_error; // 上次误差
    float error_sum; //误差之和
} INCPID_TypeDef;

// 定义温控PID结构体
typedef struct {
    float Kp; // 比例系数
    float Ki; // 积分系数
    float Kd; // 微分系数
    float Tem_target; // 目标温度
    float integral; // 积分项
    float last_error; // 上次误差
//    float pre_error; // 上上次误差
    float error_sum; //误差之和
//    uint16_t thrsod;
} TemPID_TypeDef;

typedef struct {
	float P_now;
	float P_last;
	float K;
	float R_cov;
	float Q_cov;
	float x_out;
} KF_Struct;

void PID_SETPRESSURE_RA(INCPID_TypeDef* pid,int32_t setpoint,uint32_t falling_pressure);
void PID_SETPRESSURE_R(INCPID_TypeDef* pid,int32_t setpoint);
void PID_SETPRESSURE_F(INCPID_TypeDef* pid,int32_t setpoint);
void PID_SETPRESSURE(INCPID_TypeDef* pid,int32_t setpoint);		//压力控制pid设置
void PID_SETTEMPERATURE(TemPID_TypeDef* pid,int32_t setpoint);	//温控pid设置

void  PID_SET(INCPID_TypeDef* pid, float Kp, float Ki, float Kd, float setpoint);		//设置PID参数
void  PID_SET_Tem(TemPID_TypeDef* pid, float Kp, float Ki, float Kd, float setpoint);	//设置温控PID参数

float WPID_GetValue(INCPID_TypeDef* pid, float feedback);		//获取返回值
float WPID_GetValue_Tem(TemPID_TypeDef* pid, float feedback);	//获取温控返回值

void PID_CLEAR(INCPID_TypeDef* pid);		//PID参数置0
void PID_CLEAR_Tem(TemPID_TypeDef* pid);	//温控PID参数置0

void KF_Struct_Init(KF_Struct* KFS,float pressure);	//卡尔曼滤波初始化
float KMFilter(KF_Struct* KFS,float z);				//卡尔曼滤波


#endif

