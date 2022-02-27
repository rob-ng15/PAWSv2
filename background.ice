// STRUCTURE OF A COPPER PROGRAM ENTRY
bitfield CU{
    uint3   command,
    uint3   flag,
    uint1   valueflag,
    uint10  value,
    uint4   mode,
    uint7   colour_alt,
    uint7   colour
}

algorithm background_writer(
    input   uint10  pix_x,
    input   uint9   pix_y,
    input   uint1   pix_active,
    input   uint1   pix_vblank,

    input   uint7   backgroundcolour,
    input   uint7   backgroundcolour_alt,
    input   uint4   backgroundcolour_mode,
    input   uint2   background_update,

    input   uint1   copper_program,
    input   uint7   copper_address,
    input   uint3   copper_command,
    input   uint3   copper_condition,
    input   uint11  copper_coordinate,
    input   uint4   copper_mode,
    input   uint7   copper_alt,
    input   uint7   copper_colour,

    output  uint7   BACKGROUNDcolour,
    output  uint7   BACKGROUNDalt,
    output  uint4   BACKGROUNDmode,

    simple_dualport_bram_port1 copper
) <autorun,reginputs> {
    copper.addr1 := copper_address;
    copper.wdata1 := { copper_command[0,3], copper_condition[0,3], copper_coordinate[0,11], copper_mode[0,4], copper_alt[0,7], copper_colour[0,7] };
    copper.wenable1 := copper_program;

    always_after {
        switch( background_update ) {
            case 2b00: {}                                           // CHANGE A PROGRAM LINE IN THE COPPER MEMORY
            case 2b01: { BACKGROUNDcolour = backgroundcolour; }     // UPDATE THE BACKGROUND FROM RISC-V
            case 2b10: { BACKGROUNDalt = backgroundcolour_alt; }
            case 2b11: { BACKGROUNDmode = backgroundcolour_mode; }
        }
    }
}
algorithm background_copper(
    input   uint10  pix_x,
    input   uint9   pix_y,
    input   uint1   pix_active,
    input   uint1   pix_vblank,

    input   uint7   backgroundcolour,
    input   uint7   backgroundcolour_alt,
    input   uint4   backgroundcolour_mode,

    input   uint1   copper_status,
    input   uint10  copper_cpu_input,

    output! uint7   BACKGROUNDcolour,
    output! uint7   BACKGROUNDalt,
    output! uint4   BACKGROUNDmode,

    simple_dualport_bram_port0 copper
) <autorun,reginputs> {
    uint1   copper_execute = uninitialised;         uint1   copper_branch = uninitialised;
    uint7   PC = 0;                                 uint7   PCplus1 <:: PC + 1;
    uint10  copper_variable = 0;

    // COPPER PROGRAM ENTRY
    uint10  value <:: CU(copper.rdata0).valueflag ? copper_cpu_input : CU(copper.rdata0).value;
    uint10  negvalue <:: -value;

    // COPPER FLAGS
    copper.addr0 := PC; copper_execute := 0; copper_branch := 0;

    always {
        // UPDATE THE BACKGROUND GENERATOR FROM THE COPPER
        if( copper_status ) {
            switch( CU(copper.rdata0).command ) {
                case 3b000: {
                    // JUMP ON CONDITION
                    switch( CU(copper.rdata0).flag ) {
                        default: { copper_branch = 1; }
                        case 3b001: { copper_branch = ( pix_vblank == value[0,1] ); }
                        case 3b010: { copper_branch = ( pix_active == value[0,1] ); }
                        case 3b011: { copper_branch = ( pix_y < value ); }
                        case 3b100: { copper_branch = ( pix_x < value ); }
                        case 3b101: { copper_branch = ( copper_variable < value ); }
                    }
                    PC = copper_branch ? CU(copper.rdata0).colour : PCplus1;
                }
                default: {
                    switch( CU(copper.rdata0).command ) {
                        case 3b001: { copper_execute = pix_vblank; }
                        case 3b010: { copper_execute = ~pix_active; }
                        case 3b011: { copper_execute = ( pix_y == value ); }
                        case 3b100: { copper_execute = ( pix_x == value ); }
                        case 3b101: { copper_execute = ( copper_variable == ( value[0,1] ? pix_x : pix_y ) ); }
                        case 3b110: {
                            copper_variable = CU(copper.rdata0).flag[0,1] ? value : copper_variable + ( CU(copper.rdata0).flag[2,1] ? negvalue : value );
                            copper_branch = 1;
                        }
                        default: {
                            if( CU(copper.rdata0).flag[0,1] ) { BACKGROUNDcolour = copper_variable; }
                            if( CU(copper.rdata0).flag[1,1] ) { BACKGROUNDalt = copper_variable; }
                            if( CU(copper.rdata0).flag[2,1] ) { BACKGROUNDmode = copper_variable;}
                            copper_branch = 1;
                        }
                    }
                    if( copper_execute ) {
                        if( CU(copper.rdata0).flag[0,1] ) { BACKGROUNDcolour = CU(copper.rdata0).colour; }
                        if( CU(copper.rdata0).flag[1,1] ) { BACKGROUNDalt = CU(copper.rdata0).colour_alt; }
                        if( CU(copper.rdata0).flag[2,1] ) { BACKGROUNDmode = CU(copper.rdata0).mode; }
                        copper_branch = 1;
                    }
                    if( copper_branch ) { PC = PCplus1; }
                }
            }
        } else{
            BACKGROUNDcolour = backgroundcolour; BACKGROUNDalt = backgroundcolour_alt; BACKGROUNDmode = backgroundcolour_mode;
            PC = 0;
        }
    }
}
algorithm background_display(
    input   uint10  pix_x,
    input   uint9   pix_y,
    input   uint1   pix_active,
    input   uint1   pix_vblank,
    output! uint7   pixel,

    input   uint2  staticGenerator,

    input   uint7   b_colour,
    input   uint7   b_alt,
    input   uint4   b_mode
) <autorun,reginputs> {
    // TRUE FOR COLOUR, FALSE FOR ALT
    pattern PATTERN( pix_x <: pix_x, pix_y <: pix_y, b_mode <: b_mode );                                rainbow RAINBOW( y <: pix_y[6,3] );

    always {
        // RENDER - SELECT ACTUAL COLOUR
        switch( PATTERN.condition ) {
            case 0: { pixel = b_alt; }                                              // EVERYTHING ELSE
            case 1: { pixel = b_colour; }
            case 2: { pixel = RAINBOW.colour; }                                     // RAINBOW
            case 3: { pixel = {3{staticGenerator}}; }                               // STATIC
        }
    }
}

algorithm rainbow(
    input   uint3   y,
    output  uint7   colour
) <autorun> {
    uint7   rainbow[] = { 7b100000, 7b110000, 7b111000, 7b111100, 7b001100, 7b000011, 7b010010, 7b011011 };

    colour := rainbow[ y ];
}

algorithm starfield(
    input   uint10  pix_x,
    input   uint9   pix_y,
    output  uint1   star
) <autorun> {
    // Variables for SNOW (from @sylefeb)
    int10   dotpos = 0;                             int2    speed = 0;                                  int2    inv_speed = 0;
    int12   rand_x = 0;                             int12   new_rand_x <:: rand_x * 31421 + 6927;       int32   frame = 0;

    // Increment frame number for the snow/star field
    frame := frame + ( ( pix_x == 639 ) & ( pix_y == 479 ) );

    rand_x := ( ~|pix_x )  ? 1 : new_rand_x;        speed  := rand_x[10,2];                             dotpos := ( frame >> speed ) + rand_x;
    star := ( pix_y == dotpos );
}

algorithm pattern(
    input   uint10  pix_x,
    input   uint9   pix_y,
    input   uint4   b_mode,
    output! uint2   condition
) <autorun> {
    uint1   tophalf <: ( pix_y < 240 );             uint1   lefthalf <: ( pix_x < 320 );                uint2   checkmode <: b_mode - 7;
    starfield STARS( pix_x <: pix_x, pix_y <: pix_y );

    always {
        // SELECT COLOUR OR ALT
        switch( b_mode ) {
            case 0: { condition = 1; }                                              // SOLID
            case 1: { condition = tophalf; }                                        // 50:50 HORIZONTAL SPLIT
            case 2: { condition = ( lefthalf ); }                                   // 50:50 VERTICAL SPLIT
            case 3: { condition = ( lefthalf ^ tophalf ); }                         // QUARTERS
            case 5: { condition  = STARS.star; }                                    // SNOW (from @sylefeb)
            case 11: { condition = ( pix_x[0,1] | pix_y[0,1] ); }                   // CROSSHATCH
            case 12: { condition = ( pix_x[0,2] == pix_y[0,2] ); }                  // LSLOPE
            case 13: { condition = ( pix_x[0,2] == ~pix_y[0,2] ); }                 // RSLOPE
            case 14: { condition = pix_x[0,1]; }                                    // VSTRIPES
            case 15: { condition = pix_y[0,1]; }                                    // HSTRIPES
            case 4: { condition = 2; } case 6: { condition = 3; }                   // STATIC AND RAINBOW (placeholder, done in main)
            default: { condition = ( pix_x[checkmode,1] ^ pix_y[checkmode,1] ); }   // CHECKERBOARDS (7,8,9,10)
        }
    }
}
