#include <gtest/gtest.h>

#include "gtest_verilator_wrapper.hpp"

#include "Vrs_trigger.h"

class RSTriggerTest : public AsyncVerilatorWrapperTest<Vrs_trigger>
{
protected:
  void reset_() override
  {
    dut_->set   = 0;
    dut_->reset = 0;
    step_();
  }
};

TEST_F(RSTriggerTest, check_setting)
{
  dut_->set   = 1;
  dut_->reset = 0;
  step_();
  EXPECT_EQ(dut_->q_pos, 1);
  EXPECT_EQ(dut_->q_neg, 0);

  dut_->set = 0;
  step_();

  dut_->set = 1;
  step_();
  EXPECT_EQ(dut_->q_pos, 1);
  EXPECT_EQ(dut_->q_neg, 0);
}

TEST_F(RSTriggerTest, check_resetting)
{
  dut_->set   = 0;
  dut_->reset = 1;
  step_();
  EXPECT_EQ(dut_->q_pos, 0);
  EXPECT_EQ(dut_->q_neg, 1);

  dut_->reset = 0;
  step_();

  dut_->reset = 1;
  step_();
  EXPECT_EQ(dut_->q_pos, 0);
  EXPECT_EQ(dut_->q_neg, 1);
}

TEST_F(RSTriggerTest, check_ub_by_design)
{
  dut_->set   = 1;
  dut_->reset = 1;
  step_();
  EXPECT_EQ(dut_->q_pos, 1);
  EXPECT_EQ(dut_->q_neg, 0);
}

TEST_F(RSTriggerTest, check_set_reset_sequence_without_pause)
{
  for (int i = 0; i < 3; i++) {
    dut_->set   = 1;
    dut_->reset = 0;
    step_();
    EXPECT_EQ(dut_->q_pos, 1);
    EXPECT_EQ(dut_->q_neg, 0);

    dut_->set   = 0;
    dut_->reset = 1;
    step_();
    EXPECT_EQ(dut_->q_pos, 0);
    EXPECT_EQ(dut_->q_neg, 1);
  }
}

TEST_F(RSTriggerTest, check_reset_set_sequence_without_pause)
{
  for (int i = 0; i < 3; i++) {
    dut_->set   = 0;
    dut_->reset = 1;
    step_();
    EXPECT_EQ(dut_->q_pos, 0);
    EXPECT_EQ(dut_->q_neg, 1);

    dut_->set   = 1;
    dut_->reset = 0;
    step_();
    EXPECT_EQ(dut_->q_pos, 1);
    EXPECT_EQ(dut_->q_neg, 0);
  }
}

TEST_F(RSTriggerTest, check_set_reset_sequence_with_pause)
{
  for (int i = 0; i < 3; i++) {
    dut_->set   = 1;
    dut_->reset = 0;
    step_();
    EXPECT_EQ(dut_->q_pos, 1);
    EXPECT_EQ(dut_->q_neg, 0);

    dut_->set   = 0;
    dut_->reset = 0;
    step_();

    dut_->set   = 0;
    dut_->reset = 1;
    step_();
    EXPECT_EQ(dut_->q_pos, 0);
    EXPECT_EQ(dut_->q_neg, 1);

    dut_->set   = 0;
    dut_->reset = 0;
    step_();
  }
}

TEST_F(RSTriggerTest, check_reset_set_sequence_with_pause)
{
  for (int i = 0; i < 3; i++) {
    dut_->set   = 0;
    dut_->reset = 1;
    step_();
    EXPECT_EQ(dut_->q_pos, 0);
    EXPECT_EQ(dut_->q_neg, 1);

    dut_->set   = 0;
    dut_->reset = 0;
    step_();

    dut_->set   = 1;
    dut_->reset = 0;
    step_();
    EXPECT_EQ(dut_->q_pos, 1);
    EXPECT_EQ(dut_->q_neg, 0);

    dut_->set   = 0;
    dut_->reset = 0;
    step_();
  }
}
