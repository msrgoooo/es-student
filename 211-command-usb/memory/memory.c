int main(void);

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> 
#include "device.h"
#include "../command.h"
#include "pico/stdlib.h"
#include "hardware/regs/addressmap.h"

extern char __flash_binary_start;
extern char __flash_binary_end;
extern char __boot2_start__;
extern char __boot2_end__;
extern char __etext;
extern char __data_start__;
extern char __data_end__;
extern char __bss_start__;
extern char __bss_end__;
extern char __HeapLimit;
extern char __StackBottom;
extern char __StackTop;


//uint16_t *main_code = (uint16_t *)((uintptr_t)main & ~1u);
uint32_t data_variable = 100;
uint32_t bss_variable;

static void row(const char *name, uintptr_t start, uintptr_t end)
{
    printf("%-10s 0x%08x 0x%08x %8u\n",
           name, (unsigned)start, (unsigned)end, (unsigned)(end - start));
}

void mem_info(void)
{
    printf("%-10s %-10s %-10s %8s\n", "area", "start", "end", "size");

    row("flash", XIP_BASE, XIP_BASE + PICO_FLASH_SIZE_BYTES);
    row("sram", SRAM_BASE, SRAM_BASE + (264 * 1024));
    row("rom", ROM_BASE, ROM_BASE + (16 * 1024));

    row("image", (uintptr_t)&__flash_binary_start, (uintptr_t)&__flash_binary_end);
    row("free", (uintptr_t)&__flash_binary_end, XIP_BASE + PICO_FLASH_SIZE_BYTES);
    row("boot2", (uintptr_t)&__boot2_start__, (uintptr_t)&__boot2_end__);
    row("text", (uintptr_t)&__boot2_end__, (uintptr_t)&__etext);

    uint32_t data_size = (uintptr_t)&__data_end__ - (uintptr_t)&__data_start__;
    row("data flash", (uintptr_t)&__etext, (uintptr_t)&__etext + data_size);
    row("data ram", (uintptr_t)&__data_start__, (uintptr_t)&__data_end__);
    row("bss", (uintptr_t)&__bss_start__, (uintptr_t)&__bss_end__);
    row("heap", (uintptr_t)&__bss_end__, (uintptr_t)&__HeapLimit);
    row("stack", (uintptr_t)&__StackBottom, (uintptr_t)&__StackTop);

    // Размеры областей для итогов
    uint32_t boot2_size = (uintptr_t)&__boot2_end__ - (uintptr_t)&__boot2_start__;
    uint32_t text_size  = (uintptr_t)&__etext - (uintptr_t)&__boot2_end__;
    uint32_t image_size = (uintptr_t)&__flash_binary_end - (uintptr_t)&__flash_binary_start;
    uint32_t flash_free = PICO_FLASH_SIZE_BYTES - image_size;

    uint32_t data_ram_size = (uintptr_t)&__data_end__ - (uintptr_t)&__data_start__;
    uint32_t bss_size      = (uintptr_t)&__bss_end__ - (uintptr_t)&__bss_start__;
    uint32_t ram_used      = data_ram_size + bss_size;

    uint32_t heap_size  = (uintptr_t)&__HeapLimit - (uintptr_t)&__bss_end__;
    uint32_t stack_size = (uintptr_t)&__StackTop - (uintptr_t)&__StackBottom;

    printf("\n");
    printf("total\n");
    printf("  flash image %8u = boot2 %u + text %u + data %u\n",
           image_size, boot2_size, text_size, data_size);
    printf("  flash free  %8u of %u\n", flash_free, (unsigned)PICO_FLASH_SIZE_BYTES);
    printf("  ram used    %8u = data %u + bss %u\n",
           ram_used, data_ram_size, bss_size);
    printf("  ram free    %8u for heap and %u for stack\n",
           heap_size, stack_size);
}

void fw_info(void)
{
    // считаем вызов: data_variable и bss_variable на единицу больше
    data_variable++;
    bss_variable++;
    // адреса функций со сброшенным признаком Thumb
    uintptr_t main_addr = (uintptr_t)main;
    uintptr_t fw_addr   = (uintptr_t)fw_info;

    const uint16_t *main_code = (const uint16_t *)(main_addr & ~1u);
    const uint16_t *fw_code   = (const uint16_t *)(fw_addr   & ~1u);

    // локальная переменная и блок из кучи
    uint32_t stack_variable = 1946;
    uint32_t *heap_variable = malloc(sizeof(uint32_t));

    if (heap_variable != NULL)
    {
        *heap_variable = 1951;
    }

    // шапка: объект, адрес, значение
    printf("%-18s %-12s %s\n", "object", "address", "value");

    // main, fw_info — адрес с признаком Thumb и два байта по сброшенному адресу
    printf("%-18s 0x%08x 0x%04x\n",
           "main", (unsigned)main_addr, (unsigned)*main_code);
    printf("%-18s 0x%08x 0x%04x\n",
           "fw_info", (unsigned)fw_addr, (unsigned)*fw_code);

    // commands — адрес массива
    printf("%-18s 0x%08x\n",
           "commands", (unsigned)(uintptr_t)commands);

    // обработчики — имя команды и адрес обработчика, строкой на команду
    for (uint i = 0; i < command_count; i++)
    {
        printf("  %-16s 0x%08x\n",
               commands[i].name,
               (unsigned)(uintptr_t)commands[i].handler);
    }

    // константы — адрес и значение строк паспорта из device.h
    printf("%-18s 0x%08x %s\n",
           "DEVICE_NAME", (unsigned)(uintptr_t)DEVICE_NAME, DEVICE_NAME);

    // data_variable — адрес и значение, секция .data
    printf("%-18s 0x%08x %u\n",
           "data_variable", (unsigned)(uintptr_t)&data_variable,
           (unsigned)data_variable);

    // bss_variable — адрес и значение, секция .bss
    printf("%-18s 0x%08x %u\n",
           "bss_variable", (unsigned)(uintptr_t)&bss_variable,
           (unsigned)bss_variable);

    // stack_variable — адрес и значение
    printf("%-18s 0x%08x %u\n",
           "stack_variable", (unsigned)(uintptr_t)&stack_variable,
           (unsigned)stack_variable);

    // heap_variable — адрес и значение
    if (heap_variable != NULL)
    {
        printf("%-18s 0x%08x %u\n",
               "heap_variable", (unsigned)(uintptr_t)heap_variable,
               (unsigned)*heap_variable);
    }
    else
    {
        printf("%-18s NULL\n", "heap_variable");
    }


    // возвращаем блок кучи
    free(heap_variable);
}