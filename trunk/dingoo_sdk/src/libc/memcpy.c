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
		"ld $t0, 0(%0)\n\t"
		"sd $t0, 0(%1)\n\t"
		: : "r"(dst), "r"(src)
	);
}

inline void _memcpy_4x4(void* dst, const void* src) {
	asm(
		"ld $t0, 0(%0)\n\t"
		"ld $t2, 8(%0)\n\t"
		"sd $t0, 0(%1)\n\t"
		"sd $t2, 8(%1)\n\t"
		: : "r"(dst), "r"(src)
	);
}

inline void _memcpy_4x8(void* dst, const void* src) {
	asm(
		"ld $t0,  0(%0)\n\t"
		"ld $t2,  8(%0)\n\t"
		"ld $t4, 16(%0)\n\t"
		"ld $t6, 24(%0)\n\t"
		"sd $t0,  0(%1)\n\t"
		"sd $t2,  8(%1)\n\t"
		"sd $t4, 16(%1)\n\t"
		"sd $t6, 24(%1)\n\t"
		: : "r"(dst), "r"(src)
	);
}

inline void _memcpy_4x16(void* dst, const void* src) {
	asm(
		"ld $t0,  0(%0)\n\t"
		"ld $t2,  8(%0)\n\t"
		"ld $t4, 16(%0)\n\t"
		"ld $t6, 24(%0)\n\t"
		"sd $t0,  0(%1)\n\t"
		"ld $t0, 32(%0)\n\t"
		"sd $t2,  8(%1)\n\t"
		"ld $t2, 40(%1)\n\t"
		"sd $t4, 16(%1)\n\t"
		"ld $t4, 48(%1)\n\t"
		"sd $t6, 24(%1)\n\t"
		"ld $t6, 56(%1)\n\t"
		"sd $t0, 32(%1)\n\t"
		"sd $t2, 40(%1)\n\t"
		"sd $t4, 48(%1)\n\t"
		"sd $t6, 56(%1)\n\t"
		: : "r"(dst), "r"(src)
	);
}

inline void _memcpy_4x32(void* dst, const void* src) {
	asm(
		"ld $t0,   0(%0)\n\t"
		"ld $t2,   8(%0)\n\t"
		"ld $t4,  16(%0)\n\t"
		"ld $t6,  24(%0)\n\t"
		"sd $t0,   0(%1)\n\t"
		"ld $t0,  32(%0)\n\t"
		"sd $t2,   8(%1)\n\t"
		"ld $t2,  40(%1)\n\t"
		"sd $t4,  16(%1)\n\t"
		"ld $t4,  48(%1)\n\t"
		"sd $t6,  24(%1)\n\t"
		"ld $t6,  56(%1)\n\t"
		"sd $t0,  32(%1)\n\t"
		"ld $t0,  64(%0)\n\t"
		"sd $t2,  40(%1)\n\t"
		"ld $t2,  72(%1)\n\t"
		"sd $t4,  48(%1)\n\t"
		"ld $t4,  80(%1)\n\t"
		"sd $t6,  56(%1)\n\t"
		"ld $t6,  88(%1)\n\t"
		"sd $t0,  64(%1)\n\t"
		"ld $t0,  96(%0)\n\t"
		"sd $t2,  72(%1)\n\t"
		"ld $t2, 104(%1)\n\t"
		"sd $t4,  80(%1)\n\t"
		"ld $t4, 112(%1)\n\t"
		"sd $t6,  88(%1)\n\t"
		"ld $t6, 120(%1)\n\t"
		"sd $t0,  96(%1)\n\t"
		"sd $t2, 104(%1)\n\t"
		"sd $t4, 112(%1)\n\t"
		"sd $t6, 120(%1)\n\t"
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
