#include <gtest/gtest.h>

#include <cstdint>

#include "Vcounter.h"
#include "gtest_verilator_wrapper.hpp"

namespace
{

constexpr uint32_t k_max_count = 15U;

class CounterTest : public VerilatorWrapperTest<Vcounter>
{
protected:
  void reset_() override
  {
    dut_->rst = 1;
    tick_();
    dut_->rst = 0;
  }

  uint32_t count_() const { return static_cast<uint32_t>(dut_->count); }
};

TEST_F(CounterTest, reset_clears_count)
{ EXPECT_EQ(0U, count_()); }

TEST_F(CounterTest, increments_on_every_clock)
{
  for (uint32_t expected = 1U; expected <= k_max_count; ++expected) {
    tick_();
    EXPECT_EQ(expected, count_());
  }
}

TEST_F(CounterTest, wraps_around_after_max_value)
{
  tick_(k_max_count);
  ASSERT_EQ(k_max_count, count_());

  tick_();
  EXPECT_EQ(0U, count_());
}

TEST_F(CounterTest, reset_works_in_the_middle_of_counting)
{
  tick_(5);
  ASSERT_EQ(5, count_());

  reset_();
  EXPECT_EQ(0U, count_());
}

}  // namespace
