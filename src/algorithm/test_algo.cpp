//
// Created by 31305 on 2026/3/6.
//
#include <test_algo/test_algo.hpp>
#include <opencv2/opencv.hpp>

namespace sc {
namespace algo {

bool GrayscaleAlgorithm::process(core::Frame *frame) {
  if (!frame || !frame->data) {
    return false;
  }

  // 确保输入是我们支持的格式
  if (frame->format == core::PixelFormat::BGR24) {
    // 1. 零拷贝：用 cv::Mat 包装我们的裸内存
    cv::Mat img(frame->height, frame->width, CV_8UC3, frame->data,
                frame->stride);

    // 2. 灰度化处理 (OpenCV 会在内部新开辟一块叫 gray 的内存)
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    // 3. 原地覆盖写回：为了兼容下游流水线，转回 3 通道写回 frame->data
    cv::cvtColor(gray, img, cv::COLOR_GRAY2BGR);

    return true;
  }
  return false;
}
} // namespace algo
} // namespace sc