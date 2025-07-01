#ifndef LCDMAIN_H
#define LCDMAIN_H

#include "board.h"

void lcdmain(void *pvParameters);

typedef struct
{
	unsigned char current;  				 //��ǰ����λ��
  unsigned char sonNum;						 //��������
  unsigned char sonIndex[12]; 		 //������Ӧ��ֵ
  unsigned char level;						 //����0����һ����
	void (*current_operation)(void); //����ָ�룬ָ���Ӧ����
} key_table;

typedef struct 
{
  float treatTimeD1;
  float treatTimeD7;
  float treatTimeD30;

  float ahiD1;
  float ahiD7;
  float ahiD30;

  unsigned char maskD1;
  unsigned char maskD7;
  unsigned char maskD30;

  unsigned char periodicBreathD1;
  unsigned char periodicBreathD7;
  unsigned char periodicBreathD30;

  float p90PressureD1;
  float p90PressureD7;
  float p90PressureD30;
} myInfosStrust;

typedef struct 
{
  unsigned char mode;//0 CPAP 1S 2T 3s/y
  uint32_t delayBoostPressure;   
  unsigned char delayBoostTime; //0-100
  uint32_t suctionPressure;    
  uint32_t  expiratoryPressure; 
  float maxSuctionTime;
  float minSuctionTime;
  unsigned char inspiratorySensitivity;//2-5
  unsigned char expiratorySensitivity;//2-5
  unsigned char breathFrenquency;
} breathSettingStruct;

typedef struct 
{
  unsigned char flexMode;//0,1
  unsigned char maskType;//1,2,3
  unsigned char pipeType;//1 15 2 22
  unsigned char wetMode;//0 adapt 1 fixed
  unsigned char humidifier;//1,2,3
  unsigned char pipelineTemperature;//1,2,3,
} runningSettingStruct;
#define KEY_TABLE_NUM 6

/*�ҵ���Ϣ�����ݴ洢*/
typedef struct 
{
  int	tm_mday;
  int	tm_mon;
  int	tm_year;

  float treatTime;
  float ahi;
  unsigned char mask;
  unsigned char periodicBreasizeth;
  float p90Pressure;
} myInfosFlashStruct;


#endif
