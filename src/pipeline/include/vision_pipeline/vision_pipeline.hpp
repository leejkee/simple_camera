#pragma once
#include <atomic>
#include <memory>
#include <thread>
#include <vector>

namespace sc {
namespace core {
class FrameLockFreeQueue;
class FramePool;
} // namespace core

namespace devices {
class InterfaceCamera;
}

namespace algo {
class InterfaceAlgorithm;
}

namespace pipeline {
class VisionPipeline {
public:
  VisionPipeline();

  void set_camera_source(std::shared_ptr<devices::InterfaceCamera> camera);

  void add_algorithm(std::shared_ptr<algo::InterfaceAlgorithm> algorithm);

  void start();

  void stop();

  bool is_running() const;

  core::FrameLockFreeQueue *get_out_queue() const;

  core::FramePool *get_frame_pool() const;

private:
  std::shared_ptr<devices::InterfaceCamera> m_camera;

  std::vector<std::shared_ptr<algo::InterfaceAlgorithm>> m_algo_node;

  std::unique_ptr<core::FramePool> m_pool;

  std::unique_ptr<core::FrameLockFreeQueue> m_capture_to_algo_queue;

  std::unique_ptr<core::FrameLockFreeQueue> m_algo_to_output_queue;

  std::atomic<bool> m_running;

  std::thread m_capture_thread;

  std::thread m_algorithm_thread;

  void captureLoop();

  void algorithmLoop();

  static constexpr size_t MAX_BUFFER_SIZE = 1920 * 1080 * 3;
};
} // namespace pipeline
} // namespace sc