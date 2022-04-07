algorithm tilemap(
    simple_dualport_bram_port0 tiles16x16,
    simple_dualport_bram_port0 tiles,

    input   uint10  pix_x,
    input   uint9   pix_y,
    input   uint1   pix_active,
    input   uint1   pix_vblank,
    output! uint7   pixel,
    output! uint1   tilemap_display,

    // For scrolling/wrapping
    input   int5    tm_offset_x,
    input   int5    tm_offset_y
) <autorun,reginputs> {
    int11   xscreen <: pix_active ? pix_x + 1 : 0;
    int11   yscreen <: pix_vblank ? 0 : pix_y;

    // Character position on the screen x 0-41, y 0-31 * 42 ( used to fetch the tile number and action for that cell )
    // Adjust for the offsets, effective 0 point margin is ( 1,1 ) to ( 40,30 ) with a 1 tile border
    uint6   xtmpos <: ( {{6{tm_offset_x[4,1]}}, tm_offset_x } + xscreen + 11d16 + pix_active ) >> 4;
    uint6   ytmpos <: ( {{6{tm_offset_y[4,1]}}, tm_offset_y } + ( 11d16 + pix_y ) ) >> 4;

    // Derive the x and y coordinate within the current 16x16 tilemap block x 0-15, y 0-15, adjusted for offsets
    uint4   xintm <: { 1b0, xscreen[0,4] } + tm_offset_x;                                               uint4   revx <: ~xintm;
    uint4   yintm <: { 1b0, yscreen[0,4] } + tm_offset_y;                                               uint4   revy <: ~yintm;

    // Apply rotation/reflection
    uint1   action00 <: ( ~|tmentry( tiles.rdata0 ).action );                                           uint1   action01 <: ( tmentry( tiles.rdata0 ).action == 2b01 );
    uint1   action10 <: ( tmentry( tiles.rdata0 ).action == 2b10 );
    uint4   xpixel <: tmentry( tiles.rdata0 ).rflag ? action00 ? xintm : action01 ? yintm : action10 ? revx : revy : tiles.rdata0[6,1] ? revx : xintm;
    uint4   ypixel <: tmentry( tiles.rdata0 ).rflag ? action00 ? yintm : action01 ? revx : action10 ? revy : xintm : tiles.rdata0[7,1] ? revy : yintm;

    // Set up reading of the tilemap
    tiles.addr0 := xtmpos + ytmpos * 42;

    // Setup the reading and writing of the tiles16x16 using rotation/reflection
    tiles16x16.addr0 := { tmentry( tiles.rdata0 ).tilenumber, ypixel, xpixel };

    always_after {
        tilemap_display = pix_active & ( tiles16x16.rdata0 != 64 );
        pixel = tiles16x16.rdata0;
    }
}

// CALCULATE NEW OFFSETS AND IF AT MIN/MAX
algorithm   calcoffset(
    input   int5    offset,
    input   uint4   adjust,
    output  uint1   MIN,
    output  int5    PREV,
    output  uint1   MAX,
    output  int5    NEXT
) <autorun,reginputs> {
    int6    offsetPLUS <:: { offset[4,1], offset } + adjust;
    int6    offsetMINUS <:: { offset[4,1], offset } - adjust;

    always_after {
        MIN = ( offsetMINUS < -15 );                   PREV = offsetMINUS + ( MIN ? 16 : 0 );
        MAX = ( offsetPLUS > 15 );                     NEXT= offsetPLUS - ( MAX ? 16 : 0 );
    }
}
algorithm tmxaddresses(
    input   uint6   x_cursor,
    input   uint1   tm_dodir,
    output  uint6   xNEXT,
    output  uint6   xPREV,
    output  uint11  xSAVED
) <autorun,reginputs> {
    always_after {
        xNEXT = x_cursor + 1;
        xPREV = x_cursor - 1;
        xSAVED = x_cursor + ( tm_dodir ? 1302 : 0 );
    }
}
algorithm tmyaddresses(
    input   uint11  y_cursor_addr,
    input   uint1   tm_dodir,
    output  uint11  yNEXT,
    output  uint11  yPREV,
    output  uint11  ySAVED
) <autorun,reginputs> {
    always_after {
        yNEXT = y_cursor_addr + 42;
        yPREV = y_cursor_addr - 42;
        ySAVED = y_cursor_addr + ( tm_dodir ? 41 : 0 );
    }
}
algorithm tile_map_writer(
    simple_dualport_bram_port1 tiles,

    // Set TM at x, y, character with foreground, background and rotation
    input   uint6   tm_x,
    input   uint6   tm_y,
    input   uint6   tm_character,
    input   uint3   tm_actions,
    input   uint1   tm_write,

    // For scrolling/wrapping
    output  int5    tm_offset_x(0),
    output  int5    tm_offset_y(0),

    input   uint4   tm_scrollwrap,
    input   uint4   tm_adjust,
    output  uint4   tm_lastaction,
    output  uint3   tm_active
) <autorun,reginputs> {
    // COPY OF TILEMAP FOR SCROLLING
    simple_dualport_bram uint9 tiles_copy[1344] = uninitialized;

    // OFFSET CALCULATIONS
    calcoffset TMOX( offset <: tm_offset_x, adjust <: tm_adjust );       calcoffset TMOY( offset <: tm_offset_y, adjust <: tm_adjust );

    // Scroller/Wrapper FLAGS
    uint1   tm_scroll = uninitialized;              uint1   tm_dodir = uninitialized;

    // CURSORS AND ADDRESSES FOR SCROLLING WRAPPING
    uint6   x_cursor = uninitialized;               tmxaddresses TMX( x_cursor <: x_cursor, tm_dodir <: tm_dodir );
    uint11  y_cursor_addr = uninitialized;          tmyaddresses TMY( y_cursor_addr <: y_cursor_addr, tm_dodir <: tm_dodir );

    uint11  temp_1 = uninitialized;
    uint11  temp_2 <:: x_cursor + y_cursor_addr;    uint11  temp_2NEXT1 <:: temp_2 + ( tm_dodir ? 1 : (-1) );
                                                    uint11  temp_2NEXT42 <:: temp_2 + ( tm_dodir ? 42 : (-42) );
    uint11  write_address <:: tm_x + tm_y * 42;

    // STORAGE FOR SAVED CHARACTER WHEN WRAPPING
    uint9   new_tile = uninitialized;

    // CLEARSCROLL address
    uint11  tmcsaddr = uninitialized;               uint11  tmcsNEXT <:: tmcsaddr + 1;

    // TILEMAP WRITE FLAGS
    tiles.wenable1 := 1; tiles_copy.wenable1 := 1;

    always_after {
        if( tm_write ) {
            // Write character to the tilemap
            tiles.addr1 = write_address; tiles.wdata1 = { tm_actions, tm_character };
            tiles_copy.addr1 = write_address; tiles_copy.wdata1 = { tm_actions, tm_character };
        }

        switch( tm_scrollwrap ) {                                                                                           // ACT AS PER tm_scrollwrap
            case 0: {}                                                                                                      // NO ACTION
            case 9: { tm_active = 4; tm_lastaction = 9; }                                                                   // CLEAR
            default: {                                                                                                      // SCROLL / WRAP
                tm_scroll = ( tm_scrollwrap < 5 );
                switch( ( tm_scrollwrap - 1 ) & 3 ) {
                    case 0: { if( TMOX.MAX ) { tm_dodir = 1; tm_active = 1; } tm_offset_x = TMOX.NEXT; }                    // LEFT
                    case 1: { if( TMOY.MAX ) { tm_dodir = 1; tm_active = 2; } tm_offset_y = TMOY.NEXT; }                    // UP
                    case 2: { if( TMOX.MIN ) { tm_dodir = 0; tm_active = 1; } tm_offset_x = TMOX.PREV; }                    // RIGHT
                    case 3: { if( TMOY.MIN ) { tm_dodir = 0; tm_active = 2; } tm_offset_y = TMOY.PREV; }                    // DOWN
                }
                tm_lastaction = ( |tm_active ) ? tm_scrollwrap : 0;
            }
        }
    }

    while(1) {
        if( |tm_active ) {
            onehot( tm_active ) {
                default: {                                                                                                   // SCROLL/WRAP LEFT/RIGHT UP/DOWN
                    while( tm_active[0,1] ? ( y_cursor_addr != 1344 ) : ( x_cursor != 42 ) ) {                                  // REPEAT UNTIL AT BOTTOM/RIGHT OF THE SCREEN
                        onehot( tm_active ) {
                            case 0: { x_cursor = tm_dodir ? 0 : 41; }                                                                   // SAVE CHARACTER AT START/END OF LINE FOR WRAPPING
                            default: {  y_cursor_addr = tm_dodir ? 0 : 1302; }                                                          // SAVE CHARACTER AT TOP/BOTTOM OF THE SCREEN FOR WRAPPING
                        }
                        temp_1 = y_cursor_addr + x_cursor;
                        tiles_copy.addr0 = temp_1;
                        ++:
                        new_tile = tm_scroll ? 0 : tiles_copy.rdata0;
                        while( tm_active[0,1] ? tm_dodir ? ( x_cursor != 42 ) : ( |x_cursor ) :                                         // START AT THE LEFT/RIGHT OF THE LINE
                                                tm_dodir ? ( y_cursor_addr != 1302 ) : ( |y_cursor_addr ) ) {                           // START AT TOP/BOTTOM OF THE SCREEN
                            onehot( tm_active ) {
                                case 0: { temp_1 = temp_2NEXT1; }                                                                       // SAVE THE ADJACENT CHARACTER IN LINE
                                default: { temp_1 = temp_2NEXT42; }                                                                     // SAVE THE ADJACENT CHARACTER IN COLUMN
                            }
                            tiles_copy.addr0 = temp_1;
                            ++:
                            tiles.addr1 = temp_2; tiles_copy.addr1 = temp_2;                                                            // COPY INTO NEW LOCATION
                            tiles.wdata1 = tiles_copy.rdata0; tiles_copy.wdata1 = tiles_copy.rdata0;
                            onehot( tm_active ) {
                                case 0: { x_cursor = tm_dodir ? TMX.xNEXT : TMX.xPREV;}                                                 // MOVE TO NEXT CHARACTER ON THE LINE
                                default: { y_cursor_addr = tm_dodir ? TMY.yNEXT : TMY.yPREV; }                                          // MOVE TO THE NEXT CHARACTER IN THE COLUMN
                            }
                        }
                        onehot( tm_active ) {
                            case 0: { tiles.addr1 = TMY.ySAVED; tiles_copy.addr1 = TMY.ySAVED; y_cursor_addr = TMY.yNEXT; }             // SET SAVED ADDRESS, MOVE TO NEXT LINE
                            default: { tiles.addr1 = TMX.xSAVED; tiles_copy.addr1 = TMX.xSAVED;  x_cursor = TMX.xNEXT; }                // SET SAVED ADDRESS, MOVE TO NEXT COLUMN
                        }
                        tiles.wdata1 = new_tile; tiles_copy.wdata1 = new_tile;                                                          // WRITE BLANK OR WRAPPED CHARACTER
                    }
                }
                case 2: {                                                                                                   // CLEAR
                    tiles.wdata1 = 0; tiles_copy.wdata1 = 0;
                    while( tmcsaddr != 1344 ) {
                        tiles.addr1 = tmcsaddr; tiles_copy.addr1 = tmcsaddr; tmcsaddr = tmcsNEXT;
                    }
                    tm_offset_x = 0; tm_offset_y = 0;
                }
            }
            tm_active = 0;
        } else {
            tmcsaddr = 0; y_cursor_addr = 0; x_cursor = 0;                                                                  // RESET SCROLL/WRAP
        }
    }
}
