/*ls.c*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
	int opt;
	int aflag = 0, lflag = 0;
	int index;	
	struct stat statbuff;
	
	/*check arguments: use getopt() to parse and set flags depending on option*/ 	
	
	while ((opt = getopt(argc, argv, "al")) != -1) {
		switch(opt) {
		case 'a':
			aflag = 1;
			break;
		case 'l':
			lflag = 1;
			break;
		default:
			fprintf(stderr, "Error, bad option arg: %s\n", optarg);
			exit(1);
		}
	}
	
	/*Check if non option arguments passed: TODO*/

	/*We need to handle non-option arguments passed to ls*/
	
	for (index = optind; index < argc; index++) {
	
		/*Check if non-option argument is file or directory*/
	
		if (isfile(argv[index])) {
			
			/*Print file name  (in long format if -l passed in)*/

			if (lflag == 1) {

				/*TODO: print out long format using stat*/

			} else {
				printf("%s", argv[index]);
			}
		} else if (isdirectory(argv[index])) {

			/*Open directory?*/
			/*Read contents*/
			/*format depending on -a and -l*/
				/*print out contents - subfolders and files with time*/
			/*Close directory*/
			/*exit gracefully*/
				/*TODO*/
		}
	}
	
	/*if no non option arguments, proceed with ls of current directory: TODO - turn to else statement*/

	/* Depending on what flags we passed, we expect certain unique behavior*/
	
	if (aflag) {
	
		/*list all*/
	
	} 
	if (lflag) { 
	
		/*list long*/
	
	}		
	return 0;
}

int isdirectory(const char* path) {
	struct stat statbuff;

	if (stat(path, &statbuff) != 0) {
		fprintf(stderr, "Error, %s is not a valid file or directory", path);
		exit(2);
	}
	return S_ISDIR(statbuff.st_mode);
}

int isfile(const char* path) {
	struct stat statbuff;

	if (stat(path, &statbuff) != 0) {
		fprintf(stderr, "Error, %s is not a valid file or directory", path);
		exit(3);
	}
	return S_ISREG(statbuff.st_mode);
}
