/*
 * postfix calculator (assembly language version)
 * function implementation
 */

#define	       SPACE        0x20
#define        TAB          0x9

#define        ZERO         0x30
#define        NINE         0x39

#define        MULT         0x2a
#define        DIV          0x2f
#define        ADD          0x2b
#define        SUB          0x2d

#define        NULLTERM     0x0

#define        MAX_SIZE     20 
#define	       CHAR_MASK    0xff


.section .bss
// Create an array of longs of length 20
        .align 8
dataValues: .space (MAX_SIZE * 8)

.section .rodata
eStackOverFlow: .string "Error: Stack overflow\n"
eStackUnderFlow: .string "Error: Stack underflow\n"
eIllegalChar: .string "Error: Illegal character\n"
eDivByZero: .string "Error: Division By Zero\n"
eMalformedExpr: .string "Error: Malformed Expression\n"

.section .text

/*
 * addLongs function (this is just an example function)
 */
	.globl addLongs
addLongs:
	subq $8, %rsp                /* align stack pointer */

	movq %rsi, %rax              /* move second parameter to %rax */
	addq %rdi, %rax              /* add first parameter to second */
	/* the sum is now in %rax */

	addq $8, %rsp                /* restore stack pointer */
	ret

/* 
 * accepts char
 * returns 1 if c is a space or tab {1 - 9}
 * returns 0 otherwise
 */

        .globl isDigit
isDigit:
        cmpb $ZERO, %dil		/* compare %di to '0'. Only need to use %dil since chars are 1 byte*/
        jb .LNotDigit			/* jump to not a digit clause if %dil is below '0' */
        cmpb $NINE, %dil		/* compare %di to '9' */
        ja .LNotDigit			/* jump to not a digit clause if %dil is above '9' */
        movq $1, %rax			/* neither is true, %dil must be digit, return 1 */
        ret
.LNotDigit:
        movq $0, %rax			/* non digits return 0 */
        ret


/* 
 * accepts char 
 * returns 2 if c is a space or tab {' ', '\t'}
 * returns 0 otherwise
 */

        .globl isSpace
isSpace:
        cmpb $SPACE, %dil		/* compare %dil to ' ' */
        je .LWhiteSpace			/* jump to space clause if %dil is a space */
        cmpb $TAB, %dil			/* compare %dil to '\t' */
        je .LWhiteSpace			/* jump to space clause if %dil is a tab */
        movq $0, %rax			/* neither is true, %dil is not whitespace, return 0 */
        ret
.LWhiteSpace:
        movq $2, %rax			/* whitespace returns 2 */
        ret


/*
 * accepts char
 * returns {3,4,5,6} if c is an arithemtic operation {*, /, +, -}
 * returns 0 otherwise
 */

        .globl isOperation
isOperation:
        cmpb $MULT, %dil		/* compare %dil to '*' */
        je .LMult                       /* jump to mult clause if %dil is a '*' */
        cmpb $DIV, %dil                 /* compare %dil to '/' */
        je .LDiv                        /* jump to div clause if %dil is a '/' */
	cmpb $ADD, %dil                 /* compare %dil to '+' */
        je .LAdd                        /* jump to add clause if %dil is a '+' */
        cmpb $SUB, %dil                 /* compare %dil to '-' */
        je .LSub                        /* jump to sub clause if %dil is a '-' */
        movq $0, %rax                   /* nothing is true, the cake is a lie, %dil is not an operation, so return 0 and try to escape to the surface */
        ret
.LMult:
        movq $3, %rax                   /* '*' returns 3 */
        ret
.LDiv:
        movq $4, %rax                   /* '/' returns 4 */
        ret
.LAdd:
        movq $5, %rax                   /* '+' returns 5 */
        ret
.LSub:
        movq $6, %rax                   /* '-' returns 6 */
        ret


// accepts char*
// returns values:
// 1 - digit {0-9}
// 2 - whitespace {' ', '\t'}
// 3 - multiplication {'*'}
// 4 - ddivision {'/'}
// 5 - addition {'+'}
// 6 - subtraction {'-'} 
        .globl tokenType
tokenType:
	pushq %r15			/* want to use caller saved registers, push %r15 to store it */
	pushq %r14			/* push %r14 to store it */
	

	movq (%rdi), %r14		/* move char into %r14 */
	movq $0, %r15			/* use %r15 to store sum */

	movq %r14, %rdi			/* move char into %rdi */
	call isDigit			/* test if it's a digit */
	addq %rax, %r15			/* add it to sum */

	movq %r14, %rdi			/* move char into %rdi */
        call isSpace			/* test if it's whitespace */
        addq %rax, %r15			/* add it to sum */

	movq %r14, %rdi			/* move char into %rdi */
        call isOperation		/* test if it's an operation */
        addq %rax, %r15			/* add it to sum */

	movq %r15, %rax			/* transfer sum to %rax */
	
	popq %r14			/* restore value of %r14 */
	popq %r15			/* restore value of %r15 */
	ret 


// accepts char*
// returns char*
// Tests to see if current char is a space
// if yes, it increments pointer by one and repeats previous step
// if no, it returns pointer to current char
        .globl skipws
skipws:
	pushq %r15			/* store the value in %r15 */
	
	movq %rdi, %r15			/* store the pointer of current char in %r15 */
	movq (%r15), %rdi		/* dereference pointer and store character in %rdi */
	call isSpace			/* test to see if current char is whitespace */
	jmp .LSpaceCond			/* enter loop */

.LSpaceLoop:
	inc %r15			/* move to next character pointer (chars are one byte and arrays are contiguous) */
	movq (%r15), %rdi               /* dereference pointer and store character in %rdi */
        call isSpace                    /* test to see if current char is whitespace */

.LSpaceCond:
	cmpq $2, %rax			/* actually sees if current char is whitespace */
	je .LSpaceLoop			/* enters loop body if char is ws */

	leaq (%r15), %rax		/* put current value of pointer into %rax to be returned */
	popq %r15			/* restore original value of %r15 */
	ret




// accepts char*, long*
// returns char*
// Tests to see if current char is a digit
// if yes, it multiplies val by 10, adds the current digit, 
//     increments pointer by one, and repeats previous step
// if no, it returns pointer to current char
	.globl consumeInt
consumeInt:
        pushq %r15                      /* store the value in %r15 */
	pushq %r14			/* store the value in %r14 */

        movq %rdi, %r15                 /* store the pointer of current char in %r15 */
	movq $0, %r14			/* store 0 in %r14 */

        movq (%r15), %rdi               /* dereference pointer and store character in %rdi */
        call isDigit                    /* test to see if current char is digit */
        jmp .LDigitCond			/* enter loop */

.LDigitLoop:
	imulq $10, %r14			/* multiply current number by 10 */
	movq (%r15), %r10               /* dereference pointer and store character in %r10 */
	andq $CHAR_MASK, %r10           /* preserve last byte */
	subq $ZERO, %r10		/* subtract value of '0' to find value of digit */
	addq %r10, %r14			/* add value to %r14 */

        inc %r15                        /* move to next character pointer (chars are one byte and arrays are contiguous) */
        movq (%r15), %rdi               /* dereference pointer and store character in %rdi */
        call isDigit                    /* test to see if current char is witespace */

.LDigitCond:
        cmpq $1, %rax                   /* actually sees if current char is digit */
        je .LDigitLoop                  /* enters loop body if char is ws */

        leaq (%r15), %rax               /* put current value of pointer into %rax to be returned */
	movq %r14, (%rsi)		/* store the calculated value in pointer that was passed in */
	
	popq %r14                       /* restore original value of %r14 */
        popq %r15                       /* restore original value of %r15 */
        ret




// accepts long* (stack), long* (stack size), long (value being put on stack)
// if stack is full, program exits
// Otherwise, it puts val in next empty spot and returns count + 1
	.globl stackPush
stackPush:
	movq (%rsi), %r10		/* copy stack size to %r10 */
	cmpl $MAX_SIZE, %r10d		/* compare current stack size to the max size */
	je .LStackFull			/* if they are the same, jumpt to error clause */
	
	movq %rdx, (%rdi, %r10, 8)	/* move the value into the next free entry in array */
	addb $1, (%rsi)			/* increment the size of the array - %rsi contains pointer to size*/
	ret				/* remember to return lol */

.LStackFull:
	subq $8, %rsp                   /* align stack pointer */

	movq $eStackOverFlow, %rdi	/* Move stack overflow error to %rdi */
	call printf 			/* print error */
	
	movq $1, %rdi			/* Move 1 to %rdi */

	addq $8, %rsp                   /* restore stack pointer */
	call exit			/* Exit with value 1 (error) */





// accepts long* (stack), long* (stack size)
// returns long
// if stack is empty, program exits
// Otherwise, it puts val in next empty spot and returns count + 1
        .globl stackPop
stackPop:
        movq (%rsi), %r10               /* copy stack size to %r10 */
        cmpl $0, %r10d                  /* compare current stack size to the max size */
        je .LStackEmpty                 /* if they are the same, jumpt to error clause */

	subb $1, (%rsi)                 /* decrement the size of the array - %rsi contains pointer to size*/
        movq -8(%rdi, %r10, 8), %rax    /* transfer the value in the last filled array element to %rax (adjust by 8) */
	ret

.LStackEmpty:
	subq $8, %rsp                   /* align stack pointer */

        movq $eStackUnderFlow, %rdi     /* Move stack underflow error to %rdi */
        call printf                     /* print error */

        movq $1, %rdi                   /* Move 1 to %rdi */

	addq $8, %rsp                   /* restore stack pointer */

        call exit                       /* Exit with value 1 (error) */



// accepts char*
// returns long
// 1)  creates a stack and stack count
// 2)  reads in a character from the expression and finds it's type
// 2a)     if it's a null terminator, it checks stack size is 1, and returns value on stack
// 2b)     if it's whitespace, it calls skipws to find next non whitespace char, goes back to (2)
// 2c)     if it's a digit, it calls consumeInt, pushes the new number onto the stack, goes back to (2)
// 2c)     if it's an operator, it pops 2 numbers, call function eval, and pushes result to stack, goes back to (2)
// 2d)     if it's anything else, return error
	.globl eval
eval:
	pushq %r12			/* will we used to hold stack */
	pushq %r13			/* will be used to hold stack size */
	pushq %r14			/* will be use to hold expression character */
	pushq %r15			/* will be used to hold num */
	pushq %rbx			/* will be used to hold num pointer */

	movq $dataValues, %r12		/* storing array */
	movq %rdi, %r14			/* storing expression pointer into r14 */

	subq $16, %rsp			/* allocate some memory */
	leaq (%rsp), %r13               /* create new memory for r13 */
	movq $0, (%r13)			/* store stack size in r13 */
	leaq 8(%rsp), %rbx              /* put a valid memory address in rbx */

	jmp .LTestChar

.LInvalidCharCase:
	jmp .LIllChar
.LDigitCase:
	movq %r14, %rdi			/* move current char to 1st arg */
	leaq (%rbx), %rsi		/* move pointer to num into 2nd arg */
	call consumeInt			/* read in next number to num */
	movq %rax, %r14			/* store next char into r14 */

	movq %r12, %rdi			/* move stack pointer into 1st arg */
	movq %r13, %rsi			/* move size pointer into 2nd arg */
	movq (%rbx), %rdx		/* move num into 3rd arg */
	call stackPush			/* push num to stack */

	jmp .LTestChar			/* jump to loop test condition */

.LWhiteSpaceCase:
	movq %r14, %rdi                 /* move current char to 1st arg */
        call skipws                     /* read in next number to num */
        movq %rax, %r14                 /* store next char into r14 */

        jmp .LTestChar                  /* jump to loop test condition */
.LMultCase:
	movq %r12, %rdi			/* move stack pointer to 1st arg */
	movq %r13, %rsi			/* move size pointer to 2nd arg */
	call stackPop			/* pop from stack */
	movq %rax, %r15			/* transfer num to r15 */

	movq %r12, %rdi			/* move stack pointer to first num */
        movq %r13, %rsi			/* move size pointer to second num */
        call stackPop			/* pop from stack */

	imulq %r15			/* multiply numbers together */

	movq %r12, %rdi                 /* move stack pointer into 1st arg */
        movq %r13, %rsi                 /* move size pointer into 2nd arg */
        movq %rax, %rdx                 /* move result into 3rd arg */
        call stackPush                  /* push result to stack */

	incq %r14			/* move past operation char */

        jmp .LTestChar                  /* jump to loop test condition */
.LDivCase:
        movq %r12, %rdi                 /* move stack pointer to 1st arg */
        movq %r13, %rsi                 /* move size pointer to 2nd arg */
        call stackPop                   /* pop from stack */
        movq %rax, %r15                 /* transfer num to r15 */

        movq %r12, %rdi                 /* move stack pointer to first num */
        movq %r13, %rsi                 /* move size pointer to second num */
        call stackPop                   /* pop from stack */

	cmp $0, %r15			/* make sure second num num isn't 0*/
	je .LDivZero			/* if it is return error */

	cqto				/* sign extend - fixed like 90% of failed tests :) */
        idiv %r15                       /* divide numbers and store quotient in rax */

        movq %r12, %rdi                 /* move stack pointer into 1st arg */
        movq %r13, %rsi                 /* move size pointer into 2nd arg */
        movq %rax, %rdx                 /* move result into 3rd arg */
        call stackPush                  /* push result to stack */

	incq %r14                       /* move past operation char */

        jmp .LTestChar                  /* jump to loop test condition */
.LAddCase:
        movq %r12, %rdi                 /* move stack pointer to 1st arg */
        movq %r13, %rsi                 /* move size pointer to 2nd arg */
        call stackPop                   /* pop from stack */
        movq %rax, %r15                 /* transfer num to r15 */

        movq %r12, %rdi                 /* move stack pointer to first num */
        movq %r13, %rsi                 /* move size pointer to second num */
        call stackPop                   /* pop from stack */

        addq %r15, %rax                 /* multiply numbers together */

        movq %r12, %rdi                 /* move stack pointer into 1st arg */
        movq %r13, %rsi                 /* move size pointer into 2nd arg */
        movq %rax, %rdx                 /* move result into 3rd arg */
        call stackPush                  /* push result to stack */

	incq %r14                       /* move past operation char */

        jmp .LTestChar                  /* jump to loop test condition */
.LSubCase:
        movq %r12, %rdi                 /* move stack pointer to 1st arg */
        movq %r13, %rsi                 /* move size pointer to 2nd arg */
        call stackPop                   /* pop from stack */
        movq %rax, %r15                 /* transfer num to r15 */

        movq %r12, %rdi                 /* move stack pointer to first num */
        movq %r13, %rsi                 /* move size pointer to second num */
        call stackPop                   /* pop from stack */

        subq %r15, %rax                 /* multiply numbers together */

        movq %r12, %rdi                 /* move stack pointer into 1st arg */
        movq %r13, %rsi                 /* move size pointer into 2nd arg */
        movq %rax, %rdx                 /* move result into 3rd arg */
        call stackPush                  /* push result to stack */

	incq %r14                       /* move past operation char */

        jmp .LTestChar                  /* jump to loop test condition */

.LCharLoop:
	movq %r14, %rdi                 /* move char pointer to temp reg */
	call tokenType			/* find what type of in it is */
	
	jmp *.LJumpTable(,%rax,8)	/* jump to return value of token type */

.LTestChar:
	movq (%r14), %r8		/* move char to temp reg */
	cmpb $NULLTERM, %r8b 		/* compare char to '\0' */
	jne .LCharLoop			/* if not, keep looping */

	movq (%r13), %r8		/* move size to temp reg */
	cmpq $1, %r8			/* compare size to 1 */
	jne .LMalString			/* return error if not 1 */

	movq (%r12), %rax               /* expression valid return result */

	addq $16, %rsp			/* deallocate memory */

	popq %rbx			/* restore value */
	popq %r15                       /* restore value */
        popq %r14                       /* restore value */
        popq %r13                       /* restore value */
        popq %r12                       /* restore value */

	ret

.LIllChar:
	movq $eIllegalChar, %rdi	/* move sting to rdi */
	call printf			/* print error msg */
	
	movq $1, %rdi			/* move error code to rdi */
	call exit			/* exit on error */
.LDivZero:
	movq $eDivByZero, %rdi          /* move sting to rdi */
        call printf                     /* print error msg */
        
        movq $1, %rdi                   /* move error code to rdi */
        call exit                       /* exit on error */
.LMalString:
        movq $eMalformedExpr, %rdi      /* move sting to rdi */
        call printf                     /* print error msg */

        movq $1, %rdi                   /* move error code to rdi */
        call exit                       /* exit on error */

.LJumpTable:
	.quad .LInvalidCharCase
	.quad .LDigitCase
	.quad .LWhiteSpaceCase
	.quad .LMultCase
	.quad .LDivCase
	.quad .LAddCase
	.quad .LSubCase


/* vim:ft=gas:
 */
