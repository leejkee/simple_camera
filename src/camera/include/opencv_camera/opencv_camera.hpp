#pragma once
#include <atomic>
#include <chrono>
#include <interface_camera/interface_camera.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>

namespace sc {
namespace devices {

class OpenCvCamera final : public InterfaceCamera {
public:
  OpenCvCamera(int camera_index = 0);
  ~OpenCvCamera() override { stop(); }

  bool open() override;
  bool start() override;
  bool stop() override;
  bool grabFrame(core::Frame &frame) override;
  double getFPS() const override;

private:
  int m_camera_index;
  cv::VideoCapture m_cap;

  std::chrono::steady_clock::time_point m_start_time;
  int m_frame_count;
  std::atomic<double> m_current_fps;
};
} // namespace devices
} // namespace sc