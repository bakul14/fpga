`default_nettype none

module lru_cache #(
    parameter DATA_WIDTH = 32,
    parameter DATA_DEPTH = 4
) (
    input wire                  clk,
    input wire                  rst,
    input wire                  valid_in,
    input wire [DATA_WIDTH-1:0] data_in,

    output wire [DATA_WIDTH-1:0] data_out[DATA_DEPTH]
);

endmodule
