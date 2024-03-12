#ifndef VULKAN_LIB_RENDERER_HPP
#define VULKAN_LIB_RENDERER_HPP

#include "vulkan/vulkan.hpp"
#include "vklib/buffer/buffer.hpp"
#include "vklib/math/uniform.hpp"
#include "vklib/math/mat4.hpp"
#include "vklib/texture/texture.hpp"
#include <limits>

namespace Vklib {
class Renderer final {
  public:
    Renderer(int maxFightCount);
    ~Renderer();

    // TODO: temporary draw rect, implement this function
    void DrawRect(const Rect&, Texture&);
    void DrawLine(const Vec2& p1, const Vec2& p2);

    void SetDrawColor(const Color& color);
    void SetProjectionMatrix(int left, int right, int top, int bottom, int near, int far);

    void StartRender();
    void EndRender();

  private:
    int maxFightCount_;
    int cur_Frame_;
    uint32_t imageIndex_;
    std::vector<vk::Fence> fences_;
    std::vector<vk::Semaphore> imageAvaliableSems_;
    std::vector<vk::Semaphore> renderFinishSems_;
    std::vector<vk::CommandBuffer> cmdBufs_;
    std::unique_ptr<Buffer> rectVerticesBuffer_;
    std::unique_ptr<Buffer> rectIndicesBuffer_;
    std::unique_ptr<Buffer> lineVerticesBuffer_;

    Mat4 projectionMat_;
    Mat4 viewMat_;

    std::vector<std::unique_ptr<Buffer>> uniformBuffers_;
    std::vector<std::unique_ptr<Buffer>> deviceUniformBuffers_;

    std::vector<DescriptorSetMgr::SetInfo> descriptorSets_;

    vk::Sampler sampler;

    Texture* whiteTexture;
    Color drawColor_ = {1, 1, 1};

    void InitMats();

    void CreateFences();
    void CreateSemaphores();
    void CreateCmdBuffers();
    void CreateBuffers();
    void CreateUniformBuffers(int flightCount);

    void BufferRectData();
    void BufferRectVertexData();
    void BufferRectIndicesData();

    void BufferLineData(const Vec2& p1, const Vec2& p2);

    void BufferMVPData();
    void BufferData();
    void UpdateDescriptorSets();
    void TransBuffer2Device(Buffer& src, Buffer& dst, size_t size, size_t srcOffset, size_t dstOffset);
    void CreateWhiteTexture();
};


} // namespace Vklib

#endif // VULKAN_LIB_RENDERER_HPP
