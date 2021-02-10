/* SPDX-License-Identifier: MIT */

#include <infos.h>

int strcmp(const char *l, const char *r)
{
	while (*l && *r) {
		if (*l++ != *r++) return -1;
	}
	
	return 0;
}

int strlen(const char *s)
{
	int count = 0;
	while (*s++) {
		count++;
	}
	
	return count;
}

size_t strlcpy(char *dst, const char *src, size_t dst_size)
{
    const char *src_copy = src;
    size_t dst_size_copy = dst_size;

    while (dst_size > 1 && *src != '\0') {
        *dst = *src;

        dst_size--;
        dst++;
        src++;
    }

    if (dst_size_copy != 0) {
        *dst = '\0';
        dst_size--;
    }

    // ran out of space in dst
    if (dst_size == 0) {
        while (*src) {
            src++;
        }
    }

    return (src - src_copy);
}
