//
// Created by 31305 on 2026/3/6.
//
#include <vision_pipeline/vision_pipeline.hpp>
#include <chrono>
#include <frame/frame_lock_free_queue.hpp>
#include <frame/frame_pool.hpp>
#include <interface_algo/interface_algorithm.hpp>
#include <interface_camera/interface_camera.hpp>
#include <iostream>
#include <stdexcept>

namespace sc {
namespace pipeline {
VisionPipeline::VisionPipeline() {
  m_pool = std::make_unique<core::FramePool>();
  if (!m_pool->initPool(10, MAX_BUFFER_SIZE)) {
    std::cerr << "Fatal: Failed to initialize frame pool.\n";
    throw std::runtime_error("Failed to initialize frame pool.");
  }
  m_capture_to_algo_queue = std::make_unique<core::FrameLockFreeQueue>(5);
  m_algo_to_output_queue = std::make_unique<core::FrameLockFreeQueue>(5);
}

VisionPipeline::~VisionPipeline() = default;

void VisionPipeline::set_camera_source(
    std::shared_ptr<devices::InterfaceCamera> camera) {
  m_camera = std::move(camera);
}

void VisionPipeline::add_algorithm(
    std::shared_ptr<algo::InterfaceAlgorithm> algorithm) {
  // do nothing
  m_algo_node.push_back(std::move(algorithm));
}

void VisionPipeline::start() {
  if (!m_camera) {
    throw std::runtime_error("Cannot start pipeline without a camera!");
  }
  if (!m_camera->start()) {
    throw std::runtime_error("Fatal: Camera opened, but failed to start video stream!");
  }
  m_running = true;
  m_capture_thread = std::thread(&VisionPipeline::captureLoop, this);
  m_algorithm_thread = std::thread(&VisionPipeline::algorithmLoop, this);
}

void VisionPipeline::stop() {
  if (!m_running) {
    return;
  }
  m_running = false;
  if (m_capture_thread.joinable()) {
    m_capture_thread.join();
  }
  if (m_algorithm_thread.joinable()) {
    m_algorithm_thread.join();
  }
  if (m_camera) {
    m_camera->stop();
  }
}

void VisionPipeline::captureLoop() {
  while (m_running) {
    core::Frame *frame = m_pool->acquire();
    if (!frame) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      continue;
    }

    if (m_camera->grabFrame(*frame)) {
      frame->timestamp =
          std::chrono::duration_cast<std::chrono::milliseconds>(
              std::chrono::system_clock::now().time_since_epoch())
              .count();

      if (!m_capture_to_algo_queue->push(frame)) {
        m_pool->release(frame);
      }
    } else {
      m_pool->release(frame);
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
}

void VisionPipeline::algorithmLoop() {
  while (m_running) {
    core::Frame *frame = m_capture_to_algo_queue->pop();
    if (frame) {
      for (const auto &algo : m_algo_node) {
        algo->process(frame);
      }

      if (!m_algo_to_output_queue->push(frame)) {
        m_pool->release(frame); // discard this frame
      }
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
}

core::FrameLockFreeQueue* VisionPipeline::get_out_queue() const {
  return m_algo_to_output_queue.get();
}

core::FramePool* VisionPipeline::get_frame_pool() const {
  return m_pool.get();
}

bool VisionPipeline::is_running() const {
  return m_running.load(std::memory_order_relaxed);
}

} // namespace pipeline
} // namespace sc