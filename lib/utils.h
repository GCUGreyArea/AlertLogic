#ifndef _UTILS_H__
#define _UTILS_H__

/**
 * @file utils.h
 * @author Barry Robinson (barry.w.robinson64@gmail.com)
 * @brief Utilities for the filescan application
 * @version 0.1
 * @date 2021-08-31
 * 
 * @copyright Copyright (c) 2021
 * @addtogroup library
 * @{
 * @addtogroup utils
 * @{
 */
#include <math.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

#include "utils.h"


int digits(uint16_t number, int base);
void get_ascii_string(uint16_t num, char *str, int base);
void print_as_hex(const unsigned char *buff, size_t len);
void print_as_hexdump(unsigned char *buff, size_t len);
char get_ascii(int num);
size_t to_the_power(size_t num, int power);
size_t force_power_of_two(size_t v);
bool power_of_two(size_t size);
size_t get_hexbuf_size(size_t bytes);
void hexbuf_to_hedxdump(unsigned char *buffer, char *string, size_t len);

/**
 * @}
 * @}
 */

#endif//_UTILS_H__