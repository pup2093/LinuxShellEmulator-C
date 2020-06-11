#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>


int main()
{

	//char command[100], commandtwo[100], *param[20];
	char *line; //just a string 
	char * args[20]; //array of pointers which will be array of commandline arguments
	char *environvar[] = {(char *) "PATH=/BIN", 0};

	while (1){
		print_prompt();
		line = read_line();



	}




	return 0;
}