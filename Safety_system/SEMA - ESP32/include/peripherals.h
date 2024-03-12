#include <sys_def.h>
#include <UART_handler.hpp>

void PWM_STATUS_Toggle(void);
void Init_Digital(void);
void Init_PWM(void);
void Init_Interrupt(void);
void IRAM_ATTR Ext_INT1_ISR(void);
void IRAM_ATTR Ext_INT2_ISR(void);
void IRAM_ATTR Ext_INT3_ISR(void);
