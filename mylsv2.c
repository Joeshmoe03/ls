/*myls.c*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <pwd.h>
#include <time.h>
#include <stdbool.h>

/*A buffer containing several of these structs will allow us to track file and directory info for later use*/

struct direntstat {
	char* dname;
	struct stat statbuff;	
};

int main(int argc, char *argv[]) {

	/*Set our flags*/
	
	//int multiplenonopt = 0; //TODO: SEE TODO NOTE BELOW
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

	DIR* dirp;
	struct dirent *direntp;

	/*Setup for tracking our buffer containing save entry info*/
		
	int dbuffsize = 100;
	int dbuffcount = 0;
	int dbuffindex = 0;

	/*for printing out directory entries*/

	char* entryname;
	struct passwd *user;
	struct passwd *group;
	struct tm *tm;
	char timestr[BUFSIZ];

	/*Our buffer is set with a default size that can be doubled if needed later w/ realloc + checks if failed.
	 *IMPORTANT: direntstatsp is the pointer to a buffer that holds direntstat structs (which will 
	 *contain information about a given entry's name and statbuff/metadata)*/

	struct direntstat *newdirentstatsp;
	struct direntstat *direntstatsp = (struct direntstat*)malloc(dbuffsize * sizeof(struct direntstat));
	if (direntstatsp == NULL) {
		perror("malloc");
		exit(1);
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

	//if ((argc - optind) > 1) { //TODO:FIX FOR LATER FEATURE. IS THERE BETTER WAY? IS NEEDED TO DETERMINE IF HEADER OVER ENTRIES IS RELEVANT ON MULTIPLE DIRS
	//	multiplenonopt = 1;
	//}
	
	/*loop interior for both nonopt and nonoptless cases. 
	 *We iterate through rest of args (or do this loop at least once on current dir when no nonopt)*/

	argindex = optind;
	do {

		/*were any nonopts passed as arguments? If so, use the corresponding arg. If not, current dir*/

		if (nonopt == 1) {
			path = argv[argindex];
		} else {
			path = ".";
		}

		if (stat(path, &statbuff) != -1) {
			dbuffsize = 100;
			dbuffindex = 0;
			dbuffcount = 0;

			/*If our current path is a directory, open, read + save info to buffer, close dir*/

			if (S_ISDIR(statbuff.st_mode)) {
				dirp = opendir(path);
				while ((direntp = readdir(dirp)) != NULL && errno == 0) {
					
					/*If our old buffer is full, lets make a new resized one and replace our old one*/

					if (dbuffcount >= dbuffsize) {
						dbuffsize *= 2;
						newdirentstatsp = (struct direntstat*)realloc(direntstatsp, dbuffsize);
						
						/*Lets double check success on realloc*/

						if (newdirentstatsp == NULL) {
							free(direntstatsp);
							perror("realloc");
							exit(1);
						}

						direntstatsp = newdirentstatsp;
					}
				
				/*Only if we are printing long format do we call stat and add to buff*/

				if (listlong == 1) {
					stat(direntp->d_name, &statbuff);
					direntstatsp[dbuffcount].statbuff = statbuff;
				}

					/*We save the name of the entry to our buffer and increment the count*/

					direntstatsp[dbuffcount++].dname = strdup(direntp->d_name);
				
				}
				
				closedir(dirp);

			/*Otherwise treat it as a file + save info*/

			} else if (S_ISREG(statbuff.st_mode)) {
				
				/*Only if we are printing long format do we call stat and save to buff*/

				if (listlong == 1) {
					stat(path, &statbuff);
					direntstatsp[dbuffcount].statbuff = statbuff;
				}

				/*Finally, we add the entry name to buff and increment count of buff*/

				direntstatsp[dbuffcount++].dname = path;
			} 

			/*Handle formatted printing by iterating over our buffer. Buffers are nice
			 *so that I can include sorting should I still maintain the sanity to do so*/
			
			for (dbuffindex = 0; dbuffindex < dbuffcount; dbuffindex++) {
				entryname = direntstatsp[dbuffindex].dname;
				
				/*If -a showhidden/showall arg is not passed, we skip over entries starting with "."*/
				
				if (showhidden == 0 && entryname[0] == '.') {	
					continue;
				}
				
				/*Handles outcome of either -l or no -l (long format)*/

				if (listlong == 0) {
					printf("%s\n", entryname);
				} else if (listlong == 1) {
					printf((S_ISDIR(direntstatsp[dbuffindex].statbuff.st_mode) ? "d" : "-"));

					/*User permissions*/					

					printf((S_IRUSR & direntstatsp[dbuffindex].statbuff.st_mode) ? "r" : "-");
					printf((S_IWUSR & direntstatsp[dbuffindex].statbuff.st_mode) ? "w" : "-");
					printf((S_IXUSR & direntstatsp[dbuffindex].statbuff.st_mode) ? "x" : "-");

					/*Group permissions*/

					printf((S_IRGRP & direntstatsp[dbuffindex].statbuff.st_mode) ? "r" : "-");
					printf((S_IWGRP & direntstatsp[dbuffindex].statbuff.st_mode) ? "w" : "-");
					printf((S_IXGRP & direntstatsp[dbuffindex].statbuff.st_mode) ? "x" : "-");

					/*Other permission*/

					printf((S_IROTH & direntstatsp[dbuffindex].statbuff.st_mode) ? "r" : "-");
					printf((S_IWOTH & direntstatsp[dbuffindex].statbuff.st_mode) ? "w" : "-");
					printf((S_IXOTH & direntstatsp[dbuffindex].statbuff.st_mode) ? "x" : "-");
				
					/*User and group using getpwuid and passwd structure*/

					user = getpwuid(direntstatsp[dbuffindex].statbuff.st_uid);
					group = getpwuid(direntstatsp[dbuffindex].statbuff.st_gid);
					printf(" %s", user->pw_name);
					printf(" %s", group->pw_name);
					
					
					/*File Size*/

                    printf(" %ld", direntstatsp[dbuffindex].statbuff.st_size);

                    /*Date & Time*/

                    tm = localtime(&direntstatsp[dbuffindex].statbuff.st_atime);
                    strftime(timestr, sizeof(timestr), "%b %d %R ", tm);
                    printf(" %s", timestr);
					
					/*Entry name and new line*/

					printf("%s", entryname);
					printf("\n");

					//TODO: PRINT LONG FORMAT. I THINK IS DONE??
				}
			}
		} else {

			/*Stat did not recognize the path, so it does not exist*/

			fprintf(stderr, "%s is not a recognized file or directory...\n", path);
		}

		/*We set errno = 0 so that we can call stat and print ls without a problem if
		  the previous path was garbage*/

		errno = 0;	
		argindex++;
	} while (argindex < argc);

	/*We must iterate over char pointers in the buffer to free them as they are dynamically allocated*/

	for (dbuffindex = 0; dbuffindex < dbuffcount-1; dbuffindex++) {
		free(direntstatsp[dbuffindex].dname);
	}

	/*We must free our buffer at the end to exit cleanly*/

	free(direntstatsp);
}
