#include "point_light_system.hpp"
#include "core/context.hpp"

namespace ida {
struct PointLightPushConstants {
    glm::vec4 position;
    glm::vec4 color;
    float radius;
};

PointLightSystem::PointLightSystem(vk::RenderPass renderPass, vk::DescriptorSetLayout globalSetLayout) {
    CreatePipelineLayout(globalSetLayout);
    CreatePipeline(renderPass);
}

PointLightSystem::~PointLightSystem() {
    Context::GetInstance().device.destroyPipelineLayout(pipelineLayout_);
}

void PointLightSystem::Update(FrameInfo& frameInfo, GlobalUbo& globalUbo) {
}

void PointLightSystem::Render(FrameInfo& frameInfo) {
}

void PointLightSystem::CreatePipelineLayout(vk::DescriptorSetLayout globalSetLayout) {
    auto pushConstantRange = vk::PushConstantRange()
                                 .setStageFlags(vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex)
                                 .setOffset(0)
                                 .setSize(sizeof(PointLightPushConstants));
    std::vector<vk::DescriptorSetLayout> layouts = {globalSetLayout};

    auto createInfo = vk::PipelineLayoutCreateInfo()
                          .setSetLayouts(layouts)
                          .setPushConstantRanges(pushConstantRange);
    pipelineLayout_ = Context::GetInstance().device.createPipelineLayout(createInfo);
}

void PointLightSystem::CreatePipeline(vk::RenderPass renderPass) {
    PipelineConfigInfo pipelineConfigInfo{};
    IdaPipeline::DefaultPipelineConfigInfo(pipelineConfigInfo);
    IdaPipeline::EnbaleAlphaBlending(pipelineConfigInfo);
}

} // namespace ida
