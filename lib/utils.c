#include <stdio.h>
#include <stdlib.h>

#include "internal.h"
#include "utils.h"

int digits(uint16_t number, int base) {
    int ret = 0;
    while(number) {
        number = number / base;
        ret++;
    }

    return ret;
}

void get_ascii_string(uint16_t num, char *str, int base)
{
	if(num) {
		uint16_t divide = num;
		int pos = 0;

		pos = digits(num, base);

		str[pos--] = '\0';
		divide = num;
		while(divide) {
			uint16_t new = divide % base;
			str[pos--] = get_ascii(new);
			divide = divide / base;
		}
	}
	else {
		str[0] = '0';
		str[1] = '\0';
	}
}

inline static int reset_buffer_offset(char *str_buf, int offset) {
	if (offset) {
		str_buf[offset] = ':';
		offset++;
	}

	return offset;
}

/**
 * @brief print out a character buffer as hex
 *
 * @param buff
 * @param len
 */
void print_as_hex(const unsigned char *buff, size_t len) {
	if (buff)
	{
		int i;

        // Roughly, each byte in the buffer wil be 4 bytes in the output string
		char * str_buf = (char*) alloc_mem(sizeof(char) * (len * 4) + 1);
		int len = (len <= 64) ? len : 64;
		int offset = 0;
		for (i = 0; i < len; i++)
		{
			if (i < len - 1) {
				sprintf(&str_buf[offset], "%02x:", buff[i]);
				offset += 3;
			}
			else {
				sprintf(&str_buf[offset], "%02x]", buff[i]);
			}
		}

		printf("%s", str_buf);
	}
	else {
		print_err("ERROR: NULL buffer");
	}
}

char get_ascii(int num) {
    switch(num)  {
        case 0:  return '0';
        case 1:  return '1';
        case 2:  return '2';
        case 3:  return '3';
        case 4:  return '4';
        case 5:  return '5';
        case 6:  return '6';
        case 7:  return '7';
        case 8:  return '8';
        case 9:  return '9';
        case 10: return 'A';
        case 11: return 'B';
        case 12: return 'C';
        case 13: return 'D';
        case 14: return 'E';
        case 15: return 'F';
        default:
            break;
    }

    return '!';
}

size_t to_the_power(size_t num, int power) {
    size_t result = 1;
    while (power) {
        result *= num;
        --power;
    }
    return result;
}


size_t force_power_of_two(size_t v) {
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;

	return v;
}

bool power_of_two(size_t size) {
    if((size != 0) && ((size & (size - 1)) == 0)) {
        return true;
    }

    return false;
}

size_t get_hexbuf_size(size_t bytes) {
	return (((bytes / 16) * 6) + 5) + (3 * bytes) + 1;
}

void hexbuf_to_hedxdump(unsigned char *frame, char *string, size_t len)
{
	size_t str_place = 0;
	size_t frm_place = 0;
    size_t marker  = 0;
    size_t counter = 0;

    sprintf(&string[str_place], "%04hhx\t", 00);
    str_place+=5;

	while (len > frm_place)
	{
		unsigned char ca = frame[frm_place];

		sprintf(&string[str_place], "%02hhx ", ca);

		frm_place++;
        counter++;
		str_place+=3;

        if(len > frm_place && counter==16) {
            counter=0;
            marker++;
            sprintf(&string[str_place], "\n%04zx\t", marker*16);
            str_place+=6;
        }
	}

	string[str_place]   = '\0';
}


void print_as_hexdump(unsigned char *buff, size_t len) {
    char * buf = alloc_mem(sizeof(char) * get_hexbuf_size(len));
	hexbuf_to_hedxdump(buff, buf, len);
	printf("\n%s\n", buf);

    free(buf);
}