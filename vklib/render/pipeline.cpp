#include "pipeline.hpp"

namespace ida {
// ******************************* IdaPipeline *******************************
IdaPipeline::IdaPipeline(const vk::Device& device, const std::string& vertPath, const std::string& fragPath, const PipelineConfigInfo& configInfo) {
}

IdaPipeline::~IdaPipeline() {
}

void IdaPipeline::CreateGraphicsPipeline(const std::string& vertPath, const std::string& fragPath, const PipelineConfigInfo& configInfo) {
}

void IdaPipeline::CreateShaderModule(const std::vector<char>& code, vk::ShaderModule* shaderModule) {
}

} // namespace ida
