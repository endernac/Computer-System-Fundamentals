/*
 * postfix calculator (assembly language version)
 * main function
 */

.section .rodata

mResultMsg: .string "Result is: %ld\n"

.section .text

/*
 * main function: program entry point
 *
 * Params:
 *   argc - number of command line arguments (including program name)
 *   argv - array of pointers to command line arguments
 *
 * Returns:
 *   exit code
 */
	.globl main
main:

	subq $8, %rsp			/* align stack pointer */

	movq 8(%rsi), %rdi		/* move the expression into rdi */
	call eval			/* evaluate the expression */

	movq $mResultMsg, %rdi		/* if it evaluated, print formatted result */
	movq %rax, %rsi			/* move result into rsi */
	call printf			/* print */

	 
	movq $0, %rax			/* return 0 from main */

	addq $8, %rsp			/* restore stack pointer */

	ret


/* vim:ft=gas:
*/
