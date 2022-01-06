/*
 * CSF Assignment 1
 * Arbitrary-precision integer data type
 * Header file
 */

#ifndef APINT_H
#define APINT_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    __uint64_t *sfits; // short for base 64 digits
    unsigned size; // technically limits the size, but if size overflows, then ApInt will be ~3GB
} ApInt;

/* Constructors and destructors */
ApInt *apint_create_from_u64(uint64_t val);
ApInt *apint_create_from_hex(const char *hex);
void apint_destroy(ApInt *ap);

/* Operations */
uint64_t apint_get_bits(ApInt *ap, unsigned n);
int apint_highest_bit_set(ApInt *ap);
ApInt *apint_lshift(ApInt *ap);
ApInt *apint_lshift_n(ApInt *ap, unsigned n);
char *apint_format_as_hex(ApInt *ap);
ApInt *apint_add(const ApInt *a, const ApInt *b);
ApInt *apint_sub(const ApInt *a, const ApInt *b);
int apint_compare(const ApInt *left, const ApInt *right);

#endif /* APINT_H */
