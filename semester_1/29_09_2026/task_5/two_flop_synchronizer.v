`default_nettype none

module two_flop_synchronizer #(
    parameter WIDTH = 5
) (
    input  wire             clk,
    input  wire [WIDTH-1:0] data_in,
    output reg  [WIDTH-1:0] data_out
);

  reg [WIDTH-1:0] metastable_stage;

  initial begin
    metastable_stage = 0;
    data_out         = 0;
  end

  always @(posedge clk) begin
    metastable_stage <= data_in;
    data_out         <= metastable_stage;
  end

endmodule
