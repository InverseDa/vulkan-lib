#include "render_process.hpp"
#include "vklib/shader/shader.hpp"
#include "vklib/core/context.hpp"

namespace Vklib {
void RenderProcess::InitPipeline(int width, int height) {
    vk::GraphicsPipelineCreateInfo createInfo;

    // vertex input
    vk::PipelineVertexInputStateCreateInfo inputState;
    createInfo.setPVertexInputState(&inputState);

    // vertex assembly
    vk::PipelineInputAssemblyStateCreateInfo inputAsm;
    inputAsm
        .setPrimitiveRestartEnable(false)
        .setTopology(vk::PrimitiveTopology::eTriangleList);
    createInfo.setPInputAssemblyState(&inputAsm);

    // shader
    auto stages = Shader::GetInstance().GetStage();
    createInfo.setStages(stages);

    // viewport
    vk::PipelineViewportStateCreateInfo viewportState;
    vk::Viewport viewport(0, 0, width, height, 0, 1);
    viewportState.setViewports(viewport);
    vk::Rect2D rect({0, 0}, {static_cast<uint32_t>(width), static_cast<uint32_t>(height)});
    viewportState.setScissors(rect);
    createInfo.setPViewportState(&viewportState);

    // rasterization
    vk::PipelineRasterizationStateCreateInfo rastInfo;
    rastInfo
        .setRasterizerDiscardEnable(false)
        .setCullMode(vk::CullModeFlagBits::eBack)
        .setFrontFace(vk::FrontFace::eCounterClockwise)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setLineWidth(1);
    createInfo.setPRasterizationState(&rastInfo);

    // multisample
    vk::PipelineMultisampleStateCreateInfo multisample;
    multisample
        .setSampleShadingEnable(false)
        .setRasterizationSamples(vk::SampleCountFlagBits::e1);
    createInfo.setPMultisampleState(&multisample);

    // TODO: stencil test and depth test

    // color blending
    vk::PipelineColorBlendStateCreateInfo blend;
    vk::PipelineColorBlendAttachmentState attachs;
    attachs
        .setBlendEnable(false)
        .setColorWriteMask(vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eR);
    blend
        .setLogicOpEnable(false)
        .setAttachments(attachs);
    createInfo.setPColorBlendState(&blend);

    // renderpass and layout
    createInfo
        .setRenderPass(renderPass)
        .setLayout(layout);

    auto result = Context::GetInstance().device.createGraphicsPipeline(nullptr, createInfo);
    if (result.result != vk::Result::eSuccess) {
        IO::ThrowError("Failed to create graphics pipeline");
    }
    pipeline = result.value;
}

void RenderProcess::InitLayout() {
    vk::PipelineLayoutCreateInfo createInfo;
    layout = Context::GetInstance().device.createPipelineLayout(createInfo);
}

void RenderProcess::InitRenderPass() {
    vk::RenderPassCreateInfo createInfo;
    vk::AttachmentDescription attachmentDescription;
    attachmentDescription
        .setFormat(Context::GetInstance().swapchain->info.format.format)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setSamples(vk::SampleCountFlagBits::e1);
    createInfo.setAttachments(attachmentDescription);

    vk::AttachmentReference ref;
    ref
        .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setAttachment(0);
    vk::SubpassDescription subpass;
    subpass
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
        .setColorAttachments(ref);
    createInfo.setSubpasses(subpass);

    vk::SubpassDependency dependency;
    dependency
        .setSrcSubpass(VK_SUBPASS_EXTERNAL)
        .setDstSubpass(0)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    createInfo.setDependencies(dependency);

    renderPass = Context::GetInstance().device.createRenderPass(createInfo);
}

RenderProcess::~RenderProcess() {
    auto& device = Context::GetInstance().device;
    device.destroyRenderPass(renderPass);
    device.destroyPipelineLayout(layout);
    device.destroyPipeline(pipeline);
}

} // namespace Vklib