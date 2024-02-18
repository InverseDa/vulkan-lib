#ifndef VULKAN_LIB_GRAPHICSCREATEINFOPACK_H
#define VULKAN_LIB_GRAPHICSCREATEINFOPACK_H

#include "vulkan/vulkan.h"

#include <vector>

namespace Vulkan {
struct GraphicsCreateInfoPack {
  public:
    VkGraphicsPipelineCreateInfo createInfo = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    // Vertex input
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
    // Input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    // Tessellation
    VkPipelineTessellationStateCreateInfo tessellation = {VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO};
    // Viewport
    VkPipelineViewportStateCreateInfo viewportState = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    std::vector<VkViewport> viewports;
    std::vector<VkRect2D> scissors;
    uint32_t dynamicViewportCount = 1;
    uint32_t dynamicScissorCount = 1;
    // Rasterization
    VkPipelineRasterizationStateCreateInfo rasterizer = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    // Multisample
    VkPipelineMultisampleStateCreateInfo multisampling = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    // Depth and stencil
    VkPipelineDepthStencilStateCreateInfo depthStencil = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    // Color blending
    VkPipelineColorBlendStateCreateInfo colorBlending = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
    // Dynamic state
    VkPipelineDynamicStateCreateInfo dynamicState = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    std::vector<VkDynamicState> dynamicStates;

    GraphicsCreateInfoPack() {
        SetCreateInfos();
        createInfo.basePipelineIndex = -1;
    }
    GraphicsCreateInfoPack(const GraphicsCreateInfoPack& other) noexcept {
        createInfo = other.createInfo;
        SetCreateInfos();

        vertexInputInfo = other.vertexInputInfo;
        inputAssembly = other.inputAssembly;
        tessellation = other.tessellation;
        viewportState = other.viewportState;
        rasterizer = other.rasterizer;
        multisampling = other.multisampling;
        depthStencil = other.depthStencil;
        colorBlending = other.colorBlending;
        dynamicState = other.dynamicState;

        shaderStages = other.shaderStages;
        vertexBindingDescriptions = other.vertexBindingDescriptions;
        vertexAttributeDescriptions = other.vertexAttributeDescriptions;
        viewports = other.viewports;
        scissors = other.scissors;
        colorBlendAttachments = other.colorBlendAttachments;
        dynamicStates = other.dynamicStates;
        UpdateAllArraysAddresses();
    }
    void UpdateAllArrays() {
        createInfo.stageCount = shaderStages.size();
        vertexInputInfo.vertexBindingDescriptionCount = vertexBindingDescriptions.size();
        vertexInputInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptions.size();
        viewportState.viewportCount = viewports.size() ? viewports.size() : dynamicViewportCount;
        viewportState.scissorCount = scissors.size() ? scissors.size() : dynamicScissorCount;
        colorBlending.attachmentCount = colorBlendAttachments.size();
        dynamicState.dynamicStateCount = dynamicStates.size();
        UpdateAllArraysAddresses();
    }
    operator VkGraphicsPipelineCreateInfo&() {
        return createInfo;
    }

  private:
    void SetCreateInfos() {
        createInfo.pVertexInputState = &vertexInputInfo;
        createInfo.pInputAssemblyState = &inputAssembly;
        createInfo.pTessellationState = &tessellation;
        createInfo.pViewportState = &viewportState;
        createInfo.pRasterizationState = &rasterizer;
        createInfo.pMultisampleState = &multisampling;
        createInfo.pDepthStencilState = &depthStencil;
        createInfo.pColorBlendState = &colorBlending;
        createInfo.pDynamicState = &dynamicState;
    }
    void UpdateAllArraysAddresses() {
        createInfo.pStages = shaderStages.data();
        vertexInputInfo.pVertexBindingDescriptions = vertexBindingDescriptions.data();
        vertexInputInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
        viewportState.pViewports = viewports.data();
        viewportState.pScissors = scissors.data();
        colorBlending.pAttachments = colorBlendAttachments.data();
        dynamicState.pDynamicStates = dynamicStates.data();
    }
};
} // namespace Vulkan

#endif // VULKAN_LIB_GRAPHICSCREATEINFOPACK_H
