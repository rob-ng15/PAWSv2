// ALU - ALU for immediate-register operations and register-register operations

// CALCULATES SLL SLLI SRL SRLI SRA SRAI + ROL ROR RORI
algorithm alushift(
    input   uint32  sourceReg1,
    input   uint5   shiftcount,
    input   uint1   reverse,
    output  uint32  SLL,
    output  uint32  SRL,
    output  uint32  SRA,
    output  uint32  ROTATE
) <autorun,reginputs> {
    uint32  altSLL <:: sourceReg1 << shiftother;
    uint32  altSRL <:: sourceReg1 >> shiftother;

    uint6   shiftother <:: 32 - shiftcount;
    always_after {
        SLL = sourceReg1 << shiftcount;
        SRL = sourceReg1 >> shiftcount;
        SRA = __signed(sourceReg1) >>> shiftcount;
        ROTATE = reverse ? ( altSLL | SRL ) : ( SLL | altSRL );
    }
}
// CALCULATES BCLR BCLRI BEXT BEXTI BIN BINI BSET BSETI
algorithm alubits(
    input   uint32  sourceReg1,
    input   uint5   shiftcount,
    output  uint32  CLR,
    output  uint32  INV,
    output  uint32  SET,
    output  uint1   EXT
) <autorun,reginputs> {
    uint32  mask <:: ( 1 << shiftcount );

    always_after {
        CLR = sourceReg1 & ~mask;                      INV = sourceReg1 ^ mask;
        SET = sourceReg1 | mask;                       EXT = sourceReg1[ shiftcount, 1 ];
    }
}
// CALCULATES ADD ADDI SUB
algorithm aluaddsub(
    input   uint1   dosub,
    input   int32   sourceReg1,
    input   int32   operand2,
    input   int32   negoperand2,
    output  int32   AS
) <autorun,reginputs> {
    always_after {
        AS = sourceReg1 + ( dosub ? negoperand2 : operand2 );
    }
}
// CALCULATES AND/ANDN OR/ORN XOR/XNOR
algorithm alulogic(
    input   uint32  sourceReg1,
    input   uint32  operand2,
    input   uint1   doinv,
    output  uint32  AND,
    output  uint32  OR,
    output  uint32  XOR
) <autorun,reginputs> {
    uint32  operand <:: doinv ? ~operand2 : operand2;

    always_after {
        AND = sourceReg1 & operand;                    OR = sourceReg1 | operand;
        XOR = sourceReg1 ^ operand;
    }
}
// CALCULATES SH1ADD, SH2ADD, SH3ADD
algorithm alushxadd(
    input   uint2   function3,
    input   uint32  sourceReg1,
    input   uint32  sourceReg2,
    output  uint32  result
) <autorun,reginputs> {
    always_after {
        result = sourceReg2 + ( sourceReg1 << function3 );
    }
}
// CALCULATES CLZ CTZ CPOP
algorithm alucount(
    input   uint2   shiftcount,
    input   uint32  sourceReg1,
    output  uint6   result
) <autorun,reginputs> {
    uint1   zero <:: ~|sourceReg1;

    always_after {
        if( zero ) {
            result = { ~shiftcount[1,1], 5b00000 };
        } else {
            switch( shiftcount ) {
                case 2b00: { ( result ) = clz_silice_32( sourceReg1 ); }
                case 2b01: { ( result ) = ctz_silice_32( sourceReg1 ); }
                default: { ( result ) = cpop_silice_32( sourceReg1 ); }
            }
        }
    }
}
// CALCULATES MAX MAXU MIN MINU
algorithm aluminmax(
    input   uint2   function3,
    input   uint1   signedcompare,
    input   uint1   unsignedcompare,
    input   uint32  sourceReg1,
    input   uint32  sourceReg2,
    output  uint32  result
) <autorun,reginputs> {
    always_after {
        result = function3[1,1] ^ ( function3[0,1] ? unsignedcompare : signedcompare ) ? sourceReg1 : sourceReg2;
    }
}
// CALCULATES SEXT.B SEXT.H ZEXT.H
algorithm aluextend(
    input   uint3   shiftcount,
    input   uint32  sourceReg1,
    output  uint32  result
)  <autorun,reginputs> {
    always_after {
        result = shiftcount[2,1] ? shiftcount[0,1] ? { {16{sourceReg1[15,1]}}, sourceReg1[0,16] } : { {24{sourceReg1[7,1]}}, sourceReg1[0,8] } : sourceReg1[0,16];
    }
}
// CALCULATES ORC.B REV8
algorithm aluorcrev(
    input   uint32  sourceReg1,
    output  uint32  ORC,
    output  uint32  REV8
) <autorun,reginputs> {
    always_after {
        ORC = { {8{ |sourceReg1[24,8] }}, {8{ |sourceReg1[16,8] }}, {8{ |sourceReg1[8,8] }}, {8{ |sourceReg1[0,8] }} };
        REV8 = { sourceReg1[0,8], sourceReg1[8,8], sourceReg1[16,8], sourceReg1[24,8] };
    }
}

// DECODE ALU INSTRUCTIONS
// DECODE ALU INSTRUCTIONS
algorithm aludecode(
    input   uint1   regimm,
    input   uint7   function7,
    input   uint3   function3,
    input   uint5   rs2,

    output  uint1   doalt,
    output  uint1   dosra,
    output  uint1   dorotate,
    output  uint1   dobclrext,
    output  uint1   dobinv,
    output  uint1   dobset,
    output  uint1   doshxadd,
    output  uint1   docount,
    output  uint1   dominmax,
    output  uint1   dosignx,
    output  uint1   dozerox,
    output  uint1   doorc,
    output  uint1   dorev
) <autorun,reginputs> {
    uint1   f7010000 <:: ( function7 == 7b0100000 );
    uint1   f70110000 <:: ( function7 == 7b0110000 );
    uint1   f70110100 <:: ( function7 == 7b0110100 );
    uint1   f70010100 <:: ( function7 == 7b0010100 );

    always_after {
        doalt = regimm & f7010000;                                          // ADD/SUB AND/ANDN OR/ORN XOR/XNOR ( register - register only )
        dosra = f7010000;                                                   // SRL SRA
        dorotate = f70110000;                                               // ROL ROR RORI
        dobclrext = ( function7 == 7b0100100 );                             // BCLR BCLRI BEXT BEXTI
        dobinv = f70110100;                                                 // BINV BINVI
        dobset = f70010100;                                                 // BSET BSETI
        doshxadd = regimm & ( function7 == 7b0010000 );                     // SH1ADD SH2ADD SH3ADD ( register - register only )
        docount = ~regimm & f70110000 & ~rs2[2,1];                          // CLZ CPOP CTZ ( immediate only )
        dominmax = regimm & function3[2,1] & ( function7 == 7b0000101 );    // MAX MAXU MIN MINU ( register - register only )
        dosignx = ~regimm & f70110000 & rs2[2,1];                           // SEXT.B SEXT.H
        dozerox = regimm & ( function7 == 7b0000100 );                      // ZEXT.H
        doorc = ~regimm & f70010100;                                        // ORC.B
        dorev = ~regimm & f70110100;                                        // REV8
    }
}

algorithm alu(
    input   uint5   opCode,
    input   uint3   function3,
    input   uint7   function7,
    input   uint5   rs1,
    input   uint5   rs2,
    input   int32   sourceReg1,
    input   int32   sourceReg2,
    input   int32   negSourceReg2,
    input   int32   immediateValue,
    input   uint1   LT,                                                             // SIGNED COMPARE sourceReg1 < operand2
    input   uint1   LTU,                                                            // UNSIGNED COMPARE sourceReg1 < operand2

    output  int32   result
) <autorun,reginputs> {
    aludecode AD( regimm <: opCode[3,1], function7 <: function7, function3 <: function3, rs2 <: rs2, );

    uint5   shiftcount <:: opCode[3,1] ? sourceReg2[0,5] : rs2;
    uint32  operand2 <:: opCode[3,1] ? sourceReg2 : immediateValue;

    aluaddsub ADDSUB( dosub <: AD.doalt, sourceReg1 <: sourceReg1, operand2 <: operand2, negoperand2 <: negSourceReg2 );
    alushift SHIFTS( sourceReg1 <: sourceReg1, shiftcount <: shiftcount, reverse <: function3[2,1] );
    alubits BITS( sourceReg1 <: sourceReg1, shiftcount <: shiftcount );
    alulogic LOGIC( sourceReg1 <: sourceReg1, operand2 <: operand2, doinv <: AD.doalt );
    alushxadd SHXADD( function3 <: function3[1,2], sourceReg1 <: sourceReg1, sourceReg2 <: sourceReg2 );
    alucount COUNT( shiftcount <: rs2[0,2], sourceReg1 <: sourceReg1 );
    aluminmax MINMAX( function3 <: function3[0,2], sourceReg1 <: sourceReg1, sourceReg2 <: sourceReg2, signedcompare <: LT, unsignedcompare <: LTU );
    aluextend EXTEND( shiftcount <: rs2[0,3], sourceReg1 <: sourceReg1 );
    aluorcrev ORCREV( sourceReg1 <: sourceReg1 );

    always_after {
        switch( function3 ) {
            case 3b000: { result = ADDSUB.AS; }
            case 3b001: { result = AD.dosignx ? EXTEND.result : AD.docount ? COUNT.result : AD.dobclrext ? BITS.CLR : AD.dobinv ? BITS.INV : AD.dobset ? BITS.SET : AD.dorotate ? SHIFTS.ROTATE : SHIFTS.SLL; }
            case 3b010: { result = AD.doshxadd ? SHXADD.result : LT; }
            case 3b011: { result = opCode[3,1] ? ( ~|rs1 ) ? ( |operand2 ) : LTU : ( operand2 == 1 ) ? ( ~|sourceReg1 ) : LTU; }
            case 3b100: { result = AD.dozerox ? EXTEND.result : AD.dominmax ? MINMAX.result : AD.doshxadd ? SHXADD.result : LOGIC.XOR; }
            case 3b101: { result = AD.doorc ? ORCREV.ORC : AD.dorev ? ORCREV.REV8 : AD.dominmax ? MINMAX.result : AD.dobclrext ? BITS.EXT : AD.dorotate ? SHIFTS.ROTATE : AD.dosra ? SHIFTS.SRA : SHIFTS.SRL; }
            case 3b110: { result = AD.dominmax ? MINMAX.result : AD.doshxadd ? SHXADD.result : LOGIC.OR; }
            case 3b111: { result = AD.dominmax ? MINMAX.result : LOGIC.AND; }
        }
    }
}

// ALU - M EXTENSION
// ALU FOR DIVISION
// UNSIGNED / SIGNED 32 by 32 bit division giving 32 bit remainder and quotient
algorithm douintdivide(
    input   uint1   start,
    output  uint1   busy(0),
    input   uint32  dividend,
    input   uint32  divisor,
    output  uint32  quotient,
    output  uint32  remainder
) <autorun,reginputs> {
    uint6   bit(63); uint6  bitNEXT <:: bit - 1;
    uint32  temporary <:: { remainder[0,31], dividend[bit,1] };
    uint1   bitresult <:: __unsigned(temporary) >= __unsigned(divisor);

    busy := start | ( ~&bit );

    always_after {
        if( &bit ) {
            if( start ) { bit = 31; quotient = 0; remainder = 0; }
        } else {
            quotient[bit,1] = bitresult;
            remainder = __unsigned(temporary) - ( bitresult ? __unsigned(divisor) : 0 );
            bit = bitNEXT;
        }
    }
}

algorithm aluMD(
    input   uint1   start,
    output  uint1   busy(0),
    input   uint2   function3,
    input   uint32  sourceReg1,
    input   uint32  sourceReg2,
    input   uint32  abssourceReg1,
    input   uint32  abssourceReg2,
    output  uint32  result
) <autorun,reginputs> {
    uint1   quotientremaindersign <:: ~function3[0,1] & ( sourceReg1[31,1] ^ sourceReg2[31,1] );
    uint32  sourceReg1_unsigned <:: function3[0,1] ? sourceReg1 : abssourceReg1;
    uint32  sourceReg2_unsigned <:: function3[0,1] ? sourceReg2 : abssourceReg2;

    douintdivide DODIVIDE( dividend <: sourceReg1_unsigned, divisor <: sourceReg2_unsigned );
    DODIVIDE.start := start & |sourceReg2; busy := start | DODIVIDE.busy;

    // START DIVIDER AND EXTRACT RESULT
    always_after {
        if( busy ) {
            result = ( ~|sourceReg2 ) ? function3[1,1] ? sourceReg1 : 32hffffffff :
                                        function3[1,1] ? DODIVIDE.remainder : ( quotientremaindersign ? -DODIVIDE.quotient : DODIVIDE.quotient );
        }
    }
}

// ALU FOR MULTIPLICATION
// UNSIGNED / SIGNED 33 by 33 bit multiplication giving 66 bit product using DSP blocks
algorithm aluMM(
    input   uint2   function3,
    input   int32   sourceReg1,
    input   int32   sourceReg2,
    output  int32   result
) <autorun,reginputs> {
    uint1   doupper <:: |function3;
    uint2   dosigned <:: function3[1,1] ? function3[0,1] ? 2b00 : 2b01 : 2b11;
    int33   factor_1 <:: { dosigned[0,1] & sourceReg1[ 31, 1 ], sourceReg1 }; // SIGN EXTEND IF SIGNED MULTIPLY
    int33   factor_2 <:: { dosigned[1,1] & sourceReg2[ 31, 1 ], sourceReg2 }; // SIGN EXTEND IF SIGNED MULTIPLY
    int66   product <:: factor_1 * factor_2;

    always_after {
        result = product[ { doupper, 5b0 }, 32 ];
    }
}

// ALU FOR CARRYLESS MULTIPLY FROM B-EXTENSION
algorithm doclmul(
    input   uint1   start,
    output  uint1   busy(0),
    input   uint32  sourceReg1,
    input   uint32  sourceReg2,
    input   uint6   startat,
    input   uint6   stopat,
    input   uint2   mode,
    output  uint32  result
) <autorun,reginputs> {
    uint6   count = uninitialised;
    uint32  shift <:: ( mode[1,1] ) ? ( sourceReg1 >> ( ( mode[0,1] ? 31 : 32 ) - count ) ) : ( sourceReg1 << count );

    always_after {
        if( start ) {
            busy = 1; result = 0; count = startat;
        } else {
            if( busy & sourceReg2[ count, 1 ] ) {
                result = result ^ shift;
            }
            if( count != stopat ) { count = count + 1; } else { busy = 0; }
        }
    }
}
algorithm aluCLMUL(
    input   uint1   start,
    output  uint1   busy(0),
    input   uint2   function3,
    input   uint32  sourceReg1,
    input   uint32  sourceReg2,
    output  uint32  result
) <autorun,reginputs> {
    doclmul DOCLMUL( sourceReg1 <: sourceReg1, sourceReg2 <: sourceReg2, mode <: function3, result :> result );
    DOCLMUL.start := start; DOCLMUL.startat := &function3; DOCLMUL.stopat := function3[0,1] ? 32 : 31;
    busy := start | DOCLMUL.busy;
}

// ATOMIC A EXTENSION ALU
algorithm aluA (
    input   uint7   function7,
    input   uint32  memoryinput,
    input   uint32  sourceReg2,
    output  uint32  result
) <autorun,reginputs> {
    uint1   comparison <:: function7[3,1] ? ( __unsigned(memoryinput) < __unsigned(sourceReg2) ) : ( __signed(memoryinput) < __signed(sourceReg2) );
    alulogic LOGIC( sourceReg1 <: memoryinput, operand2 <: sourceReg2 );

    always_after {
        if( function7[4,1] ) {
            result = ( function7[2,1] ^ comparison ) ? memoryinput : sourceReg2;    // AMOMAX[U] AMOMIN[U]
        } else {
            switch( function7[0,4] ) {
                default: { result = memoryinput + sourceReg2; }                     // AMOADD
                case 4b0001: { result = sourceReg2; }                               // AMOSWAP
                case 4b0100: { result = LOGIC.XOR; }                                // AMOXOR
                case 4b1000: { result = LOGIC.OR; }                                 // AMOOR
                case 4b1100: { result = LOGIC.AND; }                                // AMOAND
            }
        }
    }
}
