#include <gtest/gtest.h>

#include "gtest_verilator_wrapper.hpp"

#include "Vshader.h"

class ShaderTest : public SyncVerilatorWrapperTest<Vshader>
{
public:
  void reset_() override
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

  tick_();
  while (!dut_->ready_to_be_turned) {
    EXPECT_EQ(dut_->out0, 1);
    EXPECT_EQ(dut_->out1, 0);
    EXPECT_EQ(dut_->out2, 0);
    tick_();
  }

  tick_();
  while (!dut_->ready_to_be_turned) {
    EXPECT_EQ(dut_->out0, 0);
    EXPECT_EQ(dut_->out1, 1);
    EXPECT_EQ(dut_->out2, 0);
    tick_();
  }

  tick_();
  while (!dut_->ready_to_be_turned) {
    EXPECT_EQ(dut_->out0, 0);
    EXPECT_EQ(dut_->out1, 0);
    EXPECT_EQ(dut_->out2, 1);
    tick_();
  }
}

TEST_F(ShaderTest, check_shading_when_requested)
{
  dut_->turn_request = 1;

  for (int i = 0; i < 3; i++) {
    tick_();
    while (!dut_->ready_to_be_turned);
    EXPECT_EQ(dut_->out0, 1);
    EXPECT_EQ(dut_->out1, 0);
    EXPECT_EQ(dut_->out2, 0);

    tick_();
    while (!dut_->ready_to_be_turned);
    EXPECT_EQ(dut_->out0, 0);
    EXPECT_EQ(dut_->out1, 1);
    EXPECT_EQ(dut_->out2, 0);

    tick_();
    while (!dut_->ready_to_be_turned);
    EXPECT_EQ(dut_->out0, 0);
    EXPECT_EQ(dut_->out1, 0);
    EXPECT_EQ(dut_->out2, 1);
  }
}
