//
// Created by 31305 on 2026/3/5.
//

#include <memory>
#include <opencv_camera/opencv_camera.hpp>
#include <opencv_renderer/opencv_renderer.hpp>
#include <test_algo/test_algo.hpp>
#include <vision_pipeline/vision_pipeline.hpp>

int main() {
  std::cout << "Starting Industrial Vision GUI Pipeline...\n";
  try {
    const std::shared_ptr<sc::algo::InterfaceAlgorithm> g_algorithm =
        std::make_shared<sc::algo::GrayscaleAlgorithm>();
    const std::shared_ptr<sc::devices::InterfaceCamera> camera =
        std::make_shared<sc::devices::OpenCvCamera>(0);
    if (!camera->open()) {
      throw std::runtime_error("Failed to open camera hardware (Port 0)!");
    }
    sc::pipeline::VisionPipeline pipeline;
    pipeline.set_camera_source(camera);
    pipeline.add_algorithm(g_algorithm);
    pipeline.start();
    sc::ui::OpenCvRenderer renderer(pipeline.get_out_queue(),
                                    pipeline.get_frame_pool());
    renderer.set_camera_source(camera);
    renderer.run_event_loop([&pipeline] { return pipeline.is_running(); });
    std::cout << "Initiating shutdown sequence...\n";
    pipeline.stop();
    camera->stop();
  } catch (std::exception &e) {
    std::cerr << "\n[FATAL ERROR] Pipeline terminated abnormally:\n"
              << e.what() << "\n\n";
    return -1;
  } catch (...) {
    std::cerr << "\n[FATAL ERROR] An unknown exception occurred!\n\n";
    return -1;
  }
  std::cout << "Pipeline stopped smoothly. Goodbye!\n";
  return 0;
}