#pragma once
#include <frame/frame.hpp>

namespace sc {
namespace devices {

class InterfaceCamera {

public:
  virtual ~InterfaceCamera() = default;

  virtual bool open() = 0;

  virtual bool start() = 0;

  virtual bool stop() = 0;

  virtual bool grabFrame(core::Frame& frame) = 0;
};
} // namespace devices
} // namespace sc
