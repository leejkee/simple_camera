//
// Created by 31305 on 2026/3/7.
//
#include <chrono>
#include <frame/frame.hpp>
#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include <opencv_renderer/opencv_renderer.hpp>
#include <string>
#include <thread>

namespace sc {
namespace ui {
void OpenCvRenderer::run_event_loop(
    const std::function<bool()>& check_upstream_alive) {
  const std::string window_name = "Industrial Vision Standard UI";
  cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);

  int processed_frames = 0;
  auto last_time = std::chrono::steady_clock::now();

  while (check_upstream_alive()) {
    core::Frame *frame = m_in_queue->pop();
    if (frame) {
      // 渲染画面
      cv::Mat img(frame->height, frame->width, CV_8UC3, frame->data,
                  frame->stride);
      cv::imshow(window_name, img);

      // 处理窗口事件
      int key = cv::waitKey(1);
      if (key == 27) { // ESC
        std::cout << "[UI] ESC pressed. Exiting UI loop.\n";
        break; // 直接跳出 UI 循环
      }

      // 必须归还内存给流水线引擎
      m_pool->release(frame);

      auto now = std::chrono::steady_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time)
              .count();
      if (duration >= 1000) {
        std::cout << "[UI Thread] "
                  << "Pipeline FPS: " << processed_frames
                  << " | Camera Hardware FPS: " << m_camera->getFPS()
                  << std::endl;
        processed_frames = 0;
        last_time = now;
      }
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
  cv::destroyWindow(window_name);
}

void OpenCvRenderer::set_camera_source(
    std::shared_ptr<devices::InterfaceCamera> camera) {
  m_camera = std::move(camera);
}
} // namespace ui
} // namespace sc