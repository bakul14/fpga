#include <gtest/gtest.h>

#include "gtest_verilator_wrapper.hpp"

#include "Vdivide3_checker.h"

#include <bit>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>

namespace
{

std::vector<std::pair<uint64_t, bool>> values = {
  {3UL, true},
  {4UL, false},
  {5UL, false},
  {6UL, true},
  {123456UL, true},
  {18446744073709551614UL, false},
  {18446744073709551615UL, true}};

}

class Divide3CheckerTest : public SyncVerilatorWrapperTest<Vdivide3_checker>
{
protected:
  void reset_() final
  {
    dut_->clk      = 0;
    dut_->first    = 0;
    dut_->last     = 0;
    dut_->data_bit = 0;
    dut_->eval();
  }

  template <class T>
  void send_motorola_value_(const T value)
  {
    const auto bits = static_cast<std::make_unsigned_t<T>>(value);
    const int width = (bits == 0U) ? 1 : std::bit_width(bits);

    for (int i = width - 1; i >= 0; --i) {
      dut_->first    = (i == width - 1);
      dut_->last     = (i == 0);
      dut_->data_bit = (bits >> i) & 1U;
      tick_();
    }
  }
};

TEST_F(Divide3CheckerTest, check_all_numbers_in_vector)
{
  for (const auto &val : values) {
    send_motorola_value_(val.first);
    EXPECT_EQ(dut_->divisible, val.second);
  }
}
