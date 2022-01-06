/*
 * CSF Assignment 1
 * Arbitrary-precision integer data type
 * Function implementations
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "apint.h"

/* Ugh. So it's Sunday night and I somehow got everything to work. So I'mma explain
 * my code and then I'm oudye. Yes, it's true my functions are longer than 20 lines,
 * but it's also true that I have to finish an insanely long Stats problem set so
 * I think that I have better things to do than go through and tediously refactor my code.
 * I'm sorry. It's an unfortunate situation that we're both going to have to deal with.
 */

// param: uint64_t val
// Nothing much to say. Works the way you think it would
ApInt *apint_create_from_u64(uint64_t val) {
    ApInt *num = (ApInt*)malloc(sizeof(ApInt));
    num->sfits = (uint64_t*)malloc(sizeof(uint64_t));
    *(num->sfits) = val;
    num->size = 1;

    return num;
}

// param: const char *ptr
// param: unsigned s
// unsigned e
// Helper function for apint_create_from_hex. Reads chars from
// pointer s up to (not including) pointer e. Pretty much the 
// same exact thing as stroull. In fact I should have used that
// but it's too late now and I don't want to break anything.
uint64_t str_to_long(const char *ptr, unsigned s, unsigned e) {
    uint64_t n = 0;

    for(unsigned i = s; i < e; i++) {
	char c = *(ptr + i);

        if(c >= '0' && c <= '9') {
            n <<= 4;
            n |= (c - '0');
        } else if(c >= 'A' && c <= 'F') {
            n <<= 4;
            n |= (c - 'A' + 10);
        } else if(c >= 'a' && c <= 'f') {
            n <<= 4;
            n |= (c - 'a' + 10);
        } else {
	    assert(1 == 0);
	}
    }

    return n;
}

// param: const char *hex
// First while loop skips past all the leading 0's. Second while
// loop finds the size of the string excluding the leading zeroes.
// It's important to know the size off the bat so you don't have to 
// bother with reallocs. Final for loop creates start and end pointers
// for each chunck of 16 chars. Don't forget to add those remaining 
// leading digits! Yup. Oh. It uses little endian so it stores every-
// thing backwards. Watch out for thatin the while loop.
ApInt *apint_create_from_hex(const char *hex) {
    assert(hex != NULL);

    unsigned i = 0;
    int start = 0;
    unsigned size = 0;

    while(*(hex + start) == '0' && *(hex + start + 1) != '\0') {
        start++;
    }

    const char *shex = hex + start;

    while(*(shex + i) != '\0') {
        i++;
        if(i % 16 == 1) {
            size++;
        }
    }

    int mod = i % 16;
    if(mod <= 0) {
         mod += 16;
    }

    uint64_t *sfits = (uint64_t*)malloc(sizeof(uint64_t) * size);

    for(unsigned j = 0; j < size - 1; j++) {
        unsigned start = mod + 16 * (size - j - 2);
        unsigned end = mod + 16 * (size - j - 1);

        *(sfits + j) = str_to_long(shex, start, end);
    }

    *(sfits + size - 1) = str_to_long(shex, 0, mod);

    ApInt *num = (ApInt*)malloc(sizeof(ApInt));
    num->sfits = sfits;
    num->size = size;

    return num;
}

// param: ApInt *ap
// Destroys
void apint_destroy(ApInt *ap) {
    assert(ap != NULL);
    free(ap->sfits);
    free(ap);
}

// param: ApInt *ap
// param: unsigned n
// WHY IN GOD'S GREEN EARTH ISN'T AP A CONST???????????? WWWWWWHHHHHHHYYYYYYYY??????
// works as you would expect. WAIT. Actually
// it returns a 0 if you try to get a number
// that's greater than the size. Comes in handy
// later.
uint64_t apint_get_bits(ApInt *ap, unsigned n) {
    assert(ap != NULL);
    if(n >= ap->size) {
      return 0;
    }

    return *(ap->sfits + n);
}

// param: ApInt *ap
// bit shifts the uint64_t containing the leading digits
// until you get to the last 1
// AGAIN, WHY ISN'T AP A CONST????????????
int apint_highest_bit_set(ApInt *ap) {
    assert(ap != NULL);

    for(unsigned i = 0; i < 64; i++) {
        if (*(ap->sfits + ap->size - 1) >> i == 1) {
	    return (ap->size - 1) * 64 + i;
	}
    }

    return -1;
}


// param: ApInt *ap
// unsigned n
// Oof, yeah it's long, I know. I'm sorry. Basically it first
// calculates important quantities for the size of the new
// bit string. Then returns a 0 if ap is 0. Otherwise it creates 
// the right amount of memory and then fills up any triling zeros.
// Then it considers ap. It shifts right to find the bits that will
// be carried over. Then it shifts left to actually do the compuation
// if any bits were carried over from before, then it appends those
// as well.
ApInt *apint_lshift_n(ApInt *ap, unsigned n) {
    assert(ap != NULL);

    unsigned n_mult = n / 64;
    unsigned n_mod = n % 64;

    int high = apint_highest_bit_set(ap);
    unsigned new_size;

    if(high < 0) {
        return apint_create_from_u64(0);
    }

    new_size = (high + n) / 64 + 1;

    ApInt *new_ap = (ApInt*)malloc(sizeof(ApInt));
    new_ap->sfits = (uint64_t*)malloc(sizeof(uint64_t) * new_size);
    new_ap->size = new_size;
    unsigned pos = 0;

    while(pos < n_mult) {
        *(new_ap->sfits + pos) = 0UL;
        pos++;
    }

    uint64_t carry = 0;

    while(pos < new_size) {
        if(n_mod == 0) {
            *(new_ap->sfits + pos) = *(ap->sfits + pos - n_mult);
        } else if (pos - n_mult == ap->size) {
            *(new_ap->sfits + pos) = carry;
        } else {
            *(new_ap->sfits + pos) = *(ap->sfits + pos - n_mult) << n_mod | carry;
        }

	if(pos - n_mult < ap->size) {
	    carry = *(ap->sfits + pos - n_mult) >> (64 - n_mod);
	}
        pos++;
    }

    return new_ap;
}

// param: ApInt *ap
// Wow it's pretty cute isn't it?
// Does what you'd expect
ApInt *apint_lshift(ApInt *ap) {
    return apint_lshift_n(ap, 1);
}

// param: ApInt *ap
// Hey this one's not that long!
// Pretty much figures out the correct size and
// then prints the whole thing into a string backwards
// (since it's stored in little endian)
char *apint_format_as_hex(ApInt *ap) {
    int high = apint_highest_bit_set(ap);
    int hex_size;
    int mod;

    if(high < 0 ) {
        hex_size = 2;
    } else {
        hex_size = high / 4 + 2;
    }

    mod = hex_size % 16 - 1;
    if(mod <= 0) {
         mod += 16;
    }

    char *hex = (char*)malloc(sizeof(char) * hex_size);

    sprintf(hex, "%lx", *(ap->sfits + ap->size - 1));
    for(unsigned i = ap->size; i > 1; i--) {
        sprintf(hex + mod + 16*(ap->size - i), "%016lx", *(ap->sfits + i - 2));
    }

    return hex;
}

// param: const ApInt *a
// param: const ApInt *b
// Ok back to being long again! It's not that bad
// tho. Find the correct size of the new int. Then
// it's pretty much what you'd expect. Get a best estimate for the new
// size based on the 1 + the maximum of the old one. Then for all the
// numbers in the two addands(?)- things that get added together-, add them
// together pairwise on the same position. apint_get_bits's ability to 
// return 0's for bits outside range is pretty convienient. Wait, 
// I forgot to mention that. Hold on a sec, I need to go back and add 
// that. Then it pretty much takes advantage of the fact that adding two non 
// negative numbers results in an even greater positive unless they're
// both zero. WAIT. With C, conditional have values of 0 and 1, so I don't
// even need an if statment! (There used to be an if statment that set 
// carry to 1, in case you were wondering). Then it just copies over
// the result being careful not to cop leading 0's
ApInt *apint_add(const ApInt *a, const ApInt *b) {
    uint64_t carry = 0;
    unsigned max = 0;
    if(a->size > b->size) {
        max = a->size;
    } else {
        max = b->size;
    }

    uint64_t new[max + 1];
    unsigned i = 0;

    new[max] = 0;

    while(i < max + 1) {
        uint64_t a_i = apint_get_bits(a, i);
        uint64_t b_i = apint_get_bits(b, i);

        new[i] = a_i + b_i + carry;
        int cond1 = (new[i] < a_i);
        int cond2 = (new[i] < b_i);
        int cond3 = (new[i] == a_i) && !(b_i == 0 && carry == 0);
	int cond4 = (new[i] == b_i) && !(a_i == 0 && carry == 0);
	
	carry = (cond1 || cond2 || cond3 || cond4);
        i++;
    }

    ApInt *num = (ApInt*)malloc(sizeof(ApInt));
    if(new[max] == 0) {
        num->size = max;
    } else {
        num->size = max + 1;
    }

    num->sfits = (uint64_t*)malloc(sizeof(uint64_t) * num->size);
    for(unsigned j = 0; j < num->size; j++) {
        *(num->sfits + j) = new[j];
    }

    return num;
}

// param: const ApInt *a
// param: const ApInt *b
// Well first it compares the two numbers to see if subtraction is even valid.
// Eerily similar to addition in concept. Finds upper bound of
// new size based on max of. Wait. lol. It DOESN'T compare the
// sizes of a and b because that would be kinda dumb. Instead,
// it sets the initial new size to the size of a. It subtracts
// the same way that the previous function adds. Except that 
// subtraction isn't commutative, so it has half the conditions.
// There totally wasn't an if statment to set borrow until just now.
// Then it just just copies over the values, really making sure that
// it doesn't copy over leading 0's.
ApInt *apint_sub(const ApInt *a, const ApInt *b) {
    int comp = apint_compare(a, b);
    if(comp < 0) {
        return NULL;
    } else if(comp == 0) {
        return apint_create_from_u64(0);
    }

    unsigned max = a->size;

    uint64_t new[max];
    int borrow = 0;
    unsigned i = 0;
    new[max - 1] = 0;

    while(i < max) {
        uint64_t a_i = apint_get_bits(a, i);
        uint64_t b_i = apint_get_bits(b, i);

        new[i] = a_i - b_i - borrow;

	int cond1 = (new[i] > a_i);
        int cond2 = (new[i] == a_i) && !(b_i == 0 && borrow == 0);

        borrow = (cond1 || cond2);
        i++;
    }

    unsigned size = max;
    ApInt *num = (ApInt*)malloc(sizeof(ApInt));
    while(new[size - 1] == 0) {
        size--;
    }

    num->size = size;

    num->sfits = (uint64_t*)malloc(sizeof(uint64_t) * num->size);
    for(unsigned j = 0; j < num->size; j++) {
        *(num->sfits + j) = new[j];
    }

    return num;
}

// param: const ApInt *left
// param: const ApInt *right
// Actually should probably mention this in the subtraction method.
// So right off the bat it see which one has the higher highest bit.
// That test probably knocks off the vast majority of comparisons. If
// that comes back inconclusive then it manually (or I guess a computer's 
// equivalent for manually) goes through and checks that the numbers are
// equal and then returns the correct result when it finds a discrepany
// or returns 0 if they both check out 
int apint_compare(const ApInt *left, const ApInt *right) {
    int l_high = apint_highest_bit_set(left);
    int r_high = apint_highest_bit_set(right);

    if(l_high > r_high) {
        return 1;
    } else if(r_high > l_high) {
        return -1;
    }

    for(unsigned i = left->size; i > 0; i--) {
        if(i > right->size || *(left->sfits + i - 1) == *(right->sfits + i - 1)) {
	    continue;
	} else if (*(left->sfits + i - 1) > *(right->sfits + i - 1)) {
	    return 1;
	} else if (*(left->sfits + i - 1) < *(right->sfits + i - 1)) {
            return -1;
        }
    }

    return 0;
}
