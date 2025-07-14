#ifndef DATACONVERT_H
#define DATACONVERT_H

#include "stm32f4xx.h"                  // Device header
#include "stdarg.h"

void reverse_string(char *str, int length);

int int_to_str(int32_t x, char *str, int is_signed);

int float_to_str(float n, char *str, int afterpoint);

void convert(char *buffer, const char *format, ...);

#endif	// DATACONVERT_H
