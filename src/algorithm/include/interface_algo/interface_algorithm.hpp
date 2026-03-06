#pragma once
#include <frame/frame.hpp>

namespace sc {
namespace algo {
class InterfaceAlgorithm {
  public:
  virtual ~InterfaceAlgorithm() = default;
  virtual bool process(core::Frame* frame) = 0;
};
}
}