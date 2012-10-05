/*
** Demo on how to write to a file instead of stdout.
** Outputs to "stdout.txt" in current directory,
** NOTE requires SDK version r315 or later (due to bug in fwrite).
*/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define VERSION_MAJOR    1
#define VERSION_MINOR    0
#define VERSION_REVISION 0

#ifdef DINGOO_NATIVE
#define DINGOO_NATIVE_LOGFILE "stdout.txt"

#ifdef USE_DINGOO_SDK_INTERNALS
extern char* _app_path; /* from Native SDK entry_point.c - this is not a good idea! */
#else
#   define _app_path "printf_replacement"
#endif /* USE_DINGOO_SDK_INTERNALS */

void debug_printf_init()
{
    char tmp_buf[1024];
    int mesg_len=0;
    FILE *fptr=NULL;

    fptr = fopen(DINGOO_NATIVE_LOGFILE, "w");
    if (fptr != NULL)
    {
        mesg_len = snprintf(tmp_buf, sizeof(tmp_buf)-1, "stdout for %s\n", _app_path);
        fwrite(tmp_buf, mesg_len, 1, fptr);
        fclose(fptr);
    }
    /* else nothing we can do except maybe print to screen */
}

void debug_printf(char *fmt, ...)
{
    va_list ap;
    char tmp_buf[1024];
    int mesg_len=0;
    FILE *fptr=NULL;

    va_start(ap, fmt);
    fptr = fopen(DINGOO_NATIVE_LOGFILE, "a");
    if (fptr != NULL)
    {
        mesg_len = vsnprintf(tmp_buf, sizeof(tmp_buf)-1, fmt, ap);
        fwrite(tmp_buf, mesg_len, 1, fptr);
        fclose(fptr);
    }
    /* else nothing we can do except maybe print to screen */
    va_end(ap);
}
#define printf debug_printf
#endif /* DINGOO_NATIVE */

int main(int argc, char** argv)
{
    (void) argc; /* not used */
    (void) argv; /* not used */

#ifdef DINGOO_NATIVE
    debug_printf_init();
#endif /* DINGOO_NATIVE */

    printf("hello world\n");

    return 0;
}
