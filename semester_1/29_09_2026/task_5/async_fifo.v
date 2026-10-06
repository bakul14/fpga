`default_nettype none

module async_fifo #(
    parameter DATA_WIDTH = 8,
    parameter ADDR_WIDTH = 4
) (
    input  wire                  wr_clk,
    input  wire [DATA_WIDTH-1:0] data_in,
    input  wire                  wr_en,
    output wire                  full,

    input  wire                  rd_clk,
    output reg  [DATA_WIDTH-1:0] data_out,
    output reg                   valid_out,
    input  wire                  rd_en,
    output wire                  empty
);

  localparam PTR_WIDTH = ADDR_WIDTH + 1;
  localparam DEPTH = 1 << ADDR_WIDTH;

  reg [DATA_WIDTH-1:0] memory[0:DEPTH-1];

  reg [PTR_WIDTH-1:0] wr_ptr, rd_ptr;
  wire [PTR_WIDTH-1:0] rd_ptr_in_wr_clk, wr_ptr_in_rd_clk;

  assign full  = (wr_ptr == {~rd_ptr_in_wr_clk[PTR_WIDTH-1], rd_ptr_in_wr_clk[PTR_WIDTH-2:0]});
  assign empty = (rd_ptr == wr_ptr_in_rd_clk);

  initial begin
    wr_ptr    = 0;
    rd_ptr    = 0;
    valid_out = 0;
  end

  gray_code_synchronizer #(
      .WIDTH(PTR_WIDTH)
  ) rd_ptr_to_wr_clk (
      .src_clk   (rd_clk),
      .binary_in (rd_ptr),
      .dst_clk   (wr_clk),
      .binary_out(rd_ptr_in_wr_clk)
  );

  gray_code_synchronizer #(
      .WIDTH(PTR_WIDTH)
  ) wr_ptr_to_rd_clk (
      .src_clk   (wr_clk),
      .binary_in (wr_ptr),
      .dst_clk   (rd_clk),
      .binary_out(wr_ptr_in_rd_clk)
  );

  always @(posedge wr_clk) begin
    if (wr_en) begin
      memory[wr_ptr[ADDR_WIDTH-1:0]] <= data_in;
      wr_ptr                         <= wr_ptr + 1'b1;
    end

    if (wr_en && full) $error("wr_en при full: запись в полное FIFO");
  end

  always @(posedge rd_clk) begin
    data_out  <= memory[rd_ptr[ADDR_WIDTH-1:0]];
    valid_out <= rd_en;

    if (rd_en) rd_ptr <= rd_ptr + 1'b1;

    if (rd_en && empty) $error("rd_en при empty: чтение из пустого FIFO");
  end

endmodule
