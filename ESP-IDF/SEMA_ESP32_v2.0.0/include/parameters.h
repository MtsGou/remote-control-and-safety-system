#ifndef __MAIN_H
    #include <main.h>
#endif

using namespace std;

/* ================================= FUNCOES EXTRAS ======================================*/

double power(double a, double b);

/* ======================= ELEMENTOS PARA INTERACAO E DEPURACAO ===========================*/

// TAMANHO BUFFER RX
static const int RX_BUF_SIZE = 1024;

// BUFFER PARA RECEBIMENTO RX SERIAL
extern uint8_t *BUFFER_RX_SERIAL;

/* ======================= VARIAVEIS PARA GERACAO DOS SINAIS PWM ==========================*/

// BITS DO PWM PARA USAR A FUNCAO LEDCWRITE
extern const uint32_t size_bits;

// DUTY CYCLE DO FREIO E DO ACELERADOR

// FREIO
extern volatile float DUTYCYCLE_MAX_EMG;        // limite maximo Duty Cycle Freio
extern volatile float DUTYCYCLE_MIN_EMG;        // limite minimo Duty Cycle Freio
extern volatile uint8_t PERCENT_EMG;            // % do duty cycle dentro dos limites
extern volatile float FINAL_DUTYCYCLE_EMG;      // Duty cycle efetivo
extern volatile uint32_t PWM_EMG;               // PWM efetivo para funcao ledcwrite

// ACELERADOR
extern volatile float DUTYCYCLE_MAX_ACEL;       // limite maximo Duty Cycle Acel
extern volatile float DUTYCYCLE_MIN_ACEL;       // limite minimo Duty Cycle Freio
extern volatile uint8_t PERCENT_ACEL;           // % do dc dentro nos limites
extern volatile float FINAL_DUTYCYCLE_ACEL;     // Duty cycle efetivo
extern volatile uint32_t PWM_ACEL;              // PWM efetivo para funcao ledcwrite


/* ================================= DADOS DE CONEXAO ====================================*/

// PREENCHA COM O LOGIN DA REDE
extern string LOGIN;

// PREENCHA COM A SENHA DA REDE
extern string SENHA;

// ESPECIFICACOES REDE
extern uint8_t IP[4];
extern uint8_t GATE[4];
extern uint8_t SUBMASK[4];

/* ================================= DADOS DE PROTOCOLO ==================================*/

// IDENTIFICACAO DO CAMINHAO
extern string ID;
// Divisoria para capturar Checksum
extern string CHECKSUM_DIV;
// Start byte
extern string START_BYTE;
// End byte
extern string END_BYTE;

// =============================== VARIAVEIS DE SISTEMA ==================================*/

// VARIAVEL DE ACIONAMENTO DO RELE
extern volatile bool ESTADO_RELE;

// CONTADOR DE CLIENTES CONECTADOS
extern int NUM_CLIENTES_CONECTADOS;

// CONTADOR DE TEMPO DE RECONEXAO APOS PERDER CONEXAO
extern int COUNT_RECONNECT;

// TEMPO TIMEOUT DE RECONEXAO
extern unsigned int long TIMEOUT_NETWORK_FAIL;

// CASO A CONEXAO FALHE E NAO CONSIGA RECONECTAR APOS TIMEOUT
extern bool ABORT_RECONNECT;

// VERIFICACAO DOS SINAIS QUE VEM DO STM, UPA e CPU
extern unsigned int long TIME_LAST_SWITCH_ALIVE_STM;
extern bool STM_OK;

extern unsigned int long TIME_LAST_SWITCH_ALIVE_CPU;
extern bool CPU_OK;

extern unsigned int long TIME_LAST_SWITCH_ALIVE_UPA;
extern bool UPA_OK;

// VARIAVEL PARA TOGGLE PIN STATUS DO PROPRIO ESP32
extern bool TOGGLE_STATUS;

// HISTORICO DE OCORRENCIA DE FALHA NO STM
extern bool HISTORICO_FALHA_STM;

// HISTORICO DE ACIONAMENTO PELO CPU
extern bool HISTORICO_CPU_ORDEM_FREIO;

// HISTORICO DE FEEDBACK STM
extern bool HISTORICO_STM_FEEDBACK;

// Buffer para o relatorio 
extern string BUFFER_REPORT;

/* ====================================================================================*/