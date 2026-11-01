`default_nettype none

module skid_buffer #(
    parameter int DATA_WIDTH = 32
) (
    input wire clk,
    input wire rst,

    input  wire [DATA_WIDTH-1:0] s_axis_tdata,
    input  wire                  s_axis_tvalid,
    input  wire                  s_axis_tlast,
    output wire                  s_axis_tready,

    output wire [DATA_WIDTH-1:0] m_axis_tdata,
    output wire                  m_axis_tvalid,
    output wire                  m_axis_tlast,
    input  wire                  m_axis_tready
);

  reg [DATA_WIDTH-1:0] skid_tdata;
  reg                  skid_tlast;
  reg                  skid_tvalid;

  assign s_axis_tready = !skid_tvalid;
  assign m_axis_tvalid = s_axis_tvalid || skid_tvalid;
  assign {m_axis_tdata, m_axis_tlast} = skid_tvalid ? {skid_tdata, skid_tlast} : {s_axis_tdata, s_axis_tlast};

  always @(posedge clk) begin

    if (s_axis_tready) begin
      {skid_tdata, skid_tlast} <= {s_axis_tdata, s_axis_tlast};
    end

    if (rst || m_axis_tready) begin
      skid_tvalid <= 1'b0;
    end else if (s_axis_tvalid) begin
      skid_tvalid <= 1'b1;
    end

  end

endmodule
