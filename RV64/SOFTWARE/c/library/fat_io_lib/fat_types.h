#ifndef __FAT_TYPES_H__
#define __FAT_TYPES_H__

//-------------------------------------------------------------
// System specific types
//-------------------------------------------------------------
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

#ifndef NULL
    #define NULL 0
#endif

//-------------------------------------------------------------
// Endian Macros
//-------------------------------------------------------------
// FAT is little endian so big endian systems need to swap words

// Little Endian - No swap required
#if FATFS_IS_LITTLE_ENDIAN == 1

    #define FAT_HTONS(n) (n)
    #define FAT_HTONL(n) (n)

// Big Endian - Swap required
#else

    #define FAT_HTONS(n) ((((uint16)((n) & 0xff)) << 8) | (((n) & 0xff00) >> 8))
    #define FAT_HTONL(n) (((((uint32)(n) & 0xFF)) << 24) | \
                    ((((uint32)(n) & 0xFF00)) << 8) | \
                    ((((uint32)(n) & 0xFF0000)) >> 8) | \
                    ((((uint32)(n) & 0xFF000000)) >> 24))

#endif

//-------------------------------------------------------------
// Structure Packing Compile Options
//-------------------------------------------------------------
#ifdef __GNUC__
    #define STRUCT_PACK
    #define STRUCT_PACK_BEGIN
    #define STRUCT_PACK_END
    #define STRUCT_PACKED           __attribute__ ((packed))
#else
    // Other compilers may require other methods of packing structures
    #define STRUCT_PACK
    #define STRUCT_PACK_BEGIN
    #define STRUCT_PACK_END
    #define STRUCT_PACKED
#endif

#endif
