#include <cstdint>

#include <gtest/gtest.h>

#include "gtest_verilator_wrapper.hpp"

#include "Vasync_fifo.h"

template <typename Model>
class FifoVerilatorWrapperTest : public VerilatorWrapperTestBase<Model>
{
protected:
  void reset_() final override
  {
    this->dut_->rst = 1;
    write_(0);
    (void)read_();
    this->dut_->rst = 0;
  }

  void tick_(const size_t cycles = 1) final override {}

  void write_(const uint32_t data)
  {
    this->dut_->wr_en   = 1;
    this->dut_->wr_data = data;
    this->dut_->wr_clk  = 1;
    this->step_();
    this->settle_();
    this->dut_->wr_clk = 0;
    this->step_();
    this->settle_();
  }

  uint32_t read_()
  {
    this->dut_->rd_en  = 1;
    this->dut_->rd_clk = 1;
    this->step_();
    this->settle_();
    this->dut_->rd_clk = 0;
    this->step_();
    this->settle_();

    return this->dut_->rd_data;
  }
};

class AsyncFifoTest : public FifoVerilatorWrapperTest<Vasync_fifo>
{
};

TEST_F(AsyncFifoTest, check_startup_conditions)
{
  EXPECT_EQ(dut_->empty, 1);
  EXPECT_EQ(dut_->full, 0);
}

TEST_F(AsyncFifoTest, check_full_memory)
{
  for (uint32_t i = 0U; i < 255U; ++i) {
    write_(i);
    EXPECT_EQ(read_(), i);
  }
}
