/*ls.c*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
//#include <errno.h>

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
			//getopt handles error by default, no need to code error message unless we want to explicitly handle it
			fprintf(stderr, "%s", "Error!");
			exit(1);
		}
	}
	
	/*Check if non option arguments passed: TODO*/

	/*We need to handle non-option arguments passed to ls*/
	
	//Start loop after all option arguments are read
	for (index = optind; index < argc; index++) {
	
		/*Check if non-option argument is file or directory*/
	
		if (isfile(argv[index]) == 1) {
			
			/*Print file name  (in long format if -l passed in)*/

			if (lflag == 1) {

				/*TODO: print out long format using stat*/

			} else {
				printf("%s\n", argv[index]);
			}
		} else if (isdirectory(argv[index]) == 1) {

			/*Open directory?*/
			/*Read contents*/

			dirp = opendir("");
            if (dirp == NULL)
                	return (ERROR);
            len = strlen(name);
            while ((dp = readdir(dirp)) != NULL) {
                   if (dp->d_namlen == len && strcmp(dp->d_name, name) == 0) {
                           (void)closedir(dirp);
                           return (FOUND);
                   }
           }
           (void)closedir(dirp);
           return (NOT_FOUND);

			
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

