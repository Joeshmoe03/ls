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
#include <libgen.h>

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

/* Stat must take a relative path, so we must take directory passed as argument (path) and concatenate with d_names.
 * to do this we create char buff with a size of the theoretical path name and use snprintf to compose
 * relative path string formatted in buffer */
char* relpath(char *path, char *d_name) { 
	int pathsize = strlen(path) + strlen("/") + strlen(d_name) + 1;
	char* entrypath;
	entrypath = (char*)malloc(pathsize);
	snprintf(entrypath, pathsize, "%s/%s", path, d_name);
	
	if (entrypath == NULL) {
			free(entrypath);
			perror("malloc");
			exit(1);
	}
	return entrypath;
}

/* This function handles all formatted printing for myls. It needs info on flags, buffer size/count, and buffer pointer.
 * It iterates through all given entries stored in the buffer pointed to and depending on the flags optionally formats
 * for long or shows hidden files as well. */
void printls(int dbuffindex, int dbuffcount, int showhidden, int listlong, struct direntstat *direntstatsp) {
	struct tm *tm;
	struct passwd *user;
	struct group *group;
	char timestr[BUFSIZ];
	char* entryname;
	struct stat entrystat;


	/* Handle formatted printing by iterating over our buffer. Buffers are nice 
	 * so that I can include sorting should I still maintain the sanity to do so */
	for (dbuffindex = 0; dbuffindex < dbuffcount; dbuffindex++) {
		entryname = direntstatsp[dbuffindex].dname;
		entrystat = direntstatsp[dbuffindex].statbuff;
	
		/* If -a showhidden/showall arg is not passed, we skip over entries (relative path basenames) starting with "." */
		if (showhidden == 0 && basename(entryname)[0] == '.') {
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
			
			/* User: if fails prints uid */
			if ((user = getpwuid(entrystat.st_uid)) != NULL) {
				printf(" %s", user->pw_name);
			}else {
				printf(" %d", entrystat.st_uid);
				errno = 0;
			}

			/* Group: if fails prints gid */
			if((group = getgrgid(entrystat.st_gid))!= NULL) {
				printf(" %s", group->gr_name);
			} else {
				printf(" %d", entrystat.st_gid);
				errno = 0;
			}
		
			/* File Size */
			printf(" %ld", entrystat.st_size);

			/* Date & Time */
			tm = localtime(&entrystat.st_mtime);
			strftime(timestr, sizeof(timestr), "%b %d %R ", tm);
			printf(" %s", timestr);

			/* Entry name and new line */
			printf("%s\n", entryname);
		}
	if (dbuffcount-1 == dbuffindex && listlong == 0) {printf("\n");}
	}
	return;
}

/* This function parses our arguments and sets flags correspondingly */
void setoptflags(int *nonoptp, int *multiplenonoptp, int *showhiddenp, int *listlongp, int argc, char *argv[]) {
	int opt;
	
	/* Check arguments: use getopt() to parse and set flags depending on option */
	while ((opt = getopt(argc, argv, "al")) != -1) {
		switch(opt) {
			case 'a':
				*showhiddenp = 1;
				break;
			case 'l':
				*listlongp = 1;
				break;
			default:
				exit(1);
		}
	}

	/* Multiple non-opt */
	if ((argc - optind) > 1) {
		*multiplenonoptp = 1;
	}

	/* If some argument(s) unparsed by getopt, there exist non-option arguments */
	if (optind < argc) {
		*nonoptp = 1;
	}
	return;
}

/* This function does minor new line formatting */
void formatnewline(int argindex, int optind) {

	/* Print Formatting */
	if (argindex != optind) {
		printf("\n"); 
	}
	return;
}

/* Set path to non-opt if we are interested in a non-current-directory path */
char* setpath(int nonopt, char *path, int argindex, char *argv[]) {
	if (nonopt == 1) {
		path = argv[argindex];
	}
	return path;
}

/* Attempts a malloc for our direntstatsp buffer and handles failure */
struct direntstat* trymalloc(int dbuffsize) {

	/* Our buffer is set with a default size that can be doubled if needed later w/ realloc + checks if failed.
	 * IMPORTANT: direntstatsp is the pointer to a buffer that holds direntstat structs (which will 
	 * contain information about a given entry's name and statbuff/metadata) */
	struct direntstat *direntstatsp = (struct direntstat*)malloc(dbuffsize * sizeof(struct direntstat));
	if (direntstatsp == NULL) {
		perror("malloc");
		exit(1);
	}
	return direntstatsp;
}	

int main(int argc, char *argv[]) {

	/* Set our flags */
	int nonopt = 0;
	int multiplenonopt = 0;
	int showhidden = 0;
	int listlong = 0;
	int argindex;	
	
	/* Setup for obtaining info on a file/directory path */
	struct stat statbuff;
	char* path = ".";
	
	/* Setup for opening directories */
	DIR* dirp;
	struct dirent *direntp;

	/* Setup for tracking our buffer containing save entry info */
	int dbuffsize = 100; 
	int dbuffcount = 0;
	int dbuffindex = 0;
	struct direntstat* direntstatsp = trymalloc(dbuffsize);	
	
	/* Set all option flags by parsing through */
	setoptflags(&nonopt, &multiplenonopt, &showhidden, &listlong, argc, argv);

	/* Loop interior for both nonopt and nonoptless cases. 
	 * We iterate through rest of args (or do this loop at least once on current dir when no nonopt) */
	argindex = optind;
	do {
		/* Reset buffer info */
		dbuffsize = 100;
		dbuffindex = 0;		
		dbuffcount = 0;
		
		/* Were any nonopts passed as arguments? If so, use the corresponding arg. If not, current dir */
		path = setpath(nonopt, path, argindex, argv);

		if (stat(path, &statbuff) == -1) {
			formatnewline(argindex, optind);
	
			/* Stat did not recognize the path or file, so it does not exist, move to next one */
			fprintf(stderr, "%s, %s was not recognized...\n", strerror(errno), path);
			errno = 0;
			argindex++;
			continue;
		}

		/* If our current path is a directory, open, read + save info to buffer, close dir */
		if (S_ISDIR(statbuff.st_mode)) {
			formatnewline(argindex, optind);

			/* If the directory can't be opened move on to next non-opt/iteration (DO update argindex) */
			if((dirp = opendir(path)) == NULL && errno != 0) {
				perror("opendir");
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

					/* Our function for relative path mallocs space for a character pointer holding the relative path, so we must free it.
					   We call relpath() to retrieve the relative path as a combination of given path and d_name */					
					char *relpathp;
					relpathp = relpath(path, direntp->d_name);
					stat(relpathp, &statbuff);
					free(relpathp);
					
					/* If our stat failed go ahead and continue to next entry WITHIN current while iter */
					if (errno != 0) {
						if (showhidden == 1) {perror("stat");}
						errno = 0;
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
			
		/* If we are printing long, since we already called stat on this file and handled errors, no need to call again */
			if (listlong == 1) {
				direntstatsp[dbuffcount].statbuff = statbuff;
			}
			
			/* Finally, we add the entry name to buff and increment count of buff */
			direntstatsp[dbuffcount++].dname = strdup(path);
		}
		printls(dbuffindex, dbuffcount, showhidden, listlong, direntstatsp);

		/* We must iterate over char pointers in the buffer to free them as they are dynamically allocated */
		for (dbuffindex = 0; dbuffindex < dbuffcount; dbuffindex++) {
			free(direntstatsp[dbuffindex].dname);
		}
		argindex++;
	} while (argindex < argc);
	
	/* We must free our buffer at the end to exit cleanly */
	free(direntstatsp);
}
