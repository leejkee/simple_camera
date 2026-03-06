//
// Created by 31305 on 2026/3/5.
//
#include <opencv_camera/opencv_camera.hpp>
namespace sc {
namespace devices {

OpenCvCamera::OpenCvCamera(const int camera_index) : m_camera_index(camera_index),
m_frame_count(0), m_current_fps(0.0){
}

double OpenCvCamera::getFPS() const {
  return m_current_fps.load(std::memory_order_relaxed);
}

bool OpenCvCamera::grabFrame(core::Frame &frame) {
  if (!m_cap.isOpened()) {
    return false;
  }

  cv::Mat mat;

  if (!m_cap.read(mat) || mat.empty()) {
    return false;
  }

  size_t expected_size = mat.total() * mat.elemSize();
  if (frame.capacity < expected_size) {
    std::cout << "[Camera] Warning: Frame buffer too small, reallocating...\n";
    frame.allocate(expected_size);
  }

  // 将 OpenCV 底层的数据拷贝到我们的自定义 Frame 中
  // 注意：这是整个流水线中唯一的一次数据拷贝，后续全部是指针传递
  std::memcpy(frame.data, mat.data, expected_size);

  // 填充元数据
  frame.width = mat.cols;
  frame.height = mat.rows;
  frame.stride = mat.step; // 每行的字节数
  frame.format = core::PixelFormat::BGR24;

  // 更新并计算真实 FPS
  m_frame_count++;
  auto now = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start_time).count();
  if (duration >= 1000) {
    m_current_fps.store(m_frame_count * 1000.0 / duration, std::memory_order_relaxed);
    m_frame_count = 0;
    m_start_time = now;
  }

  return true;
}

bool OpenCvCamera::open() {
  if (!m_cap.open(m_camera_index, cv::CAP_MSMF)) {
    std::cerr << "Failed to open camera " << m_camera_index << std::endl;
    return false;
  }

  std::cout << "[Camera] Opened successfully. "
            << "Resolution: " << m_cap.get(cv::CAP_PROP_FRAME_WIDTH) << "x"
            << m_cap.get(cv::CAP_PROP_FRAME_HEIGHT) << " @ "
            << m_cap.get(cv::CAP_PROP_FPS) << " FPS\n";
  return true;
}

bool OpenCvCamera::start() {
  m_start_time = std::chrono::steady_clock::now();
  m_frame_count = 0;
  m_current_fps = 0.0;
  std::cout << "[Camera] Started capturing.\n";
  return true;
}

bool OpenCvCamera::stop() {
  if (m_cap.isOpened()) {
    m_cap.release();
    std::cout << "[Camera] Stopped capturing.\n";
  }
  return true;
}
} // namespace devices

}