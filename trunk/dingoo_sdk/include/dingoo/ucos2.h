#ifndef __ucos2_h__
#define __ucos2_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define OS_TICKS_PER_SEC 100

#define OS_DEL_NO_PEND 0
#define OS_DEL_ALWAYS  1

#define OS_NO_ERR       0
#define OS_PRIO_EXIST   40
#define OS_PRIO_ERR     41
#define OS_PRIO_INVALID 42

typedef void OS_EVENT;
typedef unsigned int OS_STK;

extern uint32_t  OSTimeGet();
extern void      OSTimeDly(uint16_t ticks);

extern uint8_t   OSTaskCreate(void (*task)(void* data), void* data, OS_STK* stack, uint8_t priority);
extern uint8_t   OSTaskDel(uint8_t priority);

extern OS_EVENT* OSSemCreate(uint16_t cnt);
extern OS_EVENT* OSSemDel(OS_EVENT *event, uint8_t option, uint8_t* error);
extern void      OSSemPend(OS_EVENT* event, uint16_t timeout, uint8_t* error);
extern uint8_t   OSSemPost(OS_EVENT* event);

#ifdef __cplusplus
}
#endif

#endif
