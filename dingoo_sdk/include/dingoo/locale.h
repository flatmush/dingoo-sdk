#ifndef __dingoo_locale_h__
#define __dingoo_locale_h__

#ifdef __cplusplus
extern "C"
{
#endif

// Language codes (at least for firmware 1.2)
#define LANG_CHINESE_SIMPLIFIED  0
#define LANG_CHINESE_TRADITIONAL 1
#define LANG_ENGLISH             2
#define LANG_JAPANESE            3
#define LANG_KOREAN              4
#define LANG_RUSSIAN             5
#define LANG_GERMAN              6
#define LANG_FRENCH              7
#define LANG_SPANISH             8

extern int get_current_language();

#ifdef __cplusplus
}
#endif

#endif
