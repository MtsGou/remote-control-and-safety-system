#include "flash_handler.h"

nvs_handle_t handle_str;

void flash_init(){
    nvs_flash_init();
    nvs_open("storage", NVS_READWRITE, &handle_str);
}

char* read_data_flash(){
    flash_init();
    size_t required_size = 0;
    nvs_get_str(handle_str, "data", NULL, &required_size);
    char* data = (char*)malloc(required_size);
    nvs_get_str(handle_str, "data", (char *) data, &required_size);
    nvs_close(handle_str);
    return data;
}

void write_data_flash(char* data){
    flash_init();
    nvs_set_str(handle_str, "data", data);
    nvs_commit(handle_str);
    nvs_close(handle_str);
}

void erase_all_data_flash(){
    flash_init();
    nvs_set_str(handle_str, "data", "\0");
    nvs_commit(handle_str);
    nvs_close(handle_str);
}