#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <fcntl.h>




//global variables
extern char **environ;

void eval(char command[], char *args[], char* envp[]) { //takes in the string that is the command line

	int x = 0;
	int background = 0;


	//check for background
	while (args[x] != NULL) {
		if (strcmp(args[x], "&") == 0) {
			background = 1;
			break;
		}
		x++;
	}



	if (background == 1) { //fork/exec without wait
		if (fork() != 0) {

			;

		} else {


			execve(command, args, envp);
			exit(0);



		}




	} else if (background == 0) { //fork/exec with wait
		if (fork() != 0) {

			wait(NULL);



		} else {


			execve(command, args, envp);
			exit(0);



		}


	}





}


void pipefunc(char *args[]) {

	int fd[2];
	int bytes_read;
	pid_t pid1;
	pid_t pid2;
	//char buf[64];
	char *arr1[100];//used to copy the commands from args so that they can each be execed properly
	//might need to memset arr array!
	char *arr2[100];
	int j, i;
	j = 0;
	i = 0;



	//copy commands and their arguments on both sides of the pipe into their own arrays to be execed
	while (strcmp(args[j], "|") != 0) {
		arr1[j] = args[j];
		j++;
	}
	arr1[j] = NULL;

	i = j + 1;

	int v = 0;

	while (args[i] != NULL) {
		arr2[v] = args[i];
		i++;
		v++;
	}
	arr2[v] = NULL;








	//parent first creates pipe
	if (pipe(fd) == -1) {
		printf("pipe error");
		exit(1);
	}



	//Now we fork so that the parent and child share the same file descriptors and can now be piped
	if ((pid1 = fork()) == -1) {
		printf("fork error");
		exit(1);
	} else if (pid1 == 0) {
		//close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		//fflush(stdout);
		execve(arr1[0], arr1, environ);
		//execlp(arr1[0], arr1[0], NULL);
		printf("exec error");
		exit(1);

	} else {


		;


	}

	if ((pid2 = fork()) == -1) {
		printf("pipe error\n");
		exit(1);
	} else if (pid2 == 0) {
		//close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		close(fd[1]);
		//fflush(stdout);
		execve(arr2[0], arr2, environ);
		//execlp(arr2[0], arr2[0], NULL);
		printf("exec error");
		exit(1);

	} else {
		//close(fd[1]);
		//dup2(fd[0], STDIN_FILENO);
		//close(fd[0]);
		;

	}

	close(fd[0]);
	close(fd[1]);
	/* Wait for the children to finish, then exit. */
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);





}




void file_redirection(char *args[], int choice) {
	char *ar1[100];
	//char *ar2[100];

	int filedes;

	if (fork() != 0) {
		wait(NULL);
	} else {

		if (choice == 1) {
			int k  = 0;
			while (strcmp(args[k], ">") != 0) {
				ar1[k] = args[k];
				k++;
			}
			ar1[k] = NULL;
			filedes = open(args[k + 1], O_CREAT | O_TRUNC | O_WRONLY, 0644); //overwrites if ">"
			//filedes = creat(output, 0644);
			dup2(filedes, STDOUT_FILENO);
			close(filedes);



		} else if (choice == 2) { //handles input redirection AND input/output redirection
			int p = 0;
			while (strcmp(args[p], "<") != 0) {
				ar1[p] = args[p];
				p++;

			}
			ar1[p] = NULL;
			if (strcmp(args[p + 2], ">") == 0) {

				filedes = open(args[p + 1], O_RDONLY, 0644);
				dup2(filedes, STDIN_FILENO);
				close(filedes);

				filedes = open(args[p + 3], O_CREAT | O_TRUNC | O_WRONLY, 0644);
				dup2(filedes, STDOUT_FILENO);
				close(filedes);

			} else {

				filedes = open(args[p + 1], O_RDONLY, 0644);
				dup2(filedes, STDIN_FILENO);
				close(filedes);
			}



		} else if (choice == 3) {
			int e = 0;
			while (strcmp(args[e], ">>") != 0) {
				ar1[e] = args[e];
				e++;
			}
			ar1[e] = NULL;
			filedes = open(args[e + 1], O_CREAT | O_APPEND | O_WRONLY, 0644); //appends
			dup2(filedes, STDOUT_FILENO);
			close(filedes);


		}

		execve(ar1[0], ar1, environ);
		exit(0);


	}




}


void read_line (char command[], char *args[]) {
	char parseline[1024];
	int count = 0, i = 0, j = 0;
	char *array[100], *tokenline;

	memset(parseline, 0 , sizeof(parseline));

	for ( ;; ) {
		//memset(parseline, '0', sizeof(parseline));
		//memset(array, '0', sizeof(array));
		int c = fgetc(stdin);
		parseline[count++] = (char) c; //use this to count characters of command
		if (c == '\n') { //get characters until the user types "return" and then we have a line of characters
			break;
		}
	}
	if (count == 1) { //this means there is no command therefore we leave the function
		return;
	}
	tokenline = strtok(parseline, " \n\t"); //note: may not need \t

	//parse line into individual words
	while (tokenline != NULL) {
		array[i++] = strdup(tokenline); //puts the arguments in the array
		tokenline = strtok(NULL, " \n\t");
	}

	//first element/word is command
	strcpy(command, array[0]);     //puts the command as first element in array

	//subsequent words are parameters
	for (int j = 0; j < i; j++) {
		args[j] = array[j];
		args[i] = NULL; //must add a NULL to end list of arguments
	}



}


void print_prompt() {

	char directory[100];

	static int num = 1;
	if (num) {
		char* CLEAR_SCREEN_ANSI = " \e[1;1H\e[2J";
		write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 14);
		num = 0;
	}

	printf("shell>"); //print prompt
	getcwd(directory, sizeof(directory));
	printf("%s", directory);
	printf("$ ");





}



int main() {

	//char command[100], commandtwo[100], *param[20];
	char command [100]; //declarded as an an array becase we will pass it in to function therefore we want it to be modifiable
	char * args[20]; //array of strings which will be array of commandline arguments
	char *envp[] = {(char *) "PATH=/BIN", 0}; //note: char *envp is an array of strings!! 0 is same as NULL
	int stout; //to hold filedescriptor of stdout. Used for output redirection of appropriate builtins
	int filed;
	FILE *fp;
	char readme[1024];
	//char *en = getenv("PATH");
	//char *environ[] = {en, NULL};


	while (1) {
		print_prompt();
		read_line(command, args); //should modify the already declared char line[100]

		char *echoarr[100];

		//Builtin commands... note: make the builtin commands as their own functions later on and put them in a separate file and make a .h file to include them in this file
		if (strcmp(command, "exit") == 0) { //EOF condition
			exit(0);
			break;

		} else if (strcmp(command, "pwd") == 0) {
			char cwd[200];
			if (getcwd(cwd, sizeof(cwd)) != NULL) {
				printf("Current working dir: %s\n", cwd);

			} else {
				perror("getcwd() error");
				return 1;
			}


		} else if (strcmp(command, "cd") == 0) {


			if (args[1] == NULL) {

				chdir(getenv("HOME"));

			} else {

				if (chdir(args[1]) != 0) {
					perror("shell"); //no such file or directory exists
				}
			}


		} else if (strcmp(command, "clear") == 0) {
			char* CLEAR_SCREEN_ANSI = " \e[1;1H\e[2J";
			write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 14);


		} else if (strcmp(command, "dir") == 0) {

			struct dirent *d;  // Pointer for directory entry

			if (args[1] != NULL) {

				if (strcmp(args[1], ">") == 0) { //redirect output
					filed = open(args[2], O_CREAT | O_TRUNC | O_WRONLY, 0644);

					stout = dup(STDOUT_FILENO);

					dup2(filed, STDOUT_FILENO);
					close(filed);

					DIR *dr = opendir(".");

					if (dr == NULL)  // opendir returns NULL if couldn't open directory
					{
						printf("Could not open current directory" );
						return 0;
					}


					while ((d = readdir(dr)) != NULL)
						printf("%s\n", d->d_name);

					closedir(dr);




					dup2(stout, STDOUT_FILENO);

				} else if (strcmp(args[1], ">>") == 0) { //redirect output
					filed = open(args[2], O_CREAT | O_APPEND | O_WRONLY, 0644);

					stout = dup(STDOUT_FILENO);

					dup2(filed, STDOUT_FILENO);
					close(filed);

					DIR *dr = opendir(".");

					if (dr == NULL)  // opendir returns NULL if couldn't open directory
					{
						printf("Could not open current directory" );
						return 0;
					}


					while ((d = readdir(dr)) != NULL)
						printf("%s\n", d->d_name);

					closedir(dr);





					dup2(stout, STDOUT_FILENO);

				}

			} else {

				DIR *dr = opendir(".");

				if (dr == NULL)  // opendir returns NULL if couldn't open directory
				{
					printf("Could not open current directory" );
					return 0;
				}


				while ((d = readdir(dr)) != NULL)
					printf("%s\n", d->d_name);

				closedir(dr);
			}

		} else if (strcmp(command, "env") == 0) {

			int i;

			if (args[1] != NULL) {

				if (strcmp(args[1], ">") == 0) { //redirect output
					filed = open(args[2], O_CREAT | O_TRUNC | O_WRONLY, 0644);

					stout = dup(STDOUT_FILENO);

					dup2(filed, STDOUT_FILENO);
					close(filed);

					for (i = 0; environ[i] != NULL; i++)
					{
						printf("\n%s", *(environ + i));
					}
					printf("%s\n", *(environ + (i + 2)));




					dup2(stout, STDOUT_FILENO);

				} else if (strcmp(args[1], ">>") == 0) { //redirect output
					filed = open(args[2], O_CREAT | O_APPEND | O_WRONLY, 0644);

					stout = dup(STDOUT_FILENO);

					dup2(filed, STDOUT_FILENO);
					close(filed);


					for (i = 0; environ[i] != NULL; i++)
					{
						printf("\n%s", *(environ + i));
					}
					printf("%s\n", *(environ + (i + 2)));




					dup2(stout, STDOUT_FILENO);
				}



			} else {



				for (i = 0; environ[i] != NULL; i++)
				{
					printf("\n%s", *(environ + i));
				}
			}

		} else if (strcmp(command, "echo") == 0) {



			int check = 0;
			int g = 0;
			int p = 1;
			int d = 1;
			int ech = 0;
			int pech = 0;


			while (args[g] != NULL) {
				if (strcmp(args[g], ">") == 0) {
					check = 1;
					break;
				}
				g++;
			}

			if (check == 0) { //if check is 0, then there was no request for redirection since we check for ">" above
				while (args[p] != 0) {
					printf("%s ", args[p]);
					p++;
				}
				printf("\n");

			} else if (check == 1) { //if check is 1, that means we encountered a ">" symbol so we should redirect echo's output to the specified file
				while (strcmp(args[d], ">") != 0) {
					echoarr[ech] = args[d];
					ech++;
					d++;
				}
				echoarr[ech] = NULL;
				d++;



				filed = open(args[d], O_CREAT | O_TRUNC | O_WRONLY, 0644);

				stout = dup(STDOUT_FILENO);

				dup2(filed, STDOUT_FILENO);
				close(filed);


				while (echoarr[pech] != NULL) {
					printf("%s ", echoarr[pech]);
					pech++;

				}


				dup2(stout, STDOUT_FILENO);







			}








		} else if (strcmp(command, "help") == 0) {


			fp = fopen("readme", "r");

			if (fp == NULL) {
				printf("Could not open file ");
				return 1;
			}

			if (args[1] != NULL) {

				if (strcmp(args[1], ">") == 0) {

					filed = open(args[2], O_CREAT | O_TRUNC | O_WRONLY, 0644);

					stout = dup(STDOUT_FILENO);

					dup2(filed, STDOUT_FILENO);
					close(filed);

					while (fgets(readme, 1024, fp) != NULL) {
						printf("%s", readme);
					}

					dup2(stout, STDOUT_FILENO);
				}

			} else {
				while (fgets(readme, 1024, fp) != NULL)
					printf("%s", readme);



			}


			fclose(fp);




			//If the command/args[0] is not a builtin, then the program will have skipped the above if-else statments and will have now jumped to this point in the program

			//} else if (args[1] == NULL) { //put this at end of if-statement becasue it should be allowed to hav arguments!! This is "best" case scenario
			//eval(command, args, environ);

		} else { //else we deal with redirections, pipes, or (lastly) one execetubale program plus its arguments being execed

			int isredirect = 0;
			int z = 0;

			while (args[z] != NULL) {
				if (strcmp(args[z], ">") == 0 || strcmp(args[z], "<") == 0 || strcmp(args[z], ">>") == 0 || strcmp(args[z], "|") == 0) {
					isredirect = 1;
					break;

				}
				z++;
			}

			if (isredirect == 1) {


				int h = 0;
				while (1) { //if it hits NULL, then we must be just running a program with/without arguments

					if (strcmp(args[h], ">") == 0) {
						file_redirection(args, 1);
						break;

					} else if (strcmp(args[h], "<") == 0) {
						file_redirection(args, 2);
						break;

					} else if (strcmp(args[h], ">>") == 0) {
						file_redirection(args, 3);
						break;


					} else if (strcmp(args[h], "|") == 0) {
						pipefunc(args);
						break;

					} /*else if (args[h] != NULL) {
					eval(args[0], args, environ); //note: background process option will be taken care of inside the eval() function
					break;
				}*/


					h++;

				}
			} else if (isredirect == 0) {
				eval(args[0], args, environ);
			}

		}




	}








	return 0;
}

