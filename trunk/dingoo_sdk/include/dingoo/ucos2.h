#ifndef __ucos2_h__
#define __ucos2_h__

#include <stdint.h>

#define OS_TICKS_PER_SEC 100

#define OS_DEL_NO_PEND
#define OS_DEL_ALWAYS

typedef void OS_EVENT;

extern uint32_t  OSTimeGet();
extern void      OSTimeDly(uint16_t ticks);

extern uint8_t   OSTaskCreate(void (*task)(), void* data, uint16_t stack_size, uint8_t priority);
extern uint8_t   OSTaskDel(uint8_t priority);

extern OS_EVENT* OSSemCreate(uint16_t cnt);
extern OS_EVENT* OSSemDel(OS_EVENT *event, uint8_t option, uint8_t* error);
extern void      OSSemPend(OS_EVENT* event, uint16_t timeout, uint8_t* error);
extern uint8_t   OSSemPost(OS_EVENT* event);

#endif
