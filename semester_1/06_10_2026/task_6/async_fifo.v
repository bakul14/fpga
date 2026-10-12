module async_fifo #(
    parameter WIDTH = 32,
    parameter DEPTH = 256
) (
    input  wire             wr_clk,
    input  wire [WIDTH-1:0] wr_data,
    input  wire             wr_en,
    output wire             full,

    input  wire             rd_clk,
    output reg  [WIDTH-1:0] rd_data,
    input  wire             rd_en,
    output wire             empty,

    input wire rst
);

  reg  [WIDTH-1:0] mem                 [DEPTH-1:0];
  reg  [     31:0] wr_ptr;
  reg  [     31:0] rd_ptr;

  wire [     31:0] rd_ptr_in_wr_domain;
  wire [     31:0] wr_ptr_in_rd_domain;

  gray_sync #(
      .WIDTH(WIDTH)
  ) wr_ptr_to_rd_domain (
      .src_clk   (wr_clk),
      .binary_in (wr_ptr),
      .dst_clk   (rd_clk),
      .binary_out(wr_ptr_in_rd_domain)
  );

  gray_sync #(
      .WIDTH(WIDTH)
  ) rd_ptr_to_wr_domain (
      .src_clk   (rd_clk),
      .binary_in (rd_ptr),
      .dst_clk   (wr_clk),
      .binary_out(rd_ptr_in_wr_domain)
  );

  assign full  = (wr_ptr - rd_ptr_in_wr_domain) == DEPTH;
  assign empty = (wr_ptr_in_rd_domain == rd_ptr);

  always @(posedge wr_clk or posedge rst) begin
    if (rst) begin
      wr_ptr <= 0;
    end else if (wr_en) begin
      if (!full) begin
        mem[wr_ptr] <= wr_data;
        wr_ptr      <= wr_ptr + 1;
      end else begin
        $error("Попытка записи в полный буфер");
      end
    end
  end

  always @(posedge rd_clk or posedge rst) begin
    if (rst) begin
      rd_ptr <= 0;
    end else if (rd_en) begin
      if (!empty) begin
        rd_data <= mem[rd_ptr];
        rd_ptr  <= rd_ptr + 1;
      end else begin
        $error("Попытка чтения из пустого буфера");
      end
    end
  end

endmodule
