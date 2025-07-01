#include "stm32f4xx.h"
void write_to_flash(uint32_t address, uint8_t* data,int size);
void read_from_flash(uint32_t address, uint8_t* data,int size);
uint32_t read_from_flash_word(uint32_t address);
typedef struct 
{
  //breathSettingStruct
  unsigned char mode;//0 CPAP 1S 2T 3s/y
  float delayBoostPressure;
  unsigned char delayBoostTime; //0-100
  unsigned char suctionPressure;
  unsigned char expiratoryPressure;
  float maxSuctionTime;
  float minSuctionTime;
  unsigned char inspiratorySensitivity;//2-5
  unsigned char expiratorySensitivity;//2-5
  unsigned char breathFrenquency;

  //runningSettingStruct
  unsigned char flexMode;//0,1
  unsigned char maskType;//1,2,3
  unsigned char pipeType;//1 15 2 22
  unsigned char wetMode;//0 adapt 1 fixed
  unsigned char humidifier;//1,2,3
  unsigned char pipelineTemperature;//1,2,3,
}flash_infoStruct ;
