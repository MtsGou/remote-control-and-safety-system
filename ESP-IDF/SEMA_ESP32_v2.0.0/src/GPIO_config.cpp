#include "GPIO_config.h"
#include "parameters.h"

TaskHandle_t ISR_STM = NULL;
TaskHandle_t ISR_CPU = NULL;
TaskHandle_t ISR_UPA = NULL;

void IRAM_ATTR gpio_interrupt_handler_STM(void *args)
{
  xTaskResumeFromISR(ISR_STM);
}

void IRAM_ATTR gpio_interrupt_handler_UPA(void *args)
{
  xTaskResumeFromISR(ISR_UPA);
}

void IRAM_ATTR gpio_interrupt_handler_CPU(void *args)
{
  xTaskResumeFromISR(ISR_CPU);
}

void Task_ISR_STM(void *args)
{
  while (1)
  {
    vTaskSuspend(NULL);
    int value = gpio_get_level(OUT_PWM_ESP);
    /*switch (value)
    {
    case 1:
      printf("\r\nSTM pressionado!");
      break;
    case 0:
      printf("\r\nSTM solto!");
      break;
    }*/
  }
}

void Task_ISR_CPU(void *args)
{
  while (1)
  {
    vTaskSuspend(NULL);
    int value = gpio_get_level(IN_PWM_CPU);
    /*switch (value)
    {
    case 1:
      printf("\r\nCPU pressionado!");
      break;
    case 0:
      printf("\r\nCPU solto!");
      break;
    }*/
  }
}

void Task_ISR_UPA(void *args)
{
  while (1)
  {
    vTaskSuspend(NULL);
    int value = gpio_get_level(IN_PWM_UPA);
    /*switch (value)
    {
    case 1:
      printf("\r\nUPA pressionado!");
      break;
    case 0:
      printf("\r\nUPA solto!");
      break;
    }*/
  }
}

void init_Digital_GPIO_pins()
{

  gpio_pad_select_gpio(RELE_FREIO_ESP);
  gpio_set_direction(RELE_FREIO_ESP, GPIO_MODE_OUTPUT);
  gpio_pad_select_gpio(ESP_IN_CLIENTE);
  gpio_set_direction(ESP_IN_CLIENTE, GPIO_MODE_OUTPUT);
  gpio_pad_select_gpio(CONNECT_ESP_IN);
  gpio_set_direction(CONNECT_ESP_IN, GPIO_MODE_OUTPUT);
  gpio_pad_select_gpio(IN_PWM_ESP);
  gpio_set_direction(IN_PWM_ESP, GPIO_MODE_OUTPUT);
  gpio_pad_select_gpio(IO_OUT_ESP_2);
  gpio_set_direction(IO_OUT_ESP_2, GPIO_MODE_OUTPUT);
  gpio_pad_select_gpio(OUT_EXTRA_CPU);
  gpio_set_direction(OUT_EXTRA_CPU, GPIO_MODE_OUTPUT);
  gpio_pad_select_gpio(IN_EXTRA_CPU);
  gpio_set_direction(IN_EXTRA_CPU, GPIO_MODE_INPUT);
  gpio_pad_select_gpio(IO_OUT_ESP_1);
  gpio_set_direction(IO_OUT_ESP_1, GPIO_MODE_INPUT);

  gpio_set_level(RELE_FREIO_ESP, LOW);
  gpio_set_level(ESP_IN_CLIENTE, LOW);
  gpio_set_level(CONNECT_ESP_IN, LOW);
  gpio_set_level(IN_PWM_ESP, LOW);
  gpio_set_level(IO_OUT_ESP_2, LOW);
  gpio_set_level(OUT_EXTRA_CPU, LOW);
}

void init_PWM_GPIO_pins()
{

  ledc_timer_config_t timer = {
      // Configuracao do timer 0

      .speed_mode = LEDC_LOW_SPEED_MODE, // Modo de Velocidade -> LOW
      .duty_resolution = RESOLUTION_PWM, // Resolucao do do ciclo de trabalho (2^10 = 1024 valores)
      .timer_num = LEDC_TIMER_0,         // Utilizado o TIMER 0
      .freq_hz = FREQ_FE,                // Frequência de operacao do sinal PWM
      .clk_cfg = LEDC_AUTO_CLK           // Seleção automatica da fonte geradora do clock

  };

  ESP_ERROR_CHECK(ledc_timer_config(&timer));

  ledc_channel_config_t channel_EMG = {                                   // Configuracao do canal do FE
                                       .gpio_num = FE_ESP,                // Seleciona o pino para atuar o PWM
                                       .speed_mode = LEDC_LOW_SPEED_MODE, // Modo de Velocidade -> LOW
                                       .channel = CHANNEL_FE,
                                       .timer_sel = LEDC_TIMER_0,
                                       .duty = 0,
                                       .hpoint = 0

  };

  ledc_channel_config_t channel_ACEL = {                                   // Configuracao do canal do FE
                                        .gpio_num = AC_ESP,                // Seleciona o pino para atuar o PWM
                                        .speed_mode = LEDC_LOW_SPEED_MODE, // Modo de Velocidade -> LOW
                                        .channel = CHANNEL_AC,
                                        .timer_sel = LEDC_TIMER_0,
                                        .duty = 0,
                                        .hpoint = 0

  };

  ESP_ERROR_CHECK(ledc_channel_config(&channel_EMG));
  ESP_ERROR_CHECK(ledc_channel_config(&channel_ACEL));

  ledc_set_duty(LEDC_LOW_SPEED_MODE, CHANNEL_FE, PWM_EMG);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, CHANNEL_FE);
  ledc_set_duty(LEDC_LOW_SPEED_MODE, CHANNEL_AC, PWM_ACEL);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, CHANNEL_AC);
}

void init_ISR_GPIO_pins()
{

  gpio_pad_select_gpio(OUT_PWM_ESP);
  gpio_set_direction(OUT_PWM_ESP, GPIO_MODE_INPUT);
  gpio_pulldown_dis(OUT_PWM_ESP);
  gpio_pullup_dis(OUT_PWM_ESP);
  gpio_set_intr_type(OUT_PWM_ESP, GPIO_INTR_ANYEDGE);

  gpio_pad_select_gpio(IN_PWM_UPA);
  gpio_set_direction(IN_PWM_UPA, GPIO_MODE_INPUT);
  gpio_pulldown_dis(IN_PWM_UPA);
  gpio_pullup_dis(IN_PWM_UPA);
  gpio_set_intr_type(IN_PWM_UPA, GPIO_INTR_ANYEDGE);

  gpio_pad_select_gpio(IN_PWM_CPU);
  gpio_set_direction(IN_PWM_CPU, GPIO_MODE_INPUT);
  gpio_pulldown_dis(IN_PWM_CPU);
  gpio_pullup_dis(IN_PWM_CPU);
  gpio_set_intr_type(IN_PWM_CPU, GPIO_INTR_ANYEDGE);

  gpio_install_isr_service(0);

  gpio_isr_handler_add(OUT_PWM_ESP, gpio_interrupt_handler_STM, NULL);
  gpio_isr_handler_add(IN_PWM_UPA, gpio_interrupt_handler_UPA, NULL);
  gpio_isr_handler_add(IN_PWM_CPU, gpio_interrupt_handler_CPU, NULL);

  xTaskCreate(Task_ISR_STM, "Task_ISR_STM", 4096, NULL, 10, &ISR_STM);
  xTaskCreate(Task_ISR_UPA, "Task_ISR_UPA", 4096, NULL, 10, &ISR_UPA);
  xTaskCreate(Task_ISR_CPU, "Task_ISR_CPU", 4096, NULL, 10, &ISR_CPU);

}