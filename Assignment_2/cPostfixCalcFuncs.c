/*
 * C postfix calculator
 * function implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include "cPostfixCalc.h"
// returns 1 if c is a digit {0-9}
// returns 0 otherwise
long isDigit(long c) {
    if(c < ZERO) {  // less than '0'
        return 0;
    }

    if(c > NINE) {  // greater than '9'
        return 0;
    }

    return 1;
}


// returns 2 if c is a space or tab {' ', '\t'}
// returns 0 otherwise
long isSpace(long c) {
    if(c == SPACE) {  // equal to ' '
        return 2;
    }

    if(c == TAB) {
        return 2;  // equals to '\t'
    }

    return 0;
}

// returns {3,4,5,6} if c is an arithemtic operation {*, /, +, -}
// returns 0 otherwise
long isOperation(long c) {
    if(c == MULT) {  // equal to '*'
        return 3;
    }

    if(c == DIV) {  // equal to '/'
        return 4;
    }

    if(c == ADD) {  // equal to '+'
        return 5;
    }

    if(c == SUB) {  // equal to '-'
        return 6;
    }

    return 0;
}

// returns values:
// 1 - digit {0-9}
// 2 - whitespace {' ', '\t'}
// 3 - multiplication {'*'}
// 4 - multiplication {'/'}
// 5 - multiplication {'+'}
// 6 - multiplication {'-'}
long tokenType(const char *s) {
    long c = (long)(*s);
    
    // helper functions already return the correct return
    // values. Since only one can be true, addition will find the
    // correct value.
    return isDigit(c) + isSpace(c) + isOperation(c);
}

// Tests to see if current char is a space
// if yes, it increments pointer by one and repeats previous step
// if no, it returns pointer to current char
const char *skipws(const char *s) {
    const char *c = s;
    int ws = isSpace((long)*c);

    while(ws) {
        c++;
        ws = isSpace((long)*c);
    }

    return c;
}

// Tests to see if current char is a digit
// if yes, it multiplies val by 10, adds the current digit, 
//     increments pointer by one, and repeats previous step
// if no, it returns pointer to current char
const char *consumeInt(const char *s, long *pVal) {
    const char *c = s;
    int dig = isDigit((long)*c);
    *pVal = 0;

    while(dig) {
	*pVal = (*pVal) * 10 + (long)(*c - ZERO);
        c++;
        dig = isDigit((long)*c);
    }

    return c;
}

// assumes the stack is not full
// Otherwise, it puts val in next empty spot and returns count + 1
void stackPush(long stack[], long *pCount, long val) {
    if(*pCount == MAX_SIZE) {
        free(stack);
	printf("Error: Stack overflow\n");
	exit(1);
    }
    
    *(stack + *pCount) = val;
    (*pCount)++;
}

// assumes the stack is not empty
// Otherwise, it puts val in next empty spot and returns count + 1
long stackPop(long stack[], long *pCount) {
    if(*pCount == 0) {
        free(stack);
        printf("Error: Stack underflow\n");
        exit(1);
    }

    (*pCount)--;
    return *(stack + *pCount);
}

// 1)  creates a stack and stack count
// 2)  reads in a character from the expression and finds it's type
// 2a)     if it's a null terminator, it checks stack size is 1, and returns value on stack
// 2b)     if it's whitespace, it calls skipws to find next non whitespace char, goes back to (2)
// 2c)     if it's a digit, it calls consumeInt, pushes the new number onto the stack, goes back to (2)
// 2c)     if it's an operator, it pops 2 numbers, call function eval, and pushes result to stack, goes back to (2)
// 2d)     if it's anything else, return error
long eval(const char *expr) {
    long *stack = (long *)malloc(sizeof(long) * MAX_SIZE);
    long size = 0;

    const char *c = expr;
    long num1 = 0;
    long num2 = 0;
    while(*c != '\0') {
        long tok = tokenType(c);

        if(tok > 2) { // handle special case of operator
            num2 = stackPop(stack, &size); // pop second
            num1 = stackPop(stack, &size); // pop first
	    c++; // remember to increment the pointer
        }

        switch (tok) {
            case 0: // invalid character
                free(stack);
                printf("Error: Illegal character\n");
                exit(1);
            case 1: // digit
                c = consumeInt(c, &num1);
		stackPush(stack, &size, num1); // remember to actually put it on stack... lollll
                break;
            case 2: // whitespace
                c = skipws(c);
                break;
            case 3: // multiplication
                stackPush(stack, &size, num1 * num2);
                break;
            case 4: // division
                if(num2 == 0) {
                    free(stack);
                    printf("Error: Division by 0\n");
                    exit(1);
                }
                stackPush(stack, &size, num1 / num2);
                break;
            case 5: // addition
                stackPush(stack, &size, num1 + num2);
                break;
            case 6: // subtraction
                stackPush(stack, &size, num1 - num2);
                break;
        }
    }

    num1 = *stack; // get first number on stack
    free(stack); // stack not needed, free it

    if(size != 1) { // make sure the expression evaluated to single number
        printf("Error: Malformed Expression\n");
        exit(1);
    }

    return num1; // if everything works return number
}
