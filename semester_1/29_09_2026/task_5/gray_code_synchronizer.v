`default_nettype none

module gray_code_synchronizer #(
    parameter WIDTH = 5
) (
    input  wire             src_clk,
    input  wire [WIDTH-1:0] binary_in,
    input  wire             dst_clk,
    output reg  [WIDTH-1:0] binary_out
);

  reg [WIDTH-1:0] gray_in_src_clk, gray_metastable, gray_in_dst_clk;

  initial begin
    gray_in_src_clk = 0;
    gray_metastable = 0;
    gray_in_dst_clk = 0;
  end

  always @(posedge src_clk) begin
    gray_in_src_clk <= binary_in ^ (binary_in >> 1);
  end

  always @(posedge dst_clk) begin
    gray_metastable <= gray_in_src_clk;
    gray_in_dst_clk <= gray_metastable;
  end

  integer bit_index;

  always @(*) begin
    for (bit_index = 0; bit_index < WIDTH; bit_index = bit_index + 1) begin
      binary_out[bit_index] = ^(gray_in_dst_clk >> bit_index);
    end
  end

endmodule
