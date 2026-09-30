`default_nettype none

module iir_filter #(
    parameter signed [63:0] Y_coeff = 3,
    parameter signed [63:0] X_coeff = 2
) (
    input  wire               clk,
    input  wire signed [31:0] data_in,
    output reg signed  [63:0] data_out,
    output wire               valid_out
);

  localparam signed [63:0] Y_coeff_squared = Y_coeff * Y_coeff;
  localparam signed [63:0] X_prev_coeff = Y_coeff * X_coeff;
  localparam LATENCY = 4;

  reg signed [63:0] x_prev, x_products_sum;
  reg [LATENCY-1:0] valid_pipeline;

  wire signed [63:0] x_curr_product, x_prev_product, y_prev_product;
  wire signed [63:0] x_curr = 64'(data_in);
  wire signed [63:0] y_next = y_prev_product + x_products_sum;

  assign valid_out = valid_pipeline[LATENCY-1];

  initial begin
    x_prev         = 0;
    x_products_sum = 0;
    data_out       = 0;
    valid_pipeline = 0;
  end

  two_cycle_multiplier x_curr_multiplier (
      .clk        (clk),
      .coeff_in   (X_coeff),
      .value_in   (x_curr),
      .product_out(x_curr_product)
  );

  two_cycle_multiplier x_prev_multiplier (
      .clk        (clk),
      .coeff_in   (X_prev_coeff),
      .value_in   (x_prev),
      .product_out(x_prev_product)
  );

  two_cycle_multiplier y_prev_multiplier (
      .clk        (clk),
      .coeff_in   (Y_coeff_squared),
      .value_in   (y_next),
      .product_out(y_prev_product)
  );

  always @(posedge clk) begin
    x_prev         <= x_curr;
    x_products_sum <= x_curr_product + x_prev_product;
    data_out       <= y_next;
    valid_pipeline <= {valid_pipeline[LATENCY-2:0], 1'b1};
  end

endmodule
