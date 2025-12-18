#ifndef __INC_MODBUS_T5L_H
#define __INC_MODBUS_T5L_H

void UART5_Init(void);
void ModBus_RTU_Slave_Init(void);
void ModBus_RTU_Poll_Slave(void);
void ModBus_RTC_ISR(void);
void test_data(void);

#endif