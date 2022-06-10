#ifndef __PAWSINTRINSICS__

// MISCELLANEOUS USEFUL INTRINSICS
static inline int _rv32_mulh(int rs1, int rs2) { int rd; __asm__ ("mulh   %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_mulhsu(int rs1, int rs2) { int rd; __asm__ ("mulhsu  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_mulhu(int rs1, int rs2) { int rd; __asm__ ("mulhu  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }

// BIT MANIPULATION INSTRUCTIONS INTRINSICS (Zba Zbb Zbc Zbs)
static inline int _rv32_andn(int rs1, int rs2) { int rd; __asm__ ("andn %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_orn(int rs1, int rs2) { int rd; __asm__ ("orn %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_xnor(int rs1, int rs2) { int rd; __asm__ ("xnor %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }

static inline int _rv32_clz(int rs1) { int rd; __asm__ ("clz     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline int _rv32_ctz(int rs1) { int rd; __asm__ ("ctz     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline int _rv32_cpop(int rs1) { int rd; __asm__ ("cpop    %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }

static inline int _rv32_sext_b(int rs1) { int rd; __asm__ ("sext.b  %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline int _rv32_sext_h(int rs1) { int rd; __asm__ ("sext.h  %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline int _rv32_zext_h(int rs1) { int rd; __asm__ ("zext.h  %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }

static inline int _rv32_min(int rs1, int rs2) { int rd; __asm__ ("min  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_minu(int rs1, int rs2) { int rd; __asm__ ("minu %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_max(int rs1, int rs2) { int rd; __asm__ ("max  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_maxu(int rs1, int rs2) { int rd; __asm__ ("maxu %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }

static inline int _rv32_bset(int rs1, int rs2) { int rd; if (__builtin_constant_p(rs2)) __asm__ ("bseti %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(31 & rs2)); else __asm__ ("bset %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_bclr(int rs1, int rs2) { int rd; if (__builtin_constant_p(rs2)) __asm__ ("bclri %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(31 & rs2)); else __asm__ ("bclr %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_binv(int rs1, int rs2) { int rd; if (__builtin_constant_p(rs2)) __asm__ ("binvi %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(31 & rs2)); else __asm__ ("binv %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_bext(int rs1, int rs2) { int rd; if (__builtin_constant_p(rs2)) __asm__ ("bexti %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(31 & rs2)); else __asm__ ("bext %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }

static inline int _rv32_rol(int rs1, int rs2) { int rd; if (__builtin_constant_p(rs2)) __asm__ ("rori    %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(31 & -rs2)); else __asm__ ("rol     %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_ror(int rs1, int rs2) { int rd; if (__builtin_constant_p(rs2)) __asm__ ("rori    %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(31 &  rs2)); else __asm__ ("ror     %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }

static inline int _rv32_rev8(int rs1)  { int rd; __asm__ ("rev8     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline int _rv32_orc_b(int rs1)  { int rd; __asm__ ("orc.b     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }

static inline int _rv32_clmul(int rs1, int rs2) { int rd; __asm__ ("clmul   %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_clmulh(int rs1, int rs2) { int rd; __asm__ ("clmulh  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_clmulr(int rs1, int rs2) { int rd; __asm__ ("clmulr  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }

static inline int _rv32_sh1add(int rs1, int rs2) { int rd; __asm__ ("sh1add %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_sh2add(int rs1, int rs2) { int rd; __asm__ ("sh2add %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_sh3add(int rs1, int rs2) { int rd; __asm__ ("sh3add %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }

// SCALAR CRYPTO BIT MANIPULATION INTRINSICS (Zbkb)
static inline int _rv32_brev8(int rs1) { int rd; __asm__ ("brev8     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline int _rv32_pack(int rs1, int rs2) { int rd; __asm__ ("pack  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_packh(int rs1, int rs2) { int rd; __asm__ ("packh  %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
static inline int _rv32_unzip(int rs1) { int rd; __asm__ ("unzip     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
static inline int _rv32_zip(int rs1) { int rd; __asm__ ("zip     %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }

#define __PAWSINTRINSICS__
#endif
