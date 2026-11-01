`default_nettype none

module rs_trigger (
    input  wire set,
    input  wire reset,
    output reg  q_pos,
    output reg  q_neg
);

  always_latch begin
    if (set) begin
      q_pos = 1'b1;
      q_neg = 1'b0;
    end else if (reset) begin
      q_pos = 1'b0;
      q_neg = 1'b1;
    end
  end

endmodule
