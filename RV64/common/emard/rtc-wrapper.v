module rtc
(
  input  wire        clk,        // System clock, wr_ctrl should be synchronous to this
  input  wire        reset,      // 1:reset - puts I2C bus into idle state
  input  wire        wr,         // write enable
  input  wire [2:0]  addr,       // 0-6:writing, 7:circular reading
  input  wire [7:0]  data,       // data to write at addr
  output reg         tick,       // ticks every second -> 1: datetime_o is valid
  output wire [55:0] datetime_o, // BCD {YY,MM,DD, WD, HH,MM,SS}
  inout  wire        sda,        // I2C Serial data line, pulled high at board level
  inout  wire        scl         // I2C Serial clock line, pulled high at board level
);
  mcp7940n
  #(
    .c_clk_mhz(25),
    .c_slow_bits(18)
  )
  mcp7940n_inst
  (
    .clk(clk),
    .reset(reset),
    .wr(0),
    .addr(7),
    .data(0),
    .tick(tick),
    .datetime_o(datetime_o),
    .sda(sda),
    .scl(scl)
  );

endmodule
