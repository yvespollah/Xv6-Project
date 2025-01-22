#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "usergroups.h"

char *argv[] = { "sh", 0 };

struct user* login()
{
	char username[STRING_SIZE];
	char password[STRING_SIZE];
	int decounter=0;

	while(decounter<3) {
		printf("Username: ");
		gets(username, STRING_SIZE);
		username[strlen(username)-1]='\0';			// remove new line from username

		printf("Password: ");
		echoOnOff();								// turn off echo
		gets(password, STRING_SIZE);
		echoOnOff();								// turn on echo
		printf("\n");
		password[strlen(password)-1]='\0';

		struct user* currUser=authenticateUser(username, password);
		if(currUser)
			return currUser;
		else{
			printf("Login incorrect!\n");
			decounter = decounter +1;
		}
	}
	printf("three attemps to login failed, please try aigain after...\n");
	wait();
	sleep(50);
	exit();
	return NULL ;
}

int
main(int argc, char *argv[])
{

	clear();
	printEtcFile("issue");
	struct user* currUser=login();
	if(currUser== NULL)
		exit();
	printEtcFile("motd");	
	chdir(currUser->homedir);
	setuid(currUser->uid);
	// exec("/bin/sh", argv);

	
	//============interactive menu================
	for (;;)
	{
		int pid;
		clear();
		printf("\n\t\t====================================================\n");
		printf("\n\t\t***** Welcome to our new version of XV6 *****\n\n\n");
		printf("\t\t\t Designed by M1 students of UY1\n");
		printf("\t\t===================================================\n");
		printf("\n* Please tell us what you want to do *\n");
		printf("1. List available programs\n");
		printf("2. Execute a shell\n");
		printf("3. Quit XV6\n");
		printf("\n====================================================\n");
		printf("Select an option: --> ");

		char input[10] = {0};  // Ensure the buffer is initialized
		gets(input, sizeof(input));  // Get user input safely

	
		if (input[0] == '1') {
			char *argv[] = { "ls", "/bin", 0 };
			// Option 1: List available commands (using `ls`)
			printf("Listing available commands...\n");
			pid = fork();
			if (pid < 0) {
				printf("init: fork failed\n");
				exit();
			}
			if (pid == 0) {
				exec("/bin/ls", argv);
				printf("init: exec ls failed\n");
			}
			wait();		// Wait for `ls` to finish	
			sleep(80);  
		} 
		else if (input[0] == '2') {
			// Option 2: Start the shell
			printf("\nEnter exit to go back\nStarting the shell...\n");
			printf("\n\n\n\n\n\n\n\n\n\n");
						pid = fork();
			if (pid < 0) {
				printf("init: fork failed\n");
				exit();
			}
			if (pid == 0) {
				exec("/bin/sh", argv);
				printf("init: exec sh failed\n");
				exit();
			}
			wait();  // Wait for the shell to exit
		} 
		else if (input[0] == '3') {
			// Option 3: Quit xv6
			printf("Shutting down XV6...\n");
			printf("Enter Ctrl+A, release the key and press X to exit the emulator\n");
			sleep(3);
			exit();
		} 
		else {
			// Invalid input
			printf("Invalid option. Please enter 1, 2, or 3.\n");
		}
		
	}
}
