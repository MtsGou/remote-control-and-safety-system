#include <sys_def.h>

///////////////////////////////////////////////////////////////////////////////////////
/* ////////////////// FUNCOES PARA ATUACAO E ACIONAMENTO ////////////////////////////*/


void AJUSTAR_DUTYCYCLE(void);
void Acionamento(int rele);
void CPU_Handler(void);
void STM_Feedback_Handler(void);
void EMERGENCY_STM_NO_RETURN(int MODE_STATUS);
void DETECCAO_FALHA(void);