#include <gtest/gtest.h>

#include "gtest_verilator_wrapper.hpp"

#include "Vskid_buffer.h"

#include <cstdint>
#include <cstdlib>

constexpr uint32_t k_words_count = 100U;

class SkidBufferTest : public SyncVerilatorWrapperTest<Vskid_buffer>
{
protected:
  void reset_() final
  {
    dut_->clk           = 0;
    dut_->s_axis_tvalid = 0;
    dut_->m_axis_tready = 0;
    dut_->rst           = 1;
    tick_();
    dut_->rst = 0;
  }
};

TEST_F(SkidBufferTest, startup_conditions)
{
  EXPECT_EQ(dut_->m_axis_tvalid, 0);
  EXPECT_EQ(dut_->s_axis_tready, 1);
}

TEST_F(SkidBufferTest, passthrough)
{
  dut_->s_axis_tvalid = 1;
  dut_->m_axis_tready = 1;

  for (uint32_t word = 0U; word < k_words_count; ++word) {
    dut_->s_axis_tdata = word;
    dut_->s_axis_tlast = (word % 4U == 3U);
    tick_();

    EXPECT_EQ(dut_->m_axis_tvalid, 1);
    EXPECT_EQ(dut_->m_axis_tdata, word);
    EXPECT_EQ(dut_->m_axis_tlast, word % 4U == 3U);
    EXPECT_EQ(dut_->s_axis_tready, 1);
  }
}

TEST_F(SkidBufferTest, backpressure)
{
  dut_->s_axis_tvalid = 1;
  dut_->m_axis_tready = 0;

  dut_->s_axis_tdata = 10;
  tick_();
  dut_->s_axis_tdata = 11;
  tick_();
  dut_->s_axis_tdata = 12;
  tick_(3);

  EXPECT_EQ(dut_->s_axis_tready, 0);
  EXPECT_EQ(dut_->m_axis_tvalid, 1);
  EXPECT_EQ(dut_->m_axis_tdata, 10);

  dut_->m_axis_tready = 1;
  tick_();
  EXPECT_EQ(dut_->m_axis_tdata, 11);
  tick_();
  EXPECT_EQ(dut_->m_axis_tdata, 12);
}

TEST_F(SkidBufferTest, random_ready)
{
  uint32_t next_input  = 0U;
  uint32_t next_output = 0U;

  dut_->s_axis_tvalid = 1;

  while (next_output < k_words_count) {
    dut_->s_axis_tdata  = next_input;
    dut_->m_axis_tready = std::rand() % 2;

    if (dut_->m_axis_tvalid && dut_->m_axis_tready) {
      EXPECT_EQ(dut_->m_axis_tdata, next_output);
      ++next_output;
    }
    if (dut_->s_axis_tready) { ++next_input; }

    tick_();
  }
}
