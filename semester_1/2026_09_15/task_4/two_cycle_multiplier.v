`default_nettype none

module two_cycle_multiplier #(
    parameter WIDTH = 64
) (
    input  wire                    clk,
    input  wire signed [WIDTH-1:0] coeff_in,
    input  wire signed [WIDTH-1:0] value_in,
    output reg signed  [WIDTH-1:0] product_out
);

  reg signed [WIDTH-1:0] coeff, value;

  initial begin
    coeff       = 0;
    value       = 0;
    product_out = 0;
  end

  always @(posedge clk) begin
    coeff       <= coeff_in;
    value       <= value_in;
    product_out <= coeff * value;
  end

endmodule
