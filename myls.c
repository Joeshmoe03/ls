/*ls.c*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>


//TODO: Get rid of functions!!!
//EXIT IS NOT ACCEPTABLE FOR NON-OPTION ARGUMENTS BEING WRONG
//stat is a system call so we don't want to repeat it

int isfile(const char* path) {
	struct stat statbuff;

	if (stat(path, &statbuff) != 0) {
		fprintf(stderr, "Error, %s is not a valid file or directory", path);
		exit(3);
	}
	return S_ISREG(statbuff.st_mode);
}

int isdirectory(const char* path) {
	struct stat statbuff;

	if (stat(path, &statbuff) != 0) {
		fprintf(stderr, "Error, %s is not a valid file or directory", path);
		exit(2);
	}
	return S_ISDIR(statbuff.st_mode);
}

int main(int argc, char *argv[]) {
	int opt;
	int aflag = 0, lflag = 0;
	int index;	
	struct stat statbuff;
	//opterr = 0; disables automated error message


	//TODO: Combine the checking of options and non options into one loop !!! by looping through all args and categorizing
	//TODO: Change flag names
	
	/*check arguments: use getopt() to parse and set flags depending on option*/ 	
	//EXIT IS ACCEPTABLE FOR OPTION ARGUMENTS BEING WRONG
	
	while ((opt = getopt(argc, argv, "al")) != -1) {
		switch(opt) {
		case 'a':
			aflag = 1;
			break;
		case 'l':
			lflag = 1;
			break;
		default:
			//getopt handles error by default, no need to code error message unless we want to explicitly handle it
			fprintf(stderr, "%s", "");
			exit(1);
		}
	}
	
	/*Check if non option arguments passed: TODO*/

	/*We need to handle non-option arguments passed to ls*/
	
	for (index = optind; index < argc; index++) {

		char* name = argv[index];
	
		/*Check if non-option argument is file or directory*/
	
		if (isfile(name) == 1) {
			
			/*Print file name  (in long format if -l passed in)*/

			if (lflag == 1) {

				/*TODO: print out long format using stat*/

			} else {
				printf("%s\n", argv[index]);
			}
		} else if (isdirectory(name) == 1) {

			DIR *dirp; //openddir returns this type, which is passed on to readdir
			struct dirent *dsp; //readdir returns this type

			/*Open directory*/
			dirp = opendir(name); // . for current directory
            if (dirp == NULL) {
				fprintf(stderr, "%s: No such file or directory:", name);
				//exit?
			}

			/*Read contents*/

			errno = 0;
            while ((dsp = readdir(dirp)) != NULL && errno == 0) {
				printf("%s \n", dsp->d_name);
           }

		   if (errno != 0){
			//error reading directory
		   }

		   else {
			//read correctly
			//close it
		   }
		   

		   //then format depending on -a and -l

			
			/*format depending on -a and -l*/
				/*print out contents - subfolders and files with time*/
			/*Close directory*/
			/*exit gracefully*/
				/*TODO*/
		}
	}
	
	/*if no non option arguments, proceed with ls of current directory: TODO - turn to else statement*/

	/* Depending on what flags we passed, we expect certain unique behavior*/
	
	// if (aflag) {
	
	// 	/*list all*/
	
	// } 
	// if (lflag) { 
	
	// 	/*list long*/
	
	// }		
	// return 0;
}

