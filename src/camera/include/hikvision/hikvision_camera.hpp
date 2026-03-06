#pragma once
#include <interface_camera/interface_camera.hpp>
#include <iostream>

namespace sc {
namespace devices {

class HikVisionCamera final : public InterfaceCamera {
public:
  bool open() override {
    std::cout << "Hikvision camera open\n";
    return true;
  }

  bool start() override {
    std::cout << "Hikvision camera start\n";
    return true;
  }

  bool stop() override {
    std::cout << "Hikvision camera stop\n";
    return true;
  }

  bool grabFrame(core::Frame &frame) override {
    std::cout << "Grab frame...\n";
    frame.format = core::PixelFormat::RGB24;
    frame.height = 400;
    frame.width = 600;
    std::cout << frame.height << "x" << frame.width << std::endl;
    return true;
  }
};

} // namespace devices

} // namespace sc