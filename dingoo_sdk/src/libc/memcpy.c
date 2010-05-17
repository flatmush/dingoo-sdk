#include "string.h"
#include <stdint.h>



inline void _memcpy_1x1(void* dst, const void* src) {
	asm(
		"lb $t0, 0(%0)\n\t"
		"sb $t0, 0(%1)\n\t"
		: : "r"(dst), "r"(src)
	);
}

inline void _memcpy_2x1(void* dst, const void* src) {
	asm(
		"lh $t0, 0(%0)\n\t"
		"sh $t0, 0(%1)\n\t"
		: : "r"(dst), "r"(src)
	);
}

inline void _memcpy_4x1(void* dst, const void* src) {
	asm(
		"lw $t0, 0(%0)\n\t"
		"sw $t0, 0(%1)\n\t"
		: : "r"(dst), "r"(src)
	);
}

inline void _memcpy_4x2(void* dst, const void* src) {
	asm(
		"lw $t0, 0(%0)\n\t"
		"lw $t1, 4(%0)\n\t"
		"sw $t0, 0(%1)\n\t"
		"sw $t1, 4(%1)\n\t"
		: : "r"(dst), "r"(src)
	);
}

inline void _memcpy_4x4(void* dst, const void* src) {
	asm(
		"lw $t0,  0(%0)\n\t"
		"lw $t1,  4(%0)\n\t"
		"lw $t2,  8(%0)\n\t"
		"lw $t3, 12(%0)\n\t"
		"sw $t0,  0(%1)\n\t"
		"sw $t1,  4(%1)\n\t"
		"sw $t2,  8(%1)\n\t"
		"sw $t3, 12(%1)\n\t"
		: : "r"(dst), "r"(src)
	);
}

inline void _memcpy_4x8(void* dst, const void* src) {
	asm(
		"lw $t0,  0(%0)\n\t"
		"lw $t1,  4(%0)\n\t"
		"lw $t2,  8(%0)\n\t"
		"lw $t3, 12(%0)\n\t"
		"lw $t4, 16(%0)\n\t"
		"lw $t5, 20(%0)\n\t"
		"lw $t6, 24(%0)\n\t"
		"lw $t7, 28(%0)\n\t"
		"sw $t0,  0(%1)\n\t"
		"sw $t1,  4(%1)\n\t"
		"sw $t2,  8(%1)\n\t"
		"sw $t3, 12(%1)\n\t"
		"sw $t4, 16(%1)\n\t"
		"sw $t5, 20(%1)\n\t"
		"sw $t6, 24(%1)\n\t"
		"sw $t7, 28(%1)\n\t"
		: : "r"(dst), "r"(src)
	);
}

inline void _memcpy_4x16(void* dst, const void* src) {
	asm(
		"lw $t0,  0(%0)\n\t"
		"lw $t1,  4(%0)\n\t"
		"lw $t2,  8(%0)\n\t"
		"lw $t3, 12(%0)\n\t"
		"lw $t4, 16(%0)\n\t"
		"lw $t5, 20(%0)\n\t"
		"lw $t6, 24(%0)\n\t"
		"lw $t7, 28(%0)\n\t"
		"lw $t8, 32(%0)\n\t"
		"lw $t9, 36(%0)\n\t"

		"sw $t0,  0(%1)\n\t"
		"lw $t0, 40(%0)\n\t"
		"sw $t1,  4(%1)\n\t"
		"lw $t1, 44(%0)\n\t"
		"sw $t2,  8(%1)\n\t"
		"lw $t2, 48(%0)\n\t"
		"sw $t3, 12(%1)\n\t"
		"lw $t3, 52(%0)\n\t"
		"sw $t4, 16(%1)\n\t"
		"lw $t4, 56(%0)\n\t"
		"sw $t5, 20(%1)\n\t"
		"lw $t5, 60(%0)\n\t"

		"sw $t6, 24(%1)\n\t"
		"sw $t7, 28(%1)\n\t"
		"sw $t8, 32(%1)\n\t"
		"sw $t9, 36(%1)\n\t"

		"sw $t0, 40(%1)\n\t"
		"sw $t1, 44(%1)\n\t"
		"sw $t2, 48(%1)\n\t"
		"sw $t3, 52(%1)\n\t"
		"sw $t4, 56(%1)\n\t"
		"sw $t5, 60(%1)\n\t"
		: : "r"(dst), "r"(src)
	);
}

inline void _memcpy_4x32(void* dst, const void* src) {
	asm(
		"lw $t0,   0(%0)\n\t"
		"lw $t1,   4(%0)\n\t"
		"lw $t2,   8(%0)\n\t"
		"lw $t3,  12(%0)\n\t"
		"lw $t4,  16(%0)\n\t"
		"lw $t5,  20(%0)\n\t"
		"lw $t6,  24(%0)\n\t"
		"lw $t7,  28(%0)\n\t"
		"lw $t8,  32(%0)\n\t"
		"lw $t9,  36(%0)\n\t"

		"sw $t0,   0(%1)\n\t"
		"lw $t0,  40(%0)\n\t"
		"sw $t1,   4(%1)\n\t"
		"lw $t1,  44(%0)\n\t"
		"sw $t2,   8(%1)\n\t"
		"lw $t2,  48(%0)\n\t"
		"sw $t3,  12(%1)\n\t"
		"lw $t3,  52(%0)\n\t"
		"sw $t4,  16(%1)\n\t"
		"lw $t4,  56(%0)\n\t"
		"sw $t5,  20(%1)\n\t"
		"lw $t5,  60(%0)\n\t"
		"sw $t6,  24(%1)\n\t"
		"lw $t6,  64(%0)\n\t"
		"sw $t7,  28(%1)\n\t"
		"lw $t7,  68(%0)\n\t"
		"sw $t8,  32(%1)\n\t"
		"lw $t8,  72(%0)\n\t"
		"sw $t9,  36(%1)\n\t"
		"lw $t9,  76(%0)\n\t"

		"sw $t0,  40(%1)\n\t"
		"lw $t0,  80(%0)\n\t"
		"sw $t1,  44(%1)\n\t"
		"lw $t1,  84(%0)\n\t"
		"sw $t2,  48(%1)\n\t"
		"lw $t2,  88(%0)\n\t"
		"sw $t3,  52(%1)\n\t"
		"lw $t3,  92(%0)\n\t"
		"sw $t4,  56(%1)\n\t"
		"lw $t4,  96(%0)\n\t"
		"sw $t5,  60(%1)\n\t"
		"lw $t5, 100(%0)\n\t"
		"sw $t6,  64(%1)\n\t"
		"lw $t6, 104(%0)\n\t"
		"sw $t7,  68(%1)\n\t"
		"lw $t7, 108(%0)\n\t"
		"sw $t8,  72(%1)\n\t"
		"lw $t8, 112(%0)\n\t"
		"sw $t9,  76(%1)\n\t"
		"lw $t9, 116(%0)\n\t"

		"sw $t0,  80(%1)\n\t"
		"lw $t0, 120(%0)\n\t"
		"sw $t1,  84(%1)\n\t"
		"lw $t1, 124(%0)\n\t"

		"sw $t2,  88(%1)\n\t"
		"sw $t3,  92(%1)\n\t"
		"sw $t4,  96(%1)\n\t"
		"sw $t5, 100(%1)\n\t"
		"sw $t6, 104(%1)\n\t"
		"sw $t7, 108(%1)\n\t"
		"sw $t8, 112(%1)\n\t"
		"sw $t9, 116(%1)\n\t"

		"sw $t0, 120(%1)\n\t"
		"sw $t1, 124(%1)\n\t"
		: : "r"(dst), "r"(src)
	);
}



inline void _memcpy_down_1(uint8_t* dst, const uint8_t* src, uint8_t* dst_end) {
	if(((uintptr_t)dst_end - (uintptr_t)dst) >= 1) {
		_memcpy_1x1(dst, src);
		dst += 1;
		src += 1;
	}
}

inline void _memcpy_down_2(uint8_t* dst, const uint8_t* src, uint8_t* dst_end) {
	if(((uintptr_t)dst_end - (uintptr_t)dst) >= 2) {
		_memcpy_2x1(dst, src);
		dst += 2;
		src += 2;
	}
	_memcpy_down_1(dst, src, dst_end);
}

inline void _memcpy_down_4(uint8_t* dst, const uint8_t* src, uint8_t* dst_end) {
	if(((uintptr_t)dst_end - (uintptr_t)dst) >= 4) {
		_memcpy_4x1(dst, src);
		dst += 4;
		src += 4;
	}
	_memcpy_down_2(dst, src, dst_end);
}

inline void _memcpy_down_8(uint8_t* dst, const uint8_t* src, uint8_t* dst_end) {
	if(((uintptr_t)dst_end - (uintptr_t)dst) >= 8) {
		_memcpy_4x2(dst, src);
		dst += 8;
		src += 8;
	}
	_memcpy_down_4(dst, src, dst_end);
}

inline void _memcpy_down_16(uint8_t* dst, const uint8_t* src, uint8_t* dst_end) {
	if(((uintptr_t)dst_end - (uintptr_t)dst) >= 16) {
		_memcpy_4x4(dst, src);
		dst += 16;
		src += 16;
	}
	_memcpy_down_8(dst, src, dst_end);
}

inline void _memcpy_down_32(uint8_t* dst, const uint8_t* src, uint8_t* dst_end) {
	if(((uintptr_t)dst_end - (uintptr_t)dst) >= 32) {
		_memcpy_4x8(dst, src);
		dst += 32;
		src += 32;
	}
	_memcpy_down_16(dst, src, dst_end);
}

inline void _memcpy_down_64(uint8_t* dst, const uint8_t* src, uint8_t* dst_end) {
	if(((uintptr_t)dst_end - (uintptr_t)dst) >= 64) {
		_memcpy_4x16(dst, src);
		dst += 64;
		src += 64;
	}
	_memcpy_down_32(dst, src, dst_end);
}

inline void _memcpy_down_128(uint8_t* dst, const uint8_t* src, uint8_t* dst_end) {
	if(((uintptr_t)dst_end - (uintptr_t)dst) >= 128) {
		_memcpy_4x32(dst, src);
		dst += 128;
		src += 128;
	}
	_memcpy_down_64(dst, src, dst_end);
}



inline void _memcpy_1(uint8_t* dst, const uint8_t* src, uint8_t* dst_end) {
	for(; dst <= (dst_end - 1); dst += 1, src += 1)
		_memcpy_1x1(dst, src);
}

inline void _memcpy_2(uint8_t* dst, const uint8_t* src, uint8_t* dst_end) {
	for(; dst <= (dst_end - 2); dst += 2, src += 2)
		_memcpy_2x1(dst, src);
	_memcpy_down_1(dst, src, dst_end);
}

inline void _memcpy_4(uint8_t* dst, const uint8_t* src, uint8_t* dst_end) {
	for(; dst <= (dst_end - 4); dst += 4, src += 4)
		_memcpy_4x1(dst, src);
	_memcpy_down_2(dst, src, dst_end);
}

inline void _memcpy_8(uint8_t* dst, const uint8_t* src, uint8_t* dst_end) {
	for(; dst <= (dst_end - 8); dst += 8, src += 8)
		_memcpy_4x2(dst, src);
	_memcpy_down_4(dst, src, dst_end);
}

inline void _memcpy_16(uint8_t* dst, const uint8_t* src, uint8_t* dst_end) {
	for(; dst <= (dst_end - 16); dst += 16, src += 16)
		_memcpy_4x4(dst, src);
	_memcpy_down_8(dst, src, dst_end);
}

inline void _memcpy_32(uint8_t* dst, const uint8_t* src, uint8_t* dst_end) {
	for(; dst <= (dst_end - 32); dst += 32, src += 32)
		_memcpy_4x8(dst, src);
	_memcpy_down_16(dst, src, dst_end);
}

inline void _memcpy_64(uint8_t* dst, const uint8_t* src, uint8_t* dst_end) {
	for(; dst <= (dst_end - 64); dst += 64, src += 64)
		_memcpy_4x16(dst, src);
	_memcpy_down_32(dst, src, dst_end);
}

inline void _memcpy_128(uint8_t* dst, const uint8_t* src, uint8_t* dst_end) {
	for(; dst <= (dst_end - 128); dst += 128, src += 128)
		_memcpy_4x32(dst, src);
	_memcpy_down_64(dst, src, dst_end);
}



// Please note that this is WIP and does not yet work correctly
void* _memcpy_fast(void* dst, const void* src, uintptr_t size) {
	if((dst == NULL) || (src == NULL))
		return NULL;

	uint8_t*       _dst = (uint8_t*)dst;
	uint8_t*       _dst_end = &_dst[size];
	const uint8_t* _src = (const uint8_t*)src;

	if((((uintptr_t)_dst & 1) != 0) && (((uintptr_t)_dst & 1) == ((uintptr_t)_src & 1)) && ((_dst + 1) <= _dst_end)) {
		_memcpy_1x1(_dst, _src);
		_dst += 1; _src += 1;
	}
	if(((uintptr_t)_dst & 1) == 0) {
		if((((uintptr_t)_dst & 3) != 0) && (((uintptr_t)_dst & 3) == ((uintptr_t)_src & 3)) && ((_dst + 2) <= _dst_end)) {
			_memcpy_2x1(_dst, _src);
			_dst += 2; _src += 2;
		}
		if(((uintptr_t)_dst & 3) == 0) {
			if((((uintptr_t)_dst & 7) != 0) && (((uintptr_t)_dst & 7) == ((uintptr_t)_src & 7)) && ((_dst + 4) <= _dst_end)) {
				_memcpy_4x1(_dst, _src);
				_dst += 4; _src += 4;
			}
			if(((uintptr_t)_dst & 7) == 0) {
				_memcpy_128(_dst, _src, _dst_end);
			} else _memcpy_4(_dst, _src, _dst_end);
		} else _memcpy_2(_dst, _src, _dst_end);
	} else _memcpy_1(_dst, _src, _dst_end);

	return dst;
}
