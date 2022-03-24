// SL @sylefeb 2020-08
//
// Simple SDcard controller
// only supports SDHC/SDXC
//
// Stays in slow xfer mode
//
// Timings for   25 MHz
// Tested ok at  50 MHz
// Tested ok at 100 MHz
//
//      GNU AFFERO GENERAL PUBLIC LICENSE
//        Version 3, 19 November 2007
//
//  A copy of the license full text is included in
//  the distribution, please refer to it for details.

group sdcardio {
  uint32 addr_sector  = 0,
  uint1  read_sector  = 0,
  uint1  write_sector = 0,
  uint16 offset       = 0,
  uint1  ready        = 0,
}

interface sdcardio_ctrl {
  input   addr_sector,
  input   read_sector,
  input   write_sector,
  output  ready,
  input   offset,
}

algorithm sdcard(
  output  uint1  sd_clk,
  output  uint1  sd_mosi,
  output  uint1  sd_csn,
  input   uint1  sd_miso,
  // read io
  sdcardio_ctrl  io,
  // storage
  simple_dualport_bram_port0 buffer_out,  // WRITE SECTOR BUFFER
  simple_dualport_bram_port1 buffer_in    // READ SECTOR BUFFER
) <autorun,reginputs> {

  // assert(sizeof(io.addr_sector) == 32);

  subroutine send(
    input  uint48   cmd,
    readwrites      sd_clk,
    writes          sd_mosi
  ) {
    uint16 count = 0; uint16 c1 <:: count + 1;
    uint48 shift = uninitialized;
    shift        = cmd;
    while (count != $2*256*48$) { // 48 clock pulses @~400 kHz (assumes 50 MHz clock)
      if (&count[0,8]) {
        sd_clk  = ~sd_clk;
        if (!sd_clk) {
          sd_mosi = shift[47,1];
          shift   = {shift[0,47],1b0};
        }
      }
      count = c1;
    }
    sd_mosi = 1;
  }

  subroutine sendbyte(
    input  uint8    byte,
    readwrites      sd_clk,
    writes          sd_mosi
  ) {
    uint8   count = 0; uint8 c1 <:: count + 1;
    uint8   shift = uninitialized;
    uint4   n = 0;
    shift        = byte;
    while( n != 8 ) {
      if ( &count ) { // swap clock
        sd_clk  = ~sd_clk;
        if (!sd_clk) {
            // send bit
            sd_mosi = shift[7,1];
            shift = { shift[0,7], 1b0 };
            n = n + 1;
        }
      }
      count = c1;
    }
  }

  subroutine waitbusy(
    readwrites      sd_clk,
    writes          sd_mosi,
    reads           sd_miso
  ) {
    uint8 count = 0; uint8 c1 <:: count + 1;
    uint1 finished = 0;
    while( ~finished ) {
        if( &count ) {
            sd_clk = ~sd_clk;
            if (!sd_clk) { finished = sd_miso; }
        }
        count = c1;
    }
    sd_clk = 0;
  }

  subroutine read(
    input  uint6    len,
    input  uint1    wait,
    output uint40   answer,
    input  uint8    rate,
    readwrites      sd_clk,
    writes          sd_mosi,
    reads           sd_miso
  ) {
    uint16 count = 0; uint16 c1 <:: count + 1;
    uint6  n     = 0; uint6 n1 <:: n + 1;
    answer       = 40hffffffffff;
    while ( // will only stop on sd_clk == 0
      (wait && answer[len-1,1]) || ((!wait) && n != len)
    ) { // read answer
      if ((count&rate) == rate) { // swap clock
        sd_clk  = ~sd_clk;
        if (!sd_clk) {
          n       = n1;
          answer  = {answer[0,39],sd_miso};
        }
      }
      count = c1;
    }
  }

  uint24 count  = 0; uint24 c1 <:: count + 1;
  uint40 status = 0;
  uint48 cmd0   = 48b010000000000000000000000000000000000000010010101;
  uint48 cmd8   = 48b010010000000000000000000000000011010101010000111;
  uint48 cmd55  = 48b011101110000000000000000000000000000000000000001;
  uint48 acmd41 = 48b011010010100000000000000000000000000000000000001;
  uint48 cmd16  = 48b010100000000000000000000000000100000000000010101;
  uint48 cmd17  = 48b010100010000000000000000000000000000000001010101;  // SINGLE BLOCK READ
  uint48 cmd24  = 48b010110000000000000000000000000000000000001010101;  // SINGLE BLOCK WRITE
  uint48 cmd13  = 48b010011010000000000000000000000000000000011111111;  // SEND STATUS
  //                 01ccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaarrrrrrr1

  uint1  do_read_sector = 0;
  uint1  do_write_sector = 0;
  uint32 do_addr_sector = 0;

  buffer_in.wenable1 := 1; // writes

  always {

    if (io.read_sector) {
      do_read_sector = 1;
      do_addr_sector = io.addr_sector;
      io.ready       = 0;
    }

    if (io.write_sector) {
      do_write_sector = 1;
      do_addr_sector  = io.addr_sector;
      io.ready        = 0;
    }

  }

  sd_mosi = 1;
  sd_csn  = 1;
  sd_clk  = 0;

  // wait 2 msec (power up), @50 MHz
  count = 0;
  while (count != 100000) { count = c1; }

  // request SPI mode
  count   = 0;
  while (count != $2*256*80$) { // 74+ clock pulses @~400 kHz (assumes 50 MHz clock)
    if (&count[0,8]) {
      sd_clk = ~sd_clk;
    }
    count = c1;
  }

  sd_csn         = 0;
  buffer_in.addr1    = 0;

  // init
  () <- send <- (cmd0);
  (status) <- read <- (8,1,255);

  () <- send <- (cmd8);
  (status) <- read <- (40,1,255);

  while (1) {
    () <- send <- (cmd55);
    (status) <- read <- (8,1,255);
    () <- send <- (acmd41);
    (status) <- read <- (8,1,255);
    if (status[0,8] == 0) {
      break;
    }
  }

  () <- send <- (cmd16);
  (status) <- read <- (8,1,255);

  io.ready = 1;

  // ready to work
  while (1) {
      uint10 progress = 0; uint10 p1 <:: progress + 1;

    if (do_read_sector) {
      do_read_sector = 0;

      // read some!
      () <- send <- ({cmd17[40,8],do_addr_sector,cmd17[0,8]});

      (status) <- read <- (8,1,3); // response

      if (~|status[0,8]) {
        (status) <- read <- (1,1,3); // start token

        buffer_in.addr1 = io.offset;
        (buffer_in.wdata1) <- read <- (8,0,3); // bytes
        while (progress != 511) {
          (buffer_in.wdata1) <- read <- (8,0,3); // bytes
          buffer_in.addr1 = buffer_in.addr1 + 1;
          progress    = p1;
        }
        (status) <- read <- (16,1,3); // CRC

        io.ready = 1;

      } else {

        io.ready = 1;

      }
    }

if (do_write_sector) {
      buffer_out.addr0 = io.offset;
      do_write_sector = 0;

      // send cmd24
      () <- send <- ({cmd24[40,8],do_addr_sector,cmd24[0,8]});

      // wait for cmd response ( 0x00 )
      ( status ) <- read <- (8,1,3);
      while( |status[0,8] ) {
          ( status ) <- read <- (8,1,3);
      }

      //send dummy clocks and start token
      () <- sendbyte <- ( 8hff ); () <- sendbyte <- ( 8hff ); () <- sendbyte <- ( 8hff ); () <- sendbyte <- ( 8hfe );

      // send 512 bytes, starting at position 0 in the buffer
      while( progress != 512 ) {
          () <- sendbyte <- ( buffer_out.rdata0 );
          buffer_out.addr0 = buffer_out.addr0 + 1;
          progress = p1;
      }

      //send CRC = 0xff 0xff
      () <- sendbyte <- ( 8hff );
      () <- sendbyte <- ( 8hff );

      // Wait for response ( should be 0x05 )
      ( status ) <- read <- ( 8,1,3);

      // wait for card to return from busy
      () <- waitbusy <- ( );

      // Request status
      () <- send <- ( cmd13 );

      // Read R1 & R2 response bytes
      ( status ) <- read <- ( 8,1,3);
      ( status ) <- read <- ( 8,1,3);

      io.ready = 1;
    }
  }

}

// -------------------------
