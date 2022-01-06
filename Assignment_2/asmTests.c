/*
 * Tests for postfix calculator
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tctest.h"

/*
 * Prototypes for assembly functions defined in asmPostfixCalc.S
 */
long addLongs(long a, long b); /* example assembly function */
long isSpace(long c);
long isDigit(long c);
long isOperation(long c);
const char *skipws(const char *s);
long tokenType(const char *s);
const char *consumeInt(const char *s, long *pVal);
void stackPush(long stack[], long *pCount, long val);
long stackPop(long stack[], long *pCount);
long eval(const char *expr);

/* #defines for constants used by your assembly functions */
#define        SPACE        0x20
#define        TAB          0x9

#define        ZERO         0x30
#define        NINE         0x39

#define        MULT         0x2a
#define        DIV          0x2f
#define        ADD          0x2b
#define        SUB          0x2d

#define        NULLTERM     0x0

#define        MAX_SIZE     20
#define        CHAR_MASK    0xff

/*
 * Test fixture data type
 */
typedef struct {
	/*
	 * TODO: add fields for data needed by test functions
	 * (for example, a representation of the operand stack)
	 */
} TestObjs;

/*
 * Test fixture setup/cleanup functions: these
 * are called automatically as part of executing each
 * test function.
 */
TestObjs *setup(void);
void cleanup(TestObjs *objs);

/* test functions */
void testAddLongs(TestObjs *objs); /* example test function */
void testIsSpace();
void testIsDigit();
void testIsOperation();
void testSkipws();
void testTokenType();
void testConsumeInt();
void testStackPush();
void testStackPushOverFlow();
void testStackPop();
void testStackPopUnderFlow();
void testEval();

/* set to nonzero if a call to exit is expected */
int expectedExit;
/* jump buffer that our version of exit can use to jump back to test function */
sigjmp_buf exitBuf;

/*
 * Custom version of exit: useful for testing functions where
 * the expected behavior is a call to exit (e.g., because
 * an argument value is invalid.)  If the expectedExit
 * variable is set to a nonzero value, uses siglongjmp (via
 * exitBuf) to return control to the test function.
 * If expectedExit is not set, immediately fails the
 * current test.
 */
void exit(int exitCode) {
    if (expectedExit) {
        /* jump back to test function */
        siglongjmp(exitBuf, 1);
    } else {
        /* exit called unexpectedly, fail the test */
        FAIL("Unexpected exit");
    }
}

int main(void) {
    TEST_INIT();	
	
    TEST(testAddLongs);
    TEST(testIsDigit);
    TEST(testIsSpace);
    TEST(testIsOperation);
    TEST(testTokenType);
    TEST(testSkipws);
    TEST(testConsumeInt);
    TEST(testStackPush);
    TEST(testStackPushOverFlow);
    TEST(testStackPop);
    TEST(testStackPopUnderFlow);
    TEST(testEval);


    TEST_FINI();
}

TestObjs *setup(void) {
    /* reset expectedExit */
    expectedExit = 0;

    TestObjs *objs = malloc(sizeof(TestObjs));
    /* TODO: initialize fields of test fixture as necessary */
    return objs;
}

void cleanup(TestObjs *objs) {
    free(objs);
}

/*
 * Example test function
 */
void testAddLongs(TestObjs *objs) {
	ASSERT(2L == addLongs(1L, 1L));
	ASSERT(11L == addLongs(9L, 2L));
	ASSERT(-3L == addLongs(-14L, 11L));
}



void testIsDigit() {
    long c =  0;

    while(c < 128) {
        if(c < (long) '0' || c > (long) '9') {
            ASSERT(isDigit(c) == 0);
        } else {
            ASSERT(isDigit(c) == 1);
        }
        c++;
    }
}


void testIsSpace() {
    long c = 0;

    while(c < 128) {
        if(c == (long) '\t' || c == (long) ' ') {
            ASSERT(isSpace(c) == 2);
        } else {
            ASSERT(isSpace(c) == 0);
        }
        c++;
    }

}

void testIsOperation() {
    long c = 0;

    while(c < 128) {
        if(c == (long) '*') {
            ASSERT(isOperation(c) == 3);
        } else if(c == (long) '/') {
            ASSERT(isOperation(c) == 4);
        } else if(c == (long) '+') {
            ASSERT(isOperation(c) == 5);
        } else if(c == (long) '/') {
            ASSERT(isOperation(c) == 6);
        }

        c++;
    }
}


void testTokenType() {
    long c = 0;

    while(c < 128) {
        char val = '\0' + c;
        if(c >= '0' && c <= '9') {
            ASSERT(tokenType(&val) == 1);
        } else if(c == ' ' || c == '\t') {
            ASSERT(tokenType(&val) == 2);
        } else if(c == '*') {
            ASSERT(tokenType(&val) == 3);
        } else if(c == '/') {
            ASSERT(tokenType(&val) == 4);
        } else if(c == '+') {
            ASSERT(tokenType(&val) == 5);
        } else if(c == '-') {
            ASSERT(tokenType(&val) == 6);
        } else {
            ASSERT(tokenType(&val) == 0);
        }

        c++;
    }
}


void testSkipws() {
    // Various Strings made up of leading ' ' and '\t' followed by non space characters
    // should get the first character
    char s1[] = "       -43853989                        ";
    char s2[] = "                           289545 - 994505405 +";
    char s3[] = "        + 4309090              ";
    char s4[] = "                                       9 - ------";
    char s5[] = "";

    const char *c1 = skipws(s1);
    const char *c2 = skipws(s2);
    const char *c3 = skipws(s3);
    const char *c4 = skipws(s4);
    const char *c5 = skipws(s5);

    ASSERT(*c1 == '-');
    ASSERT(*c2 == '2');
    ASSERT(*c3 == '+');
    ASSERT(*c4 == '9');
    ASSERT(*c5 == '\0');
}


void testConsumeInt() {
    // Various Strings made up of leading ' ' and '\t' followed by non space characters
    // should get the first character
    char s1[] = "43853989                        ";
    char s2[] = "089545 - 994505405 +";
    char s3[] = "99420j9hh7             ";
    char s4[] = "9	    - ------";
    char s5[] = "-----";
    char s6[] = "";

    long num = 0;

    const char *c1 = consumeInt(s1, &num);
    ASSERT(*c1 == ' ');
    ASSERT(num == 43853989);

    const char *c2 = consumeInt(s2, &num);
    ASSERT(*c2 == ' ');
    ASSERT(num == 89545);

    const char *c3 = consumeInt(s3, &num);
    ASSERT(*c3 == 'j');
    ASSERT(num == 99420);

    const char *c4 = consumeInt(s4, &num);
    ASSERT(*c4 == '\t');
    ASSERT(num == 9);

    const char *c5 = consumeInt(s5, &num);
    ASSERT(*c5 == '-');
    ASSERT(num == 0);

    const char *c6 = consumeInt(s6, &num);
    ASSERT(*c6 == '\0');
    ASSERT(num == 0);
}


void testStackPush() {
    // make sure the stack size and element values are updated correctly
    //
    long *stack = (long *)malloc(sizeof(long) * 20);
    long size = 0;

    long i = 0;
    while(i < 20) {
        stackPush(stack, &size, i);
        i++;
        ASSERT(size == i);
    }

    i = 0;
    while(i < 20) {
        ASSERT(stack[i] == i);
        i++;
    }

    //make sure the array elements are allotted enough space using long max
    size = 0;
    for(i = 0; i < 20; i++) {
        stackPush(stack, &size, 9223372036854775807);
    }

    for(i = 0; i < 20; i++) {
        ASSERT(stack[i] == 9223372036854775807);
    }

    free(stack);
}


void testStackPushOverFlow() {
    // StackPush should break if there is overflow (pushing more than 20 elements)

    long *stack = (long *)malloc(sizeof(long) * 20);
    long size = 0;

    long i = 0;
    while(i < 20) {
        stackPush(stack, &size, i);
        i++;
    }

    expectedExit = 1;

    if (sigsetjmp(exitBuf, 1) == 0) {
        stackPush(stack, &size, 0);
        FAIL("stackPush tried to go past allotted memory");
    } else {
        printf("stackPush exited, good...");
    }

    free(stack);	// function doesn't free arry anymore
}


void testStackPop() {
    // Make sure the stack size and returned element values are correct

    long *stack = (long *)malloc(sizeof(long) * 20);
    long size = 0;

    long i = 0;
    long num = 0;
    while(i < 20) {
        stackPush(stack, &size, i);
        i++;
    }

    i = 20;
    ASSERT(size == i);
    while(i > 0) {
        num = stackPop(stack, &size);
        ASSERT(num == i - 1);
        i--;
        ASSERT(size == i);
    }

    free(stack);
}

void testStackPopUnderFlow() {
    // StackPop should underflow if you try to pop from empty stack

    long *stack = (long *)malloc(sizeof(long) * 20);
    long size = 0;

    expectedExit = 1;

    if (sigsetjmp(exitBuf, 1) == 0) {
        stackPop(stack, &size);
        FAIL("stackPop tried to read from empty stack");
    } else {
        printf("stackPop exited, good...");
    }

    free(stack);	// function doesn' free array anymore
}

// Since this 'unit' test is essentially the same as the system tests, I will just
// perform a few tests here and put the majority of my tests in systests
void testEval() {
    char s1[] = "1 1 +";
    char s2[] = "1 2 -";
    char s3[] = "3 5 *";
    char s4[] = "6 6 6 6 6 6 6 + + + + + +";
    char s5[] = "6 6 6 * *";
    char s6[] = "11 5 /";


    ASSERT(eval(s1) == 2);
    ASSERT(eval(s2) == -1);
    ASSERT(eval(s3) == 15);
    ASSERT(eval(s4) == 42);
    ASSERT(eval(s5) == 216);
    ASSERT(eval(s6) == 2);

}

