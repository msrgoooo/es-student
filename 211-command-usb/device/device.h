#pragma once

#include <stdint.h>

#define DEVICE_NAME "es-led-module"
#define FIRMWARE_VERSION "1.0.0"

#define DEVICE_PROJECT "211-command-usb"
#define DEVICE_REPO "https://github.com/msrgoooo/es-student"

#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

// Паспорт прибора в памяти: ревизия, версия, имя.
struct info_t
{
    uint32_t version;  // 4 байта кратно 4
    uint8_t  revision; // 1 байт + дыра?
    char     name[13]; // 13 байт + дыра?
};

// Определена в device.c; здесь только объявление
extern struct info_t device_card;

void device_info(void);
void dev_info(void);