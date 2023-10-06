# Myls

Our implementation of the ls command making use of buffers. The current configuration is set up such that sorting for each non-option directory can be included as a feature later down the line. 

## Known Issues

NO MAJOR ISSUES IDENTIFIED: as far as the maintainers are aware, no issues were identified apart from some minor print formatting (of an extra blank line at times when printing a dir with only hidden files). No issues could be identified with various non-options passed using Valgrind. All option cases seem to function. 

Possible Minor Issues: In instances where we use -l flag, valgrind reports that some memory is "still reachable". Otherwise, we could not detect memory leaks.

## Consulted Works

    Link: https://stackoverflow.com/questions/58357052/allocate-memory-for-buffer-of-type-struct-in-c
    Description: Referenced this source as an example of how to properly allocate a buffer for structs containing pointers, and how to ensure freeing is occuring properly to avoid memory leaks.

    Link: https://stackoverflow.com/questions/47625515/c-programming-how-often-should-realloc-be-used
    Description: An explanantion of how frequently one should use realloc. Specifically, we payed attention to advice about doubling size on every reallocation to avoid inefficiencies of reallocating every iteration (doubling our dbuff size).

    Link: http://web.eecs.utk.edu/~jplank/plank/classes/cs360/360/notes/Stat/lecture.html
    Description: Used to get a better idea of how readdir/opendir/closedir work in tandem by seeing example code.
    
    SEE MANPAGES FOR strdup, malloc/realloc/free, readdir/opendir/closedir, stat(2), inode(7), getopt, strftime, getpwnam, localtime, snprintf

## Maintainers
    - Josef Liem            jliem@middlebury.edu
    - Hedavam Solano        hsolano@middlebury.edu
