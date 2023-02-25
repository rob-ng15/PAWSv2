// OVERRIDE GCC BUILTINS
void *__wrap_memcpy( void *restrict destination, const void *restrict source, size_t count ) {
    return( paws_memcpy( destination, source, count ) );
}

void *__wrap_memset( void *restrict destination, int value, size_t count ) {
    return( paws_memset( destination, value, count ) );
}

int __wrap___clzsi2(unsigned int a) {
    return( _rv64_clzw( a ) );
}

int __wrap___clzdi2(unsigned long a) {
    return( _rv64_clz( a ) );
}

int __wrap___ctzsi2(unsigned int a) {
    return( _rv64_ctzw( a ) );
}

int __wrap___ctzdi2(unsigned long a) {
    return( _rv64_ctz( a ) );
}

int __wrap___popcountsi2(unsigned int a) {
    return( _rv64_cpopw( a ) );
}

int __wrap___popcountdi2(unsigned long a) {
    return( _rv64_cpop( a ) );
}

int __wrap___paritysi2(unsigned int a) {
    return( _rv64_cpopw( a ) & 1 );
}

int __wrap___paritydi2(unsigned long a) {
    return( _rv64_cpop( a ) & 1 );
}

int __wrap___bswapsi2(unsigned int a) {
    return( _rv64_rev8( a ) >> 32 );
}

int __wrap___bswapdi2(unsigned long a) {
    return( _rv64_rev8( a ) );
}

int __wrap___builtin_clz(unsigned int a) {
    return( _rv64_clzw( a ) );
}

int __wrap___builtin_ctz(unsigned int a) {
    return( _rv64_ctzw( a ) );
}

int __wrap___builtin_popcount(unsigned int a) {
    return( _rv64_cpopw( a ) );
}

int __wrap___builtin_parity(unsigned int a) {
    return( _rv64_cpopw( a ) & 1 );
}

int __wrap___builtin_clzl(unsigned long a) {
    return( _rv64_clz( a ) );
}

int __wrap___builtin_ctzl(unsigned long a) {
    return( _rv64_ctz( a ) );
}

int __wrap___builtin_popcountl(unsigned long a) {
    return( _rv64_cpop( a ) );
}

int __wrap___builtin_parityl(unsigned long a) {
    return( _rv64_cpop( a ) & 1 );
}

unsigned short __wrap___builtin_bswap16(unsigned short a) {
    return( _rv64_rev8( a ) >> 48 );
}

unsigned int __wrap___builtin_bswap32(unsigned int a) {
    return( _rv64_rev8( a ) >> 32 );
}

unsigned long __wrap___builtin_bswap64(unsigned long a) {
    return( _rv64_rev8( a ) );
}

#include "newlib/libm/paws_conversions.c"
float __wrap_cosf(float x) {
    return paws_cosf(x);
}
float __wrap_sinf(float x) {
    return paws_sinf(x);
}
float __wrap_tanf(float x) {
    return paws_tanf(x);
}
float __wrap_powf(float x, float y) {
    return paws_powf(x,y);
}
float __wrap_scalbnf(float x, int n ) {
    return paws_scalbnf(x,n);
}
