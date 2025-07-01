#include "stm32f4xx.h"
#include "myflash.h"
#include <stdio.h>
void write_to_flash_word(uint32_t address, uint32_t data) {
    FLASH_Unlock();
    
    // 擦除扇区
    FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3); // 修改为需要的扇区和电压范围
    
    // 写入数据
    FLASH_ProgramWord(address, data);
    
    FLASH_Lock();
}
uint32_t read_from_flash_word(uint32_t address) {
    return *(__IO uint32_t*)address; // 读取数据
}
void write_to_flash(uint32_t address, uint8_t* data,int size) {
    FLASH_Unlock();
    
    // 擦除扇区
    FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3); // 修改为需要的扇区和电压范围

    FLASH_ProgramWord(0x080E0000, 0x12345678);//写入标志数据

    // 写入数据
    for (size_t i = 0; i < size; i += 4) {
        uint32_t data_word = *((uint32_t*)((uint8_t*)data + i));
        FLASH_ProgramWord(address + i, data_word);
    }
    
    FLASH_Lock();
}

void read_from_flash(uint32_t address, uint8_t* data,int size) {
    for (size_t i = 0; i < size; i += 4) {
        *((uint32_t*)((uint8_t*)data + i)) = *(__IO uint32_t*)(address + i);
    }

}

