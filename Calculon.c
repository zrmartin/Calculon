/*
Zach Martin | 357-07 | PROJECT 4 | Calculon.c | 5/19/2016
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "Calculon.h"

int main (int argc, char **argv) {
   Prog **prog = calloc(BUF, sizeof(Prog *));
   int mainPID = getpid(), make;
   char dir[BUF];
   char **lines;
   int trash = open("/dev/null", O_WRONLY);
   
   if (argc < 2) {
      fprintf(stderr, "usage: Calculon <suite definition file>\n");
      exit(EXIT_FAILURE);
   }

   lines = (char **) ParseSuite(prog, *(argv + 1));
   
   sprintf(dir, ".%d", mainPID);
   mkdir(dir, PERM);
   chdir(dir);

   RunTests(prog, make, trash);

   FreeLines(lines);
   free(lines);
   //FreeProg(prog);
   free(prog);

   return 0;
}

int CopyFiles(Prog *prog, int trash) {
   int make;
   
   CopyCFiles(prog, trash);
   make = CopyMakefile(trash);
   CopyIOFiles(prog, trash);

   return make;
}

void CopyCFiles(Prog *prog, int trash) {
   Prog *cur = prog;
   char prevDir[BUF];
   char **files;
   int pid, status; 

   files = cur->files;
   while (*files) {
      if ((pid = fork()) < 0) {
         fprintf(stderr, "fork failed\n"); 
         exit(EXIT_FAILURE);
      }
      else if (pid == 0) {
         dup2(trash, 1);
         dup2(trash, 2);
         sprintf(prevDir, "../%s", *files);
         execl("/bin/cp", "cp", prevDir, ".", NULL);
         fprintf(stderr, "exec failed\n");
         exit(EXIT_FAILURE);
      }
      else {
         wait(&status);
         if (WEXITSTATUS(status)) {
            fprintf(stderr, "Could not find required test file"
             "'%s'\n", *files);
            exit(EXIT_FAILURE);
         }
         files++;
      }
   }
}

int CopyMakefile(int trash) {
   int pid, status, make = 0;
   
   if ((pid = fork()) < 0) {
      fprintf(stderr, "fork failed\n"); 
      exit(EXIT_FAILURE);
   }
   else if (pid == 0) {
      dup2(trash, 1);
      dup2(trash, 2);
      execl("/bin/cp", "cp", "../Makefile", ".", NULL);
      fprintf(stderr, "exec failed\n");
      exit(EXIT_FAILURE);
   }
   else  {
      wait(&status);
      if (!WEXITSTATUS(status))
         make = 1;
      return make;
   }
}

void CopyIOFiles(Prog *prog, int trash) {
   Prog *cur = prog;
   char prevDir[BUF];
   int pid, status; 
   Test **test;
   
   test = cur->tests;
   while (*test) {
      if ((pid = fork()) < 0) {
         fprintf(stderr, "fork failed\n"); 
         exit(EXIT_FAILURE);
      }
      else if (pid == 0) {
         dup2(trash, 1);
         dup2(trash, 2);
         sprintf(prevDir, "../%s", (*test)->input);
         execl("/bin/cp", "cp", prevDir, ".", NULL);
      }
      else {
         wait(&status);
         if (WEXITSTATUS(status)) {
            fprintf(stderr, "Could not find required test file"
             " '%s'\n", (*test)->input);
            exit(EXIT_FAILURE);
         }
         if ((pid = fork()) < 0) {
            fprintf(stderr, "fork failed\n"); 
            exit(EXIT_FAILURE);
         }
         else if (pid == 0) {
            dup2(trash, 1);
            dup2(trash, 2);
            sprintf(prevDir, "../%s", (*test)->output);
            execl("/bin/cp", "cp", prevDir, ".", NULL);
         }
         else {
            wait(&status);
            if (WEXITSTATUS(status)) {
               fprintf(stderr, "Could not find required test file"
                " '%s'\n", (*test)->output);
               exit(EXIT_FAILURE);
            }
         }
      }
      test++;
   }
}

void BuildMake(Prog *prog, int trash) {
   Prog *cur = prog;
   int pid, status;
   
   if ((pid = fork()) < 0) {
      fprintf(stderr, "fork failed\n"); 
      exit(EXIT_FAILURE);
   }
   else if (pid == 0) {
      dup2(trash, 1);
      execl("/usr/bin/make", "make", cur->executable, NULL);
      fprintf(stderr, "exec failed\n");
      exit(EXIT_FAILURE);
   }
   else {
      wait(&status);
      if (WEXITSTATUS(status))
         fprintf(stderr, "Failed: make %s\n", cur->executable);
      else 
         cur->built = 1;
   }
}

void BuildGcc(Prog *prog, int trash) {
   Prog *cur = prog;
   char **files, **argv;
   int pid, status, ndx = 1;
   
   files = cur->files;
   argv = malloc(BUF * sizeof(char *));
   *argv = "gcc";
   while (*files) {
      if ((strstr(*files, ".c")) != NULL) {
         *(argv + ndx) = *files;
         ndx++;
      } 
      files++;
   }

   *(argv + ndx) = "-o";
   ndx++;
   *(argv + ndx) = cur->executable;
   ndx++;
   *(argv + ndx) = NULL;

   if ((pid = fork()) < 0) {
      fprintf(stderr, "fork failed\n"); 
      exit(EXIT_FAILURE);
   }
   else if (pid == 0) {
      execv("/usr/bin/gcc", argv);
      fprintf(stderr, "exec failed\n");
      exit(EXIT_FAILURE);
   }
   else {
      wait(&status);
      if (WEXITSTATUS(status)) {
         fprintf(stderr, "Failed:");
         while (*argv)
            fprintf(stderr, " %s", *argv++);
         fprintf(stderr, "\n");
      }
      else {
         cur->built = 1;
         free(argv);
		  
	  }
      ndx = 1;
   }
}

void RunTests(Prog **prog, int make, int trash) {
   Prog **temp = prog;
   Prog *cur;
   char **argv;
   int pid, status, out;
   Test **test;

   while (*temp) {
      cur = *temp;
      test = cur->tests;
      make = CopyFiles(cur, trash);
      if (make)
         BuildMake(cur, trash);
      else
         BuildGcc(cur, trash);
      if (cur->built) {
         while (*test) {
            argv = CreateArgv(cur, test, trash);
            if ((pid = fork()) < 0) {
               fprintf(stderr, "fork failed\n"); 
               exit(EXIT_FAILURE);
            }
            else if (pid == 0) {
               RunTest(argv, (*test)->input);
            }
            else {
               wait(&status);
               if (WEXITSTATUS(status)) {
                  (*test)->runtime = 1;
                  if ((WEXITSTATUS(status)) == TIMEOUT)
                     (*test)->timeout = 1;
               }
               FreeArgv(argv);
               free(argv);			   
               CheckOutput(*test, trash);
               test++;
            }
         }
         PrintResults(cur);
      }

      RemoveFiles(cur);
      temp++;
   }   
}

void RemoveFiles(Prog *cur) {
   char **files = cur->files;
   Test **tests = cur->tests;

   remove(cur->executable);
   while (*files)
      remove(*files++);
   while (*tests) {
      remove((*tests)->input);
      remove((*tests)->output);
      tests++;      
   }
}

char ** CreateArgv(Prog *cur, Test **test, int trash) {
   char **temp = (*test)->argv, **argv = malloc(BUF * sizeof(char *));
   char *arg = malloc(BUF);
   int ndx = 1;
   
   *argv = "SafeRun";
   
   *(argv + ndx) = "-p30";
   ndx++;
   
   sprintf(arg, "-t%s", (*test)->time);
   *(argv + ndx) = arg;
   ndx++;
   
   arg = malloc(BUF);
   sprintf(arg, "-T%s0", (*test)->time);
   *(argv + ndx) = arg;
   ndx++;
   
   arg = malloc(BUF);
   sprintf(arg, "./%s", cur->executable);
   *(argv + ndx) = arg;
   ndx++;
   
   while (*temp) {
      *(argv + ndx) = *temp++;
      ndx++;
   }

   *(argv + ndx) = NULL;
   
   return argv;
}

void FreeArgv(char **argv) {
   char *temp;

   while (*argv) {
      temp = *argv;
      argv++;
      free(temp);
   }   
}

void RunTest(char **argv, char *infile) {
   int in = open(infile, O_RDONLY);
   int out = open("test.output.temp", O_RDWR | O_CREAT | O_TRUNC, PERM);
   
   dup2(in, 0);
   dup2(out, 1);
   dup2(out, 2);
   execv("/home/grade-cstaley/bin/warden/SafeRun", argv);
   fprintf(stderr, "exec failed\n");
   exit(EXIT_FAILURE);  
}

void CheckOutput(Test *test, int trash) {
   int pid, status;
   
   if ((pid = fork()) < 0) {
      fprintf(stderr, "fork failed\n"); 
      exit(EXIT_FAILURE);
   }
   else if (pid == 0) {
      dup2(trash, 1);
      dup2(trash, 2);
      execl("/usr/bin/diff", "diff", "test.output.temp", test->output, NULL);
   }
   else {
      wait(&status);
      if ((WEXITSTATUS(status)) != 0)
         test->diff = 1;
   }
}

void PrintResults(Prog *prog) {
   Prog *cur = prog;
   int pid, status, ndx = 1, passed = 1;
   Test **test;
   
   if (cur->built) {
      test = cur->tests;
      while (*test) {
         if ((*test)->runtime || (*test)->timeout || (*test)->diff) {
            PrintFailure(cur, *test, ndx);
            passed = 0;
         }
         test++;
         ndx++;
      }
      if (passed) {
         printf("%s %d of %d tests passed.\n", cur->executable,
          ndx - 1, ndx - 1);
      }
   }
}

void PrintFailure(Prog *cur, Test *test, int ndx) {
   printf("%s test %d failed: ", cur->executable, ndx);
   
   if (test->diff) {
      printf("diff failure");
      if (test->runtime)
         printf(", runtime error");
      if (test->timeout)
         printf(", timeout");
   }
   
   else if (test->runtime) {
      printf("runtime error");
      if (test->timeout)
         printf(", timeout");
   }
   printf("\n");
}

void FreeLines(char **lines) {
   char *temp;
   
   while (*lines) {
      temp = *lines;
      lines++;
      free(temp);
   }
}

void FreeProg(Prog **prog) {
   Prog *cur = *prog;

   while (cur) {
      while (*(cur->tests)) {
         FreeArgv((*cur->tests)->argv);
         free((*cur->tests)->argv);
	  }
      free((*prog)->tests);
      free((*prog)->files);
	  prog++;
      cur = *prog;
   }
}


