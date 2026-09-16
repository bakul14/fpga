`default_nettype none

// Алгоритм Гаусса:
// (a + bj)(c + dj)
// Внутренние вычисления: P1 = a * c, P2 = b * d, P3 = (a + b) * (c + d)
// Получает результат так: Re = P1 - P2,  Im = P3 - P1 - P2

module complex_mux (
    input  wire               clk,
    input  wire               in_valid,
    input  wire signed [31:0] a_in,
    input  wire signed [31:0] bj_in,
    input  wire signed [31:0] c_in,
    input  wire signed [31:0] dj_in,
    output reg                out_valid,
    output reg signed  [65:0] re_out,
    output reg signed  [65:0] im_out
);

  wire signed [32:0] a_wide = {a_in[31], a_in};
  wire signed [32:0] c_wide = {c_in[31], c_in};
  reg signed [32:0] a, bj, c, dj;

  reg signed [65:0] p1, p2;

  localparam IDLE   = 2'd0;  // ждём in_valid, с ним считаем P1 = a * c прямо со входов
  localparam MUL_2 = 2'd1;  // P2 = bj * dj
  localparam MUL_3 = 2'd2;  // P3 = (a + bj) * (c + dj)

  reg [1:0] mux_state;

  reg signed [32:0] factor_1, factor_2;

  always @(*) begin
    case (mux_state)
      MUL_2: begin
        factor_1 = bj;
        factor_2 = dj;
      end
      MUL_3: begin
        factor_1 = a + bj;
        factor_2 = c + dj;
      end
      default: begin
        factor_1 = a_wide;
        factor_2 = c_wide;
      end
    endcase
  end

  wire signed [65:0] product = factor_1 * factor_2;

  initial begin
    mux_state = IDLE;
    out_valid = 0;
  end

  always @(posedge clk) begin
    if (in_valid) begin
      a         <= a_wide;
      bj        <= {bj_in[31], bj_in};
      c         <= c_wide;
      dj        <= {dj_in[31], dj_in};
      p1        <= product;
      mux_state <= MUL_2;
    end else begin
      case (mux_state)
        MUL_2: begin
          p2    <= product;
          mux_state <= MUL_3;
        end
        MUL_3: begin
          re_out    <= p1 - p2;
          im_out    <= product - p1 - p2;
          out_valid <= 1;
          mux_state <= IDLE;
        end
        default: ;
      endcase
    end
  end

endmodule
