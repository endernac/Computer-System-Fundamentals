/*
 * Common header file for C version of postfix calculator
 */

#ifndef CPOSTFIXCALC_H
#define CPOSTFIXCALC_H

// Constants
#define        SPACE	    0x20
#define        TAB	    0x9

#define        ZERO	    0x30
#define        NINE	    0x39

#define        MULT         0x2a
#define        DIV          0x2f
#define        ADD          0x2b
#define        SUB          0x2d

#define        NULLTERM     0x0

#define        MAX_SIZE     20  // putting this in hex is kinda extra, even for me

long isSpace(long c);
long isDigit(long c);
long isOperation(long c);
const char *skipws(const char *s);
long tokenType(const char *s);
const char *consumeInt(const char *s, long *pVal);
void stackPush(long stack[], long *pCount, long val);
long stackPop(long stack[], long *pCount);
long eval(const char *expr);

#endif /* CPOSTFIXCALC_H */
