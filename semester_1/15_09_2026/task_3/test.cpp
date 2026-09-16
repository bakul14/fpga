#include <gtest/gtest.h>

#include "gtest_verilator_wrapper.hpp"

#include "Vcomplex_mux.h"

#include <cstdint>
#include <vector>

namespace
{

// (a + bj)(c + dj) = re + im j
struct Axiom {
  int32_t a, bj, c, dj;
  int64_t re, im;
};

const std::vector<Axiom> axioms = {
  {1, 2, 3, 4, -5, 10},
  {0, 0, 5, -7, 0, 0},
  {1, 0, 5, -7, 5, -7},
  {0, 1, 5, -7, 7, 5},
  {-3, 4, -3, -4, 25, 0},
  {1 << 30, 1 << 30, 1 << 30, -(1 << 30), 1LL << 61, 0},
  {-(1 << 30), -(1 << 30), -(1 << 30), -(1 << 30), 0, 1LL << 61}};

}  // namespace

class ComplexMuxTest : public SyncVerilatorWrapperTest<Vcomplex_mux>
{
protected:
  void reset_() final
  {
    dut_->clk      = 0;
    dut_->in_valid = 0;
    dut_->eval();
  }

  // Подаёт (a + bj)(c + dj) и делает один такт.
  void send_(const Axiom &axiom)
  {
    dut_->in_valid = 1;
    dut_->a_in     = axiom.a;
    dut_->bj_in    = axiom.bj;
    dut_->c_in     = axiom.c;
    dut_->dj_in    = axiom.dj;
    tick_();
    dut_->in_valid = 0;
  }

  static int64_t low64_(const VlWide<3> &w)
  { return static_cast<int64_t>(w[0] | static_cast<uint64_t>(w[1]) << 32); }

  void expect_(const Axiom &axiom)
  {
    EXPECT_EQ(dut_->out_valid, 1);
    EXPECT_EQ(low64_(dut_->re_out), axiom.re);
    EXPECT_EQ(low64_(dut_->im_out), axiom.im);
  }
};

TEST_F(ComplexMuxTest, stream)
{
  for (const Axiom &axiom : axioms) {
    send_(axiom);
    tick_(2);
    expect_(axiom);
  }
}
