#ifndef __libc_signal_h__
#define __libc_signal_h__

#define SIGABRT    1
#define SIGALRM    2
#define SIGBUS     3
#define SIGCHLD    4
#define SIGCONT    5
#define SIGFPE     6
#define SIGHUP     7
#define SIGILL     8
#define SIGINT     9
#define SIGKILL   10
#define SIGPIPE   11
#define SIGQUIT   12
#define SIGSEGV   13
#define SIGSTOP   14
#define SIGTERM   15
#define SIGTSTP   16
#define SIGTTIN   17
#define SIGTTOU   18
#define SIGUSR1   19
#define SIGUSR2   20
#define SIGPOLL   21
#define SIGPROF   22
#define SIGSYS    23
#define SIGTRAP   24
#define SIGURG    25
#define SIGVTALRM 26
#define SIGXCPU   27
#define SIGXFSZ   28
#define _SIGCOUNT SIGXFSZ

extern int   raise(int sig);
extern void* signal(int sig, void (*func)(int));

extern void* SIG_IGN;
extern void* SIG_DFL;
extern void* SIG_ERR;

#endif
