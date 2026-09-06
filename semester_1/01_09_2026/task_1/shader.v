module shader (
    input       clk,
    input       turn_request,
    output wire ready_to_be_turned,
    output wire out0,
    output wire out1,
    output wire out2
);

  reg [1:0] state;

  assign ready_to_be_turned = 1'b1;  // stub, always ready

  always @(posedge clk) begin

    if (turn_request && ready_to_be_turned) begin
      state <= state + 1;
      if (state >= 3) state <= 1;
    end

  end

  assign out0 = (state == 1);
  assign out1 = (state == 2);
  assign out2 = (state == 3);

endmodule
