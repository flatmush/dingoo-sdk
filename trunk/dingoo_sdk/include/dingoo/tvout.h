#ifndef __dingoo_tvout_h__
#define __dingoo_tvout_h__

#ifdef __cplusplus
extern "C"
{
#endif

#define TVOUT_OFF 0
#define TVOUT_ON  1

extern int isTVON();
extern void tv_open();
extern void tv_close();

#ifdef __cplusplus
}
#endif

#endif
