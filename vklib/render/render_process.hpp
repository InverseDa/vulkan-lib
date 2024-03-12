#ifndef VULKAN_LIB_RENDER_PROCESS_HPP
#define VULKAN_LIB_RENDER_PROCESS_HPP

#include "vulkan/vulkan.hpp"
#include "shader/shader.hpp"

namespace Vklib {
class RenderProcess final {
  public:
    // pipeline是图形渲染管线的一个实例，它定义了图形渲染的各个阶段和操作的配置。
    // 在Vulkan中，管线由多个阶段组成，包括顶点输入、顶点着色器、几何着色器、光栅化、片段着色器和输出合并等。
    // pipeline对象包含了这些阶段的配置信息，例如顶点着色器和片段着色器的着色器模块、顶点输入的格式、光栅化的方式、深度测试和混合操作等。
    // pipeline对象是图形渲染的核心，用于配置GPU的渲染操作。
    vk::Pipeline graphicsPipelineTriangleTopology = nullptr;
    vk::Pipeline graphicsPipelineLineTopology = nullptr;
    vk::RenderPass renderPass = nullptr;
    // pipelineLayout是一个Vulkan中的对象，用于定义着色器程序（Shader）和管线资源之间的接口。
    // 它描述了着色器程序所需的Uniform缓冲区、纹理和其他资源绑定点。pipelineLayout定义了着色器程序的资源布局，
    // 包括绑定顺序、描述符类型和访问权限等信息。
    // 它可以被多个pipeline共享，以确保在不同的管线中使用相同的资源布局和绑定点。
    vk::PipelineLayout layout = nullptr;

    RenderProcess();
    ~RenderProcess();

    void CreateGraphicsPipeline(const Shader& shader);
    void CreateRenderPass();

  private:
    vk::PipelineCache pipelineCache_ = nullptr;
    vk::PipelineLayout CreateLayout();
    vk::Pipeline InternalCreateGraphicsPipeline(const Shader& shader, vk::PrimitiveTopology topology);
    vk::RenderPass InternalCreateRenderPass();
    vk::PipelineCache CreatePipelineCache();
};
} // namespace Vklib

#endif // VULKAN_LIB_RENDER_PROCESS_HPP
