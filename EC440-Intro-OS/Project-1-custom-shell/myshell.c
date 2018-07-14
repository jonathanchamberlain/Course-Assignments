/*
Author Jonathan Chamberlain (jdchambo@bu.edu)
Parser for custom Bash Shell
Developed for EC 440
Referenced https://brennan.io/2015/01/16/write-a-shell-in-c/
  for aid in understanding memory allocation logic
Referenced Prof. Giles's runline and Parent-Child.odt examples
  for aid in understanding how to fork processes within shell
Referenced SO question Implementation of multiple pipes in C for 
  pseudocode to implement piping across mulitple processes
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>

// define constants 
#define CMD_BUFF 512 // user input asumed <512 chars

int parsetokens(char*, int*, int*); // parses tokens
void exectokens(char*, int*, int*, int); // executes tokens
void tokenizer(char*, char*, int*, int*, int); // renders input as tokens

int main(int argc, char* argv[]) {
  /* 
     runs the bash shell
     loops for input until user terminates program
     assumes command lines < 512 characters
  */
  
  char command[CMD_BUFF]; // input from user
  int numtoks; // number of tokens
  bool printprompt = true;
  if (argc > 1) {
    if (strcmp(argv[1],"-n") == 0) printprompt = false;
  } 
  if (printprompt) printf("my_shell>"); // intial startup prompt
  // loop for user input, continue until get EOF
  while(fgets(command, CMD_BUFF, stdin) != NULL) {
    int *tokenpos = malloc(CMD_BUFF); // position of tokens
    int *tokenlen = malloc(CMD_BUFF); // length of tokens
    numtoks = parsetokens(command, tokenpos, tokenlen); // parse tokens
    exectokens(command, tokenpos,tokenlen, numtoks); // execute tokens
    if (printprompt) printf("my_shell>"); // prompt for next line
    free(tokenpos); // reset position array for next loop
    free(tokenlen); // reset length array for next loop 
  }
  exit(0);
  // printf("\n"); // print new line upon program exit
  // return 0;
}

int parsetokens(char *line, int *pos, int *len) {
  /*
     parses the token list
     line - the input line from the user
     pos - the positions of the tokens from the line
     len - the lengths of the tokens from the line
  */
  
  int numtok = 0; // number of tokens in array
  bool activetok = false; // flag for "collecting chars" for token

  int i;
  for(i=0; line[i]!='\0'; i++) {
    if (line[i] == '|' || line[i] == '&' || line[i] == '>' || line[i] == '<') {
      // next char is meta
      if (activetok) {
        // no space delimiter between metachar and argument
        len[numtok] = i - pos[numtok];
        activetok = false;
        numtok++;
      } 
      pos[numtok] = i; // metachar position
      len[numtok] = 1; // all valid metas have len 1
      numtok++;
    } else if (line[i] == ' ' || line[i] == '\t' || line[i+1] == '\0') {
      // next char is whitespace, or am at final char in the line
      if (activetok) {
        // was collecting chars for token
        len[numtok] = i - pos[numtok];
        activetok = false;
        numtok++;
      }
    } else {
      // next char is nonmeta, nonwhitespace
      if (!activetok) {
        // not currently building token
        pos[numtok] = i;
        activetok = true;
      }
    }  // end if-else block
  } // end for loop
  return numtok;
}

void exectokens(char *line, int *pos, int *len, int n) {
  /*
     executes the tokens provided by the user
     line - the user's input
     pos - the positions of each token
     len - the length of each token
     n - the number of tokens
  */
  
  // copy line into /0 delimited string
  char tokenized[CMD_BUFF + n + 1];
  tokenizer(line, tokenized, pos, len, n);

  /*
    scan for command tokens, argument tokens, metas
    pass commands, args into separate array
    handle metas as appropriate
  */
  int offset = 0; // addr marker
  int tokno = 0; // marker for current token in tokenized

  char *argslist[n];  
  int argno = 0; // marker for current arg in argslist

  int numcmds = 0; // total number of commands
  int numargs[n]; // number of args in each command
  int ccargs = 0; // number of args in current command

  char *redir[]={NULL,NULL}; // redirect file names

  bool foregrd = true;
  
  while (tokno < n) {
    switch(tokenized[offset]) {
      case '<' :
	// iput redirect
        if (redir[0]) {
          printf("ERROR: Only one input redirect allowed\n");
          return;
        }
        if (numcmds > 0) {
          printf("ERROR: Input redirect only on first command\n");
	  return;
        }
	offset = offset + len[tokno] + 1;
	tokno++;
	redir[0] = &tokenized[offset];
	break;
      case '>' :
	// oput redirect
        if (redir[1]) {
          printf("ERROR: Only one output redirect allowed\n");
          return;
        }
	offset = offset + len[tokno] + 1;
	tokno++;
	redir[1] = &tokenized[offset];
	break;
      case '&' :
	// background marker
        if (tokno < (n-1)) {
	  printf("ERROR: Background flag must be last argument\n");
          return;
        }
        foregrd = false;
	break;
      case '|' :
        // pipe
	if (redir[1]) {
          printf("ERROR: Output redirect only on final command\n");
          return;
        }
        if (ccargs == 0) {
	  printf("ERROR: No command specified in pipe\n");
	  return;
	}
	numargs[numcmds] = ccargs;
	numcmds++;
	ccargs = 0;
	break;
      default :
	ccargs++;
	argslist[argno] = &tokenized[offset];
	argno++;
    }
    offset = offset + len[tokno] + 1;
    tokno++;
  }
  // last command won't be pipe terminated
  if (ccargs == 0) {
    printf("ERROR: No command specified in pipe\n");
    return;
  }
  numargs[numcmds] = ccargs;
  numcmds++;
  
  // set up redir fid collection, create all needed pipes before executing
  int redirfd[2]; // redir file ids
  int numpipes = numcmds-1;
  int pipefds[2*(numpipes)];
  int i;
  for (i = 0; i < numpipes; i++) {
    if (pipe(pipefds + i*2) == -1) {
      printf("ERROR: Pipe Open Failure\n");
      return;
    }
  }

  // loop over commands
  int ccmd = 0;
  int startarg = 0;
  int numargscc; // number of arguments in current command
  while (ccmd < numcmds) {
    // build next args list
    numargscc = numargs[ccmd];
    char *myargv[numargscc+1];
    for (i = 0; i < numargscc; i++) {
      myargv[i] = argslist[startarg + i];
    }
    myargv[numargscc] = '\0';
    startarg += numargs[ccmd];
    // execute command
    pid_t pid;
    int status;
    pid = fork();
    if (pid < 0) {
      printf("ERROR: Fork failed\n");
      return;
    }
    if (pid) {
      // parent process
      // close paren copies of open pipes
      if (ccmd > 0) {
        // close read end
        if (close(pipefds[(ccmd-1)*2]) < 0) {
	  printf("ERROR: Parent Pipe Read close failure\n");
	  return;
        }
      }
      if (ccmd < (numcmds - 1)) {
	// close write end
	if (close(pipefds[ccmd*2+1]) < 0) {
	  printf("ERROR: Parent Pipe Write close failure\n");
          return;
	}
      }
      // if foreground process, wait; if in background, don't
      if (foregrd) {
        wait(&status);
      }
    } else {
      // child process
      // iput redirect - if first cmd
      if (redir[0] && ccmd == 0) {
        redirfd[0] = open(redir[0],O_RDONLY);
        if (redirfd[0] < 0) {
	  printf("ERROR: Input File open failure\n");
          return;
        }
        if (dup2(redirfd[0],0) < 0) {
	  printf("ERROR: Dup2 Failure on Input File\n");
          return;
        }
        if (close(redirfd[0]) < 0) {
	  printf("ERROR: Input File close failure\n");
          return;
	}
      }
      // if not first cmd - input from pipe
      if (ccmd != 0) {
        if (dup2(pipefds[(ccmd-1)*2],0) < 0) {
	  printf("ERROR: Dup2 Failure on Pipe Read\n");
          return;
        }
        if (close(pipefds[(ccmd-1)*2]) < 0) {
	  printf("ERROR: Pipe Read close failure\n");
          return;
	}
      }
      // oput redirect - if last cmd
      if (redir[1] && ccmd == (numcmds-1)) {
        redirfd[1] = open(redir[1],O_WRONLY+O_CREAT+O_TRUNC);
        if (redirfd[1] < 0) {
          printf("ERROR: Output File open failure\n");
          return;
	}
	if (dup2(redirfd[1],1) < 0) {
	  printf("ERROR: Dup2 Failure on Output File\n");
          return;
        }
        if (close(redirfd[1]) < 0) {
	  printf("ERROR: Output File close failure\n");
          return;
	}
      }
      // if not last cmd - output to next pipe
      if (ccmd != (numcmds-1)) {
        if (dup2(pipefds[ccmd*2+1],1) < 0) {
	  printf("ERROR: Dup2 Failure on Pipe Write\n");
          return;
        }
        if (close(pipefds[ccmd*2+1]) < 0) {
	  printf("ERROR: Pipe write close failure\n");
          return;
	}
      }
      if (execvp(myargv[0], myargv) == -1) {
        printf("ERROR: Command Not Found\n");
        exit(EXIT_FAILURE);
        return;
      }    
    }
    ccmd++;
  }
}

void tokenizer(char *line, char *tokens, int *pos, int *len, int n) {
  /*
     copies cmdline input into \0 delimited string
     line - the user's input
     tokens - the string of \0 delimited inputs
     pos - the positions of each token
     len - the length of each token
     n - the number of tokens
  */
  int tpos = 0; // index of position in tokenized
  int tokstrt; // starting index of next token
  int tokend; // end of next token
  int i, j; 
  // copy into toeknized
  for (i = 0; i < n; i++) {
    tokstrt = pos[i];
    tokend = tokstrt + len[i];
    // copy next token into string
    for (j = tokstrt; j < tokend; j++) {
      tokens[tpos] = line[j];
      tpos++;
    }
    // \0 terminate token
    tokens[tpos] = '\0';
    tpos++;
  }
}
