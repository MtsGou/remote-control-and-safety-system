#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <stdint.h>
#include <sys_def.h>

void FIRMWARE_BEGIN(void);
void wait_for_serial(uint32_t time_ms);
void timestamp(void);
void Handle_user(void);