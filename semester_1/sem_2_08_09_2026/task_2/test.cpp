#include <gtest/gtest.h>

#include "gtest_verilator_wrapper.hpp"

#include "Vdivide3_checker.h"

#include <bit>
#include <cstdint>
#include <type_traits>
#include <vector>

std::vector<std::pair<__uint128_t, bool>> values = {
  {3ULL, true},
  {4ULL, false},
  {5ULL, false},
  {6ULL, true},
  {123456ULL, true},
  {18446744073709551614ULL, false},
  {18446744073709551615ULL, true},
  /* {static_cast<__uint128_t>(340282366920938463463374607431768211455), true} */};  // для 128 не
                                                                                     // компилируется

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
