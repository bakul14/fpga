`default_nettype none

module lru_cache #(
    parameter int DATA_WIDTH = 32,
    parameter int DATA_DEPTH = 4
) (
    input wire                  clk,
    input wire                  rst,
    input wire                  valid_in,
    input wire [DATA_WIDTH-1:0] data_in,

    output reg [DATA_WIDTH-1:0] data_out[DATA_DEPTH]
);

  int pos;

  always @(*) begin
    pos = DATA_DEPTH - 1;
    for (int i = 0; i < DATA_DEPTH; ++i) begin
      if (data_out[i] == data_in) pos = i;
    end
  end

  always @(posedge clk) begin
    for (int i = 0; i < DATA_DEPTH; ++i) begin
      if (rst) begin
        data_out[i] <= '0;
      end else if (valid_in && i <= pos) begin
        data_out[i] <= (i == 0) ? data_in : data_out[i-1];
      end
    end
  end

endmodule
