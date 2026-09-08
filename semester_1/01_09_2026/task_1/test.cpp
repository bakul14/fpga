#include <gtest/gtest.h>

#include "gtest_verilator_wrapper.hpp"

#include "Vshader.h"

class ShaderTest : public SyncVerilatorWrapperTest<Vshader>
{
public:
  void reset_() final
  {
    dut_->clk          = 0;
    dut_->turn_request = 0;
    dut_->eval();
    EXPECT_EQ(dut_->ready_to_be_turned, 1);
  }
};

TEST_F(ShaderTest, check_silence_when_no_request)
{
  for (int i = 0; i < 1000; i++) {
    tick_();
    while (!dut_->ready_to_be_turned) {
      tick_();
      EXPECT_EQ(dut_->out0, 0);
      EXPECT_EQ(dut_->out1, 0);
      EXPECT_EQ(dut_->out2, 0);
    }
    EXPECT_EQ(dut_->out0, 0);
    EXPECT_EQ(dut_->out1, 0);
    EXPECT_EQ(dut_->out2, 0);
  }
}

TEST_F(ShaderTest, check_silence_when_not_ready)
{
  dut_->turn_request = 1;

  dut_->clk = 1;
  step_();

  EXPECT_EQ(dut_->ready_to_be_turned, 0);
  EXPECT_EQ(dut_->out0, 1);
  EXPECT_EQ(dut_->out1, 0);
  EXPECT_EQ(dut_->out2, 0);

  for (int i = 0; i < 5; i++) {
    dut_->clk = 0;
    step_();
    dut_->clk = 1;
    step_();

    EXPECT_EQ(dut_->ready_to_be_turned, 0);
    EXPECT_EQ(dut_->out0, 1);
    EXPECT_EQ(dut_->out1, 0);
    EXPECT_EQ(dut_->out2, 0);
  }

  settle_();
  EXPECT_EQ(dut_->ready_to_be_turned, 1);
}

TEST_F(ShaderTest, check_shading_when_requested)
{
  dut_->turn_request = 1;

  for (int i = 0; i < 3; i++) {
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
  }
}

TEST_F(ShaderTest, check_busy_window)
{
  for (int i = 0; i < 10; ++i) {
    dut_->turn_request = !(dut_->turn_request);

    for (int j = 0; j < 10; ++j) {
      dut_->clk = 1;
      step_();
      EXPECT_NE(dut_->ready_to_be_turned, dut_->turn_request);
      dut_->clk = 0;
      step_();
      EXPECT_NE(dut_->ready_to_be_turned, dut_->turn_request);
    }

    settle_();
    EXPECT_EQ(dut_->ready_to_be_turned, 1);
  }
}
