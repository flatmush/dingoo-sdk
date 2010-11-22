/*
** Use regular opendir/readdir to list directory
** NOTE also includes some BSD extensions (d_type and DT_DIR)
** for accessing file type information too.
** Outputs to "stdout.txt" in current directory,
** use ebook reader to check output.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include <sys/types.h>

#include <dirent.h>
#include <sys/stat.h>


#define VERSION_MAJOR    1
#define VERSION_MINOR    0
#define VERSION_REVISION 0

char     gamePath[256];

char* gamePathInit(const char* inPath) {
	uintptr_t i, j;
	for(i = 0, j = 0; inPath[i] != '\0'; i++) {
		if((inPath[i] == '\\') || (inPath[i] == '/'))
			j = i + 1;
	}
	strncpy(gamePath, inPath, j);
	gamePath[j] = '\0';
    /*
    ** Static (hard coded) path examples
    strcpy(gamePath, "a:\\"); // WORKS
    strcpy(gamePath, "a:/"); // unix style path FAILS
    strcpy(gamePath, "a:\\"); // DEBUG NOTE multiple slashes which results in look up failure 
    */
	return gamePath;
}


void dodemo()
{
    char search_dir[256];
    FILE *myout=NULL;
    DIR *dirp=NULL;
    struct dirent *dp=NULL;

#define CREATE_OUT_FILE
#ifdef CREATE_OUT_FILE
    myout = fopen("stdout.txt", "w");
    if (myout == NULL)
    {
        exit(10);
    }
#else 
    myout=stdout;
#endif /* CREATE_OUT_FILE */
    
    
    strcpy(search_dir, ".");
    strcpy(search_dir, "B:\\");
    strcpy(search_dir, "A:\\");
    fprintf(myout, "search_dir : >%s<\n", search_dir);
    fprintf(myout, "================\n");
    
    dirp = opendir(search_dir);
    while ((dp = readdir(dirp)) != NULL)
    {
        fprintf(myout, "name: >%s< ", dp->d_name);
        #ifdef  DT_DIR
        /*
                #define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
        */
        
        fprintf(myout, "DT_REG 0x%02x ", (unsigned int) dp->d_type );
        if ((dp->d_type & DT_REG) == DT_REG)
            fprintf(myout, " DT_REG ");
        if ((dp->d_type & DT_DIR) == DT_DIR)
            fprintf(myout, "dir");
        else
            fprintf(myout, "maybe a file");
        #endif /* DT_DIR */
        
        #ifdef DTTOIF
            #ifdef S_ISDIR
            if (S_ISDIR(DTTOIF(dp->d_type)))
                fprintf(myout, " S_ISDIR ");
            #endif /* S_ISDIR */
        #endif /* DTTOIF */
        fprintf(myout, "\n");
    }
    (void) closedir(dirp);
    
#ifdef CREATE_OUT_FILE
    fclose(myout);
#endif /* CREATE_OUT_FILE */

}

int main(int argc, char** argv) {
	(void) argc; /* not used */
	
	gamePathInit(argv[0]); /* workout directory containing this exe and list it */
	int ref = EXIT_SUCCESS;

	dodemo();

	return ref;
}
