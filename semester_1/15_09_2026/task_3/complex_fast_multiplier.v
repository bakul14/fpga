module complex_fast_multiplier (
    input  wire               clk,
    input  reg signed  [31:0] a_in,
    input  reg signed  [31:0] bi_in,
    input  reg signed  [31:0] c_in,
    input  reg signed  [31:0] di_in,
    output wire signed [63:0] Re_out,
    output wire signed [63:0] Im_out,
    output reg                valid_out
);

  localparam [1:0] STAGE_P1 = 2'b00, STAGE_P2 = 2'b01, STAGE_P3 = 2'b10;

  reg [1:0] stage = STAGE_P1;

  reg signed [63:0] P1, P2, P3;

  assign Re_out = P1 - P2;
  assign Im_out = P3 - P2 - P1;

  task update_stage;
    case (stage)
      STAGE_P1: begin
        stage <= STAGE_P2;
      end
      STAGE_P2: begin
        stage <= STAGE_P3;
      end
      STAGE_P3: begin
        stage <= STAGE_P1;
      end
      default: begin
        stage <= STAGE_P1;
      end
    endcase
  endtask

  task step_multiply;
    case (stage)
      STAGE_P1: begin
        P1 <= a_in * c_in;
      end
      STAGE_P2: begin
        P2 <= bi_in * di_in;
      end
      STAGE_P3: begin
        P3 <= (64'(a_in) + 64'(bi_in)) * (64'(c_in) + 64'(di_in));
      end
      default: begin
      end
    endcase
  endtask

  task update_output;
    if (STAGE_P3 == stage) begin
      valid_out <= 1'b1;
    end else begin
      valid_out <= 1'b0;
    end

  endtask

  always @(posedge clk) begin
    update_stage();
    step_multiply();
    update_output();
  end

endmodule
