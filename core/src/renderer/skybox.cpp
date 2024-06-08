#include "PurrfectEngine/PurrfectEngine.hpp"

#include <array>

namespace PurrfectEngine {

  static PurrfectEngineContext *sContext = nullptr;
  static fr::frPipeline *sSkyboxPipeline = nullptr;

  void initPipe() {
    if (sSkyboxPipeline) delete sSkyboxPipeline;
    sSkyboxPipeline = new fr::frPipeline();
    fr::frShader *vertShdr = new fr::frShader();
    vertShdr->initialize(sContext->frRenderer, "../test/shaders/PBR/skybox_v.spv", VK_SHADER_STAGE_VERTEX_BIT);
    sSkyboxPipeline->addShader(vertShdr);

    fr::frShader *fragShdr = new fr::frShader();
    fragShdr->initialize(sContext->frRenderer, "../test/shaders/PBR/skybox_f.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
    sSkyboxPipeline->addShader(fragShdr);

    sSkyboxPipeline->setVertexInputState<Vertex3D>();

    sSkyboxPipeline->setInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE
    });

    static VkPipelineColorBlendAttachmentState *sColorBlendAttachment = new VkPipelineColorBlendAttachmentState();
    sColorBlendAttachment->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    sColorBlendAttachment->blendEnable = VK_FALSE;

    sSkyboxPipeline->setColorBlendState(VkPipelineColorBlendStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_FALSE,
      VK_LOGIC_OP_COPY,
      1,
      sColorBlendAttachment,
      {0.0f, 0.0f, 0.0f, 0.0f}
    });

    sSkyboxPipeline->setRasterizationState(VkPipelineRasterizationStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE,
      VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f
    });

    std::vector<VkDynamicState> dynamicStates = {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR
    };

    sSkyboxPipeline->setDynamicState(VkPipelineDynamicStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      static_cast<uint32_t>(dynamicStates.size()), dynamicStates.data()
    });

    sSkyboxPipeline->setViewportState(VkPipelineViewportStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      1, nullptr, 1, nullptr
    });

    sSkyboxPipeline->setDepthStencilState(VkPipelineDepthStencilStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_FALSE, VK_TRUE, VK_COMPARE_OP_LESS,
      VK_FALSE, VK_FALSE,
      {}, {},
      1.0f, 1.0f
    });

    sSkyboxPipeline->addDescriptor(sContext->frUboLayout);
    sSkyboxPipeline->addDescriptor(sContext->frTextureLayout);

    sSkyboxPipeline->setMultisampleInfo(VkPipelineMultisampleStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      (VkSampleCountFlagBits)sContext->frMsaa, VK_FALSE, 0.0f, VK_NULL_HANDLE,
      VK_FALSE, VK_FALSE
    });

    sSkyboxPipeline->initialize(sContext->frRenderer, sContext->frSceneRenderPass);

    delete vertShdr;
    delete fragShdr;
  }

  fr::frPipeline *initRectToCubemap(int width, int height) {
    fr::frPipeline *pipeline = new fr::frPipeline();
    fr::frShader *vertShdr = new fr::frShader();
    vertShdr->initialize(sContext->frRenderer, "../test/shaders/PBR/skybox_v.spv", VK_SHADER_STAGE_VERTEX_BIT);
    pipeline->addShader(vertShdr);

    fr::frShader *fragShdr = new fr::frShader();
    fragShdr->initialize(sContext->frRenderer, "../test/shaders/PBR/rectToCubemap_f.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
    pipeline->addShader(fragShdr);

    pipeline->setVertexInputState<Vertex3D>();

    pipeline->setInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE
    });

    static VkPipelineColorBlendAttachmentState *sColorBlendAttachment = new VkPipelineColorBlendAttachmentState();
    sColorBlendAttachment->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    sColorBlendAttachment->blendEnable = VK_FALSE;

    pipeline->setColorBlendState(VkPipelineColorBlendStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_FALSE,
      VK_LOGIC_OP_COPY,
      1,
      sColorBlendAttachment,
      {0.0f, 0.0f, 0.0f, 0.0f}
    });

    pipeline->setRasterizationState(VkPipelineRasterizationStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE,
      VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f
    });

    pipeline->setDynamicState(VkPipelineDynamicStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      0, VK_NULL_HANDLE
    });

    VkViewport viewport = {
      0.0f, 0.0f,
      static_cast<float>(width), static_cast<float>(height),
      0.0f, 1.0f
    };
    VkRect2D scissor = {
      {0, 0},
      {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}
    };
    pipeline->setViewportState(VkPipelineViewportStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      1, &viewport, 1, &scissor
    });

    pipeline->setDepthStencilState(VkPipelineDepthStencilStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS,
      VK_FALSE, VK_FALSE,
      {}, {},
      1.0f, 1.0f
    });

    pipeline->addDescriptor(sContext->frUboLayout);
    pipeline->addDescriptor(sContext->frTextureLayout);

    pipeline->setMultisampleInfo(VkPipelineMultisampleStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      (VkSampleCountFlagBits)sContext->frMsaa, VK_FALSE, 0.0f, VK_NULL_HANDLE,
      VK_FALSE, VK_FALSE
    });

    pipeline->initialize(sContext->frRenderer, sContext->frSceneRenderPass);

    delete vertShdr;
    delete fragShdr;

    return pipeline;  
  }

  fr::frPipeline *initIrradiance(int width, int height) {
    fr::frPipeline *pipeline = new fr::frPipeline();
    fr::frShader *vertShdr = new fr::frShader();
    vertShdr->initialize(sContext->frRenderer, "../test/shaders/PBR/skybox_v.spv", VK_SHADER_STAGE_VERTEX_BIT);
    pipeline->addShader(vertShdr);

    fr::frShader *fragShdr = new fr::frShader();
    fragShdr->initialize(sContext->frRenderer, "../test/shaders/PBR/irradiance_f.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
    pipeline->addShader(fragShdr);

    pipeline->setVertexInputState<Vertex3D>();

    pipeline->setInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE
    });

    static VkPipelineColorBlendAttachmentState *sColorBlendAttachment = new VkPipelineColorBlendAttachmentState();
    sColorBlendAttachment->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    sColorBlendAttachment->blendEnable = VK_FALSE;

    pipeline->setColorBlendState(VkPipelineColorBlendStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_FALSE,
      VK_LOGIC_OP_COPY,
      1,
      sColorBlendAttachment,
      {0.0f, 0.0f, 0.0f, 0.0f}
    });

    pipeline->setRasterizationState(VkPipelineRasterizationStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE,
      VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f
    });

    pipeline->setDynamicState(VkPipelineDynamicStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      0, VK_NULL_HANDLE
    });

    VkViewport viewport = {
      0.0f, 0.0f,
      static_cast<float>(width), static_cast<float>(height),
      0.0f, 1.0f
    };
    VkRect2D scissor = {
      {0, 0},
      {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}
    };
    pipeline->setViewportState(VkPipelineViewportStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      1, &viewport, 1, &scissor
    });

    pipeline->setDepthStencilState(VkPipelineDepthStencilStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS,
      VK_FALSE, VK_FALSE,
      {}, {},
      1.0f, 1.0f
    });

    pipeline->addDescriptor(sContext->frUboLayout);
    pipeline->addDescriptor(sContext->frTextureLayout);

    pipeline->setMultisampleInfo(VkPipelineMultisampleStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      (VkSampleCountFlagBits)sContext->frMsaa, VK_FALSE, 0.0f, VK_NULL_HANDLE,
      VK_FALSE, VK_FALSE
    });

    pipeline->initialize(sContext->frRenderer, sContext->frSceneRenderPass);

    delete vertShdr;
    delete fragShdr;

    return pipeline;  
  }

  fr::frPipeline *initPreFilter(int width, int height) {
    fr::frPipeline *pipeline = new fr::frPipeline();
    fr::frShader *vertShdr = new fr::frShader();
    vertShdr->initialize(sContext->frRenderer, "../test/shaders/PBR/skybox_v.spv", VK_SHADER_STAGE_VERTEX_BIT);
    pipeline->addShader(vertShdr);

    fr::frShader *fragShdr = new fr::frShader();
    fragShdr->initialize(sContext->frRenderer, "../test/shaders/PBR/preFilter_f.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
    pipeline->addShader(fragShdr);

    pipeline->setVertexInputState<Vertex3D>();

    pipeline->setInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE
    });

    static VkPipelineColorBlendAttachmentState *sColorBlendAttachment = new VkPipelineColorBlendAttachmentState();
    sColorBlendAttachment->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    sColorBlendAttachment->blendEnable = VK_FALSE;

    pipeline->setColorBlendState(VkPipelineColorBlendStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_FALSE,
      VK_LOGIC_OP_COPY,
      1,
      sColorBlendAttachment,
      {0.0f, 0.0f, 0.0f, 0.0f}
    });

    pipeline->setRasterizationState(VkPipelineRasterizationStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE,
      VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f
    });

    pipeline->setDynamicState(VkPipelineDynamicStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      0, VK_NULL_HANDLE
    });

    VkViewport viewport = {
      0.0f, 0.0f,
      static_cast<float>(width), static_cast<float>(height),
      0.0f, 1.0f
    };
    VkRect2D scissor = {
      {0, 0},
      {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}
    };
    pipeline->setViewportState(VkPipelineViewportStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      1, &viewport, 1, &scissor
    });

    pipeline->setDepthStencilState(VkPipelineDepthStencilStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS,
      VK_FALSE, VK_FALSE,
      {}, {},
      1.0f, 1.0f
    });

    pipeline->addDescriptor(sContext->frUboLayout);
    pipeline->addDescriptor(sContext->frTextureLayout);
    pipeline->addPushConstant(VkPushConstantRange{
      VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(glm::vec4)
    });

    pipeline->setMultisampleInfo(VkPipelineMultisampleStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      (VkSampleCountFlagBits)sContext->frMsaa, VK_FALSE, 0.0f, VK_NULL_HANDLE,
      VK_FALSE, VK_FALSE
    });

    pipeline->initialize(sContext->frRenderer, sContext->frSceneRenderPass);

    delete vertShdr;
    delete fragShdr;

    return pipeline;  
  }

  fr::frPipeline *initBRDF(int width, int height) {
    fr::frPipeline *pipeline = new fr::frPipeline();
    fr::frShader *vertShdr = new fr::frShader();
    vertShdr->initialize(sContext->frRenderer, "../test/shaders/PBR/skybox_v.spv", VK_SHADER_STAGE_VERTEX_BIT);
    pipeline->addShader(vertShdr);

    fr::frShader *fragShdr = new fr::frShader();
    fragShdr->initialize(sContext->frRenderer, "../test/shaders/PBR/integrateBRDF_f.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
    pipeline->addShader(fragShdr);

    pipeline->setVertexInputState<Vertex3D>();

    pipeline->setInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE
    });

    static VkPipelineColorBlendAttachmentState *sColorBlendAttachment = new VkPipelineColorBlendAttachmentState();
    sColorBlendAttachment->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    sColorBlendAttachment->blendEnable = VK_FALSE;

    pipeline->setColorBlendState(VkPipelineColorBlendStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_FALSE,
      VK_LOGIC_OP_COPY,
      1,
      sColorBlendAttachment,
      {0.0f, 0.0f, 0.0f, 0.0f}
    });

    pipeline->setRasterizationState(VkPipelineRasterizationStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE,
      VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f
    });

    pipeline->setDynamicState(VkPipelineDynamicStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      0, VK_NULL_HANDLE
    });

    VkViewport viewport = {
      0.0f, 0.0f,
      static_cast<float>(width), static_cast<float>(height),
      0.0f, 1.0f
    };
    VkRect2D scissor = {
      {0, 0},
      {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}
    };
    pipeline->setViewportState(VkPipelineViewportStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      1, &viewport, 1, &scissor
    });

    pipeline->setDepthStencilState(VkPipelineDepthStencilStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS,
      VK_FALSE, VK_FALSE,
      {}, {},
      1.0f, 1.0f
    });

    pipeline->addDescriptor(sContext->frUboLayout);

    pipeline->setMultisampleInfo(VkPipelineMultisampleStateCreateInfo{
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, VK_NULL_HANDLE, 0,
      (VkSampleCountFlagBits)sContext->frMsaa, VK_FALSE, 0.0f, VK_NULL_HANDLE,
      VK_FALSE, VK_FALSE
    });

    pipeline->initialize(sContext->frRenderer, sContext->frSceneRenderPass);

    delete vertShdr;
    delete fragShdr;

    return pipeline;  
  }

  purrSkybox::purrSkybox()
  {}

  purrSkybox::~purrSkybox() {
    cleanup();
  }

  bool purrSkybox::initialize(purrTexture *texture, int width, int height) {
    purrCubemap *cubemap = textureToCubemap(texture, width, height);
    if (!cubemap) return false;
    return initialize(cubemap);
  }

  bool purrSkybox::initialize(purrCubemap *cubemap) {
    if (!sSkyboxPipeline) initPipe();
    mSkyboxCubemap = cubemap;

    fr::frCommands *commands = new fr::frCommands();
    commands->initialize(sContext->frRenderer);

    std::vector<fr::frDescriptor*> camDescs(6);
    std::vector<fr::frBuffer*> camBufs(6);
    { // Initialize camera descriptors for further passes
      struct {
        glm::mat4 proj;
        glm::mat4 view;
      } camData;
      camData.proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

      for (size_t i = 0; i < 6; ++i) {
        glm::vec3 rot = glm::vec3(0.0f);
        switch (i) {
        case 0: { camData.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)); } break;
        case 1: { camData.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)); } break;
        case 2: { camData.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)); } break;
        case 3: { camData.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)); } break;
        case 4: { camData.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)); } break;
        case 5: { camData.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)); } break;
        default: { break; }
        }

        camBufs[i] = new fr::frBuffer();
        camBufs[i]->initialize(sContext->frRenderer, fr::frBuffer::frBufferInfo{
          sizeof(glm::mat4)*2, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, {}
        });
        camBufs[i]->copyData(0, sizeof(glm::mat4)*2, (void*)&camData);

        camDescs[i] = sContext->frSkyboxDescriptors->allocate(1, sContext->frUboLayout)[0];
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = camBufs[i]->get();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(glm::mat4)*2;
        camDescs[i]->update(fr::frDescriptor::frDescriptorWriteInfo{
          0, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
          VK_NULL_HANDLE, &bufferInfo, VK_NULL_HANDLE
        });
      }
    }

    purrMesh *cube = purrMesh::getCubeMesh();
    { // Irradiance map pass
      commands->beginSingleTimeFrame();

      fr::frPipeline *pipeline = initIrradiance(32, 32);
      
      std::array<purrRenderTarget*, 6> faces{};
      std::array<purrTexture*, 6> colors{};
      
      VkCommandBuffer cmdBuf = commands->beginSingleTime();
      cube->bind(cmdBuf);
      uint32_t indexCount = static_cast<uint32_t>(cube->getIndexCount());
      for (size_t i = 0; i < 6; ++i) {
        faces[i] = new purrRenderTarget(commands);
        faces[i]->initialize({ 32, 32 });
        colors[i] = faces[i]->getColorTarget();

        faces[i]->begin(cmdBuf);
        pipeline->bind(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS);
        pipeline->bindDescriptor(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, camDescs[i]);
        pipeline->bindDescriptor(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, 1, mSkyboxCubemap->getDescriptor());
        vkCmdDrawIndexed(cmdBuf, indexCount, 1, 0, 0, 0);
        faces[i]->end(cmdBuf);
      }
      commands->endSingleTime(sContext->frRenderer, cmdBuf);

      mIrradianceMap = new purrCubemap();
      mIrradianceMap->initialize(colors, sContext->frHdrFormat);

      commands->endSingleTimeFrame(sContext->frRenderer);

      for (purrRenderTarget *face: faces) delete face;
      delete pipeline;
    }

    { // preFilter pass
      int width = 256;
      int height = 256;
      fr::frPipeline *pipeline = initPreFilter(256, 256);

      int mipMaps = 5;
      mPreFilterMap = new purrCubemap();
      mPreFilterMap->initialize({ width, height }, sContext->frHdrFormat, purrSampler::getDefault(), mipMaps);

      for (int mip = 0; mip < mipMaps; ++mip) {
        const uint32_t mipWidth = static_cast<uint32_t>(width * std::pow(0.5, mip));
        const uint32_t mipHeight = static_cast<uint32_t>(height * std::pow(0.5, mip));

        glm::vec4 data = {};
        data.x = static_cast<float>(mip) / 4.0f;

        std::array<purrRenderTarget*, 6> faces{};
        std::array<purrTexture*, 6> colors{};

        for (size_t i = 0; i < 6; ++i) {
          faces[i] = new purrRenderTarget(commands);
          faces[i]->initialize({ width, height });
          colors[i] = faces[i]->getColorTarget();

          VkCommandBuffer cmdBuf = commands->beginSingleTime();
          faces[i]->begin(cmdBuf);
          pipeline->bind(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS);
          pipeline->bindDescriptor(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, camDescs[i]);
          pipeline->bindDescriptor(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, 1, mSkyboxCubemap->getDescriptor());
          pipeline->pushConstant(cmdBuf, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(glm::vec4), &data);
          cube->render(cmdBuf);
          faces[i]->end(cmdBuf);
          commands->endSingleTime(sContext->frRenderer, cmdBuf);

          colors[i]->getImage()->transitionLayout(sContext->frRenderer, sContext->frCommands, fr::frImage::frImageTransitionInfo{
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_TRANSFER_READ_BIT
          });
        }

        mPreFilterMap->copy(colors, mipWidth, mipHeight, mip);

        for (purrRenderTarget *face: faces) delete face;
      }

      mPreFilterMap->getImage()->transitionLayout(sContext->frRenderer, sContext->frCommands, fr::frImage::frImageTransitionInfo{
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT
      });

      delete pipeline;
    }

    { // BRDF pass
      commands->beginSingleTimeFrame();

      fr::frPipeline *pipeline = initBRDF(512, 512);
      
      purrRenderTarget *target = new purrRenderTarget(commands);
      target->initialize({512, 512});

      VkCommandBuffer cmdBuf = commands->beginSingleTime();
      uint32_t indexCount = static_cast<uint32_t>(cube->getIndexCount());
      target->begin(cmdBuf);
      pipeline->bind(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS);
      pipeline->bindDescriptor(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, camDescs[4]);
      cube->render(cmdBuf);
      target->end(cmdBuf);
      commands->endSingleTime(sContext->frRenderer, cmdBuf);

      mBRDFLut = target->getColorTarget();
      mBRDFTarget = target;

      commands->endSingleTimeFrame(sContext->frRenderer);

      delete pipeline;
    }

    for (fr::frBuffer *buf: camBufs) delete buf;
    for (fr::frDescriptor *desc: camDescs) delete desc;
    delete commands;

    mSkyboxDesc = sContext->frSkyboxDescriptors->allocate(1, sContext->frSkyboxLayout)[0];
    {
      VkDescriptorImageInfo imageInfo = mIrradianceMap->getDescImageInfo();
      mSkyboxDesc->update(fr::frDescriptor::frDescriptorWriteInfo{
        0, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        &imageInfo, VK_NULL_HANDLE, VK_NULL_HANDLE
      });
    }
    {
      VkDescriptorImageInfo imageInfo = mPreFilterMap->getDescImageInfo();
      mSkyboxDesc->update(fr::frDescriptor::frDescriptorWriteInfo{
        1, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        &imageInfo, VK_NULL_HANDLE, VK_NULL_HANDLE
      });
    }
    {
      VkDescriptorImageInfo imageInfo = mBRDFLut->getDescImageInfo();
      mSkyboxDesc->update(fr::frDescriptor::frDescriptorWriteInfo{
        2, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        &imageInfo, VK_NULL_HANDLE, VK_NULL_HANDLE
      });
    }

    return true;
  }
  
  void purrSkybox::cleanup() {
    if (mSkyboxDesc) delete mSkyboxDesc;
    if (mSkyboxCubemap) delete mSkyboxCubemap;
    if (mIrradianceMap) delete mIrradianceMap;
    if (mPreFilterMap) delete mPreFilterMap;
    if (mBRDFTarget) delete mBRDFTarget;
  }

  void purrSkybox::bind(purrPipeline *pipeline) {
    pipeline->get()->bindDescriptor(sContext->frActiveCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, 3, mSkyboxDesc);
  }

  void purrSkybox::render(int width, int height) {
    purrMesh *cube = purrMesh::getCubeMesh();

    sSkyboxPipeline->bind(sContext->frActiveCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS);
    {
      VkViewport viewport{};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = static_cast<float>(width);
      viewport.height = static_cast<float>(width);
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(sContext->frActiveCmdBuf, 0, 1, &viewport);

      VkRect2D scissor{};
      scissor.offset = {0, 0};
      scissor.extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
      vkCmdSetScissor(sContext->frActiveCmdBuf, 0, 1, &scissor);
    }
    sSkyboxPipeline->bindDescriptor(sContext->frActiveCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, sContext->frCameraDesc);
    sSkyboxPipeline->bindDescriptor(sContext->frActiveCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, 1, mSkyboxCubemap->getDescriptor());
    cube->render(sContext->frActiveCmdBuf);
  }

  void purrSkybox::cleanupAll() {
    if (sSkyboxPipeline) delete sSkyboxPipeline;
  }

  void purrSkybox::setContext(PurrfectEngineContext *context) {
    sContext = context;
  }

  purrCubemap *purrSkybox::textureToCubemap(purrTexture *texture, int width, int height) {
    fr::frCommands *commands = new fr::frCommands();
    commands->initialize(sContext->frRenderer);
    commands->beginSingleTimeFrame();

    std::array<purrRenderTarget *, 6> targets{};
    std::array<purrTexture *, 6> colors{};
    for (size_t i = 0; i < 6; ++i) {
      targets[i] = new purrRenderTarget(commands);
      targets[i]->initialize({ width, height });
      colors[i] = targets[i]->getColorTarget();
    }

    purrMesh *cube = purrMesh::getCubeMesh();

    fr::frPipeline *rectToCubemapPipeline = initRectToCubemap(width, height);

    struct {
      glm::mat4 proj;
      glm::mat4 view;
    } camData;
    camData.proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

    std::vector<fr::frBuffer*> camBufs(6);
    std::vector<fr::frDescriptor*> camDescs(6);

    for (size_t i = 0; i < 6; ++i) {
      glm::vec3 rot = glm::vec3(0.0f);
      switch (i) {
      case 0: { camData.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)); } break;
      case 1: { camData.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)); } break;
      case 2: { camData.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)); } break;
      case 3: { camData.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)); } break;
      case 4: { camData.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)); } break;
      case 5: { camData.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)); } break;
      default: { break; }
      }

      camBufs[i] = new fr::frBuffer();
      camBufs[i]->initialize(sContext->frRenderer, fr::frBuffer::frBufferInfo{
        sizeof(glm::mat4)*2, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, {}
      });
      camBufs[i]->copyData(0, sizeof(glm::mat4)*2, (void*)&camData);

      camDescs[i] = sContext->frSkyboxDescriptors->allocate(1, sContext->frUboLayout)[0];
      VkDescriptorBufferInfo bufferInfo = {};
      bufferInfo.buffer = camBufs[i]->get();
      bufferInfo.offset = 0;
      bufferInfo.range = sizeof(glm::mat4)*2;
      camDescs[i]->update(fr::frDescriptor::frDescriptorWriteInfo{
        0, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        VK_NULL_HANDLE, &bufferInfo, VK_NULL_HANDLE
      });
    }

    commands->endSingleTimeFrame(sContext->frRenderer);

    VkCommandBuffer cmdBuf = commands->beginSingleTime(); {
      size_t i = 0;
      cube->bind(cmdBuf);
      uint32_t indexCount = static_cast<uint32_t>(cube->getIndexCount());
      for (purrRenderTarget *target: targets) {
        target->begin(cmdBuf);
        rectToCubemapPipeline->bind(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS);
        rectToCubemapPipeline->bindDescriptor(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, camDescs[i++]);
        rectToCubemapPipeline->bindDescriptor(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, 1, texture->getDescriptor());
        vkCmdDrawIndexed(cmdBuf, indexCount, 1, 0, 0, 0);
        target->end(cmdBuf);
      }
    } commands->endSingleTime(sContext->frRenderer, cmdBuf);

    purrCubemap *cubemap = new purrCubemap();
    cubemap->initialize(colors, sContext->frHdrFormat);

    for (purrRenderTarget *target: targets) delete target;

    for (fr::frBuffer *buf: camBufs) delete buf;
    for (fr::frDescriptor *desc: camDescs) delete desc;

    delete rectToCubemapPipeline;
    delete commands;

    return cubemap;
  }

}