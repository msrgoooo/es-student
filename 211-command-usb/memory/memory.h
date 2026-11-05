#ifndef MEMORY_H
#define MEMORY_H

// Печатает карту памяти RP2040:
// аппаратные области (flash, sram, rom), области из символов линкера
// (image, free, boot2, text, data flash, data ram, bss, heap, stack)
// и итог — вес образа, остаток флеша, занятая и свободная RAM.
void mem_info(void);

#endif // MEMORY_H