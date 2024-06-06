#include "PurrfectEngine/PurrfectEngine.hpp"

#include <array>

namespace PurrfectEngine {

  static PurrfectEngineContext *sContext = nullptr;

  purrCubemap::purrCubemap()
  {}

  purrCubemap::~purrCubemap() {
    cleanup();
  }

  bool purrCubemap::initialize(std::array<purrTexture *,6> textures, VkFormat format, purrSampler *sampler) {
    for (purrTexture *texture: textures) {
      int w = mWidth, h = mHeight;
      if ((texture->getSize(&mWidth, &mHeight), true) && w > 0 && h > 0 && (mWidth != w || mHeight != h)) return false;
    }

    mImage = new fr::frImage();
    mImage->initialize(sContext->frRenderer, fr::frImage::frImageInfo{
      mWidth, mHeight, 6, mFormat = format,
      (VkImageUsageFlagBits)(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT),
      true, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      VK_IMAGE_ASPECT_COLOR_BIT, true
    });
    mImage->transitionLayout(sContext->frRenderer, sContext->frCommands, fr::frImage::frImageTransitionInfo{
      VK_IMAGE_LAYOUT_UNDEFINED,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      0, VK_ACCESS_TRANSFER_WRITE_BIT,
    });

    VkDeviceSize imageSize = mWidth * mHeight * 4;
    fr::frBuffer *stagingBuf = new fr::frBuffer();
    stagingBuf->initialize(sContext->frRenderer, fr::frBuffer::frBufferInfo{
      imageSize*6*sizeof(float), (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, {}
    });
    for (size_t i = 0; i < 6; ++i) {
      textures[i]->getImage()->transitionLayout(sContext->frRenderer, sContext->frCommands, fr::frImage::frImageTransitionInfo{
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_ACCESS_SHADER_READ_BIT,
        VK_ACCESS_TRANSFER_READ_BIT,
      });
      textures[i]->getImage()->copyToBuffer(sContext->frRenderer, sContext->frCommands, stagingBuf, mWidth*mHeight*4*i);
      textures[i]->getImage()->transitionLayout(sContext->frRenderer, sContext->frCommands, fr::frImage::frImageTransitionInfo{
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        VK_ACCESS_TRANSFER_READ_BIT,
        VK_ACCESS_SHADER_READ_BIT,
      });
    }

    mImage->copyFromBuffer(sContext->frRenderer, sContext->frCommands, stagingBuf);
    delete stagingBuf;

    mImage->transitionLayout(sContext->frRenderer, sContext->frCommands, fr::frImage::frImageTransitionInfo{
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
    });

    return true;
  }
  
  void purrCubemap::cleanup() {
    if (mImage) delete mImage;
    if (mDescriptor) delete mDescriptor;
    mImage = nullptr;
    mDescriptor = nullptr;
  }

  void purrCubemap::setContext(PurrfectEngineContext *context) {
    sContext = context;
  }

}