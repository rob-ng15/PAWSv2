algorithm multiplex_display(
    output! uint8   pix_red,
    output! uint8   pix_green,
    output! uint8   pix_blue,

    // DISPLAY ORDER AND COLOUR/BW MODE
    input   uint2   display_order,
    input   uint2   colour,

    // BACKGROUND
    input uint7 background_p,

    // TILEMAPS
    input uint7 lower_tilemap_p,
    input uint1 lower_tilemap_display,
    input uint7 upper_tilemap_p,
    input uint1 upper_tilemap_display,

    // SPRITES
    input uint7 lower_sprites_p,
    input uint1 lower_sprites_display,
    input uint7 upper_sprites_p,
    input uint1 upper_sprites_display,

    // BITMAP
    input uint7 bitmap_p,
    input uint1 bitmap_display,

    // CHARACTER MAP
    input uint7 character_map_p,
    input uint1 character_map_display,

    // TERMINAL
    input uint1 terminal_p,
    input uint1 terminal_display
) <autorun,reginputs> {
    selectlayer LAYER(
        display_order <: display_order,
        terminal_display <: terminal_display,
        terminal <: terminal_p,
        character_map_display <: character_map_display,
        character_map <: character_map_p,
        upper_sprites_display <: upper_sprites_display,
        upper_sprites <: upper_sprites_p,
        bitmap_display <: bitmap_display,
        bitmap <: bitmap_p,
        lower_sprites_display <: lower_sprites_display,
        lower_sprites <: lower_sprites_p,
        lower_tilemap_display <: lower_tilemap_display,
        lower_tilemap <: lower_tilemap_p,
        upper_tilemap_display <: upper_tilemap_display,
        upper_tilemap <: upper_tilemap_p,
        background <: background_p
    );

    uint8   grey = uninitialised;                                                                       uint8   R[] = { 153, 255, 034, 070, 138, 255, 135, 229 };
    uint8   G[] = { 076, 215, 139, 130, 043, 192, 206, 255 };                                           uint8   B[] = { 000, 000, 034, 180, 226, 203, 235, 204 };

    pix_red := 0; pix_green := 0; pix_blue := 0;

    always {
        switch( colour ) {
            case 2b00: {                                                                // PAWSv2 PALETTE, V1 + GRADIENTS
                if( LAYER.pixel[6,1] ) {
                    // ROUGHLY 10%, 20%, 30%, 40%, 50%, 60%, 70%, 80%
                    grey = ( LAYER.pixel[0,3] + 1 ) * 25;
                    switch( LAYER.pixel[3,3] ) {
                        // GRADIENTS, 0% -> 3%,
                        case 0: { pix_red = grey; pix_green = grey; pix_blue = grey; }  // GREYS
                        case 1: { pix_red = grey; }                                     // REDS
                        case 2: { pix_green = grey; }                                   // GREENS
                        case 3: { pix_blue = grey; }                                    // BLUES
                        case 4: { pix_red = grey; pix_green = grey; }                   // YELLOWS
                        case 5: { pix_red = grey; pix_blue = grey; }                    // MAGENTAS
                        case 6: { pix_green = grey; pix_blue = grey; }                  // CYANS
                        case 7: {                                                       // 8 SPECIAL COLOURS
                            pix_red = R[ LAYER.pixel[0,3] ];
                            pix_green = G[ LAYER.pixel[0,3] ];
                            pix_blue = B[ LAYER.pixel[0,3] ];
                        }
                    }
                } else {
                    pix_red = {4{LAYER.pixel[4,2]}};
                    pix_green = {4{LAYER.pixel[2,2]}};
                    pix_blue = {4{LAYER.pixel[0,2]}};
                    }
            }
            case 2b01: {                                                                // PAWSv2 RRGGGBB
                pix_red = {4{LAYER.pixel[4,2]}};
                pix_green = { LAYER.pixel[6,1] ? { |LAYER.pixel[2,2], 1b1 } : 2b00, {3{LAYER.pixel[2,2]}} };
                pix_blue = {4{LAYER.pixel[0,2]}};
            }
            case 2b10: {                                                                // PAWSv1 + 64 GREY
                if( LAYER.pixel[6,1] ) {
                    grey = { LAYER.pixel[0,6], LAYER.pixel[0,2] };
                    pix_red = grey; pix_green = grey; pix_blue = grey;
                } else {
                    pix_red = {4{LAYER.pixel[4,2]}};
                    pix_green = {4{LAYER.pixel[2,2]}};
                    pix_blue = {4{LAYER.pixel[0,2]}};
                }
            }
            case 2b11: {                                                                // PAWSv2 GREYSCALE
                grey = { LAYER.pixel[0,7], LAYER.pixel[0,1] };
                pix_red = grey; pix_green = grey; pix_blue = grey;
            }
        }
    }
}

// CHOOSE LAY TO DISPLAY
algorithm selectlayer(
    input   uint2   display_order,
    input   uint1   terminal_display,
    input   uint1   terminal,
    input   uint1   character_map_display,
    input   uint7   character_map,
    input   uint1   upper_sprites_display,
    input   uint7   upper_sprites,
    input   uint1   bitmap_display,
    input   uint7   bitmap,
    input   uint1   lower_sprites_display,
    input   uint7   lower_sprites,
    input   uint1   lower_tilemap_display,
    input   uint7   lower_tilemap,
    input   uint1   upper_tilemap_display,
    input   uint7   upper_tilemap,
    input   uint7   background,
    output! uint7   pixel
) <autorun> {
    // CONVERT TERMINAL COLOUR TO BLUE OR WHITE
    uint7   terminalcolour <: { {5{terminal}}, 2b11 };

    always {
        switch( display_order ) {
            case 0: { // BACKGROUND -> LOWER TILEMAP -> UPPER TILEMAP -> LOWER_SPRITES -> BITMAP -> UPPER_SPRITES -> CHARACTER_MAP -> TERMINAL
                pixel = ( terminal_display ) ? terminalcolour :
                            ( character_map_display ) ? character_map :
                            ( upper_sprites_display ) ? upper_sprites :
                            ( bitmap_display ) ? bitmap :
                            ( lower_sprites_display ) ? lower_sprites :
                            ( upper_tilemap_display ) ? upper_tilemap :
                            ( lower_tilemap_display ) ? lower_tilemap :
                            background;
            }
            case 1: { // BACKGROUND -> LOWER TILEMAP -> UPPER TILEMAP -> BITMAP -> LOWER_SPRITES -> UPPER_SPRITES -> CHARACTER_MAP -> TERMINAL
                pixel = ( terminal_display ) ? terminalcolour :
                        ( character_map_display ) ? character_map :
                        ( upper_sprites_display ) ? upper_sprites :
                        ( lower_sprites_display ) ? lower_sprites :
                        ( bitmap_display ) ? bitmap :
                        ( upper_tilemap_display ) ? upper_tilemap :
                        ( lower_tilemap_display ) ? lower_tilemap :
                        background;
            }
            case 2: { // BACKGROUND -> BITMAP -> LOWER TILEMAP -> UPPER TILEMAP -> LOWER_SPRITES -> UPPER_SPRITES -> CHARACTER_MAP -> TERMINAL
                pixel = ( terminal_display ) ? terminalcolour :
                        ( character_map_display ) ? character_map :
                        ( upper_sprites_display ) ? upper_sprites :
                        ( lower_sprites_display ) ? lower_sprites :
                        ( upper_tilemap_display ) ? upper_tilemap :
                        ( lower_tilemap_display ) ? lower_tilemap :
                        ( bitmap_display ) ? bitmap :
                        background;
            }
            case 3: { // BACKGROUND -> LOWER TILEMAP -> UPPER TILEMAP -> LOWER_SPRITES -> UPPER_SPRITES -> BITMAP -> CHARACTER_MAP -> TERMINAL
                pixel = ( terminal_display ) ? terminalcolour :
                        ( character_map_display ) ? character_map :
                        ( bitmap_display ) ? bitmap :
                        ( upper_sprites_display ) ? upper_sprites :
                        ( lower_sprites_display ) ? lower_sprites :
                        ( upper_tilemap_display ) ? upper_tilemap :
                        ( lower_tilemap_display ) ? lower_tilemap :
                        background;
            }
        }
    }
}
