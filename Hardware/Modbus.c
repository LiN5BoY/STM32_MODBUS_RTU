#include "stm32f10x.h"
#include "Timer.h"
#include "Serial.h"
uint8_t Serial_TxPacket[100] = {0};                        // 发送内容
extern uint8_t Serial_RxPacket[100];                       // 接收内容
extern uint16_t Serial_RxLength;

extern uint16_t modbus_io[100];                                   // modbus寄存器内数据
// uint16_t modbus_id = 0X01;                              // id号
uint16_t modbus_function;                                  // 功能码
uint16_t modbus_check;                                     // 校验位
uint16_t modbus_packege_times = 0;                         // 总包计数
uint16_t CRC_check_result;                                 // CRC校验的结果



uint16_t calculate_crc16(const uint8_t *data, size_t len) {
    // printf("%d\n",len);
    
    // 初始化crc为0xFFFF
    uint16_t crc = 0xFFFF;

    // 循环处理每个数据字节
    for (size_t i = 0; i < len; i++) {
        // 将每个数据字节与crc进行异或操作
        crc ^= data[i];

        // 对crc的每一位进行处理:如果最低位为1，则右移一位并执行异或0xA001操作(即0x8005按位颠倒后的结果)
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } 
            // 如果最低位为0，则仅将crc右移一位
            else {
                crc = crc >> 1;
            }
        }
    }
    return crc;
}


void Data_Funcion_3(void){
    Serial_TxPacket[0] = Serial_RxPacket[0];       // ID
    Serial_TxPacket[1] = Serial_RxPacket[1];       // 功能码
    // 字节长度，根据接收的内容4，5位来判断
    Serial_TxPacket[2] = (Serial_RxPacket[4] << 8 | Serial_RxPacket[5]) * 2;


    for(modbus_packege_times = 0;modbus_packege_times<Serial_TxPacket[2];modbus_packege_times+=2)
    {
        Serial_TxPacket[3+modbus_packege_times] = modbus_io[modbus_packege_times / 2] >> 8;
        Serial_TxPacket[4+modbus_packege_times] = modbus_io[modbus_packege_times / 2];
    }         
    // 校验码
    CRC_check_result = calculate_crc16(Serial_TxPacket,Serial_TxPacket[2] + 3);
    Serial_TxPacket[3+modbus_packege_times] = (CRC_check_result) & 0xFF;
    Serial_TxPacket[4+modbus_packege_times] = (CRC_check_result>>8) & 0xFF;


      

    Serial_SendArray(Serial_TxPacket,5+modbus_packege_times);
    return ;
}


void Data_Funcion_6(void){
    // Serial_TxPacket[0] = Serial_RxPacket[0];       // ID
    // Serial_TxPacket[1] = Serial_RxPacket[1];               // 功能码
    modbus_io[Serial_RxPacket[3] - 1] = Serial_RxPacket[4];         
    modbus_io[Serial_RxPacket[3]] = Serial_RxPacket[5];
    
    Serial_SendArray(Serial_RxPacket,Serial_RxLength);
    return ;
}


void Data_Resolve(void){

    // 需增加校验位计算
    modbus_check = calculate_crc16(Serial_RxPacket,Serial_RxLength-2);
    
    if(modbus_check != 0)                                  // 校验是否通过
    {   
        Serial_TxPacket[0] = 0x01;                         // 预设id
        if(Serial_RxPacket[0] == Serial_TxPacket[0]){           // 确认id号是否一致

            modbus_function = Serial_RxPacket[1];
            switch(modbus_function)
            { 
                case 3 :                            // 根据03功能码，主机要求从机反馈内容 
                    Data_Funcion_3();
                    break;
                case 6 :
                    Data_Funcion_6();
                    break;
                case 16 :
                    Serial_SendArray(Serial_TxPacket,Serial_RxLength);
                    break;
                default :
                    break;
            }

        }

    }
    Serial_RxFlag = 0;
    Serial_RxLength = 0;
}




