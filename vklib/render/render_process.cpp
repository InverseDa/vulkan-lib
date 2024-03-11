#include "render_process.hpp"
#include "vklib/core/context.hpp"
#include "vklib/math/vec2.hpp"

namespace Vklib {

RenderProcess::RenderProcess() {
    layout = CreateLayout();
    CreateRenderPass();
    graphicsPipeline = nullptr;
}

RenderProcess::~RenderProcess() {
    auto& ctx = Context::GetInstance();
    ctx.device.destroyRenderPass(renderPass);
    ctx.device.destroyPipelineLayout(layout);
    ctx.device.destroyPipeline(graphicsPipeline);
}

void RenderProcess::CreateGraphicsPipeline(const Shader& shader) {
    graphicsPipeline = InternalCreateGraphicsPipeline(shader);
}

void RenderProcess::CreateRenderPass() {
    renderPass = InternalCreateRenderPass();
}

vk::PipelineLayout RenderProcess::CreateLayout() {
    vk::PipelineLayoutCreateInfo createInfo;
    auto range = Context::GetInstance().shader->GetPushConstantRange();
    createInfo
        .setSetLayouts(Context::GetInstance().shader->GetDescriptorSetLayouts())
        .setPushConstantRanges(range);

    return Context::GetInstance().device.createPipelineLayout(createInfo);
}

vk::Pipeline RenderProcess::InternalCreateGraphicsPipeline(const Shader& shader) {
    auto& ctx = Context::GetInstance();

    vk::GraphicsPipelineCreateInfo createInfo;

    // shader source
    std::array<vk::PipelineShaderStageCreateInfo, 2> stageCreateInfos;
    stageCreateInfos[0]
        .setModule(shader.GetVertexModule())
        .setPName("main")
        .setStage(vk::ShaderStageFlagBits::eVertex);
    stageCreateInfos[1]
        .setModule(shader.GetFragModule())
        .setPName("main")
        .setStage(vk::ShaderStageFlagBits::eFragment);

    // vertex input
    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    auto attribute = Vec2::GetAttributeDescription();
    auto binding = Vec2::GetBindingDescription();
    vertexInputStateCreateInfo
        .setVertexAttributeDescriptions(attribute)
        .setVertexBindingDescriptions(binding);

    // vertex assembly
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
    inputAssemblyStateCreateInfo
        .setPrimitiveRestartEnable(false)
        .setTopology(vk::PrimitiveTopology::eTriangleList);

    // viewport and scissor
    vk::PipelineViewportStateCreateInfo viewportState;
    vk::Viewport viewport(0, 0, ctx.swapchain->GetExtent().width, ctx.swapchain->GetExtent().height, 0, 1);
    vk::Rect2D scissor({0, 0}, ctx.swapchain->GetExtent());
    viewportState
        .setViewports(viewport)
        .setScissors(scissor);

    // rasterizer
    vk::PipelineRasterizationStateCreateInfo rasterInfo;
    rasterInfo
        .setRasterizerDiscardEnable(false)
        .setCullMode(vk::CullModeFlagBits::eFront)
        .setFrontFace(vk::FrontFace::eCounterClockwise)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setLineWidth(1)
        .setDepthClampEnable(false);

    // multisampler
    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
    multisampleStateCreateInfo
        .setSampleShadingEnable(false)
        .setRasterizationSamples(vk::SampleCountFlagBits::e1);

    // TODO: stencil test and depth test

    // color blending
    // src: tobe drawn color
    // dst: color already in the framebuffer ( such as background color )

    // newRGB = (srcFactor * srcRGB) <op> (dstFactor * dstRGB)
    // newA = (srcFactor * srcA) <op> (dstFactor * dstA)
    //
    // newRGB = 1 * srcRGB + (1 - srcA) * dstRGB
    // newA = srcA === 1 * srcA + 0 * dstA
    vk::PipelineColorBlendAttachmentState blendAttachmentStateInfo;
    blendAttachmentStateInfo
        .setBlendEnable(true)
        .setColorWriteMask(vk::ColorComponentFlagBits::eA |
                           vk::ColorComponentFlagBits::eB |
                           vk::ColorComponentFlagBits::eG |
                           vk::ColorComponentFlagBits::eR)
        .setSrcColorBlendFactor(vk::BlendFactor::eOne)
        .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
        .setColorBlendOp(vk::BlendOp::eAdd)
        .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
        .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
        .setAlphaBlendOp(vk::BlendOp::eAdd);

    vk::PipelineColorBlendStateCreateInfo blendStateCreateInfo;
    blendStateCreateInfo
        .setLogicOpEnable(false)
        .setAttachments(blendAttachmentStateInfo);

    // dynamic changing state of pipeline
    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo;
    std::array states = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    dynamicStateCreateInfo.setDynamicStates(states);

    // renderpass and layout
    createInfo
        .setStages(stageCreateInfos)
        .setLayout(layout)
        .setPVertexInputState(&vertexInputStateCreateInfo)
        .setPInputAssemblyState(&inputAssemblyStateCreateInfo)
        .setPViewportState(&viewportState)
        .setPRasterizationState(&rasterInfo)
        .setPMultisampleState(&multisampleStateCreateInfo)
        .setPColorBlendState(&blendStateCreateInfo)
        .setRenderPass(renderPass);
    // .setPDynamicState(&dynamicState);

    auto result = ctx.device.createGraphicsPipeline(nullptr, createInfo);
    if (result.result != vk::Result::eSuccess) {
        IO::ThrowError("Failed to create graphics graphicsPipeline");
    }

    return result.value;
}

vk::RenderPass RenderProcess::InternalCreateRenderPass() {
    auto& ctx = Context::GetInstance();

    vk::RenderPassCreateInfo createInfo;

    vk::SubpassDependency dependency;
    dependency
        .setSrcSubpass(VK_SUBPASS_EXTERNAL)
        .setDstSubpass(0)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);

    vk::AttachmentDescription attachmentDescription;
    attachmentDescription
        .setFormat(ctx.swapchain->GetFormat().format)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    vk::AttachmentReference reference;
    reference
        .setAttachment(0)
        .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription subpassDescription;
    subpassDescription
        .setColorAttachments(reference)
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

    createInfo
        .setAttachments(attachmentDescription)
        .setDependencies(dependency)
        .setSubpasses(subpassDescription);

    return ctx.device.createRenderPass(createInfo);
}

} // namespace Vklib