/*
Zach Martin | 357-07 | PROJECT 4 | Calculon.h | 5/19/2016
*/

#ifndef CALCULON_H
#define CALCULON_H

#define BUF 100
#define PERM 0777
#define TIMEOUT 201

typedef struct Test {
   char *input;
   char *output;
   char *time;
   char **argv;
   int runtime;
   int timeout;
   int diff;
} Test;

typedef struct Prog {
   char *executable;
   char **files;
   Test **tests;
   int built;
} Prog;

int CopyFiles(Prog *prog, int trash);

void CopyCFiles(Prog *prog, int trash);

int CopyMakefile(int trash);

void CopyIOFiles(Prog *prog, int trash);

void BuildMake(Prog *prog, int trash);

void BuildGcc(Prog *prog, int trash);

void RunTests(Prog **prog, int make, int trash);

void RemoveFiles(Prog *cur);

char ** CreateArgv(Prog *cur, Test **test, int trash);

void FreeArgv(char **argv);

void RunTest(char **argv, char *infile);

void CheckOutput(Test *test, int trash);

void PrintResults(Prog *prog);

void PrintFailure(Prog *cur, Test *test, int ndx);

void FreeLines(char **lines);

void FreeProg(Prog **prog);



#endif

