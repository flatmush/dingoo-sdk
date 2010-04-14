#ifndef __inttypes_h__
#define __inttypes_h__

#include <stdint.h>

#define PRId8  "hhd"
#define PRId16 "hd"
#define PRId32 "ld"
#define PRId64 "lld"

#define PRIdFAST8  "d"
#define PRIdFAST16 "d"
#define PRIdFAST32 "ld"
#define PRIdFAST64 "lld"

#define PRIdLEAST8  "hhd"
#define PRIdLEAST16 "hd"
#define PRIdLEAST32 "ld"
#define PRIdLEAST32 "lld"

#define PRIdPTR PRId32

#define PRIi8  "hhi"
#define PRIi16 "hi"
#define PRIi32 "li"
#define PRIi64 "lli"

#define PRIiFAST8  "i"
#define PRIiFAST16 "i"
#define PRIiFAST32 "li"
#define PRIiFAST64 "lli"

#define PRIiLEAST8  "hhi"
#define PRIiLEAST16 "hi"
#define PRIiLEAST32 "li"
#define PRIiLEAST64 "lli"

#define PRIiPTR PRIi32

#define PRIo8  "hho"
#define PRIo16 "ho"
#define PRIo32 "lo"
#define PRIo64 "llo"

#define PRIoFAST8  "o"
#define PRIoFAST16 "o"
#define PRIoFAST32 "lo"
#define PRIoFAST64 "llo"

#define PRIoLEAST8  "hho"
#define PRIoLEAST16 "ho"
#define PRIoLEAST32 "lo"
#define PRIoLEAST64 "llo"

#define PRIoPTR PRIo32

#define PRIu8  "hhu"
#define PRIu16 "hu"
#define PRIu32 "lu"
#define PRIu64 "llu"

#define PRIuFAST8  "u"
#define PRIuFAST16 "u"
#define PRIuFAST32 "lu"
#define PRIuFAST64 "llu"

#define PRIuLEAST8  "hhu"
#define PRIuLEAST16 "hu"
#define PRIuLEAST32 "lu"
#define PRIuLEAST64 "llu"

#define PRIuPTR PRIu32

#define PRIX8  "hhX"
#define PRIx8  "hhx"
#define PRIX16 "hX"
#define PRIx16 "hx"
#define PRIX32 "lX"
#define PRIx32 "lx"
#define PRIX64 "llX"
#define PRIx64 "llx"

#define PRIXFAST8  "X"
#define PRIxFAST8  "x"
#define PRIXFAST16 "X"
#define PRIxFAST16 "x"
#define PRIXFAST32 "lX"
#define PRIxFAST32 "lx"
#define PRIXFAST64 "llX"
#define PRIxFAST64 "llx"

#define PRIXLEAST8  "hhX"
#define PRIxLEAST8  "hhx"
#define PRIXLEAST16 "hX"
#define PRIxLEAST16 "hx"
#define PRIXLEAST32 "lX"
#define PRIxLEAST32 "lx"
#define PRIXLEAST64 "llX"
#define PRIxLEAST64 "llx"

#define PRIXPTR PRIX32
#define PRIxPTR PRIx32

#define SCNd8  "hhd"
#define SCNd16 "hd"
#define SCNd32 "ld"
#define SCNd64 "lld"

#define SCNdFAST8  "d"
#define SCNdFAST16 "d"
#define SCNdFAST32 "ld"
#define SCNdFAST64 "lld"

#define SCNdLEAST16 "hd"
#define SCNdLEAST32 "ld"

#define SCNdPTR SCNd32

#define SCNi8  "hhi"
#define SCNi16 "hi"
#define SCNi32 "li"
#define SCNi64 "lli"

#define SCNiFAST8   "i"
#define SCNiFAST16  "i"
#define SCNiFAST32  "li"
#define SCNiFAST64  "lli"

#define SCNiLEAST8  "hhi"
#define SCNiLEAST16 "hi"
#define SCNiLEAST32 "li"
#define SCNiLEAST64 "lli"

#define SCNiPTR SCNi32

#define SCNo8  "hho"
#define SCNo16 "ho"
#define SCNo32 "lo"
#define SCNo64 "llo"

#define SCNoFAST8  "o"
#define SCNoFAST16 "o"
#define SCNoFAST32 "lo"
#define SCNoFAST64 "llo"

#define SCNoLEAST8  "hho"
#define SCNoLEAST16 "ho"
#define SCNoLEAST32 "lo"
#define SCNoLEAST64 "llo"

#define SCNoPTR SCNo32

#define SCNu8  "hhu"
#define SCNu16 "hu"
#define SCNu32 "lu"
#define SCNu64 "llu"

#define SCNuFAST8  "u"
#define SCNuFAST16 "u"
#define SCNuFAST32 "lu"
#define SCNuFAST64 "llu"

#define SCNuLEAST8  "hhu"
#define SCNuLEAST16 "hu"
#define SCNuLEAST32 "lu"
#define SCNuLEAST64 "llu"

#define SCNuPTR SCNu32

#define SCNx8  "hhx"
#define SCNx16 "hx"
#define SCNx32 "lx"
#define SCNx64 "llx"

#define SCNxFAST8  "x"
#define SCNxFAST16 "x"
#define SCNxFAST32 "lx"
#define SCNxFAST64 "llx"

#define SCNxLEAST8  "hhx"
#define SCNxLEAST16 "hx"
#define SCNxLEAST32 "lx"
#define SCNxLEAST64 "llx"

#define SCNxPTR SCNx32

typedef int32_t  int_farptr_t
typedef uint32_t uint_farptr_t

#endif
