/*ls.c*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <pwd.h>
#include <time.h>


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

int print_file_time(const char *pathname) {
    struct stat statbuf;
    struct tm *tm;
	char timestr[BUFSIZ];

	if(stat(pathname, &statbuf) == -1)
		return -1;
	if((tm = localtime(&statbuf.st_mtime)) == NULL)
		return -1;
	if(strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", tm) == 0)
		return -1;
	printf("%s: %s.%09ld\n", pathname, timestr, statbuf.st_mtim.tv_nsec);
    
	return 0;

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

		// 	//Print including hidden //WORKING
        //     while ((dsp = readdir(dirp)) != NULL && errno == 0) { //NEED THE ASSIGMENT TO HAPPEN IN THE FOR LOOP OR ELSE WE GET INFINITE LOOP
		// 		printf("%s \n", dsp->d_name);
        //    }

		//    //Print without hidden files //Hidden files start with "." //WORKING
		     
		// 	while ((dsp = readdir(dirp)) != NULL && errno == 0) {
		// 		if ((dsp->d_name[0]) != '.') {
		// 			printf("%s \n", dsp->d_name);
		// 			}
		// 	}



			/*LONG: Check behavior when you pass file and when you pass current directory and directory with multiple files*/
			//for user and group name
			struct passwd *user; 
			struct passwd *group;
			struct tm *tm;
			char timestr[BUFSIZ];
			//#define st_mtime st_mtim.tv_sec //should i not use this?

			//Print with long formatting (same process for file but without having the outer loop for dsp)
			while ((dsp = readdir(dirp)) != NULL && errno == 0) {
				//get info about files (file by file using stat)
				bool fileExists = (stat(dsp->d_name, &statbuff) != 0); //dsp->d_name is the file name (I WANNA USE VARs.)
				if (fileExists) {

					//get stuff
					//gotta put all the output on one line

					//for permissions use bitmasks combined with st_mode
					//Use ternary operation: condition ? valueIfTrue : valueIfFalse
					//& (NOT &&) for BITWISE AND
					

					//if directory is specified (we want this output to be about the files in the directory)
					//UH OH: might not actually be looping through the files
					//my statbuff has info about directory

					
					/*Permissions*/

					//directory bit
					printf((S_ISDIR(statbuff.st_mode) ? "d" : "-"));

					//user
					printf((S_IRUSR & statbuff.st_mode) ? "r" : "-");
					printf((S_IWUSR & statbuff.st_mode) ? "w" : "-");
					printf((S_IXUSR & statbuff.st_mode) ? "x" : "-");

					//group
					printf((S_IRGRP & statbuff.st_mode) ? "r" : "-");
					printf((S_IWGRP & statbuff.st_mode) ? "w" : "-");
					printf((S_IXGRP & statbuff.st_mode) ? "x" : "-");

					//other
					printf((S_IROTH & statbuff.st_mode) ? "r" : "-");
					printf((S_IWOTH & statbuff.st_mode) ? "w" : "-");
					printf((S_IXOTH & statbuff.st_mode) ? "x" : "-");


					/*user and group using getpwuid and passwd structure*/
					user = getpwuid(statbuff.st_uid);
					group = getpwuid(statbuff.st_gid);
					printf(" %s", user->pw_name);
					printf(" %s", group->pw_name);


					/*File Size*/ //(debug)?
					
					printf(" %ld", statbuff.st_size);

					/*Date & Time*/ //(debug)

					//if(file)
					tm = localtime(&statbuff.st_atime);
					strftime(timestr, sizeof(timestr), "%b %d %R ", tm);
					printf(" %s", timestr);

					//if(directory)
					// tm = localtime(&statbuff.st_mtime);
					// strftime(timestr, sizeof(timestr), "%b %d %R ", tm);
					// printf(" %s", timestr);
					
					//print name (could be file or directory) - do it outside loop?

					//if(file) passed
					//printf("%s",dsp->d_name);

					//if(directory)
					printf("%s", dsp->d_name);

					//update
					//dsp = readdir(dirp);
           		}

				else {
					//why are we getting in here first??? TWICE
					//the two hidden files goofy
					//but why and HOW are we pulling information from the directory???
					//struct stat also available for directories probably, yes but are we not getting into the files of the directory??
					//IF i want file stuff, i probably need to use dsp
					//need to update statbuff addresss?? 

					//check behavior for -a and normal
					printf("what's going on?   ");
				}

				//dsp = readdir(dirp);

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
