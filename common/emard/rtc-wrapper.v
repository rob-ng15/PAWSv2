module rtc
(
  input  clk,        // System clock, wr_ctrl should be synchronous to this
  input  reset,      // 1:reset - puts I2C bus into idle state
  input  wr,
  input  [2:0] addr,
  input  [7:0] r_data,
  output tick,       // ticks every second -> 1: datetime_o is valid
  output [55:0] datetime, // BCD {YY,MM,DD, WD, HH,MM,SS}
  inout  sda,        // I2C Serial data line, pulled high at board level
  inout  scl         // I2C Serial clock line, pulled high at board level
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
    .datetime_o(datetime),
    .sda(gpdi_sda),
    .scl(gpdi_scl)
  );

endmodule
