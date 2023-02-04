#ifndef __PAWSINTRINSICS__

static inline long _rv64_bset(long rs1, long rs2) { long rd; if (__builtin_constant_p(rs2)) __asm__ ("bseti %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(63 & rs2)); else __asm__ ("bset %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_bclr(long rs1, long rs2) { long rd; if (__builtin_constant_p(rs2)) __asm__ ("bclri %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(63 & rs2)); else __asm__ ("bclr %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_binv(long rs1, long rs2) { long rd; if (__builtin_constant_p(rs2)) __asm__ ("binvi %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(63 & rs2)); else __asm__ ("binv %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_bext(long rs1, long rs2) { long rd; if (__builtin_constant_p(rs2)) __asm__ ("bexti %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(63 & rs2)); else __asm__ ("bext %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }

static inline long _rv64_andn(long rs1, long rs2) { long rd; __asm__ ("andn %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_orn(long rs1, long rs2) { long rd; __asm__ ("orn %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_xnor(long rs1, long rs2) { long rd; __asm__ ("xnor %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }

static inline long _rv64_clz(long rs1) { long rd; __asm__ ("clz     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline long _rv64_ctz(long rs1) { long rd; __asm__ ("ctz     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline long _rv64_cpop(long rs1) { long rd; __asm__ ("cpop    %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }

static inline int _rv64_clzw(int rs1) { int rd; __asm__ ("clzw     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline int _rv64_ctzw(int rs1) { int rd; __asm__ ("ctzw     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline int _rv64_cpopw(int rs1) { int rd; __asm__ ("cpopw    %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }

static inline long _rv64_sext_b(long rs1) { long rd; __asm__ ("sext.b  %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline long _rv64_sext_h(long rs1) { long rd; __asm__ ("sext.h  %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline int _rv64_zext_h(int rs1) { int rd; __asm__ ("zext.h  %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }

static inline long _rv64_min(long rs1, long rs2) { long rd; __asm__ ("min  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_minu(long rs1, long rs2) { long rd; __asm__ ("minu %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_max(long rs1, long rs2) { long rd; __asm__ ("max  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_maxu(long rs1, long rs2) { long rd; __asm__ ("maxu %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }

static inline long _rv64_rol(long rs1, long rs2) { long rd; if (__builtin_constant_p(rs2)) __asm__ ("rori    %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(63 & -rs2)); else __asm__ ("rol     %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_ror(long rs1, long rs2) { long rd; if (__builtin_constant_p(rs2)) __asm__ ("rori    %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(63 &  rs2)); else __asm__ ("ror     %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }

static inline long _rv64_pack(long rs1, long rs2) { long rd; __asm__ ("pack  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_packh(long rs1, long rs2) { long rd; __asm__ ("packh  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv64_packw(int rs1, int rs2) { int rd; __asm__ ("packw  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }

#define __PAWSINTRINSICS__
#endif
