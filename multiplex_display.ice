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

    // Grey calculations
    uint8   grey <: GREY[ LAYER.pixel[0,3] ];                                                           // PAWSv2
    uint8   greyv1 <: { LAYER.pixel[0,6], LAYER.pixel[0,2] };                                           // PAWSv1 Grey
    uint8   greyv2 <: { LAYER.pixel[0,7], LAYER.pixel[0,1] };                                           // PAWSv2 Grey

    // SPECIAL COLOUR PALETTES
    uint8   R[] = { 153, 255, 034, 070, 138, 255, 135, 229 };                                           uint8   G[] = { 076, 215, 139, 130, 043, 192, 206, 255 };
    uint8   B[] = { 000, 000, 034, 180, 226, 203, 235, 204 };                                           uint3   lookup <: LAYER.pixel[0,3];

    // GRADIENTS
    uint8   GREY[] = { 26, 51, 77, 102, 128, 153, 179, 204 };                                           // GRADIENTS AT 10%, 20% ... 80%
    uint8   GREEN[] = { 0, 36, 73, 109, 146, 182, 219, 255 };                                           // GGG -> 255

    // DEFAULT to PAWSv1 COLOUR EXPANSIONS
    pix_red := {4{LAYER.pixel[4,2]}};               pix_green := {4{LAYER.pixel[2,2]}};                 pix_blue := {4{LAYER.pixel[0,2]}};

    always {
        switch( colour ) {
            case 2b00: {                                                                                // PAWSv2 PALETTE, V1 + GRADIENTS
                if( LAYER.pixel[6,1] ) {
                    pix_red = 0; pix_green = 0; pix_blue = 0;
                    switch( LAYER.pixel[3,3] ) {
                        case 0: { pix_red = grey; pix_green = grey; pix_blue = grey; }                  // GREYS
                        case 1: { pix_red = grey; }                                                     // REDS
                        case 2: { pix_green = grey; }                                                   // GREENS
                        case 3: { pix_blue = grey; }                                                    // BLUES
                        case 4: { pix_red = grey; pix_green = grey; }                                   // YELLOWS
                        case 5: { pix_red = grey; pix_blue = grey; }                                    // MAGENTAS
                        case 6: { pix_green = grey; pix_blue = grey; }                                  // CYANS
                        case 7: {                                                                       // 8 SPECIAL COLOURS
                            pix_red = R[ lookup ];
                            pix_green = G[ lookup ];
                            pix_blue = B[ lookup ];
                        }
                    }
                }
            }
            case 2b01: {                                                                                // PAWSv2 RRGGGBB
                pix_green = GREEN[ { LAYER.pixel[6,1], LAYER.pixel[2,2] } ];
            }
            case 2b10: {                                                                                // PAWSv1 + 64 GREY
                if( LAYER.pixel[6,1] ) {
                    pix_red = greyv1; pix_green = greyv1; pix_blue = greyv1;
                }
            }
            case 2b11: {                                                                                // PAWSv2 GREYSCALE
                pix_red = greyv2; pix_green = greyv2; pix_blue = greyv2;
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
    uint7   terminalcolour <: { 1b0, {4{terminal}}, 2b11 };

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
