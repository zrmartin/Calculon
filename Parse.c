/*
Zach Martin | 357-07 | PROJECT 4 | Parse.c | 5/19/2016
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "Parse.h"

char ** ParseSuite(Prog **prog, char *fileName) {
   FILE *file;
   char *line;
   char **lines, tempLines;
   Test **temp;

   prog--;
   if ((file = fopen(fileName, "r")) == NULL) {
      fprintf(stderr, "%s: No such file or directory\n", fileName);
      exit(0);
   }

   lines = (char **) CheckFile(file);
   tempLines = (char **) lines;
   while (*lines) {
      if (**lines == 'P') {
         prog++;
         (*lines)++;
         AddProgram(prog, *lines);
         temp = (*prog)->tests;
      }
      else { 
         (*lines)++;
         AddTest(temp, *lines);
         temp++;
      }
      lines++;
   }
   
   return tempLines;
}

char ** CheckFile(FILE *file) {
   char *line = calloc(BUFF_SIZE, 1);
   char **lines = calloc(BUFF_SIZE, sizeof(char *));
   char **temp = lines;
   int c;
   int idx = 0;
   int num = 1;

   while ((c = fgetc(file)) != EOF) {
      if (idx > BUFF_SIZE * num - 1)
         DoubleBuffer(&line, &num);

      if (c == '\n') { 
         *temp++ = line;
         line = calloc(BUFF_SIZE, num);
         idx = 0;
      }
      else {
         *(line + idx) = c;
         idx++; 
      }
   }
   free(line);
   *temp = NULL;
   
   return lines;
}

void DoubleBuffer(char **line, int *num) {
   char *temp;
   
   *num++;
   temp = calloc(BUFF_SIZE, *num);
   strncpy(temp, *line, BUFF_SIZE);
   free(*line);
   *line = temp;
}

void AddProgram(Prog **prog, char *line) {
   char *file;
   char **temp;

   InitializeProg(prog);
   (*prog)->executable = strtok(line, " ");
   temp = (*prog)->files;
   
   while ((file = strtok(NULL, " "))) 
      *temp++ = file;  
}

void InitializeProg(Prog **prog) {
   *prog = malloc(sizeof(Prog));
   (*prog)->files = calloc(MAX_P, sizeof(char *));
   (*prog)->tests = calloc(MAX_T, sizeof(Test *));
   (*prog)->built = 0;
}

void AddTest(Test **test, char *line) {
   char *arg;
   char **temp;

   InitializeTest(test);
   temp = (*test)->argv;

   (*test)->input = strtok(line, " ");
   (*test)->output = strtok(NULL, " ");
   (*test)->time = strtok(NULL, " ");
   
   while ((arg = strtok(NULL, " ")))
      *temp++ = arg;
   
}

void InitializeTest(Test **test) {
   *test = malloc(sizeof(Test));
   (*test)->argv = calloc(MAX_T, sizeof(char *));
   (*test)->runtime = 0;
   (*test)->timeout = 0;
   (*test)->diff = 0;
}

/***** FOR TESTING *****/
void PrintSuite(Prog **prog) {
   int ndx = 0;
   Test **test;
   char **files = (*prog)->files;
   char **temp;
   Prog *cur;
   
   while (*prog) {
      cur = *prog;
      test = cur->tests;
      files = cur->files;
      printf("Program %d: %s Files: ", ndx, cur->executable);
      while (*files)
         printf("%s ", *files++); 
      printf("\n");

      while (*test) {
         temp = (*test)->argv;
         printf("%s %s %s ", (*test)->input, (*test)->output, (*test)->time);
         while (*temp)
            printf("%s ", *temp++);
         printf("\n");	
         test++;
      }
      prog++;
      ndx++;
   }
}