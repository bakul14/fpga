`default_nettype none

module divide3_checker (
    input  wire clk,
    input  wire first,
    input  wire last,
    input  wire data_bit,
    output reg  divisible
);

  reg [1:0] remainder;
  reg [1:0] remainder_table[0:2][0:1];

  initial begin
    remainder_table[0][0] = 0;
    remainder_table[0][1] = 1;
    remainder_table[1][0] = 2;
    remainder_table[1][1] = 0;
    remainder_table[2][0] = 1;
    remainder_table[2][1] = 2;
  end

  wire [1:0] remainder_old = first ? 0 : remainder;

  wire [1:0] remainder_new = remainder_table[remainder_old][data_bit];

  always @(posedge clk) begin
    remainder <= remainder_new;
    if (last) begin
      divisible <= (remainder_new == 0) ? 1 : 0;
    end
  end

endmodule
