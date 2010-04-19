#include "signal.h"
#include <stdlib.h>
#include <errno.h>

uint32_t _SIG_IGN;
uint32_t _SIG_DFL;
uint32_t _SIG_ERR;

void* SIG_IGN = (void*)&_SIG_IGN;
void* SIG_DFL = (void*)&_SIG_DFL;
void* SIG_ERR = (void*)&_SIG_ERR;

void (*_signal_func[_SIGCOUNT])(int);



int raise(int sig) {
	if((sig <= 0) || (sig > _SIGCOUNT)) {
		errno = EINVAL;
		return -1;
	}

	if((void*)_signal_func[sig - 1] == SIG_IGN)
		return 0;
	if((void*)_signal_func[sig - 1] == SIG_DFL) {
		switch(sig) {
			case SIGABRT:
			case SIGBUS:
			case SIGFPE:
			case SIGILL:
			case SIGQUIT:
			case SIGSEGV:
			case SIGSYS:
			case SIGTRAP:
			case SIGXCPU:
			case SIGXFSZ:
				abort();
			case SIGALRM:
			case SIGHUP:
			case SIGINT:
			case SIGKILL:
			case SIGPIPE:
			case SIGTERM:
			case SIGUSR1:
			case SIGUSR2:
			case SIGPOLL:
			case SIGPROF:
			case SIGVTALRM:
				exit(EXIT_FAILURE);
			default:
				return 0;
		}
		return 0;
	}

	_signal_func[sig - 1](sig);
	return 0;
}

void* signal(int sig, void (*func)(int)) {
	if((sig <= 0) || (sig > _SIGCOUNT)) {
		errno = EINVAL;
		return SIG_ERR;
	}
	if((sig == SIGKILL) || (sig == SIGSTOP)) {
		errno = EINVAL;
		return SIG_ERR;
	}

	void* tempRet = _signal_func[sig - 1];
	_signal_func[sig - 1] = func;
	return tempRet;
}
