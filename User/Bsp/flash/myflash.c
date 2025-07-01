#include "stm32f4xx.h"
#include "myflash.h"
#include <stdio.h>
void write_to_flash_word(uint32_t address, uint32_t data) {
    FLASH_Unlock();
    
    // ��������
    FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3); // �޸�Ϊ��Ҫ�������͵�ѹ��Χ
    
    // д������
    FLASH_ProgramWord(address, data);
    
    FLASH_Lock();
}
uint32_t read_from_flash_word(uint32_t address) {
    return *(__IO uint32_t*)address; // ��ȡ����
}
void write_to_flash(uint32_t address, uint8_t* data,int size) {
    FLASH_Unlock();
    
    // ��������
    FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3); // �޸�Ϊ��Ҫ�������͵�ѹ��Χ

    FLASH_ProgramWord(0x080E0000, 0x12345678);//д���־����

    // д������
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

