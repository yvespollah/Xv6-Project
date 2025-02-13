#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "usergroups.h"

/*
	Sintax: groupadd [-g gid] groupname

	arguments[0] -> value for gid
	arguments[1] -> value for groupname
*/	

int getPositionInArguments(char* command) {
	if(!strcmp(command, "-g")) return 0;
	return -1;
}

int parseCommandLineArguments(char arguments[4][STRING_SIZE],int argc,char* argv[])
{
	if(argc<2)													// if no username is suplied, return error
			return 0;

	strcpy(arguments[1], argv[argc-1]);							// username is mandatory last argument

	int i=1;
	while(i<argc-1) {
		int position=getPositionInArguments(argv[i]);			// get the command
		i++;
 
		if(position==-1)										// if command is not supported, return error
			return 0;

		strcpy(arguments[position],argv[i]);					// get the command value
		i++;
	}

	return 1;
}

int
main(int argc, char *argv[])
{
	char arguments[4][STRING_SIZE];

	int valid=parseCommandLineArguments(arguments, argc, argv);
	if(!valid) {
		printf("Error while creating group!\n");
		exit();
	}
		
	struct group* newGroup=createGroup(arguments[1], arguments[0], DONT_ADD_USER);
	if(newGroup==NULL)
		printf("Error while creating group!\n");
	else
		addNewGroup(newGroup);

	exit();	
}
