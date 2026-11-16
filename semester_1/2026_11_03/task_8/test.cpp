#include <gtest/gtest.h>

#include "gtest_verilator_wrapper.hpp"

#include "Vlru_cache.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <deque>

constexpr size_t k_depth = 4U;

class LruCacheTest : public SyncVerilatorWrapperTest<Vlru_cache>
{
protected:
  void reset_() final
  {
    dut_->clk      = 0;
    dut_->valid_in = 0;
    dut_->rst      = 1;
    step_();
    tick_();
    dut_->rst = 0;
  }

  void push_(const uint32_t value)
  {
    dut_->valid_in = 1;
    dut_->data_in  = value;
    tick_();
    dut_->valid_in = 0;
  }

  void expect_out_(const std::array<uint32_t, k_depth> &expected)
  {
    for (size_t i = 0U; i < k_depth; ++i) {
      EXPECT_EQ(dut_->data_out[i], expected[i]);
    }
  }
};

TEST_F(LruCacheTest, startup_conditions)
{
  expect_out_({0, 0, 0, 0});
}

TEST_F(LruCacheTest, board_example)
{
  push_(1);
  expect_out_({1, 0, 0, 0});
  push_(2);
  expect_out_({2, 1, 0, 0});
  push_(3);
  expect_out_({3, 2, 1, 0});
  push_(1);
  expect_out_({1, 3, 2, 0});
  push_(4);
  expect_out_({4, 1, 3, 2});
  push_(3);
  expect_out_({3, 4, 1, 2});
  push_(2);
  expect_out_({2, 3, 4, 1});
}

TEST_F(LruCacheTest, repeat_mru_holds)
{
  push_(5);
  push_(7);
  push_(7);
  push_(7);
  expect_out_({7, 5, 0, 0});
}

TEST_F(LruCacheTest, no_valid_no_change)
{
  push_(1);
  push_(2);
  dut_->data_in = 9;
  tick_(5);
  expect_out_({2, 1, 0, 0});
}

TEST_F(LruCacheTest, random_vs_model)
{
  std::deque<uint32_t> model(k_depth, 0U);

  for (int step = 0; step < 10'000; ++step) {
    const uint32_t value = std::rand() % 8U;
    push_(value);

    const auto it = std::find(model.begin(), model.end(), value);
    model.erase(it != model.end() ? it : model.end() - 1);
    model.push_front(value);

    for (size_t i = 0U; i < k_depth; ++i) {
      ASSERT_EQ(dut_->data_out[i], model[i]);
    }
  }
}
