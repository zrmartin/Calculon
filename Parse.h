/*
Zach Martin | 357-07 | PROJECT 4 | Parse.h | 5/19/2016
*/

#ifndef PARSE_H
#define PARSE_H

#define BUFF_SIZE 100
#define MAX_P 33
#define MAX_T 33

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

char ** ParseSuite(Prog **prog, char *fileName);

char ** CheckFile(FILE *file);

void DoubleBuffer(char **line, int *num);

void AddProgram(Prog **prog, char *line);

void InitializeProg(Prog **prog);

void AddTest(Test **test, char *line);

void InitializeTest(Test **test);

/***** FOR TESTING *****/
void PrintSuite(Prog **prog);

#endif