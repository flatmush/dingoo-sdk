#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>
#include <dingoo/ucos2.h>
#include <pthread/noreturn.h>



typedef struct
{
	uint8_t  priority;
	uint8_t  stack[16384];

	bool     ended;
	unsigned locked;
	void*    value;

	void*  (*entry)(void*);
	void*    arg;
} uthread_t;

static uthread_t* uthread_list[256] = {0};



static void uthread_unlock(
	volatile uthread_t* thread)
{
	if (thread->locked)
	{
		thread->locked--;
		return;
	}
	uthread_list[thread->priority] = NULL;
	free((void*)thread);
}

static bool uthread_lock(
	volatile uthread_t* thread)
{
	if (!thread
		|| ((thread->locked + 1) == 0))
		return false;
	thread->locked++;
	return true;
}



static noreturn void uthread_exit(
	volatile uthread_t* thread)
{
	thread->ended = true;
	uint8_t p = thread->priority;
	uthread_unlock(thread);
	OSTaskDel(p);
	exit(0);
}

static noreturn void uthread_entry(
	volatile uthread_t* thread)
{
	thread->value
		= thread->entry(thread->arg);
	uthread_exit(thread);
}



int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
    void *(*start_routine)(void*), void *arg)
{
	(void)attr; /* Not used. */

	uthread_t* uthread
		= (uthread_t*)malloc(
			sizeof(uthread_t));
	if (!uthread)
		return EAGAIN;

	memset(uthread->stack, 0x00,
		sizeof(uthread->stack));
	uthread->ended  = false;
	uthread->value  = NULL;
	uthread->locked = 0;
	uthread->entry  = start_routine;
	uthread->arg    = arg;

	for (uthread->priority = 5;; uthread->priority++)
	{
		uint8_t t = OSTaskCreate(
			(void*)uthread_entry,
			uthread,
			(OS_STK*)uthread->stack,
			uthread->priority);
		if (t == OS_NO_ERR)
		{
			*thread = (uintptr_t)uthread;
			uthread_list[t] = uthread;
			return 0;
		}

	}

	return EAGAIN;
}


noreturn void pthread_exit(void *value_ptr)
{
	unsigned stack_val
		= (unsigned)value_ptr;
	void* stack_ptr = &stack_val;
	uintptr_t stack_addr
		= (uintptr_t)stack_ptr;

	unsigned i;
	for (i = 0; i < 256; i++)
	{
		uthread_t* t = uthread_list[i];
		if (!t) continue;
		uintptr_t start
			= (uintptr_t)&(t->stack);
		uintptr_t end = start + sizeof(t->stack);
		if ((stack_addr > start)
			&& (stack_addr < end))
		{
			t->value = value_ptr;
			uthread_exit(t);
		}
	}

	exit(0);
}

int pthread_join(pthread_t thread, void **value_ptr)
{
	if (!thread)
		return EINVAL;

	volatile uthread_t* t
		= (volatile uthread_t*)thread;
	
	if (!uthread_lock(t))
		return EINVAL;
	while (!t->ended);
	if (value_ptr)
		*value_ptr = t->value;
	uthread_unlock(t);
	return 0;
}
