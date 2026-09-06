#include <gtest/gtest.h>

#include "gtest_verilator_wrapper.hpp"

#include "Vshader.h"

class ShaderTest : public VerilatorWrapperTest<Vshader>
{
public:
  void reset_() override
  {
    dut_->clk          = 0;
    dut_->turn_request = 0;
    dut_->out0         = 0;
    dut_->out1         = 0;
    dut_->out2         = 0;
    dut_->eval();
  }
};

TEST_F(ShaderTest, check_silence_when_no_request)
{
  for (int i = 0; i < 1000; i++) {
    tick_();
    EXPECT_EQ(dut_->out0, 0);
    EXPECT_EQ(dut_->out1, 0);
    EXPECT_EQ(dut_->out2, 0);
  }
}

TEST_F(ShaderTest, check_shading_when_requested)
{
  dut_->turn_request = 1;

  tick_();
  EXPECT_EQ(dut_->out0, 1);
  EXPECT_EQ(dut_->out1, 0);
  EXPECT_EQ(dut_->out2, 0);

  tick_();
  EXPECT_EQ(dut_->out0, 0);
  EXPECT_EQ(dut_->out1, 1);
  EXPECT_EQ(dut_->out2, 0);

  tick_();
  EXPECT_EQ(dut_->out0, 0);
  EXPECT_EQ(dut_->out1, 0);
  EXPECT_EQ(dut_->out2, 1);

  tick_();
  EXPECT_EQ(dut_->out0, 0);
  EXPECT_EQ(dut_->out1, 0);
  EXPECT_EQ(dut_->out2, 0);
}
