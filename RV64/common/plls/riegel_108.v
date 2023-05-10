/**
 * PLL configuration
 */

module pll(
	input  clock_in,
	output clock_out
	);

  SB_PLL40_PAD #(.FEEDBACK_PATH("SIMPLE"),
                  .PLLOUT_SELECT("GENCLK"),
                  .DIVR(4'b0000),
// 108
                  .DIVF(7'b0010001),
                  .DIVQ(3'b011),
//
                  .FILTER_RANGE(3'b001),
                  .DELAY_ADJUSTMENT_MODE_FEEDBACK("FIXED"),
                  .FDA_FEEDBACK(4'b0000),
                  .DELAY_ADJUSTMENT_MODE_RELATIVE("FIXED"),
                  .FDA_RELATIVE(4'b0000),
                  .SHIFTREG_DIV_MODE(2'b00),
                  .ENABLE_ICEGATE(1'b0)
                 ) uut (
                         .PACKAGEPIN(clock_in),
                         .PLLOUTCORE(clock_out),
                         .EXTFEEDBACK(),
                         .DYNAMICDELAY(),
                         .LATCHINPUTVALUE(),
                         .RESETB(1'b1),
                         .BYPASS(1'b0)
                        );

endmodule
