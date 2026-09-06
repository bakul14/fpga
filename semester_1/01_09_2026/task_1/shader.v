module shader (
    input       clk,
    input       turn_request,
    output wire out0,
    output wire out1,
    output wire out2
);

  reg [1:0] state;

  always @(posedge clk) begin

    if (turn_request) begin
      state <= state + 1;
      if (state >= 3) state <= 0;
    end

  end

  assign out0 = (state == 1);
  assign out1 = (state == 2);
  assign out2 = (state == 3);

endmodule
