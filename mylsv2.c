/*ls.c*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>

int main(int argc, char *argv[]) {

	/*Set our flags*/
	
	int multiplenonopt = 0; //NOTE: if your are wondering what this does, it helps format when printing later
	int nonopt = 0;
	int showhidden = 0;
	int listlong = 0;
	
	/*Setup for parsing and iterating non-options afterwards*/

	int opt;
	int argindex;
	
	/*Setup for obtaining info on a file/directory path*/

	struct stat statbuff;
	char* path;
	
	/*Setup for opening directories*/

	DIR *dirp;
	struct dirent *direntp;

	/*Setup for reading and saving directories into a buffer*/
	
	int dnamessize = 100;
	int dnamesindex = 0;
	int dnamescount = 0;

	/*Our buffer is set with a default size that can be doubled if needed later*/

	char** dnames = (char**)malloc(dnamessize * sizeof(char*));
		if (dnames == NULL) {
			perror("malloc");
			exit(2);
		}

	/*check arguments: use getopt() to parse and set flags depending on option*/

	while ((opt = getopt(argc, argv, "al")) != -1) {
		switch(opt) {
			case 'a':
				showhidden = 1;
				break;
			case 'l':
				listlong = 1;
				break;
			default:
				exit(1);
		}
	}

	/*if some argument(s) unparsed by getopt, there exist non-option arguments*/

	if (optind < argc) {
		nonopt = 1;
	}

	if ((argc - optind) > 1) {
		multiplenonopt = 1;
	}
	
	/*loop interior for both nonopt and nonoptless cases*/

	argindex = optind;
	do {
		if (nonopt) {
			path = argv[argindex];
		} else {
			path = ".";
		}

		if (stat(path, &statbuff) != -1) {
			dnamesindex = 0;
			dnamescount = 0;

			/*If our current path is a directory, open, read + save info to buffer, close dir*/

			if (S_ISDIR(statbuff.st_mode)) {
				dirp = opendir(path);
				while ((direntp = readdir(dirp)) != NULL && errno == 0) {
					
					/*If our old buffer is full, lets make a new resized one*/

					if (dnamesindex >= dnamessize) {
						dnamessize *= 2;
						char** newdnames = (char**)realloc(dnames, dnamessize);
						dnames = newdnames;
					}
					dnames[dnamescount++] = strdup(direntp->d_name);
				}
				
				closedir(dirp);

			/*Otherwise treat it as a file + save info*/

			} else if (S_ISREG(statbuff.st_mode)) {
				dnames[dnamescount++] = path;
			}

			/*Handle formatted printing*/
			
			for (dnamesindex = 0; dnamesindex < dnamescount; dnamesindex++) {
				printf("%s\n", dnames[dnamesindex]);
			}
		}
		
		argindex++;
	} while (argindex < argc);

	free(dnames);







	
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

