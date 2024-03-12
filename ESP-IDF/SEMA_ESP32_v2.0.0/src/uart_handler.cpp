#include "uart_handler.h"
#include "parameters.h"

using namespace std;

void init_uart(){
    
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    uart_driver_install(UART, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    ESP_ERROR_CHECK(uart_param_config(UART, &uart_config));
    uart_set_pin(UART, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void send_uart(const char *data){
    uart_write_bytes(UART, data, strlen(data));
}

bool serial_available(){
    free(BUFFER_RX_SERIAL);
    BUFFER_RX_SERIAL = (uint8_t*) malloc(RX_BUF_SIZE);
    int len = uart_read_bytes(UART, BUFFER_RX_SERIAL, sizeof(BUFFER_RX_SERIAL)/4, 100 / portTICK_RATE_MS);
    memset(&BUFFER_RX_SERIAL[len], 0, RX_BUF_SIZE - len);
    bool available = len > 0 ? true : false;
    return available;
}

string serial_read_char(){
    free(BUFFER_RX_SERIAL);
    BUFFER_RX_SERIAL = (uint8_t*) malloc(RX_BUF_SIZE);
    int len = uart_read_bytes(UART, BUFFER_RX_SERIAL, sizeof(BUFFER_RX_SERIAL)/4, 100 / portTICK_RATE_MS);
    if (len <= 0){
        return "";
    }
    memset(&BUFFER_RX_SERIAL[len], 0, RX_BUF_SIZE - len);
    char *user_char = (char *)BUFFER_RX_SERIAL;
    string char_user = (string) user_char;
    return char_user;
}

string serial_read_string_until(string stop_str){
    string char_user = "";
    string complete_msg = "";
    send_uart("\n\r");
    
    while (char_user != stop_str){
        free(BUFFER_RX_SERIAL);
        BUFFER_RX_SERIAL = (uint8_t*) malloc(RX_BUF_SIZE);
        int len = uart_read_bytes(UART, BUFFER_RX_SERIAL, sizeof(BUFFER_RX_SERIAL)/4, 100 / portTICK_RATE_MS);
        memset(&BUFFER_RX_SERIAL[len], 0, RX_BUF_SIZE - len);
        char *user_char = (char *)BUFFER_RX_SERIAL;
        send_uart(user_char);
        char_user = (string) user_char;
        complete_msg += char_user;
    }
    complete_msg = complete_msg.substr(0, complete_msg.length() - stop_str.length());
    return complete_msg;
}