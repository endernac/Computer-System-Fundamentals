#
# CSF Assignment 2 Makefile
#

# Executables to build for the "all" target
EXES = cPostfixCalc cTests asmPostfixCalc asmTests

# C source files
C_SRCS = cPostfixCalcMain.c cPostfixCalcFuncs.c cTests.c tctest.c

# Assembler source files
ASM_SRCS = asmPostfixCalcMain.S asmPostfixCalcFuncs.S

# Files to be submitted
SUBMIT_FILES = $(shell ls *.c) $(shell ls *.S) $(shell ls *.h) \
	Makefile funcs.sh sysTests.sh

# C compiler flags
CFLAGS = -Og -g -Wall -no-pie

# Flags to use when assembling
ASMFLAGS = -g -no-pie

# Flags to use when linking an executable
LDFLAGS = -g -no-pie

# Rule for assembling a .S file to produce a .o file
%.o : %.S
	gcc -c $(ASMFLAGS) $*.S -o $*.o

# Rule for compiling and assembling a .c file to produce a .o file
%.o : %.c
	gcc $(CFLAGS) -c $*.c -o $*.o

# Default target: build all executables
all : $(EXES)

# C version of postfix calculator
cPostfixCalc : cPostfixCalcMain.o cPostfixCalcFuncs.o
	gcc -o $@ $(LDFLAGS) cPostfixCalcMain.o cPostfixCalcFuncs.o

# Unit test program for C functions
cTests : cTests.o tctest.o cPostfixCalcFuncs.o
	gcc -o $@ $(LDFLAGS) cTests.o tctest.o cPostfixCalcFuncs.o

# Assembly language version of postfix calculator
asmPostfixCalc : asmPostfixCalcMain.o asmPostfixCalcFuncs.o
	gcc -o $@ $(LDFLAGS) asmPostfixCalcMain.o asmPostfixCalcFuncs.o

# Unit test program for assembly language functions
asmTests : asmTests.o tctest.o asmPostfixCalcFuncs.o
	gcc -o $@ $(LDFLAGS) asmTests.o tctest.o asmPostfixCalcFuncs.o

# This target creates a zipfile (solution.zip) that can be uploaded
# to Gradescope.
solution.zip : $(SUBMIT_FILES)
	rm -f $@
	zip -9r $@ $(SUBMIT_FILES)

# Clean up generated files (such as object files and executables)
clean :
	rm -f *.s *.o $(EXES) depend.mak solution.zip

depend.mak :
	touch $@

# This target generates C header file dependencies (so that C modules
# get recompiled whenever their source or included header files get
# modified).
depend :
	gcc -M $(CFLAGS) $(C_SRCS) >> depend.mak

include depend.mak
