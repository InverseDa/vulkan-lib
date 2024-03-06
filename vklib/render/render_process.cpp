#include "render_process.hpp"
#include "vklib/core/context.hpp"
#include "vklib/mesh/vertex.hpp"

namespace Vklib {

RenderProcess::RenderProcess() {
    layout = CreateLayout();
    renderPass = CreateRenderPass();
    graphicsPipeline = nullptr;
}

RenderProcess::~RenderProcess() {
    auto& ctx = Context::GetInstance();
    ctx.device.destroyRenderPass(renderPass);
    ctx.device.destroyPipelineLayout(layout);
    ctx.device.destroyPipeline(graphicsPipeline);
}

void RenderProcess::RecreateGraphicsPipeline(const std::vector<char>& vertexSource, const std::vector<char>& fragSource) {
    if (graphicsPipeline) {
        Context::GetInstance().device.destroyPipeline(graphicsPipeline);
    }
    graphicsPipeline = CreateGraphicsPipeline(vertexSource, fragSource);
}

void RenderProcess::RecreateRenderPass() {
    if (renderPass) {
        Context::GetInstance().device.destroyRenderPass(renderPass);
    }
    renderPass = CreateRenderPass();
}

vk::PipelineLayout RenderProcess::CreateLayout() {
    vk::PipelineLayoutCreateInfo createInfo;
    createInfo
        .setPushConstantRangeCount(0)
        .setSetLayoutCount(0);
    return Context::GetInstance().device.createPipelineLayout(createInfo);
}

vk::Pipeline RenderProcess::CreateGraphicsPipeline(const std::vector<char>& vertexSource, const std::vector<char>& fragSource) {
    auto& ctx = Context::GetInstance();

    vk::GraphicsPipelineCreateInfo createInfo;

    // shader source
    vk::ShaderModuleCreateInfo vertexModuleCreateInfo, fragModuleCreateInfo;
    vertexModuleCreateInfo
        .setCodeSize(vertexSource.size())
        .setPCode((std::uint32_t*)vertexSource.data());
    fragModuleCreateInfo
        .setCodeSize(fragSource.size())
        .setPCode((std::uint32_t*)fragSource.data());

    auto vertexModule = ctx.device.createShaderModule(vertexModuleCreateInfo);
    auto fragModule = ctx.device.createShaderModule(fragModuleCreateInfo);

    std::array<vk::PipelineShaderStageCreateInfo, 2> stageCreateInfos;
    stageCreateInfos[0]
        .setModule(vertexModule)
        .setPName("main")
        .setStage(vk::ShaderStageFlagBits::eVertex);
    stageCreateInfos[1]
        .setModule(fragModule)
        .setPName("main")
        .setStage(vk::ShaderStageFlagBits::eFragment);

    // vertex input
    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    auto attribute = Vertex::GetAttribute();
    auto binding = Vertex::GetBinding();
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
    vk::PipelineColorBlendAttachmentState blendAttachmentStateInfo;
    blendAttachmentStateInfo
        .setBlendEnable(false)
        .setColorWriteMask(vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eR);
    vk::PipelineColorBlendStateCreateInfo blendStateCreateInfo;
    blendStateCreateInfo
        .setLogicOpEnable(false)
        .setAttachments(blendAttachmentStateInfo);

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

    auto result = Context::GetInstance().device.createGraphicsPipeline(nullptr, createInfo);
    if (result.result != vk::Result::eSuccess) {
        IO::ThrowError("Failed to create graphics graphicsPipeline");
    }

    // clear shader module
    ctx.device.destroyShaderModule(vertexModule);
    ctx.device.destroyShaderModule(fragModule);

    return result.value;
}

vk::RenderPass RenderProcess::CreateRenderPass() {
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