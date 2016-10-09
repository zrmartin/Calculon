Zach Martin

README:
Calculon is an automated testing system. Calculon loads a suite description file and then executes programs, passing input files and collecting output for diff checking.

Suite Definition File:
A test suite consists of a number of programs and a number of test cases to apply to each program. A test case is defined by an input file, an output file to diff against, the timeout for that test, and an optional list of commandline arguments. Calculon gets test suite information from a suite definition file. The suite definition file contains a series of program lines that start with P followed by one or more test lines for that program, each starting with T. Each P line gives the name of the program to run followed by all of the files required to create that program. Each T line gives the name of the input file, the name of the output file, the timeout for that test in milliseconds, and the commandline arguments to pass to the program when running it (if any).

Below is an example suite definition file, Suite1.suite:

P a.out main.c header.h
T test1.in test1.out 1000 -a -b -c
T test2.in test2.out 1000 -xyz
T test3.in test3.out 800
P b.out bprogram.c SmartAlloc.c SmartAlloc.h
T test1.in test1.out 10000
T test4.in test4.out 30000 -verbose

If all test cases for a program pass, Calculon outputs the program name and the number of passed tests:
$ ./Calculon Suite1.suite
a.out 3 of 3 tests passed.
b.out 2 of 2 tests passed.

In the event of test failures, Calculon outputs the name of the program, test number, and reason for failure.
$ ./Calculon Suite1.suite
b.out test 1 failed: diff failure, runtime error
b.out test 2 failed: timeout

Possible reasons for test failure are:
diff failure
timeout
runtime error

Compilation:
Each program must be built before running tests. If a file called Makefile exists in the directory where Calculon is run, Calculon copies that file into the temporary test directory and executes make <name of test program>. If no such file exists, Calculon simply executes gcc on all .c files specified for the program. 
