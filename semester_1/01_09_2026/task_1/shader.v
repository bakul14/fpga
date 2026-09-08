`timescale 1ns/1ps

module shader (
    input       clk,
    input       turn_request,
    output reg  ready_to_be_turned,
    output wire out0,
    output wire out1,
    output wire out2
);

  reg [1:0] state;

  initial ready_to_be_turned = 1'b1;

  always @(posedge clk) begin
    if (turn_request && ready_to_be_turned) begin
      ready_to_be_turned <= 1'b0;
      #30 ready_to_be_turned <= 1'b1;
    end
  end

  always @(posedge clk) begin

    if (turn_request && ready_to_be_turned) begin
      state <= state + 1;
      if (state >= 3) state <= 1;
    end

  end

  assign out0 = (state == 1);
  assign out1 = (state == 2);
  assign out2 = (state == 3);

  initial begin
    #10
    $display(
        "Тестовая задержка инициализации, t = %0t", $realtime
    );
    wait (turn_request);
    $display("Получен turn_request, t = %0t", $realtime);
  end

endmodule
