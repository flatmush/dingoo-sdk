#ifndef __errno_h__
#define __errno_h__

#ifdef __cplusplus
extern "C"
{
#endif

#define EACCES 1
#define EADDRINUSE 2
#define EADDRNOTAVAIL 3
#define EAFNOSUPPORT 4
#define EAGAIN 5
#define EALREADY 6
#define EBADF 7
#define EBADMSG 8
#define EBUSY 9
#define ECANCELED 10
#define ECHILD 11
#define ECONNABORTED 12
#define ECONNREFUSED 13
#define ECONNRESET 14
#define EDEADLK 15
#define EDESTADDRREQ 16
#define EDOM 17
#define EDQUOT 18
#define EEXIST 19
#define EFAULT 20
#define EFBIG 21
#define EHOSTUNREACH 22
#define EIDRM 23
#define EILSEQ 24
#define EINPROGRESS 25
#define EINTR 26
#define EINVAL 27
#define EIO 28
#define EISCONN 29
#define EISDIR 30
#define ELOOP 31
#define EMFILE 32
#define EMLINK 33
#define EMSGSIZE 34
#define EMULTIHOP 35
#define ENAMETOOLONG 36
#define ENETDOWN 37
#define ENETRESET 38
#define ENETUNREACH 39
#define ENFILE 40
#define ENOBUFS 41
#define ENODATA 42
#define ENODEV 43
#define ENOENT 44
#define ENOEXEC 45
#define ENOLCK 46
#define ENOLINK 47
#define ENOMEM 48
#define ENOMSG 49
#define ENOPROTOOPT 50
#define ENOSPC 51
#define ENOSR 52
#define ENOSTR 53
#define ENOSYS 54
#define ENOTCONN 55
#define ENOTDIR 56
#define ENOTEMPTY 57
#define ENOTSOCK 58
#define ENOTSUP 59
#define ENOTTY 60
#define ENXIO 61
#define EOPNOTSUPP 62
#define EOVERFLOW 63
#define EPERM 64
#define EPIPE 65
#define EPROTO 66
#define EPROTONOSUPPORT 67
#define EPROTOTYPE 68
#define ERANGE 69
#define EROFS 70
#define ESPIPE 71
#define ESRCH 72
#define ESTALE 73
#define ETIME 74
#define ETIMEDOUT 75
#define ETXTBSY 76
#define EWOULDBLOCK 77
#define EXDEV 78
#define E2BIG 79

#define ETOOBIG E2BIG

extern int errno;
extern int * __errno_location(void);

#ifdef __cplusplus
}
#endif

#endif
