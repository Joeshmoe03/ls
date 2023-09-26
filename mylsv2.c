/*ls.c*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
//#include <errno.h>

#define NONOPT 1
#define SHOWHIDDEN 2
#define LISTLONG 4

int main(int argc, char *argv[]) {
	int opt;
	int flags = 0;
	int index;
	struct stat statbuff;
	char* path;
	
	/*check arguments: use getopt() to parse and set flags depending on option*/

	while ((opt = getopt(argc, argv, "al")) != -1) {
		switch(opt) {
			case 'a':
				flags |= SHOWHIDDEN;
				break;
			case 'l':
				flags |= LISTLONG;
				break;
			default:
				exit(1);
		}
	}

	/*if some argument(s) unparsed by getopt, there exist non-option arguments*/

	if (optind < argc) {
		flags |= NONOPT;
	}
	
	/*loop interior for both nonopt and nonoptless cases*/

	index = optind;
	do {
		if (flags && NONOPT) {
			path = argv[index];
		} else {
			path = ".";
		}
		
		if (stat(path, &statbuff) != -1) {
			if (S_ISDIR(statbuff.st_mode)) {
				//TODO: DO DIRECTORY THINGS: OPEN DIR, READ DIR, LIST OUT CONTENTS, CLOSE
				DIR *dirp;
				dirp = opendir(path);

			} else if (S_ISREG(statbuff.st_mode)) {
				//TODO: do file things
			}
		} else {
			fprintf(stderr, "Error, %s is not a valid file or directory", path);
		}

		switch(flags) {
			case flags && SHOWHIDDEN:
				break;
			case flags && LISTLONG:
				break;
		}
		
		index++;
	} while (index < argc);







	
	/*Check if non option arguments passed: TODO*/

	/*We need to handle non-option arguments passed to ls*/
	
	//Start loop after all option arguments are read

		/*Check if non-option argument is file or directory*/
	
	//	if (isfile(argv[index]) == 1) {
			
			/*Print file name  (in long format if -l passed in)*/

	//		if (lflag == 1) {

				/*TODO: print out long format using stat*/

	//		} else {
	//			printf("%s\n", argv[index]);
	//		}
	//	} else if (isdirectory(argv[index]) == 1) {

			/*Open directory?*/
			/*Read contents*/

	//		dirp = opendir("");
    //        if (dirp == NULL)
    //            	return (ERROR);
    //        len = strlen(name);
    //        while ((dp = readdir(dirp)) != NULL) {
    //               if (dp->d_namlen == len && strcmp(dp->d_name, name) == 0) {
    //                       (void)closedir(dirp);
    //                       return (FOUND);
    //               }
     //      }
    //       (void)closedir(dirp);
    //       return (NOT_FOUND);

			
			/*format depending on -a and -l*/
				/*print out contents - subfolders and files with time*/
			/*Close directory*/
			/*exit gracefully*/
				/*TODO*/
	//	}
	//}
	
	/*if no non option arguments, proceed with ls of current directory: TODO - turn to else statement*/

	/* Depending on what flags we passed, we expect certain unique behavior*/
	
	//if (aflag) {
	
		/*list all*/
	
	//} 
	//if (lflag) { 
	
		/*list long*/
	//printf("%d\n", index);
	//}		
	return 0;
}

