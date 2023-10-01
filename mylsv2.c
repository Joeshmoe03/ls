/*myls.c*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>

/*A buffer containing several of these structs will allow us to track file and directory info for later use*/

struct direntstat {
	char* dname;
	struct stat statbuff;	
};

int main(int argc, char *argv[]) {

	/*Set our flags*/
	
	//int multiplenonopt = 0; //TODO: if your are wondering what this does, it helps format when printing later
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

	/*Our buffer is set with a default size that can be doubled if needed later + checks if failed*/

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

	//if ((argc - optind) > 1) { //TODO:FIX FOR LATER FEATURE
	//	multiplenonopt = 1;
	//}
	
	/*loop interior for both nonopt and nonoptless cases. 
 	 *We iterate through rest of args (or do this loop at least once)*/

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
					
					/*If our old buffer is full, lets make a new resized one*/

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
					direntstatsp[dbuffcount].statbuff = statbuff; //TODO: WIP
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
					direntstatsp[dbuffcount].statbuff = statbuff; //TODO: WIP
				}

				/*Finally, we add the entry name to buff and increment count of buff*/

				direntstatsp[dbuffcount++].dname = path;
			} 

			/*Handle formatted printing*/
			
			for (dbuffindex = 0; dbuffindex < dbuffcount; dbuffindex++) {
				entryname = direntstatsp[dbuffindex].dname;
				
				/*If -a showhidden/showall arg is not passed, we skip over this entry*/
				
				if (showhidden == 0 && entryname[0] == '.') {	
					continue;
				}
				
				/*Handles outcome of either -l or no -l (long format)*/

				if (listlong == 0) {
					printf("%s\n", entryname);
				} else if (listlong == 1) {
					printf("%ld\n", direntstatsp[dbuffindex].statbuff.st_size);
					//TODO: PRINT LONG FORMAT
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

	//TODO: FREE DNAME via iter AS IT IS CHAR POINTER and not stored in buffer directly
	for (dbuffindex = 0; dbuffindex < dbuffcount; dbuffindex++) {
		free(direntstatsp[dbuffindex].dname);
	}

	free(direntstatsp);
}
