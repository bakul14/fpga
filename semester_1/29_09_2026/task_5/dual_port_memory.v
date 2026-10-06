`default_nettype none

module dual_port_memory #(
    parameter DATA_WIDTH = 8,
    parameter ADDR_WIDTH = 4
) (
    input  wire                  wr_clk,
    input  wire                  wr_en,
    input  wire [ADDR_WIDTH-1:0] wr_addr,
    input  wire [DATA_WIDTH-1:0] wr_data,
    input  wire [ADDR_WIDTH-1:0] rd_addr,
    output wire [DATA_WIDTH-1:0] rd_data
);

  localparam DEPTH = 1 << ADDR_WIDTH;

  reg [DATA_WIDTH-1:0] memory[0:DEPTH-1];

  assign rd_data = memory[rd_addr];

  always @(posedge wr_clk) begin
    if (wr_en) memory[wr_addr] <= wr_data;
  end

endmodule
