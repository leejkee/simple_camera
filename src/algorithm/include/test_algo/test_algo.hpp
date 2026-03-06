#pragma once
#include <interface_algo/interface_algorithm.hpp>

namespace sc {
namespace algo {

class GrayscaleAlgorithm final : public InterfaceAlgorithm {
public:
  bool process(core::Frame* frame) override;
};

} // namespace algo
} // namespace sc