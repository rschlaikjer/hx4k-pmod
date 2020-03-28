#pragma once

namespace FPGA {
void init();
void assert_reset();
void deassert_reset();
bool is_reset();
} // namespace FPGA
