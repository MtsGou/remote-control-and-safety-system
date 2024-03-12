#pragma once
#include <sys_def.h>
#include <UART_handler.hpp>

static void handleDisconnect(void *arg, AsyncClient *client);
static void handleNewClient(void *arg, AsyncClient *client);
static void handleData(void *arg, AsyncClient *client, void *data, size_t len);