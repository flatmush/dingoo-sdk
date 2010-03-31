#ifndef __entry_h__
#define __entry_h__

extern int _sys_judge_event(void*);

extern wchar_t* __to_unicode_le(char*);
extern char*    __to_locale_ansi(wchar_t*);

#endif
