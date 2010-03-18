#ifndef __ucos2_h__
#define __ucos2_h__

#include <stdint.h>

#define OS_TICKS_PER_SEC 100

#define OS_DEL_NO_PEND
#define OS_DEL_ALWAYS

#define OS_NO_ERR 0x0

typedef void OS_EVENT;
typedef unsigned int OS_STK;

extern uint32_t  OSTimeGet();
extern void      OSTimeDly(uint16_t ticks);

#define OS_PRIO_EXIST   0x40
#define OS_PRIO_ERR     0x41
#define OS_PRIO_INVALID 0x42

extern uint8_t   OSTaskCreate(void (*task)(void* data), void* data, OS_STK* stack, uint8_t priority);
extern uint8_t   OSTaskDel(uint8_t priority);

extern OS_EVENT* OSSemCreate(uint16_t cnt);
extern OS_EVENT* OSSemDel(OS_EVENT *event, uint8_t option, uint8_t* error);
extern void      OSSemPend(OS_EVENT* event, uint16_t timeout, uint8_t* error);
extern uint8_t   OSSemPost(OS_EVENT* event);

#endif
