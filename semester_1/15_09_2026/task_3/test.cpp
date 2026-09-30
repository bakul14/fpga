#include <gtest/gtest.h>

#include "gtest_verilator_wrapper.hpp"

#include "Vcomplex_fast_multiplier.h"

#include <cstdint>
#include <vector>

namespace
{

struct Axiom {
  int64_t a, bi, c, di;
  int64_t re, im;
};

const std::vector<Axiom> axioms = {{1, 2, 3, 4, -5, 10}};

}  // namespace

class ComplexMuxTest : public SyncVerilatorWrapperTest<Vcomplex_fast_multiplier>
{
protected:
  void reset_() final
  {
    dut_->clk = 0;
    dut_->eval();
  }

  void set_inputs_(const Axiom &axiom)
  {
    dut_->a_in  = axiom.a;
    dut_->bi_in = axiom.bi;
    dut_->c_in  = axiom.c;
    dut_->di_in = axiom.di;
  }

  void expect_(const Axiom &axiom)
  {
    EXPECT_EQ(static_cast<int64_t>(dut_->valid_out), 1);
    EXPECT_EQ(static_cast<int64_t>(dut_->Re_out), axiom.re);
    EXPECT_EQ(static_cast<int64_t>(dut_->Im_out), axiom.im);
  }
};

TEST_F(ComplexMuxTest, stream)
{
  for (const Axiom &axiom : axioms) {
    set_inputs_(axiom);
    tick_(3);
    expect_(axiom);
  }
}
