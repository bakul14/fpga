#include <gtest/gtest.h>

#include "gtest_verilator_wrapper.hpp"

#include "Vdivide3_checker.h"

#include <bit>
#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace
{

std::vector<std::pair<uint64_t, bool>> axioms = {
  {3UL, true},
  {4UL, false},
  {5UL, false},
  {6UL, true},
  {123456UL, true},
  {18446744073709551614UL, false},
  {18446744073709551615UL, true}};

const std::string value_name(const testing::TestParamInfo<std::pair<uint64_t, bool>> &info)
{ return "value_" + std::to_string(info.param.first); }

}  // namespace

class Divide3CheckerTest : public SyncVerilatorWrapperTest<Vdivide3_checker>,
                           public ::testing::WithParamInterface<std::pair<uint64_t, bool>>
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
    const T bits    = static_cast<std::make_unsigned_t<T>>(value);
    const int width = (bits == 0U) ? 1 : std::bit_width(bits);

    for (int i = width - 1; i >= 0; --i) {
      dut_->first    = (i == width - 1);
      dut_->last     = (i == 0);
      dut_->data_bit = (bits >> i) & 1U;
      tick_();
    }
  }
};

TEST_P(Divide3CheckerTest, check)
{
  send_motorola_value_(GetParam().first);
  EXPECT_EQ(dut_->divisible, GetParam().second);
}

INSTANTIATE_TEST_SUITE_P(divide3, Divide3CheckerTest, ::testing::ValuesIn(axioms), value_name);
