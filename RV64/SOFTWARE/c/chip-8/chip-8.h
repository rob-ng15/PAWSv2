#include <PAWSlibrary.h>
#include <PAWSintrinsics.h>

enum MODES { CHIP8, CHIP48, SCHIP, XOCHIP };                                                                                                   // MODELS SUPPORTED
enum CRASHES { NONE, INVALIDINSN, WRONGMODE, INVALIDFILE, STACKOFLOW, STACKUFLOW };                                             // CRASH REASONS
enum STATES { PAUSED, RUNNING };
enum SPEEDS { MAX, VIP, CYCLE };

// MACRO TO SAFELY ADD TO AN ADDRESS WITH 12-bit MASKING FOR PC, 12/16 BIT MASKING FOR I
#define PADDRESS(x,y) ( ( x + y ) & 0xfff )
#define IADDRESS(x,y) ( ( x + y ) & ( ( machine.MODE == XOCHIP ) ? 0xffff : 0xfff ) )
#define ADDRESS(x,y) ( ( x + y ) )

// MACRO TO FETCH TWO BYTES AND COMBINE TO ONE HALF-WORD
#define FETCH(x) ( machine.MEMORY[ ADDRESS(x,0) ] << 8 ) | machine.MEMORY[ ADDRESS(x,1) ]

// NUMBER OF BYTES IN THE DISPLAY BUFFER UNSIGNED LONG * 2 PER ROW * 64 ROWS * 2 BUFFERS
#define PLANESIZE 8 * 2 * 64
#define DISPLAYSIZE 2 * PLANESIZE

// MEMORY SIZE, 4096 OR 65536  XO-CHIP SUPPORTED
#define MEMSIZE ( machine.MODE == XOCHIP ? 65536 : 4096 )

// STRUCTURE TO HOLD THE MCHINE STATE
struct C8 {                                                                                                                     // STRUCTURE FOR THE CHIP 8 CPU AND MEMORY
    enum MODES MODE;                                                                                                            // OPERATING MODE ( CHIP8 SCHIP XOCHIP )
    uint16_t PC;                                                                                                                // PC
    uint16_t I;                                                                                                                 // INDEX REGISTER
    uint8_t V[16], FLAGS[16];                                                                                                   // REGISTERS AND STORAGE SPACE FOR COPIES
    uint8_t MEMORY[ 65536 ];                                                                                                     // MACHINE RAM
    uint16_t KEYS;                                                                                                              // KEY PRESSEED BITMAP
    uint64_t DISPLAY[ 2 ][ 64 ][ 2 ];                                                                                           // DISPLAY 128(bits) x 64(lines) x 2(planes)
    uint8_t HIRES;                                                                                                              // HI RESOLUTION SWITCH
    uint8_t PLANES;                                                                                                             // BITMASK OF PLANES TO DRAW ON
    uint8_t timer;                                                                                                              // 60Hz timer
    uint8_t audio_timer;                                                                                                        // 60Hz audio timer

    int STACKTOP;                                                                                                               // POINTER TO TOP OF STACK - -1 == NOTHING
    uint16_t STACK[ 16 ];                                                                                                       // 16 STACK ENTRIES

    int quit;                                                                                                                   // QUIT
    int restart;                                                                                                                // RESTART FROM 0x200 REQUESTED
    int running;                                                                                                                // MACHINE IS RUNNING
    int loading;                                                                                                                // LOAD REQUESTED
    int limit;                                                                                                                  // LIMIT TO 1000 ISNS PER SECOND
    int debug;                                                                                                                  // OUTPUT DEBUG INFORMATION
    enum CRASHES crashed;                                                                                                       // CRASHED, ENCODES REASON
    uint16_t lastPC;                                                                                                            // PC OF LAST INSTRUCTION
    uint16_t lastinstruction;                                                                                                   // LAST INSTRUCTION
};

// SIMPLE ACCESS TO THE FLAG REGISTER
#define VF machine.V[15]

