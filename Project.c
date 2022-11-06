
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#define LINE_LEN     80
#define MAX_ARGS     64
#define MAX_ARG_LEN  16
#define MAX_PATH 	 64
#define	MAX_PATH_LEN 96
#define WHITESPACE	" .,\t\n"

#ifndef NULL
#define NULL 
#endif

 struct command_t {
 
 // keeps name of the binary program for the command 
 char *name;
 int argc;
 char *argv[MAX_ARGS]; 
 
 };

char *lookupPath(char **argv, char **dir) {
	/* This function searches the directories identified by 
	the dir argument to see if argv[0] (the file name) appears there.
	Allocate a new string, place the full path name in it, 
	then return the string. */
	int i;
	char *result;
	char pName[MAX_PATH_LEN];
	int pathSize = sizeof(*dir)/sizeof(char);

	char filepath[1024];
	strcpy(filepath, "");

	/* Check to see if file name is already an absolute path name */ 
	if(*argv[0] == '/') {
		return argv[0]; 
	}



	/* Look in PATH directories
	   use access() to see if the file is in a dir */
	for (i = 1; i < pathSize; i++) {
		dir++;
		strcat(filepath, *dir);
		strcat(filepath, "/");
		strcat(filepath, argv[0]);
		

		if (access(filepath, F_OK) == 0) {
			printf("Found valid filepath: %s\n", filepath);
			return filepath;
		}

		// printf("filepath: %s\n", filepath); // extremely useful for debugging, prints the full filepath 
		memset(filepath, 0, 1024); //clear the char array
		char filepath[1024] = "";

	}


	// if file name not found in any path variable till now then
	return NULL;
}

int parseCommand(char* cLine, struct command_t* cmd) {

	int argc;
	char** clPtr;

	// Initialization 
	clPtr = &cLine; // cLine is the command line
	argc = 0;

	cmd->argv[argc] = (char *)malloc(MAX_ARG_LEN);

	//Filling argv[]
	while ((cmd->argv[argc] = strsep(clPtr, WHITESPACE)) != NULL) {

		cmd->argv[++argc] = (char *)malloc(MAX_ARG_LEN);

	}

	// Setting the command name and argc
	cmd->argc = argc - 1;
	cmd->name = (char *)malloc(sizeof(cmd->argv[0]));
	strcpy(cmd->name, cmd->argv[0]);

	return 1;
}

int parsePath(char **dirs) {
	char *pathEnvVar; 
	char *thePath;

	const char delimiter[2] = ":";

    int i;
	
	while(*dirs++) {
		*dirs = NULL; 
	} 
	
	pathEnvVar = (char *) getenv ("PATH"); 
	thePath = (char *) malloc(strlen(pathEnvVar) + 1);
	strcpy(thePath, pathEnvVar);
	
   char *token = (char *) malloc(strlen(pathEnvVar));
   
   /* get first token */
   token = strtok(thePath, delimiter);

   /* get other tokens */
   while( token != NULL ) 
	{
	  token = strtok(NULL, delimiter);
	  *dirs = token;
	  dirs++;
   }

   return 1;
}

void printPrompt() {

	char promptString[] = "\n[user@machine1]$ ";
	printf("%s", promptString);
}

void readCommand(char *buffer) {

	gets(buffer);
}

int main(int argc, char *argv[]) {

 
 // Shell initialization 
 
	 //char *pathv[MAX_PATH];
	 //char commandLine[LINE_LEN];
	 struct command_t command;
	 int pid, status;
	 pid_t child;
	 char buffer[MAX_PATH_LEN];

	 char *commandLine = (char *) malloc(LINE_LEN);
	 char **pathv = (char **)malloc(sizeof(char)*MAX_ARGS);
	 
	 parsePath(pathv);
	 
	 while (1){
		 
		 printPrompt();
		 
		 // Read the command line and parse it
		 
		 readCommand(commandLine);
		 
		 if (strcmp(commandLine, "exit") == 0)
			 exit(0);
			 			 
		 else {
		 if (strlen(commandLine) > 0){
			 
			 parseCommand(commandLine, &command);
			 command.name = lookupPath(command.argv, pathv);
		 
		 // Get the full pathname for the file
				if (command.name == NULL){
			 
				//error report
				fprintf(stderr, "command \"%s\" is unknown", command.name);	
				continue;
			}
		 
				else {
				// creates child and executes the command
				
		 
				if ((pid = fork()) == 0){
			 
					execv(command.name, command.argv);
						exit(0);
					 }
					 
		 // Wait for the child to terminate
		 
				else 
				wait(NULL);
		 
			}
	 
	 // Shell termination 
	 
	 if (command.name != NULL)
		free(command.name);
	 
			}
		}
	 }
	 return 0;
}