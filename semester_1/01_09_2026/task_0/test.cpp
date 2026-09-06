#include <gtest/gtest.h>

#include <cstdint>
#include <format>

#include "Vcounter.h"
#include "gtest_verilator_wrapper.hpp"

namespace
{

constexpr uint32_t k_width         = 4U;
constexpr uint32_t k_max_count     = (1U << k_width) - 1U;
constexpr uint32_t k_period        = k_max_count + 1U;
constexpr uint32_t k_partial_count = 5U;
constexpr uint32_t k_reset_cycles  = 3U;

class CounterTest : public SyncVerilatorWrapperTest<Vcounter>
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
    SCOPED_TRACE(std::format("такт {} из {}", expected, k_max_count));
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

TEST_F(CounterTest, continues_counting_after_wrap_around)
{
  tick_(k_period);
  ASSERT_EQ(0U, count_());

  tick_();
  EXPECT_EQ(1U, count_());
}

TEST_F(CounterTest, holds_zero_while_reset_is_asserted)
{
  tick_(k_partial_count);
  ASSERT_EQ(k_partial_count, count_());

  dut_->rst = 1;

  for (uint32_t cycle = 1U; cycle <= k_reset_cycles; ++cycle) {
    SCOPED_TRACE(std::format("такт удержания сброса {} из {}", cycle, k_reset_cycles));
    tick_();
    EXPECT_EQ(0U, count_());
  }

  dut_->rst = 0;
}

TEST_F(CounterTest, reset_works_in_the_middle_of_counting)
{
  tick_(k_partial_count);
  ASSERT_EQ(k_partial_count, count_());

  reset_();
  EXPECT_EQ(0U, count_());
}

}  // namespace
