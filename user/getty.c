#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "usergroups.h"

char *argv[] = { "sh", 0 };

void get_username(char *username, int size) {
    int i = 0;
    char ch;

    while (i < size - 1) {
        ch = getchar();  // Read one character at a time

        if (ch == '\n' || ch == '\r') {
            break;  // Stop input on Enter
        }

        if (ch == ' ' || ch == '\t') {
            continue;  // Skip whitespace characters
        }

        username[i++] = ch;  // Store the character
    }
    username[i] = '\0';  // Null-terminate the string
}

struct user* login()
{
	char username[STRING_SIZE];
	char password[STRING_SIZE];
	int decounter=0;

	while(decounter<3) {
		printf("Username: ");
		get_username(username, STRING_SIZE);  // Get username without spaces

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
	printf("three attemps to login failed, please try again after...\n");
	wait();
	sleep(50);
	exit();
	return NULL ;
}

int
main(int argc, char *argv[])
{

	clear();
//	printEtcFile("issue");
	struct user* currUser=login();
	if(currUser== NULL)
		exit();
//	printEtcFile("motd");	
	chdir(currUser->homedir);
	setuid(currUser->uid);
	// exec("/bin/sh", argv);

	
	//============interactive menu================
	for (;;)
	{
		int pid;
		clear();
		printf("\n\t\t====================================================\n");

		// Bold green title
		printf("\n\t\t\033[1;32m***** Welcome to our new version of XV6 *****\033[0m\n");

		// Cyan for the designer text
		printf("\n\t\t     \033[0;36mDesigned by M1 students of UY1\033[0m\n");

		// Blue for the divider
		printf("\t\t===================================================\n");

		// Yellow for the prompt to tell user to select an option
		printf("\n\033[1;33m* Please tell us what you want to do *\033[0m\n");
		printf("------------------------------------------------------\n");

		// Bold blue menu items
		printf("\033[1;34m1. List available programs\033[0m\n");
		printf("\033[1;34m2. Execute a shell\033[0m\n");
		printf("\033[1;34m3. Log out\033[0m\n");

		printf("\n====================================================\n");

		// Highlight the selection prompt with yellow
		printf("\033[1;33mSelect an option: --> \033[0m");



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
			// Listen for Enter key to exit
			printf("\n\n\nPress Enter to exit...\n");
			while (1) {
				char ch = getchar();  // Wait for user input
				if (ch == '\n' || ch == '\r') {
					break;  // Exit loop if Enter key is pressed
				}
			}
		//	printf("Exiting...\n");
		//	exit();  // Exit after pressing Enter 
		} 
		else if (input[0] == '2') {
			// Option 2: Start the shell
			printf("\nAt any time, enter \033[1;33mexit\033[0m to go back\nStarting the shell...\n");
			printf("\n\n\n\n\n\n\n\n\n\n");
			sleep(50);
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
			printf("Logging out...\n");
		//	printf("Enter Ctrl+A, release the key and press X to exit the emulator\n");
			sleep(150);
			exit();
		} 
		else {
			// Invalid input
			printf("Invalid option. Please enter 1, 2, or 3.\n");
		}
		
	}
}
