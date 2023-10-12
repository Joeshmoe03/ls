/* myls.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>


/* A buffer containing several of these structs will allow us to track file and directory info for later use */
struct direntstat {
	char* dname;
	struct stat statbuff;
};

/* This will increase our direntstatsp buffer size when needed */
void* resizebuff(struct direntstat *buffer, int *size, int *count) {
	if (*count >= *size) {
		*size *= 2;
		buffer = (struct direntstat*)realloc(buffer, *size * sizeof(struct direntstat));
		if (buffer == NULL) {
			free(buffer);
			perror("realloc");
			exit(1);						
		}
	}
	return buffer;
}

int main(int argc, char *argv[]) {

	/* Set our flags */
	int nonopt = 0;
	int multiplenonopt = 0;
	int showhidden = 0;
	int listlong = 0;
	
	/* Setup for parsing and iterating non-options afterwards */
	/* extern int optind; implicitly derived from getopt(), is the index of the next element to be processed in argv */
	int opt; 
	int argindex;	
	
	/* Setup for obtaining info on a file/directory path */
	struct stat statbuff;
	char* path;
	
	/* Setup for opening directories */
	DIR* dirp;
	struct dirent *direntp;

	/* Setup for tracking our buffer containing save entry info */
	int dbuffsize = 100; 
	int dbuffcount = 0;
	int dbuffindex = 0;

	/* Setup for print formatting */
	int pathsize;
	char* entryname;

	struct stat entrystat;
	struct passwd *user;
	struct group *group;
	struct tm *tm;
	char timestr[BUFSIZ];

	/* Our buffer is set with a default size that can be doubled if needed later w/ realloc + checks if failed.
	 * IMPORTANT: direntstatsp is the pointer to a buffer that holds direntstat structs (which will 
	 * contain information about a given entry's name and statbuff/metadata) */
	struct direntstat *direntstatsp = (struct direntstat*)malloc(dbuffsize * sizeof(struct direntstat));
	if (direntstatsp == NULL) {
		perror("malloc");
		exit(1);
	}

	/* Check arguments: use getopt() to parse and set flags depending on option */
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

	/*Multiple non-opt*/ //NEW TODO
	if ((argc - optind) > 1) {
		multiplenonopt = 1;
	}

	/* If some argument(s) unparsed by getopt, there exist non-option arguments */
	if (optind < argc) {
		nonopt = 1;
	}

	/* Loop interior for both nonopt and nonoptless cases. 
	 * We iterate through rest of args (or do this loop at least once on current dir when no nonopt) */
	argindex = optind;
	do {
		
		/* Reset buffer info */
		dbuffsize = 100;
		dbuffindex = 0;		
		dbuffcount = 0;
		
		/* Were any nonopts passed as arguments? If so, use the corresponding arg. If not, current dir */
		if (nonopt == 1) {
			path = argv[argindex];
		} else {
			path = ".";
		}
		if (stat(path, &statbuff) == -1) {
		
			/* Stat did not recognize the path, so it does not exist */
			fprintf(stderr, "%s is not a recognized file or directory!\n", path);
			
			/* Reset errno before next iteration for next nonopt */
			errno = 0;
			argindex++;
			continue;
		}

		/* If our current path is a directory, open, read + save info to buffer, close dir */
		if (S_ISDIR(statbuff.st_mode)) {

			printf("\n"); //TODO

			/* If the directory can't be opened move on to next non-opt/iteration (DO update argindex) */
			if((dirp = opendir(path)) == NULL && errno != 0) {
				errno = 0;
				argindex++;
				continue;
			}

			/* Loop through given directory's content */
			while ((direntp = readdir(dirp)) != NULL && errno == 0) {	

				/* If our old buffer is full, lets make a new resized one and replace the old */
				direntstatsp = resizebuff(direntstatsp, &dbuffsize, &dbuffcount);

				/* Only if we are printing long format do we call stat and add to buff */
				if (listlong == 1) {

					/* Stat must take a relative path, so we must take directory passed as argument (path) and concatenate with d_names.
					* to do this we create char buff with a size of the theoretical path name and use snprintf to compose
					* relative path string formatted in buffer */
					pathsize = sizeof(*path) + sizeof("/") + sizeof(direntp->d_name);
					char entrypath[pathsize];
					snprintf(entrypath, pathsize, "%s/%s", path, direntp->d_name);
					stat(entrypath, &statbuff);

					/* If our stat failed go ahead and continue to next entry WITHIN current while iter (don't update argindex as we are not
 					 * moving to next do-while iter yet) */
					if (errno != 0) {
						errno = 0;
						continue;
					}
					direntstatsp[dbuffcount].statbuff = statbuff;
				}
				
				/* We save the name of the entry to our buffer and increment the count */
				direntstatsp[dbuffcount++].dname = strdup(direntp->d_name);
			}
				
			closedir(dirp);

			if(multiplenonopt == 1) {
				printf("%s:\n", path);
			}
		
		/* Otherwise treat it as a file + save info */
		} else if (S_ISREG(statbuff.st_mode)) {
			
			printf("\n"); //TODO

			/* Only if we are printing long format do we call stat and save to buff */
			if (listlong == 1) {
				stat(path, &statbuff);
				
				/* If our stat failed go ahead and move on to next nonopt (DO update argindex) */
				if (errno != 0) {
					errno = 0;
					argindex++;
					continue;
				}
				direntstatsp[dbuffcount].statbuff = statbuff;
			}

			/* Finally, we add the entry name to buff and increment count of buff */
			direntstatsp[dbuffcount++].dname = strdup(path);
		}

		/* Handle formatted printing by iterating over our buffer. Buffers are nice 
		 * so that I can include sorting should I still maintain the sanity to do so */
		for (dbuffindex = 0; dbuffindex < dbuffcount; dbuffindex++) {
			entryname = direntstatsp[dbuffindex].dname;
			entrystat = direntstatsp[dbuffindex].statbuff;
			
			/* If -a showhidden/showall arg is not passed, we skip over entries starting with "." */
			if (showhidden == 0 && entryname[0] == '.') {	
				continue;
			}
			
			/* Handles outcome of either -l or no -l (long format) */
			if (listlong == 0) {
				printf("%s ", entryname);
			} else if (listlong == 1) {

				/* Directory Bit */
				printf((S_ISDIR(entrystat.st_mode) ? "d" : "-"));

				/* User permissions */					
				printf((S_IRUSR & entrystat.st_mode) ? "r" : "-");
				printf((S_IWUSR & entrystat.st_mode) ? "w" : "-");
				printf((S_IXUSR & entrystat.st_mode) ? "x" : "-");

				/* Group permissions */
				printf((S_IRGRP & entrystat.st_mode) ? "r" : "-");
				printf((S_IWGRP & entrystat.st_mode) ? "w" : "-");
				printf((S_IXGRP & entrystat.st_mode) ? "x" : "-");

				/* Other permission */
				printf((S_IROTH & entrystat.st_mode) ? "r" : "-");
				printf((S_IWOTH & entrystat.st_mode) ? "w" : "-");
				printf((S_IXOTH & entrystat.st_mode) ? "x" : "-");

				/* Number of hard links to the file */
				printf(" %ld", entrystat.st_nlink);
			
				/* User */
				errno = 0;
				if ((user = getpwuid(entrystat.st_uid)) != NULL && errno == 0) {
					printf(" %s", user->pw_name);
				}

				// else {
				// 	printf(stderr)
				// }

				/* Group */
				errno = 0;
				if((group = getgrgid(entrystat.st_gid))!= NULL && errno == 0) {
					printf(" %s", group->gr_name);
				}

				// else {
				// 	//print error ??
				// }
		
				/* File Size */
				printf(" %ld", entrystat.st_size);

				/* Date & Time */
				tm = localtime(&entrystat.st_atime);
				strftime(timestr, sizeof(timestr), "%b %d %R ", tm);
				printf(" %s", timestr);

				/* Entry name and new line */
				printf("%s\n", entryname);
			}
		if (listlong == 0) {printf("\n");}
		}

		/* We must iterate over char pointers in the buffer to free them as they are dynamically allocated */
		/* Free inside of loop to prevent memory leaks due to iteratively overwriting character pointers in our struct */
		for (dbuffindex = 0; dbuffindex < dbuffcount; dbuffindex++) {
			free(direntstatsp[dbuffindex].dname);
		}
		argindex++;
	} while (argindex < argc);
	
	/* We must free our buffer at the end to exit cleanly */
	free(direntstatsp);
}
