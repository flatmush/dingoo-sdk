#ifndef __cache_h__
#define __cache_h__

#ifdef __cplusplus
extern "C"
{
#endif

extern void _icache_flush_all();
extern void _dcache_writeback_all();
extern void _dcache_invalidate_all();
extern void _icache_invalidate_all();

#ifdef __cplusplus
}
#endif

#endif
