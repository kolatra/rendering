#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <engine/graphics/metal/MetalRenderer.hpp>
#include <spdlog/spdlog.h>

namespace Luna::Metal {
     void Implementation::init(IWindow *window, Configuration *config) {
          _pDevice = MTL::CreateSystemDefaultDevice();

          throw std::runtime_error("Metal is not ready to run yet!");
     }

     Implementation::~Implementation() {
     }

     void Implementation::beginFrame() {
     }

     void Implementation::endFrame() {
     }

     void Implementation::clear(const glm::vec4 &colour) {
     }
}
