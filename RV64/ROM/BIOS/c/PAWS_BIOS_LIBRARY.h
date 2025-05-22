static inline long _rv64_rol(long RS1, long RS2) { long rd; if (__builtin_constant_p(RS2)) __asm__ ("rori    %0, %1, %2" : "=r"(rd) : "r"(RS1), "i"(63 & -RS2)); else __asm__ ("rol     %0, %1, %2" : "=r"(rd) : "r"(RS1), "r"(RS2)); return rd; }
static inline long _rv64_rev8(long RS1) { long rd; __asm__ ("rev8     %0, %1" : "=r"(rd) : "r"(RS1)); return rd; }

// BIOS MALLOC - ALLLOCATE FROM TOP OF MEMORY DOWN
void *HEAPEND;
void *bios_malloc( int size ) {
    HEAPEND = HEAPEND - size;
    return( HEAPEND );
}

// DMA CONTROLLER
void DMASTART( const void *restrict source, void *restrict destination, unsigned int count, unsigned short mode ) {
    *DMASOURCE = (unsigned long)source;
    *DMADEST = (unsigned long)destination;
    *DMACOUNT = count;
    *DMAMODE = mode;
}

// STANDARD C FUNCTIONS ( from @sylefeb mylibc )
void *memset(void *dest, int val, size_t len) {
    *DMASET = val;
    DMASTART( (const void *restrict)DMASET, dest, len, DMA_SET_TO_M );
    return dest;
}
void *memset32( void *restrict destination, int value, size_t count ) {
    *DMASET32 = value; DMASTART( (const void *restrict)DMASET, destination, count, DMA_SET_TO_M );
    return( destination );
}
void *memcpy( void *dest, void *src, size_t len ) {
    DMASTART( src, dest, len, DMA_CPY_M_TO_M );
    return dest;
}

short strlen( char *s ) {
    short i = 0;
    while( *s ) {
        s++;
        i++;
    }
    return(i);
}

// RISC-V CSR FUNCTIONS
int CSRisa() {
    int isa;
    asm volatile (
        "csrr %0, 0x301"
        : "=r"(isa));
    return isa;
}

long CSRcycles() {
    long cycles;
    asm volatile(
        "rdcycle %0\n"
        : "=r"(cycles));
    return cycles;
}

long CSRinstructions() {
    long insns;
    asm volatile(
        "rdinstret %0\n"
        : "=r"(insns));
    return insns;
}

long CSRtime() {
    long timer;
    asm volatile(
        "rdtime %0\n"
        : "=r"(timer));
    return timer;
}

// IRQ FUNCTIONS
void IRQ_VECTOR( void *function ) {
    asm(
        "csrw mtvec, %0\n\t"
        :
        : "r"(function)
        :
    );
}

unsigned long IRQ_CAUSE() {
    long cause;
    asm volatile(
        "csrr %0, mcause\n\t"
        : "=r"(cause));
    return cause;
}

void IRQ_ON( int IRQ, int MIE ) {
    asm(
        "csrs mie, %0\n\t"
        :
        : "r"(IRQ)
        :
    );
    if( MIE ) {
        asm(
            "csrsi mstatus, 8\n\t"
        );
    }
}

void IRQ_OFF( int IRQ, int MIE ) {
    asm(
        "csrc mie, %0\n\t"
        :
        : "r"(IRQ)
        :
    );
    if( MIE ) {
        asm(
            "csrwi   mstatus,0\n\t"
        );
    }
}

void IRQ_ACK( long IRQ ) {
    asm(
        "csrc mip, %0\n\t"
        :
        : "r"(IRQ)
        :
    );
}

void IRQ_SET_TIMER( long pulses ) {
    *IRQ_TIMER_COMPARATOR = CSRtime() + pulses;
}

void IRQ_SET_TIMER_QUICK( unsigned int divider ) {
    *IRQ_TIMER_NEXT = divider;
}

// SLEEP FOR counter milliseconds
void sleep( unsigned short counter ) {
    unsigned long target = CSRtime() + ( counter * 50000 );
    while( CSRtime() < target );
}
// SLEEP FOR counter milliseconds
void sleep1khz( unsigned short counter, unsigned char timer ) {
    unsigned long target = CSRtime() + ( counter * 50000 );
    while( CSRtime() < target );
}

// I/O FUNCTIONS
// READ THE ULX3S JOYSTICK BUTTONS
unsigned short get_buttons( void ) {
    return( *BUTTONS );
}

// WAIT FOR VBLANK TO START
void await_vblank( void ) {
    while( !*VBLANK );
}
void await_vblank_finish( void ) {
    while( *VBLANK );
}

// BACKGROUND GENERATOR
void set_background( unsigned char colour, unsigned char altcolour, unsigned char backgroundmode ) {
    *BACKGROUND_COPPER_STARTSTOP = 0;
    *BACKGROUND_COLOUR = colour;
    *BACKGROUND_ALTCOLOUR = altcolour;
    *BACKGROUND_MODE = backgroundmode;
}

// GPU AND BITMAP
// The bitmap is 320 x 240 pixels (0,0) is top left
// The GPU can draw pixels, filled rectangles, lines, (filled) circles, filled triangles and has a 16 x 16 pixel blitter from user definable tiles

// INTERNAL FUNCTION - WAIT FOR THE GPU TO FINISH THE LAST COMMAND
inline void wait_gpu( void )  __attribute__((always_inline));
void wait_gpu( void ) {
    while( *GPU_STATUS );
}

// SET GPU TO RECEIVE A PIXEL BLOCK, SEND INDIVIDUAL PIXELS, STOP
void gpu_pixelblock_start( short x, short y, unsigned short w ) {
    wait_gpu();
    *GPU_X = x;
    *GPU_Y = y;
    *GPU_PARAM0 = w;
    *GPU_PARAM1 = TRANSPARENT;
    *GPU_WRITE = 10;
}

// DRAW A FILLED RECTANGLE from (x1,y1) to (x2,y2) in colour
void gpu_rectangle( unsigned char colour, short x1, short y1, short x2, short y2 ) {
    *GPU_COLOUR = colour;
    *GPU_X = x1;
    *GPU_Y = y1;
    *GPU_PARAM0 = x2;
    *GPU_PARAM1 = y2;

    wait_gpu();
    *GPU_WRITE = 3;
}

// DRAW A LINE FROM (x1,y1) to (x2,y2) in colour - uses Bresenham's Line Drawing Algorithm - single pixel width
void gpu_line( unsigned char colour, short x1, short y1, short x2, short y2 ) {
    *GPU_COLOUR = colour;
    *GPU_X = x1; *GPU_Y = y1; *GPU_PARAM0 = x2; *GPU_PARAM1 = y2; *GPU_PARAM2 = 1;
    wait_gpu();
    *GPU_WRITE = 2;
}

// DRAW A LINE FROM (x1,y1) to (x2,y2) in colour - uses Bresenham's Line Drawing Algorithm - multi-pixel width
void gpu_wideline( unsigned char colour, short x1, short y1, short x2, short y2, unsigned char width ) {
    if( width ) {
        *GPU_COLOUR = colour;
        *GPU_X = x1; *GPU_Y = y1; *GPU_PARAM0 = x2; *GPU_PARAM1 = y2; *GPU_PARAM2 = width;
        wait_gpu();
        *GPU_WRITE = 2;
    }
}

// DRAW A FILLED TRIANGLE with vertices (x1,y1) (x2,y2) (x3,y3) in colour
// VERTICES SHOULD BE PRESENTED CLOCKWISE FROM THE TOP ( minimal adjustments made to the vertices to comply )
void gpu_triangle( unsigned char colour, short x1, short y1, short x2, short y2, short x3, short y3 ) {
    *GPU_COLOUR = colour;
    *GPU_X = x1; *GPU_Y = y1; *GPU_PARAM0 = x2; *GPU_PARAM1 = y2;
    *GPU_PARAM2 = x3; *GPU_PARAM3 = y3;
    wait_gpu();
    *GPU_WRITE = 6;
}

// CLEAR THE BITMAP by drawing a transparent rectangle from (0,0) to (639,479) and resetting the bitamp scroll position
void gpu_cs( void ) {
    wait_gpu();
    gpu_rectangle( 64, 0, 0, 319, 239 );
}

// BLIT A 16 x 16 ( blit_size == 1 doubled to 32 x 32 ) TILE ( from tile 0 to 31 ) to (x1,y1) in colour
void gpu_blit( unsigned char colour, short x1, short y1, short tile, unsigned char blit_size ) {
    *GPU_COLOUR = colour;
    *GPU_X = x1;
    *GPU_Y = y1;
    *GPU_PARAM0 = tile;
    *GPU_PARAM1 = blit_size;
    *GPU_PARAM2 = 0; // NO REFLECTION

    wait_gpu();
    *GPU_WRITE = 7;
}

// BLIT AN 8 x8  ( blit_size == 1 doubled to 16 x 16, blit_size == 1 doubled to 32 x 32 ) CHARACTER ( from tile 0 to 255 ) to (x1,y1) in colour
void gpu_character_blit( unsigned char colour, short x1, short y1, unsigned short tile, unsigned char blit_size ) {
    *GPU_COLOUR = colour;
    *GPU_X = x1;
    *GPU_Y = y1;
    *GPU_PARAM0 = tile;
    *GPU_PARAM1 = blit_size;
    *GPU_PARAM2 = 0; // NO REFLECTION

    wait_gpu();
    *GPU_WRITE = 8;
}

// OUTPUT A STRING TO THE GPU
void gpu_outputstring( unsigned char colour, short x, short y, char bold, char *s, unsigned char size ) {
    while( *s ) {
        gpu_character_blit( colour, x, y, ( bold ? 256 : 0 ) + *s++, size );
        x = x + ( 8 << size );
    }
}
void gpu_outputstringcentre( unsigned char colour, short y, char bold, char *s, unsigned char size ) {
    gpu_rectangle( TRANSPARENT, 0, y, 319, y + ( 8 << size ) - 1 );
    gpu_outputstring( colour, 160 - ( ( ( 8 << size ) * strlen(s) ) >> 1) , y, bold, s, size );
}

// SET THE BLITTER TILE to the 16 x 16 pixel bitmap ( count is 32 as is halfed by dma engine)
void set_blitter_bitmap( unsigned char tile, unsigned short *bitmap ) {
    *BLIT_WRITER_TILE = tile;
    DMASTART( bitmap, (void *restrict)BLIT_WRITER_BITMAP, 32, DMA_CPY_M_TO_S );
}

// STOP PIXEL BLOCK - SENT DURING RESET TO ENSURE GPU RESETS
void gpu_pixelblock_stop( void ) {
    *PB_STOP = 3;
}

// CHARACTER MAP FUNCTIONS
// The character map is an 80 x 60 character window with a 512 character 8 x 8 pixel character generator ROM )
// NO SCROLLING, CURSOR WRAPS TO THE TOP OF THE SCREEN

// CLEAR THE CHARACTER MAP
void tpu_cs( void ) {
    memset32( ( void *)TPUBUFFER, ( 64 << 16 ), 4800 * 4 );
}
unsigned char tpu_x = 0, tpu_y = 0, tpu_background = TRANSPARENT, tpu_foreground = WHITE, tpu_attributes = TPU_NORMAL;
// POSITION THE CURSOR to (x,y) and set background and foreground colours
void tpu_set( unsigned char x, unsigned char y, unsigned char background, unsigned char foreground, unsigned char attributes ) {
    *TPU_X = tpu_x = x;
    *TPU_Y = tpu_y = y;
    *TPU_BACKGROUND = tpu_background = background;
    *TPU_FOREGROUND = tpu_foreground = foreground;
    tpu_attributes = attributes;
}

// MOVE TO NEXT TPU CELL
void tpu_next( void ) {
    tpu_x++;
    if( tpu_x == 80 ) {
        tpu_x = 0;
        tpu_y++;
        if( tpu_y == 59)
            tpu_y = 0;
    }
    *TPU_X = tpu_x; *TPU_Y = tpu_y;
}

// OUTPUT CHARACTER, STRING EQUIVALENT FOR THE TPU
void tpu_output_character( unsigned char c ) {
    TPUBUFFER[ tpu_y * 80 + tpu_x ] = ( tpu_attributes << 24 ) + ( tpu_background << 16 ) + ( tpu_foreground << 8 ) + c;
    tpu_next();
}
void tpu_outputstring( char *s ) {
    while( *s ) {
        tpu_output_character( *s );
        s++;
    }
}

void tpu_outputhex( unsigned long hex ) {
    unsigned long output = hex;
    static unsigned char *hexdigits = "0123456789ABCDEF";

    for( int i = 0; i < 16; i ++ ) {
        output = _rv64_rol( output, 4 );
        tpu_output_character( hexdigits[ output & 0xf ] );
    }
}

// SET THE TILEMAP TILE at (x,y) to tile
void set_tilemap_tile( unsigned char tm_layer, unsigned char x, unsigned char y, unsigned char tile, unsigned char action ) {
    ( tm_layer ? UTMAPBUFFER : LTMAPBUFFER )[ y * 64 + x ] = ( action << 6 ) + tile;
}

// SCROLL WRAP or CLEAR the TILEMAP by amount ( 0 - 15 ) pixels
//  action == 1 to 4 move the tilemap amount pixels LEFT, UP, RIGHT, DOWN
//  action == 5 reset base and offset
//  action == 7 reset offset
//  RETURNS 0 if no action taken other than pixel shift, action if SCROLL was actioned
unsigned char tilemap_scroll( unsigned char tm_layer, unsigned char action, unsigned char amount ) {
    *( tm_layer ? UPPER_TM_SCROLLWRAPAMOUNT : LOWER_TM_SCROLLAMOUNT ) = amount;
    *( tm_layer ? UPPER_TM_SCROLLWRAPCLEAR : LOWER_TM_SCROLLWRAPCLEAR ) = action;
    return( tm_layer ? *UPPER_TM_SCROLLWRAPCLEAR : *LOWER_TM_SCROLLWRAPCLEAR );
}

void tm_cs( unsigned char tm_layer ) {
    memset( ( void *)( tm_layer ? UTMAPBUFFER : LTMAPBUFFER ), 0, 64 * 64 * 2 );

    *( tm_layer ? UPPER_TM_X : UPPER_TM_X ) = 0;
    *( tm_layer ? UPPER_TM_Y : UPPER_TM_Y ) = 0;
    *( tm_layer ? UPPER_TM_OFFSET_X : UPPER_TM_OFFSET_X ) = 0;
    *( tm_layer ? UPPER_TM_OFFSET_Y : UPPER_TM_OFFSET_Y ) = 0;
    tilemap_scroll( tm_layer, 5, 0 );
}

// SET THE BITMAPS FOR sprite_number in sprite_layer to the 8 x 16 x 16 pixel bitmaps ( 2048 ARRGGBB pixels )
void set_sprite_bitmaps( unsigned char sprite_layer, unsigned char sprite_number, unsigned char *sprite_bitmaps ) {
    *( sprite_layer ? UPPER_SPRITE_WRITER_NUMBER : LOWER_SPRITE_WRITER_NUMBER ) = sprite_number;
    DMASTART( sprite_bitmaps, (void *restrict)(sprite_layer ? UPPER_SPRITE_WRITER_COLOUR : LOWER_SPRITE_WRITER_COLOUR), 2048, DMA_TO_IO );
}

// SET SPRITE sprite_number in sprite_layer to active status, in colour to (x,y) with bitmap number tile ( 0 - 7 ) in sprite_attributes bit 0 size == 0 16 x 16 == 1 32 x 32 pixel size, bit 1 x-mirror bit 2 y-mirror
void set_sprite( unsigned char sprite_layer, unsigned char sprite_number, unsigned char active, short x, short y, unsigned char tile, unsigned char sprite_attributes ) {
    switch( sprite_layer ) {
        case 0:
            LOWER_SPRITE_ACTIVE[sprite_number] = active;
            LOWER_SPRITE_TILE[sprite_number] = tile;
            LOWER_SPRITE_X[sprite_number] = x;
            LOWER_SPRITE_Y[sprite_number] = y;
            LOWER_SPRITE_ACTIONS[sprite_number] = sprite_attributes;
            break;

        case 1:
            UPPER_SPRITE_ACTIVE[sprite_number] = active;
            UPPER_SPRITE_TILE[sprite_number] = tile;
            UPPER_SPRITE_X[sprite_number] = x;
            UPPER_SPRITE_Y[sprite_number] = y;
            UPPER_SPRITE_ACTIONS[sprite_number] = sprite_attributes;
            break;
    }
}

// SET or GET ATTRIBUTES for sprite_number in sprite_layer
//  attribute == 0 active status ( 0 == inactive, 1 == active )
//  attribute == 1 tile number ( 0 to 7 )
//  attribute == 2 colour
//  attribute == 3 x coordinate
//  attribute == 4 y coordinate
//  attribute == 5 attributes bit 0 = size == 0 16x16 == 1 32x32. bit 1 = x-mirror bit 2 = y-mirror
void set_sprite_attribute( unsigned char sprite_layer, unsigned char sprite_number, unsigned char attribute, short value ) {
    if( sprite_layer == 0 ) {
        switch( attribute ) {
            case 0:
                LOWER_SPRITE_ACTIVE[sprite_number] = ( unsigned char) value;
                break;
            case 1:
                LOWER_SPRITE_TILE[sprite_number] = ( unsigned char) value;
                break;
            case 2:
                break;
            case 3:
                LOWER_SPRITE_X[sprite_number] = value;
                break;
            case 4:
                LOWER_SPRITE_Y[sprite_number] = value;
                break;
            case 5:
                LOWER_SPRITE_ACTIONS[sprite_number] = ( unsigned char) value;
                break;
        }
    } else {
        switch( attribute ) {
            case 0:
                UPPER_SPRITE_ACTIVE[sprite_number] = ( unsigned char) value;
                break;
            case 1:
                UPPER_SPRITE_TILE[sprite_number] = ( unsigned char) value;
                break;
            case 2:
                break;
            case 3:
                UPPER_SPRITE_X[sprite_number] = value;
                break;
            case 4:
                UPPER_SPRITE_Y[sprite_number] = value;
                break;
            case 5:
                UPPER_SPRITE_ACTIONS[sprite_number] = ( unsigned char) value;
                break;
        }
    }
}

// UPDATE A SPITE moving by x and y deltas, with optional wrap/kill and optional changing of the tile
void update_sprite( unsigned char sprite_layer, unsigned char sprite_number, unsigned char kill, short dx, short dy, unsigned char dt ) {
    static short sizes[] = { 16, 32, 64, 128, 16, 8, 4, 2 };

    short x = ( sprite_layer ? UPPER_SPRITE_X : LOWER_SPRITE_X )[ sprite_number ] + dx;
    short y = ( sprite_layer ? UPPER_SPRITE_Y : LOWER_SPRITE_Y )[ sprite_number ] + dy;
    unsigned char tile = ( sprite_layer ? UPPER_SPRITE_TILE : LOWER_SPRITE_TILE )[ sprite_number ] + dt;
    unsigned char active = ( sprite_layer ? UPPER_SPRITE_ACTIVE : LOWER_SPRITE_ACTIVE )[ sprite_number ];
    short size = sizes[ (sprite_layer ? UPPER_SPRITE_ACTIONS : LOWER_SPRITE_ACTIONS )[ sprite_number ] >> 3 ];

    if( ( ( x > 640 ) | ( x < -size ) ) ) { active = ( kill & 1 ) != 0; x = ( x > 640 ) ? -size : 640; }
    if( ( ( y > 480 ) | ( y < -size ) ) ) { active = ( kill & 2 ) != 0; y = ( y > 480 ) ? -size : 480; }

    ( sprite_layer ? UPPER_SPRITE_X : LOWER_SPRITE_X )[ sprite_number ] = x;
    ( sprite_layer ? UPPER_SPRITE_Y : LOWER_SPRITE_Y )[ sprite_number ] = y;
    ( sprite_layer ? UPPER_SPRITE_TILE : LOWER_SPRITE_TILE )[ sprite_number ] = tile;
    ( sprite_layer ? UPPER_SPRITE_ACTIVE : LOWER_SPRITE_ACTIVE )[ sprite_number ] = active;
}

// AUDIO CONTROLS
void beep( unsigned char channel_number, unsigned char waveform, unsigned char note, unsigned short duration ) {
    *AUDIO_WAVEFORM = waveform; *AUDIO_FREQUENCY = note; *AUDIO_DURATION = duration; *AUDIO_START = channel_number;
}
void volume( unsigned char left, unsigned char right ) {
    *AUDIO_L_VOLUME = left; *AUDIO_R_VOLUME = right;
}
void sample_upload( unsigned char channel_number, unsigned short length, unsigned char *samples ) {
    beep( channel_number, 0, 0, 0 );
    *AUDIO_NEW_SAMPLE = channel_number;
    if( channel_number & 1 ) { DMASTART( samples, (void *restrict)AUDIO_LEFT_SAMPLE, length, DMA_TO_IO ); }
    if( channel_number & 2 ) { DMASTART( samples, (void *restrict)AUDIO_RIGHT_SAMPLE, length, DMA_TO_IO ); }
}

// PAWS LOGO BLITTER TILE
unsigned short PAWSLOGO[] = {
    0b0000000001000000,
    0b0000100011100000,
    0b0001110011100000,
    0b0001110011100000,
    0b0001111011100100,
    0b0000111001001110,
    0b0010010000001110,
    0b0111000000001110,
    0b0111000111001100,
    0b0111001111110000,
    0b0011011111111000,
    0b0000011111111000,
    0b0000011111111100,
    0b0000111111111100,
    0b0000111100001000,
    0b0000010000000000
};

// BACKGROUND PATTERN GENERATOR
#define BKG_SOLID 0
#define BKG_5050_V 1
#define BKG_5050_H 2
#define BKG_CHKBRD_5 3
#define BKG_RAINBOW 4
#define BKG_SNOW 5
#define BKG_STATIC 6
#define BKG_CHKBRD_1 7
#define BKG_CHKBRD_2 8
#define BKG_CHKBRD_3 9
#define BKG_CHKBRD_4 10

// IRQ FLAGS
#define IRQ_SOFTWARE    ( 1 << 3 )
#define IRQ_TIMER       ( 1 << 7 )
#define IRQ_VBLANK      ( 1 << 11 )

