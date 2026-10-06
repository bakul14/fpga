`default_nettype none

module async_fifo #(
    parameter DATA_WIDTH = 8,
    parameter ADDR_WIDTH = 4
) (
    input  wire                  wr_clk,
    input  wire                  wr_en,
    input  wire [DATA_WIDTH-1:0] wr_data,
    output reg                   full,

    input  wire                  rd_clk,
    input  wire                  rd_en,
    output wire [DATA_WIDTH-1:0] rd_data,
    output reg                   empty
);

  localparam PTR_WIDTH = ADDR_WIDTH + 1;

  reg [PTR_WIDTH-1:0] wr_ptr_bin, wr_ptr_gray;
  wire [PTR_WIDTH-1:0] rd_ptr_gray_in_wr_clk;

  reg [PTR_WIDTH-1:0] rd_ptr_bin, rd_ptr_gray;
  wire [PTR_WIDTH-1:0] wr_ptr_gray_in_rd_clk;

  wire wr_accepted = wr_en & ~full;
  wire rd_accepted = rd_en & ~empty;

  wire [PTR_WIDTH-1:0] wr_ptr_bin_next = wr_ptr_bin + PTR_WIDTH'(wr_accepted);
  wire [PTR_WIDTH-1:0] wr_ptr_gray_next = wr_ptr_bin_next ^ (wr_ptr_bin_next >> 1);

  wire [PTR_WIDTH-1:0] rd_ptr_bin_next = rd_ptr_bin + PTR_WIDTH'(rd_accepted);
  wire [PTR_WIDTH-1:0] rd_ptr_gray_next = rd_ptr_bin_next ^ (rd_ptr_bin_next >> 1);

  wire full_next = (wr_ptr_gray_next == {~rd_ptr_gray_in_wr_clk[PTR_WIDTH-1:PTR_WIDTH-2],
                                         rd_ptr_gray_in_wr_clk[PTR_WIDTH-3:0]});

  wire empty_next = (rd_ptr_gray_next == wr_ptr_gray_in_rd_clk);

  initial begin
    wr_ptr_bin  = 0;
    wr_ptr_gray = 0;
    rd_ptr_bin  = 0;
    rd_ptr_gray = 0;
    full        = 0;
    empty       = 1;
  end

  dual_port_memory #(
      .DATA_WIDTH(DATA_WIDTH),
      .ADDR_WIDTH(ADDR_WIDTH)
  ) storage (
      .wr_clk (wr_clk),
      .wr_en  (wr_accepted),
      .wr_addr(wr_ptr_bin[ADDR_WIDTH-1:0]),
      .wr_data(wr_data),
      .rd_addr(rd_ptr_bin[ADDR_WIDTH-1:0]),
      .rd_data(rd_data)
  );

  two_flop_synchronizer #(
      .WIDTH(PTR_WIDTH)
  ) rd_ptr_to_wr_clk (
      .clk     (wr_clk),
      .data_in (rd_ptr_gray),
      .data_out(rd_ptr_gray_in_wr_clk)
  );

  two_flop_synchronizer #(
      .WIDTH(PTR_WIDTH)
  ) wr_ptr_to_rd_clk (
      .clk     (rd_clk),
      .data_in (wr_ptr_gray),
      .data_out(wr_ptr_gray_in_rd_clk)
  );

  always @(posedge wr_clk) begin
    wr_ptr_bin  <= wr_ptr_bin_next;
    wr_ptr_gray <= wr_ptr_gray_next;
    full        <= full_next;
  end

  always @(posedge rd_clk) begin
    rd_ptr_bin  <= rd_ptr_bin_next;
    rd_ptr_gray <= rd_ptr_gray_next;
    empty       <= empty_next;
  end

endmodule
