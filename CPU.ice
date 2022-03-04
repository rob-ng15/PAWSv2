// RISC-ICE-V
// inspired by https://github.com/sylefeb/Silice/blob/master/projects/ice-v/ice-v.ice
//
// A simple Risc-V RV32IMAFC processor

// RISC-V - MAIN CPU LOOP
//          ALU FUNCTIONALITY LISTED IN ALU-

algorithm PAWSCPU(
    input   uint1   clock_CPUdecoder,
    output  uint2   accesssize,
    output  uint27  address,
    output  uint1   cacheselect,
    output  uint16  writedata,
    output  uint1   writememory,
    input   uint16  readdata,
    output  uint1   readmemory,
    input   uint1   memorybusy,

    // SMT
    input   uint1   SMTRUNNING,
    input   uint27  SMTSTARTPC,

    // MINI DMA CONTROLLER
    input   uint27  DMASOURCE,
    input   uint27  DMADEST,
    input   uint27  DMACOUNT,
    input   uint3   DMAMODE,
    output  uint1   DMAACTIVE(0)
) <autorun,reginputs> {
    // COMMIT TO REGISTERS FLAG AND HART (0/1) SELECT
    uint1   COMMIT = uninitialized;                 uint1   SMT = 0;

    // COMPRESSED INSTRUCTION EXPANDER
    uint32  instruction = uninitialized;            uint1   compressed = uninitialized;
    compressed00 COMPRESSED00 <@clock_CPUdecoder> ( i16 <: readdata ); compressed01 COMPRESSED01 <@clock_CPUdecoder> ( i16 <: readdata ); compressed10 COMPRESSED10 <@clock_CPUdecoder> ( i16 <: readdata );

    // RISC-V 32 BIT INSTRUCTION DECODER + MEMORY ACCESS SIZE
    decode RV32DECODER <@clock_CPUdecoder> ( instruction <: instruction );
    memoryaccess MEMACCESS <@clock_CPUdecoder> ( cacheselect <: cacheselect, DMAACTIVE <: DMAACTIVE, opCode <: RV32DECODER.opCode, function7 <: RV32DECODER.function7[2,5], function3 <: RV32DECODER.function3, AMO <: RV32DECODER.AMO, accesssize :> accesssize );

    // RISC-V REGISTERS
    uint1   frd <:: IFASTSLOW.FASTPATH ? IFASTSLOW.frd : EXECUTESLOW.frd;
    uint1   REGISTERSwrite <:: COMMIT & IFASTSLOW.writeRegister & ~frd & ( |RV32DECODER.rd );
    registersI REGISTERS <@clock_CPUdecoder> (
        SMT <:: SMT,
        rs1 <: RV32DECODER.rs1,
        rs2 <: RV32DECODER.rs2,
        result <: result,
        rd <: RV32DECODER.rd,
        write <: REGISTERSwrite
    );
    // RISC-V FLOATING POINT REGISTERS
    uint1   REGISTERSFwrite <:: COMMIT & IFASTSLOW.writeRegister & frd;
    registersF REGISTERSF <@clock_CPUdecoder> (
        SMT <:: SMT,
        rs1 <: RV32DECODER.rs1,
        rs2 <: RV32DECODER.rs2,
        rs3 <: RV32DECODER.rs3,
        result <: result,
        rd <: RV32DECODER.rd,
        write <: REGISTERSFwrite
    );
    // NEGATIVE OF REGISTERS FOR ABS AND ADD/SUB
    int32   negRS1 <:: -REGISTERS.sourceReg1;                 int32   negRS2 <:: -REGISTERS.sourceReg2;
    // EXTRACT ABSOLUTE VALUE FOR MULTIPLICATION AND DIVISION
    absolute ARS1 <@clock_CPUdecoder> ( number <: REGISTERS.sourceReg1, negative <: negRS1 ); absolute ARS2 <@clock_CPUdecoder> ( number <: REGISTERS.sourceReg2, negative <: negRS2 );


    // RISC-V PROGRAM COUNTERS AND STATUS - SMT -> RUNNING ON HART 1 WITH DUPLICATE PROGRAM COUNTER AND REGISTER FILE
    uint27  pc = 0;                     uint27  pc_next <:: SMT ? pc :  NEWPC.newPC;                                    // HART 0 pc + UPDATE
    uint27  pcSMT = uninitialized;      uint27  pcSMT_next <:: SMT ? NEWPC.newPC : SMTRUNNING ? pcSMT : SMTSTARTPC;     // HART 1 pc + update
    uint27  PC <:: SMT ? pcSMT : pc;                                                                                    // SELECT PC FOR THIS CYCLE

    // RISC-V ADDRESS GENERATOR
    addressgenerator AGU <@clock_CPUdecoder> (
        instruction <: instruction,
        immediateValue <: RV32DECODER.immediateValue,
        PC <: PC,
        sourceReg1 <: REGISTERS.sourceReg1,
        AMO <: RV32DECODER.AMO
    );

    // GENERATE PLUS 2 ADDRESSES FOR 32 BIT MEMORY OPERATIONS
    addrplus2 PC2 <@clock_CPUdecoder> ( address <: PC ); addrplus2 LA2 <@clock_CPUdecoder> ( address <: AGU.loadAddress ); addrplus2 SA2 <@clock_CPUdecoder> ( address <: AGU.storeAddress );

    // SIGN EXTENDER FOR 8 AND 16 BIT LOADS
    signextend SIGNEXTEND <@clock_CPUdecoder> ( readdata <: readdata, is16or8 <: accesssize[0,1], byteaccess <: AGU.loadAddress[0,1], dounsigned <: RV32DECODER.function3[2,1] );

    // CPU EXECUTE BLOCKS
    uint32  memoryinput = uninitialized;
    uint32  result <:: IFASTSLOW.FASTPATH ? EXECUTEFAST.result : EXECUTESLOW.result;
    uint16  storeLOW <:: IFASTSLOW.FASTPATH ? EXECUTEFAST.memoryoutput[0,16] : EXECUTESLOW.memoryoutput[0,16];
    uint16  storeHIGH <:: IFASTSLOW.FASTPATH ? EXECUTEFAST.memoryoutput[16,16] : EXECUTESLOW.memoryoutput[16,16];

    // CLASSIFY THE INSTRUCTION TO FAST/SLOW
    uint1   isALUM <:: RV32DECODER.opCode[3,1] & ( RV32DECODER.function7 == 7b0000001 );
    uint1   isALUCLM <:: RV32DECODER.opCode[3,1] & ~RV32DECODER.function3[2,1] & ( RV32DECODER.function7 == 7b0000101 );

    Iclass IFASTSLOW <@clock_CPUdecoder> ( opCode <: RV32DECODER.opCode, function3 <: RV32DECODER.function3, isALUM <: isALUM, isALUCLM <: isALUCLM );

    // EXECUTE MULTICYCLE INSTRUCTIONS, INTEGER DIVIDE, FPU, CSR AND ALU-A
    cpuexecuteSLOWPATH EXECUTESLOW(
        SMT <: SMT,
        instruction <: instruction,
        opCode <: RV32DECODER.opCode,
        function3 <: RV32DECODER.function3,
        function7 <: RV32DECODER.function7,
        rs1 <: RV32DECODER.rs1,
        rs2 <: RV32DECODER.rs2,
        sourceReg1 <: REGISTERS.sourceReg1,
        sourceReg2 <: REGISTERS.sourceReg2,
        abssourceReg1 <: ARS1.value,
        abssourceReg2 <: ARS2.value,
        sourceReg1F <: REGISTERSF.sourceReg1,
        sourceReg2F <: REGISTERSF.sourceReg2,
        sourceReg3F <: REGISTERSF.sourceReg3,
        memoryinput <: memoryinput,
        incCSRinstret <: COMMIT
    );

    // EXECUTE SINGLE CYLE INSTRUCTIONS, MOST OF BASE PLUS INTEGER MULTIPLICATION
    uint1   takeBranch <:: IFASTSLOW.FASTPATH & EXECUTEFAST.takeBranch;
    cpuexecuteFASTPATH EXECUTEFAST(
        opCode <: RV32DECODER.opCode,
        function3 <: RV32DECODER.function3,
        function7 <: RV32DECODER.function7,
        rs1 <: RV32DECODER.rs1,
        rs2 <: RV32DECODER.rs2,
        sourceReg1 <: REGISTERS.sourceReg1,
        sourceReg2 <: REGISTERS.sourceReg2,
        negSourceReg2 <: negRS2,
        sourceReg2F <: REGISTERSF.sourceReg2,
        immediateValue <: RV32DECODER.immediateValue,
        memoryinput <: memoryinput,
        AUIPCLUI <: AGU.AUIPCLUI,
        nextPC <: NEWPC.nextPC,
        isALUMM <: isALUM,
        isLOAD <: MEMACCESS.memoryload
    );

    // SELECT NEXT PC
    newpc NEWPC <@clock_CPUdecoder> (
        opCode <: RV32DECODER.opCode,
        PC <: PC,
        compressed <: compressed,
        incPC <: IFASTSLOW.incPC,
        takeBranch <: takeBranch,
        branchAddress <: AGU.branchAddress,
        jumpAddress <: AGU.jumpAddress,
        loadAddress <: AGU.loadAddress
    );

    // MINI DMA CONTROLLER
    dma DMA( DMASOURCE <: DMASOURCE, DMADEST <: DMADEST, DMACOUNT <: DMACOUNT, DMAMODE <: DMAMODE );
    DMA.start :=0; DMA.update := 0;

    readmemory := 0; writememory := 0; EXECUTESLOW.start := 0; COMMIT := 0;

    if( ~reset ) {
        DMAACTIVE = 0;                                                                                                                              // ON RESET CANCEL DMA
        while( memorybusy | EXECUTESLOW.busy ) {}                                                                                                   // WAIT FDR MEMORY AND CPU TO FINISH
    }

    cacheselect = 0; address = PC; readmemory = 1;                                                                                                  // FETCH FIRST INSTRUCTION

    while(1) {
        while( memorybusy ) {}                                                                                                                      // FETCH POTENTIAL COMPRESSED OR 1ST 16 BITS
        compressed = ( ~&readdata[0,2] );
        if( compressed ) {
            switch( readdata[0,2] ) {                                                                                                               // EXPAND COMPRESSED INSTRUCTION
                case 2b00: { instruction = { COMPRESSED00.i32, 2b11 }; }
                case 2b01: { instruction = { COMPRESSED01.i32, 2b11 }; }
                case 2b10: { instruction = { COMPRESSED10.i32, 2b11 }; }
                default: {}
            }
            ++:
        } else {
            instruction[0,16] = readdata; address = PC2.addressplus2; readmemory = 1; while( memorybusy ) {} instruction[16,16] = readdata;         // 32 BIT INSTRUCTION FETCH 2ND 16 BITS
            ++: ++:
        }
        // DECODE, REGISTER FETCH, ADDRESS GENERATION AUTOMATICALLY TAKES PLACE AS SOON AS THE INSTRUCTION IS LOADED

        cacheselect = 1;
        if( MEMACCESS.memoryload ) {
            address = AGU.loadAddress; readmemory = 1; while( memorybusy ) {}                                                                       // READ 1ST 8 or 16 BITS
            if( accesssize[1,1] ) {
                memoryinput[0,16] = readdata; address = LA2.addressplus2; readmemory = 1; while( memorybusy ) {} memoryinput[16,16] = readdata;     // READ 2ND 16 BITS
            } else {
                memoryinput = SIGNEXTEND.memory168;                                                                                                 // 8 or 16 BIT SIGN EXTENDED
            }
        }

        if( ~IFASTSLOW.FASTPATH ) {
            EXECUTESLOW.start = 1; while( EXECUTESLOW.busy ) {}                                                                                     // FPU, ALU-A, INTEGER DIVISION, CLMUL, CSR
        } else {
            COMMIT = 0;                                                                                                                             // INTEGER ALU, LOAD, STORE, BRANCH, JUMP, ETC
        }
        COMMIT = 1;                                                                                                                                 // COMMIT REGISTERS

        if( MEMACCESS.memorystore ) {
            address = AGU.storeAddress; writedata = storeLOW; writememory = 1; while( memorybusy ) {}                                               // STORE 8 OR 16 BIT
            if( accesssize[1,1] ) {
                address = SA2.addressplus2; writedata = storeHIGH; writememory = 1;  while( memorybusy ) {}                                         // 32 BIT WRITE 2ND 16 BITS
            }
        }

        pc = pc_next; pcSMT = pcSMT_next; SMT = ~SMT & SMTRUNNING;                                                                                  // UPDATE PC AND SMT

        if( |DMAMODE ) {                                                                                                                            // PROCESS MINI-DMA ENGINE REQUESTS
            // PROCESS A DMA REQUEST - USES DATA CACHE AND BYTE ACCESS MODE
            DMAACTIVE = 1; DMA.start = 1;
            while( |DMA.dmacount ) {
                address = DMA.dmasrc; readmemory = 1; while( memorybusy ) {}                                                                        // DMA FETCH
                address = DMA.dmadest; writedata = readdata[ { DMA.dmasrc[0,1], 3b000 }, 8 ]; writememory = 1; while( memorybusy ) {}               // DMA STORE
                DMA.update = 1;
            }
            DMAACTIVE = 0;
        }

        cacheselect = 0; address = PC; readmemory = 1;                                                                                              // START FETCH OF NEXT INSTRUCTION
    } // RISC-V
}

algorithm cpuexecuteSLOWPATH(
    input   uint1   start,
    output  uint1   busy(0),
    input   uint1   SMT,
    input   uint32  instruction,
    input   uint5   opCode,
    input   uint3   function3,
    input   uint7   function7,
    input   uint5   rs1,
    input   uint5   rs2,
    input   int32   sourceReg1,
    input   int32   sourceReg2,
    input   int32   abssourceReg1,
    input   int32   abssourceReg2,
    input   uint32  sourceReg1F,
    input   uint32  sourceReg2F,
    input   uint32  sourceReg3F,
    input   int32   memoryinput,
    output  uint1   frd,
    output  int32   memoryoutput,
    output  int32   result,
    input   uint1   incCSRinstret
) <autorun,reginputs> {
    // M EXTENSION - DIVISION
    aluMD ALUMD( function3 <: function3[0,2], sourceReg1 <: sourceReg1, sourceReg2 <: sourceReg2, abssourceReg1 <: abssourceReg1, abssourceReg2 <: abssourceReg2 );

    // ATOMIC MEMORY OPERATIONS
    aluA ALUA( function7 <: function7, memoryinput <: memoryinput, sourceReg2 <: sourceReg2 );

    // B EXTENSION - CLMUL
    aluCLMUL ALUBCLMUL( function3 <: function3[0,2], sourceReg1 <: sourceReg1, sourceReg2 <: sourceReg2 );

    // FLOATING POINT INSTRUCTION CLASSIFICATION
    Fclass FCLASS( opCode <: opCode, function7 <: function7 );

    // FLOATING POINT REGISTERS CLASSIFICATION
    classifyF class1F( a <: sourceReg1F ); classifyF class2F( a <: sourceReg2F ); classifyF class3F( a <: sourceReg3F );

    // FLOATING POINT SLOW OPERATIONS - CALCULATIONS AND CONVERSIONS
    floatconvert FPUCONVERT(
        FPUflags <: CSR.FPUflags,
        function7 <: function7[2,5], rs2 <: rs2[0,1],
        sourceReg1 <: sourceReg1, abssourceReg1 <: abssourceReg1, sourceReg1F <: sourceReg1F,
        classA <: class1F.class
    );

    floatcalc FPUCALC(
        FPUflags <: CSR.FPUflags,
        opCode <: opCode, function7 <: function7[2,5],
        sourceReg1F <: sourceReg1F, sourceReg2F <: sourceReg2F, sourceReg3F <: sourceReg3F,
        classA <: class1F.class, classB <: class2F.class, classC <: class3F.class
    );

    // FLOATING POINT FAST OPERATIONS
    fpufast FPUFAST(
        FPUflags <: CSR.FPUflags,
        function3 <: function3[0,2], function7 <: function7[2,5],
        sourceReg1 <: sourceReg1, sourceReg1F <: sourceReg1F, sourceReg2F <: sourceReg2F,
         classA <: class1F.class, classB <: class2F.class
    );

    // MANDATORY RISC-V CSR REGISTERS + HARTID == 0 MAIN THREAD == 1 SMT THREAD
    uint5   FPUnewflags <:: FCLASS.FASTPATHFPU ? FPUFAST.FPUnewflags : fpuconvert ? FPUCONVERT.FPUnewflags : FPUCALC.FPUnewflags;
    CSRblock CSR(
        SMT <: SMT,
        instruction <: instruction,
        function3 <: function3,
        rs1 <: rs1,
        sourceReg1 <: sourceReg1,
        FPUnewflags <: FPUnewflags,
        incCSRinstret <: incCSRinstret
    );

    // Classify the instruction
    uint1   csr <:: ( opCode == 5b11100 );           uint1   atomic <:: ( opCode == 5b01011 );            uint1 fpu <:: opCode[4,1] & ~csr;
    uint1   alufpu <:: ~csr & ~atomic;

    uint1   fpuconvert <:: ( opCode == 5b10100 ) & ( function7[4,3] == 3b110 );
    uint1   fpufast <:: ( fpu & FCLASS.FASTPATHFPU ) | fpuconvert;
    uint1   fpucalc <:: fpu & ~fpufast;

    uint4   operation <:: { ~|{fpufast,atomic,csr}, fpufast, atomic, csr };

    // START FLAGS
    ALUMD.start := start & ~fpu & function3[2,1];                                           // INTEGER DIVISION
    ALUBCLMUL.start := start & ~fpu & ~function3[2,1];                                      // CARRYLESS MULTIPLY
    FPUCALC.start := start & fpucalc;                                                       // FPU CALCULATIONS
    CSR.start := start & csr & |function3;                                                  // CSR

    // Deal with updating fpuflags and writing to fpu registers
    CSR.updateFPUflags := 0; frd := fpuconvert ? FPUCONVERT.frd : fpufast ? FPUFAST.frd : fpucalc ? 1 : 0;

    while(1) {
        if( start ) {
            busy = 1;
            onehot( operation ) {
                case 0: { ++: result = |function3 ? CSR.result : 0; }                        // CSR
                case 1: {                                                                   // ATOMIC OPERATIONS
                    if( function7[3,1] ) {
                        result = memoryinput; memoryoutput = ALUA.result;                   // ATOMIC LOAD - MODIFY - STORE
                    } else {
                        result = function7[2,1] ? 0 : memoryinput;                          // LR.W SC.W
                        memoryoutput = sourceReg2;
                    }
                }
                case 2: { result = fpuconvert ? FPUCONVERT.result : FPUFAST.result; }       // FPU FAST COMPARE, MIN/MAX, CLASS, MOVE, CONVERT
                case 3: {
                        while( FPUCALC.busy | ALUMD.busy | ALUBCLMUL.busy ) {}              // FPU CALCULATIONS AND INTEGER DIVISION
                        result = fpucalc ? FPUCALC.result : function3[2,1] ? ALUMD.result : ALUBCLMUL.result;
                }
            }
            busy = 0;
            CSR.updateFPUflags = fpuconvert | fpucalc;
        }
    }
}
algorithm cpuexecuteFASTPATH(
    input   uint5   opCode,
    input   uint3   function3,
    input   uint7   function7,
    input   uint5   rs1,
    input   uint5   rs2,
    input   int32   sourceReg1,
    input   int32   sourceReg2,
    input   int32   negSourceReg2,
    input   uint32  sourceReg2F,
    input   int32   immediateValue,
    input   int32   memoryinput,
    input   uint32  AUIPCLUI,
    input   uint32  nextPC,
    input   uint1   isALUMM,
    input   uint1   isLOAD,
    output  uint1   takeBranch,
    output  int32   memoryoutput,
    output  int32   result
) <autorun> {
    // COMPARISON UNIT
    compare COMPARE( sourceReg1 <: sourceReg1, sourceReg2 <: sourceReg2, immediateValue <: immediateValue, regimm <: opCode[3,1] );

    // BRANCH COMPARISON UNIT
    branchcomparison BRANCHUNIT( function3 <: function3, sourceReg1 <: sourceReg1, sourceReg2 <: sourceReg2, LT <: COMPARE.LT, LTU <: COMPARE.LTU, EQ <: COMPARE.EQ );

    // ALU
    alu ALU(
        opCode <: opCode, function3 <: function3, function7 <: function7,
        rs1 <: rs1, rs2 <: rs2,
        sourceReg1 <: sourceReg1, sourceReg2 <: sourceReg2, negSourceReg2 <: negSourceReg2,
        immediateValue <: immediateValue,
        LT <: COMPARE.LT, LTU <: COMPARE.LTU
    );

    // M EXTENSION - MULTIPLICATION
    aluMM ALUMM( function3 <: function3[0,2], sourceReg1 <: sourceReg1, sourceReg2 <: sourceReg2 );

    // CLASSIFY THE TYPE FOR INSTRUCTIONS THAT WRITE TO REGISTER
    uint1   isAUIPCLUI <:: ( opCode[0,3] == 3b101 );
    uint1   isJAL <:: ( opCode[2,3] == 3b110 ) & opCode[0,1];

    always_after {
        takeBranch = ( opCode == 5b11000 ) & BRANCHUNIT.takeBranch;    // BRANCH
        memoryoutput = opCode[0,1] ? sourceReg2F : sourceReg2;         // FLOAT STORE OR STORE
        result = isAUIPCLUI ? AUIPCLUI :                               // LUI AUIPC
                            isJAL ? nextPC :                         // JAL[R]
                            isLOAD ? memoryinput :                   // [FLOAT]LOAD
                            isALUMM ? ALUMM.result : ALU.result;     // INTEGER ALU AND MULTIPLICATION
    }
}

// MINI DMA CONTROLLER ADDRESS GENERATOR
algorithm dma(
    input   uint27  DMASOURCE,
    input   uint27  DMADEST,
    input   uint27  DMACOUNT,
    input   uint3   DMAMODE,

    output  uint27  dmasrc,
    output  uint27  dmadest,
    output  uint27  dmacount,

    input   uint1   start,
    input   uint1   update
) <autorun,reginputs> {
    uint3   dmamode = uninitialized;
    uint27  dmasrc1 <:: dmasrc + 1;                 uint27  dmadest1 <:: dmadest + 1;                   uint27  dmadest2 <:: dmadest + 2;
    uint27  dmacount1 <:: dmacount - 1;

    uint1   dmadestblue <:: ( dmadest == 27hd676 );

    while(1) {
        if( start ) {
            dmamode = DMAMODE; dmasrc = DMASOURCE; dmadest = DMADEST; dmacount = DMACOUNT;
        } else {
            if( update ) {
                switch( dmamode ) {
                    default: {}                                                                                             // DMA single-src to single-dest SET TILE/CBLITTER to single value
                    case 1: { dmasrc = dmasrc1; }                                                                           // DMA multi-src to single-dest PIXEL BLOCK 7/8 bit + SDCARD WRITE
                    case 2: { dmasrc = dmasrc1; if( dmadestblue ) { dmadest = 27hd672; } else { dmadest = dmadest2; } }     // DMA SPECIAL PIXEL BLOCK RGB
                    case 3: { dmasrc = dmasrc1; dmadest = dmadest1; }                                                       // DMA multi-src to multi-dest MEMCPY
                    case 4: { dmadest = dmadest1; }                                                                         // DMA single-src to multi-dest MEMSET + SDCARD WRITE
                }
                dmacount = dmacount1;
            }
        }
    }
}
