/*
 * Unit tests for arbitrary-precision integer data type
 *
 * These tests are by no means comprehensive.  You will need to
 * add more tests of your own!  In particular, make sure that
 * you have tests for more challenging situations, such as
 *
 * - large values
 * - adding/subtracting/comparing values with different lengths
 * - special cases (carries when adding, borrows when subtracting, etc.)
 * - etc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apint.h"
#include "tctest.h"

typedef struct {
	ApInt *ap0;
	ApInt *ap1;
	ApInt *ap110660361;
	ApInt *max1;
	/* TODO: add additional fields of test fixture */
} TestObjs;

TestObjs *setup(void);
void cleanup(TestObjs *objs);

void testCreateFromU64(TestObjs *objs);
void testHighestBitSet(TestObjs *objs);
void testLshiftN(TestObjs *objs);
void testCompare(TestObjs *objs);
void testFormatAsHex(TestObjs *objs);
void testAdd(TestObjs *objs);
void testSub(TestObjs *objs);
void testFormatHexStress();
void testAddStress();
void testSubStress();
void testCompStress();
void testShiftStress();
/* TODO: add more test function prototypes */

int main(int argc, char **argv) {
	TEST_INIT();

	if (argc > 1) {
		/*
		 * name of specific test case to execute was provided
		 * as a command line argument
		 */
		tctest_testname_to_execute = argv[1];
	}

	TEST(testCreateFromU64);
	TEST(testHighestBitSet);
	TEST(testLshiftN);
	TEST(testCompare);
	TEST(testFormatAsHex);
	TEST(testAdd);
	TEST(testSub);
	
	TEST(testFormatHexStress);
	TEST(testAddStress);
	TEST(testSubStress);
	TEST(testCompStress);
	TEST(testShiftStress);
	/* TODO: use TEST macro to execute more test functions */

	TEST_FINI();
}

TestObjs *setup(void) {
	TestObjs *objs = malloc(sizeof(TestObjs));
	objs->ap0 = apint_create_from_u64(0UL);
	objs->ap1 = apint_create_from_u64(1UL);
	objs->ap110660361 = apint_create_from_u64(110660361UL);
	objs->max1 = apint_create_from_u64(0xFFFFFFFFFFFFFFFFUL);
	/* TODO: initialize additional members of test fixture */

	return objs;
}

void cleanup(TestObjs *objs) {
	apint_destroy(objs->ap0);
	apint_destroy(objs->ap1);
	apint_destroy(objs->ap110660361);
	apint_destroy(objs->max1);
	/* TODO: destroy additional members of test fixture */
	
	free(objs);
}

void testCreateFromU64(TestObjs *objs) {
	ASSERT(0UL == apint_get_bits(objs->ap0, 0));
	ASSERT(1UL == apint_get_bits(objs->ap1, 0));
	ASSERT(110660361UL == apint_get_bits(objs->ap110660361, 0));
	ASSERT(0xFFFFFFFFFFFFFFFFUL == apint_get_bits(objs->max1, 0));
}

void testHighestBitSet(TestObjs *objs) {
	ASSERT(-1 == apint_highest_bit_set(objs->ap0));
	ASSERT(0 == apint_highest_bit_set(objs->ap1));
	ASSERT(26 == apint_highest_bit_set(objs->ap110660361));
	ASSERT(63 == apint_highest_bit_set(objs->max1));
}

void testLshiftN(TestObjs *objs) {
	ApInt *result;

	result = apint_lshift_n(objs->ap0, 17);
	ASSERT(0UL == apint_get_bits(result, 0));
	ASSERT(0UL == apint_get_bits(result, 1));
	apint_destroy(result);

	result = apint_lshift_n(objs->ap1, 17);
	ASSERT(0x20000UL == apint_get_bits(result, 0));
	ASSERT(0UL == apint_get_bits(result, 1));
	apint_destroy(result);

	result = apint_lshift_n(objs->ap110660361, 17);
	ASSERT(0xD3116120000UL == apint_get_bits(result, 0));
	ASSERT(0UL == apint_get_bits(result, 1));
	apint_destroy(result);
}

void testCompare(TestObjs *objs) {
	/* 1 > 0 */
	ASSERT(apint_compare(objs->ap1, objs->ap0) > 0);
	/* 0 < 1 */
	ASSERT(apint_compare(objs->ap0, objs->ap1) < 0);
	/* 110660361 > 0 */
	ASSERT(apint_compare(objs->ap110660361, objs->ap0) > 0);
	/* 110660361 > 1 */
	ASSERT(apint_compare(objs->ap110660361, objs->ap1) > 0);
	/* 0 < 110660361 */
	ASSERT(apint_compare(objs->ap0, objs->ap110660361) < 0);
	/* 1 < 110660361 */
	ASSERT(apint_compare(objs->ap1, objs->ap110660361) < 0);
}

void testFormatAsHex(TestObjs *objs) {
	char *s;

	ASSERT(0 == strcmp("0", (s = apint_format_as_hex(objs->ap0))));
	free(s);

	ASSERT(0 == strcmp("1", (s = apint_format_as_hex(objs->ap1))));
	free(s);

	ASSERT(0 == strcmp("6988b09", (s = apint_format_as_hex(objs->ap110660361))));
	free(s);

	ASSERT(0 == strcmp("ffffffffffffffff", (s = apint_format_as_hex(objs->max1))));
	free(s);
}

void testAdd(TestObjs *objs) {
	ApInt *sum;
	char *s;

	/* 0 + 0 = 0 */
	sum = apint_add(objs->ap0, objs->ap0);
	ASSERT(0 == strcmp("0", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	/* 1 + 0 = 1 */
	sum = apint_add(objs->ap1, objs->ap0);
	ASSERT(0 == strcmp("1", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	/* 1 + 1 = 2 */
	sum = apint_add(objs->ap1, objs->ap1);
	ASSERT(0 == strcmp("2", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	/* 110660361 + 1 = 110660362 */
	sum = apint_add(objs->ap110660361, objs->ap1);
	ASSERT(0 == strcmp("6988b0a", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	/* FFFFFFFFFFFFFFFF + 1 = 10000000000000000 */
	sum = apint_add(objs->max1, objs->ap1);
	ASSERT(0 == strcmp("10000000000000000", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);
}

void testSub(TestObjs *objs) {
	ApInt *a, *b, *diff;
	char *s;

	/* subtracting 1 from ffffffffffffffff is fffffffffffffffe */
	diff = apint_sub(objs->max1, objs->ap1);
	ASSERT(0 == strcmp("fffffffffffffffe", (s = apint_format_as_hex(diff))));
	apint_destroy(diff);
	free(s);

	/* subtracting 0 from 1 is 1 */
	diff = apint_sub(objs->ap1, objs->ap0);
	ASSERT(0 == strcmp("1", (s = apint_format_as_hex(diff))));
	ASSERT(0 == apint_compare(diff, objs->ap1));
	apint_destroy(diff);
	free(s);

	/* subtracting 1 from 1 is 0 */
	diff = apint_sub(objs->ap1, objs->ap1);
	ASSERT(0 == strcmp("0", (s = apint_format_as_hex(diff))));
	ASSERT(0 == apint_compare(diff, objs->ap0));
	apint_destroy(diff);
	free(s);

	/* subtracting 1 from 0 can't be represented because it is negative */
	ASSERT(NULL == apint_sub(objs->ap0, objs->ap1));

	/* test involving larger values */
	a = apint_create_from_hex("7e35207519b6b06429378631ca460905c19537644f31dc50114e9dc90bb4e4ebc43cfebe6b86d");
	b = apint_create_from_hex("9fa0fb165441ade7cb8b17c3ab3653465e09e8078e09631ec8f6fe3a5b301dc");
	diff = apint_sub(a, b);
	ASSERT(0 == strcmp("7e35207519b6afc4883c6fdd8898213a367d73b918de95f20766963b0251c622cd3ec4633b691",
		(s = apint_format_as_hex(diff))));
	apint_destroy(diff);
	apint_destroy(b);
	apint_destroy(a);
	free(s);
}

// The testing format you guys provided was annoying so I made an better one
// it reads formatted test text from a file and verifies my methods against 
// ground truth in the file. To debug, just add a counter to the while loop 
// and it'll show what line of the file caused a failure.
// All this one does is create a number from a string, convert it back into 
// a string, and compare it to the original. As for the actual tests, I tested
// on all the single hex digit input. Then on some interesting repeated
// bit pattersn, like f's which is all 1's, 3's which is alternating 1's,
// 5's which is alternating groups of paired 1's, and a number with 1 1 followed
// by a lot of 0's to test 0 formatiing.
void testFormatHexStress() {
    char *s;
    FILE * fp;
    int max_size = 101;

    char a[max_size];

    fp = fopen("./tests_format.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    ApInt *num1;
    while(fscanf(fp, "%s", a) == 1) {
        num1 = apint_create_from_hex(a);
	s = apint_format_as_hex(num1);
        ASSERT(0 == strcmp(a, s));
        free(s);
        apint_destroy(num1);
    }

    fclose(fp);
}

// Uses the same framework as the previous test
// Uses a new testing file that has the format
// a + b = c
// And so it converts the strings a, b, and c to
// apints and then verifies that my algorithm says
// a + b = c
// b + a = c
// The commuttivity of addition provides two checks
// for the price of one.
// I tested with a lot of randomly generated ruby tests
// and I tested edge cases like a lot of f's added together to make sure
// my carries were good
void testAddStress() {
    char *s;
    FILE * fp;
    int max_size = 101;

    char a[max_size];
    char b[max_size];
    char c[max_size];

    fp = fopen("./tests_add.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    ApInt *num1;
    ApInt *num2;
    ApInt *num3;

    while(fscanf(fp, "%s + %s = %s", a, b, c) == 3) {
        num1 = apint_create_from_hex(a);
        num2 = apint_create_from_hex(b);
        num3 = apint_add(num1, num2);

	s = apint_format_as_hex(num3);
	ASSERT(0 == strcmp(c, s));
	free(s);
	apint_destroy(num3);

	num3 = apint_add(num2, num1);
        s = apint_format_as_hex(num3);
        ASSERT(0 == strcmp(c, s));

        free(s);
        apint_destroy(num1);
        apint_destroy(num2);
        apint_destroy(num3);
    }

    fclose(fp);
}


// This one technically uses a test file that was
// meant for addition, but addition and subtraction
// are inverses, so it's alright.
// the test file has the format:
// a + b = c
// So this test makes sure that
// c - a = b
// c - b = a
// Again, we get two tests from one line in the test file
void testSubStress() {
    char *s;
    FILE * fp;
    int max_size = 101;

    char a[max_size];
    char b[max_size];
    char c[max_size];

    fp = fopen("./tests_add.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    ApInt *num1;
    ApInt *num2;
    ApInt *num3;
    ApInt *result;

    while(fscanf(fp, "%s + %s = %s", a, b, c) == 3) {
        num1 = apint_create_from_hex(a);
        num2 = apint_create_from_hex(b);
        num3 = apint_create_from_hex(c);

	result = apint_sub(num3, num2);
        s = apint_format_as_hex(result);
        ASSERT(0 == strcmp(s, a));
	apint_destroy(result);
	free(s);

	result = apint_sub(num3, num1);
        s = apint_format_as_hex(result);
        ASSERT(0 == strcmp(s, b));

        free(s);
        apint_destroy(num1);
        apint_destroy(num2);
        apint_destroy(num3);
	apint_destroy(result);
    }

    fclose(fp);
}


// Again this one uses the test addition file, but
// all the files are formatted so that positive numbers are
// being added together, so for every line we know
// c > a
// c > b
// a < c
// a < b
// a = a
// b = b
// c = c
// And wow! we get 7 tests for each line!
void testCompStress() {
    int n;
    FILE * fp;
    int max_size = 101;

    char a[max_size];
    char b[max_size];
    char c[max_size];

    fp = fopen("./tests_add.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    ApInt *num1;
    ApInt *num2;
    ApInt *num3;

    while(fscanf(fp, "%s + %s = %s", a, b, c) == 3) {
        num1 = apint_create_from_hex(a);
        num2 = apint_create_from_hex(b);
        num3 = apint_create_from_hex(c);

	n = apint_compare(num1, num1);
        ASSERT(n == 0);

	n = apint_compare(num2, num2);
        ASSERT(n == 0);

	n = apint_compare(num3, num3);
        ASSERT(n == 0);

	n = apint_compare(num1, num3);
        ASSERT(n < 0);

	n = apint_compare(num2, num3);
        ASSERT(n < 0);

	n = apint_compare(num3, num1);
        ASSERT(n > 0);

        n = apint_compare(num3, num2);
        ASSERT(n > 0);

        apint_destroy(num1);
        apint_destroy(num2);
        apint_destroy(num3);
    }

    fclose(fp);
}


// I had to write a new test file that included a lot of randomly generated ruby
// tests and then bit shifting a lot of f's by 1
void testShiftStress() {
    char *s;
    FILE * fp;
    int max_size = 404;

    char a[max_size];
    unsigned b;
    char c[max_size];

    fp = fopen("./tests_shift.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    ApInt *num1;
    ApInt *num2;

    while(fscanf(fp, "%s << %u = %s", a, &b, c) == 3) {
        num1 = apint_create_from_hex(a);
        num2 = apint_lshift_n(num1, b);

        s = apint_format_as_hex(num2);
        ASSERT(0 == strcmp(c, s));

        free(s);
        apint_destroy(num1);
        apint_destroy(num2);
    }

    fclose(fp);
}

