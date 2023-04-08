#ifndef __PAWSINTRINSICS__

// BIT MANIPULATION AND SCALAR CRYPTO INTRINSICS
static inline long _rv64_adduw(long rs1, long rs2) { long rd; __asm__ ("add.uw %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_andn(long rs1, long rs2) { long rd; __asm__ ("andn %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_bclr(long rs1, long rs2) { long rd; if (__builtin_constant_p(rs2)) __asm__ ("bclri %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(63 & rs2)); else __asm__ ("bclr %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_bext(long rs1, long rs2) { long rd; if (__builtin_constant_p(rs2)) __asm__ ("bexti %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(63 & rs2)); else __asm__ ("bext %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_binv(long rs1, long rs2) { long rd; if (__builtin_constant_p(rs2)) __asm__ ("binvi %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(63 & rs2)); else __asm__ ("binv %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_bset(long rs1, long rs2) { long rd; if (__builtin_constant_p(rs2)) __asm__ ("bseti %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(63 & rs2)); else __asm__ ("bset %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_clmul(long rs1, long rs2) { long rd; __asm__ ("clmul  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_clmulh(long rs1, long rs2) { long rd; __asm__ ("clmulh  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_clmulr(long rs1, long rs2) { long rd; __asm__ ("clmulr  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_clz(long rs1) { long rd; __asm__ ("clz     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline int _rv64_clzw(int rs1) { int rd; __asm__ ("clzw     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline long _rv64_cpop(long rs1) { long rd; __asm__ ("cpop    %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline int _rv64_cpopw(int rs1) { int rd; __asm__ ("cpopw    %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline long _rv64_ctz(long rs1) { long rd; __asm__ ("ctz     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline int _rv64_ctzw(int rs1) { int rd; __asm__ ("ctzw     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline long _rv64_max(long rs1, long rs2) { long rd; __asm__ ("max  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_maxu(long rs1, long rs2) { long rd; __asm__ ("maxu %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_min(long rs1, long rs2) { long rd; __asm__ ("min  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_minu(long rs1, long rs2) { long rd; __asm__ ("minu %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_orn(long rs1, long rs2) { long rd; __asm__ ("orn %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_pack(long rs1, long rs2) { long rd; __asm__ ("pack  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_packh(long rs1, long rs2) { long rd; __asm__ ("packh  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv64_packw(int rs1, int rs2) { int rd; __asm__ ("packw  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_rev8(long rs1) { long rd; __asm__ ("rev8     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline long _rv64_rol(long rs1, long rs2) { long rd; if (__builtin_constant_p(rs2)) __asm__ ("rori    %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(63 & -rs2)); else __asm__ ("rol     %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_ror(long rs1, long rs2) { long rd; if (__builtin_constant_p(rs2)) __asm__ ("rori    %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(63 &  rs2)); else __asm__ ("ror     %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_sext_b(long rs1) { long rd; __asm__ ("sext.b  %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline long _rv64_sext_h(long rs1) { long rd; __asm__ ("sext.h  %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline long _rv64_sh1add(long rs1, long rs2) { long rd; __asm__ ("sh1add  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_sh1adduw(long rs1, long rs2) { long rd; __asm__ ("sh1add.uw  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_sh2add(long rs1, long rs2) { long rd; __asm__ ("sh2add  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_sh2adduw(long rs1, long rs2) { long rd; __asm__ ("sh2add.uw  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_sh3add(long rs1, long rs2) { long rd; __asm__ ("sh3add  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_sh3adduw(long rs1, long rs2) { long rd; __asm__ ("sh3add.uw  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline long _rv64_slliuw(long rs1, int rs2) { long rd; __asm__ ("slli.uw %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(63 & rs2)); return rd; }
static inline long _rv64_xnor(long rs1, long rs2) { long rd; __asm__ ("xnor %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv64_zext_h(int rs1) { int rd; __asm__ ("zext.h  %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }

// FLOATING POINT INTRINSICS - SINGLE PRECISION
static inline float _rv64_fadds(float rs1, float rs2) { float rd; __asm__ ("fadd.s %0, %1, %2" : "=f"(rd) : "f"(rs1), "f"(rs2)); return rd; }
static inline int _rv64_fclasss(float rs1) { int rd; __asm__ ("fclass.s %0, %1" : "=r"(rd) : "f"(rs1)); return rd; }
static inline float _rv64_fdivs(float rs1, float rs2) { float rd; __asm__ ("fdiv.s %0, %1, %2" : "=f"(rd) : "f"(rs1), "f"(rs2)); return rd; }
static inline float _rv64_fmuls(float rs1, float rs2) { float rd; __asm__ ("fmul.s %0, %1, %2" : "=f"(rd) : "f"(rs1), "f"(rs2)); return rd; }
static inline float _rv64_fsubs(float rs1, float rs2) { float rd; __asm__ ("fsub.s %0, %1, %2" : "=f"(rd) : "f"(rs1), "f"(rs2)); return rd; }
static inline float _rv64_fsqrts(float rs1) { float rd; __asm__ ("fsqrt.s %0, %1" : "=f"(rd) : "f"(rs1)); return rd; }

// FLOATING POINT INTRINSICS - DOUBLE PRECISION
static inline double _rv64_faddd(double rs1, double rs2) { double rd; __asm__ ("fadd.d %0, %1, %2" : "=f"(rd) : "f"(rs1), "f"(rs2)); return rd; }
static inline int _rv64_fclassd(double rs1) { int rd; __asm__ ("fclass.d %0, %1" : "=r"(rd) : "f"(rs1)); return rd; }
static inline double _rv64_fdivd(double rs1, double rs2) { double rd; __asm__ ("fdiv.d %0, %1, %2" : "=f"(rd) : "f"(rs1), "f"(rs2)); return rd; }
static inline double _rv64_fmuld(double rs1, double rs2) { double rd; __asm__ ("fmul.d %0, %1, %2" : "=f"(rd) : "f"(rs1), "f"(rs2)); return rd; }
static inline double _rv64_fsubd(double rs1, double rs2) { double rd; __asm__ ("fsub.d %0, %1, %2" : "=f"(rd) : "f"(rs1), "f"(rs2)); return rd; }
static inline double _rv64_fsqrtd(double rs1) { double rd; __asm__ ("fsqrt.d %0, %1" : "=f"(rd) : "f"(rs1)); return rd; }

#define __PAWSINTRINSICS__
#endif
