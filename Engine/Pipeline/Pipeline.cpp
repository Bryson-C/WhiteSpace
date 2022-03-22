//
// Created by Owner on 2/18/2022.
//

#include "Pipeline.h"



namespace ws {


    void PipelineFactory::addShaderComponent(Shader shader) {
        m_ShaderStages.emplace_back(shader);
    }

    void PipelineFactory::addShaderComponent(std::initializer_list<Shader> shaders) {
        for (auto i: shaders) {
            m_ShaderStages.emplace_back(i);
        }
    }

    void PipelineFactory::addVertexInputComponent(VertexInputData vertexData) {
        m_VertexInfo = Info::Pipeline::VertexInputCreateInfo(vertexData.bindings, vertexData.attributes);
    }

    void PipelineFactory::addInputAssemblyComponent(VkPrimitiveTopology topology) {
        m_InputAssembly = Info::Pipeline::InputAssemblyCreateInfo(topology);
    }

    void PipelineFactory::addViewportComponent(VkExtent2D extent) {
        m_Viewport = Info::Viewport(0.0f, 0.0f, extent);
        m_Scissor = {{0, 0}, extent};
    }

    void PipelineFactory::addViewportComponent(Window window) {
        addViewportComponent(window.size());
    }

    void PipelineFactory::addRasterizationComponent(VkPolygonMode drawMode, VkCullModeFlags cullmode,
                                                    VkFrontFace frontface) {
        m_Rasterization = Info::Pipeline::RasterizationCreateInfo(VK_FALSE, drawMode, cullmode, frontface);
    }

    void PipelineFactory::addMultiSampleComponent(VkSampleCountFlagBits samples) {
        m_Multisample = Info::Pipeline::MultisampleCreateInfo(samples);
    }

    void PipelineFactory::addColorBlendComponent(vector<VkPipelineColorBlendAttachmentState> attachments) {
        colorBlendAttachment.resize(attachments.size());
        if (attachments.empty()) {
            colorBlendAttachment.resize(1);
            colorBlendAttachment[0] = Info::Pipeline::ColorBlendAttachmentCreateInfo(VK_FALSE, BLEND_ONE, BLEND_ONE,
                                                                                     BLEND_ZERO, BLEND_ZERO);
        } else {
            colorBlendAttachment = attachments;
        }
    }

    static DescriptorAllocator descriptorAllocator;

    void PipelineFactory::addLayoutObjects(std::vector<VkDescriptorSetLayout> layouts) {
        m_DescriptorLayouts = layouts;
    }

    void PipelineFactory::addPushConstants(std::vector<VkPushConstantRange> pushConstants) {
        m_PushConstants = pushConstants;
    }

    PipelineObject *PipelineFactory::build(ws::Device device, VkRenderPass renderPass) {

        VkPipelineViewportStateCreateInfo m_ViewportState = Info::Pipeline::Viewport(m_Viewport, m_Scissor);
        VkPipelineColorBlendStateCreateInfo m_ColorBlend = Info::Pipeline::ColorBlendCreateInfo(colorBlendAttachment);

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages(m_ShaderStages.size());
        for (int i = 0; i < shaderStages.size(); i++) {
            shaderStages[i] = Info::Pipeline::ShaderStageCreateInfo(m_ShaderStages[i]);
        }

        VkPipelineLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.pPushConstantRanges = m_PushConstants.data();
        layoutInfo.pushConstantRangeCount = m_PushConstants.size();
        layoutInfo.setLayoutCount = m_DescriptorLayouts.size();
        layoutInfo.pSetLayouts = m_DescriptorLayouts.data();
        vkCreatePipelineLayout(device.get(), &layoutInfo, VK_NULL_HANDLE, &m_Pipelines.Layout);


        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = nullptr;
        pipelineInfo.flags = 0;
        pipelineInfo.stageCount = shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &m_VertexInfo;
        pipelineInfo.pInputAssemblyState = &m_InputAssembly;
        pipelineInfo.pTessellationState = nullptr;
        pipelineInfo.pViewportState = &m_ViewportState;
        pipelineInfo.pRasterizationState = &m_Rasterization;
        pipelineInfo.pMultisampleState = &m_Multisample;
        pipelineInfo.pDepthStencilState = nullptr;
        pipelineInfo.pColorBlendState = &m_ColorBlend;
        pipelineInfo.pDynamicState = nullptr;
        pipelineInfo.layout = m_Pipelines.Layout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        vkCreateGraphicsPipelines(device.get(), VK_NULL_HANDLE, 1, &pipelineInfo, VK_NULL_HANDLE,
                                  &m_Pipelines.Pipeline);
        m_ShaderStages.clear();

        return &m_Pipelines;
    }


    PipelineFactory pipelineFactory;


    VkRenderPass createRenderPass(ws::Device device, ws::Swapchain swapchain) {
        auto StartTime = clock();
        std::vector<VkAttachmentDescription> colorAttchment = {
                Info::RenderPassAttachmentCreateInfo(swapchain.getFormats().Format.format, VK_SAMPLE_COUNT_1_BIT,
                                                     VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)};
        auto colorAttachmentRef = Info::RenderPassAttachmentReferenceCreateInfo(0,
                                                                                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        std::vector<VkSubpassDescription> subpass = {
                Info::SubpassDescriptionCreateInfo(VK_PIPELINE_BIND_POINT_GRAPHICS, colorAttachmentRef)};
        std::vector<VkSubpassDependency> dependancy = {
                Info::SubpassDependencyCreateInfo(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                                  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0,
                                                  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)};
        auto info = Info::RenderPassCreateInfo(colorAttchment, subpass, dependancy);

        VkRenderPass renderPass;
        VkResult renderpassResult = vkCreateRenderPass(device.get(), &info, nullptr, &renderPass);
        printf("Render Pass: %lims\n", clock() - StartTime);
        return renderPass;
    }

    void destroyRenderPass(ws::Device device, VkRenderPass renderPass) {
        vkDestroyRenderPass(device.get(), renderPass, nullptr);
    }

    void GraphicsPipeline::create(GraphicsPipelineConstructor) {
        auto StartTime = clock();

        descriptorAllocator.init(device.get());
        m_RenderPass = createRenderPass(device, swapchain);

        // create descriptors

        pipelineFactory.addShaderComponent({vShader, fShader});
        pipelineFactory.addVertexInputComponent(inputData);
        pipelineFactory.addInputAssemblyComponent(TOPOLOGY_TRIANGLE_LIST);
        pipelineFactory.addMultiSampleComponent(VK_SAMPLE_COUNT_1_BIT);
        pipelineFactory.addColorBlendComponent();
        pipelineFactory.addRasterizationComponent(VK_POLYGON_MODE_FILL);
        pipelineFactory.addViewportComponent(swapchain.getExtent());

        m_Descriptor.Cache.init(device.get());

        u32 descriptorCount = swapchain.getImageCount() /*uniforms*/;
        m_Descriptor.Sets.resize(descriptorCount);

        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bindings.push_back(Info::DescriptorLayoutBinding(0, swapchain.getImageCount(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VERTEX_SHADER));

        m_Descriptor.Layouts.resize(1);

        for (int i = 0; i < m_Descriptor.Sets.size(); i++) {
            VkDescriptorBufferInfo bufferInfo =
                    Info::DescriptorBufferInfo(uniformBuffers[0].get()[i], uniformBuffers[0].getSize(), 0);
            DescriptorBuilder::begin(&m_Descriptor.Cache, &descriptorAllocator)
                    .bind_buffer(0, &bufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VERTEX_SHADER)
                    .build(m_Descriptor.Sets[i], m_Descriptor.Layouts[0]);
        }

        pipelineFactory.addLayoutObjects(m_Descriptor.Layouts);
        if (inputData.pushConstants != nullptr) {
            pipelineFactory.addPushConstants(*inputData.pushConstants);
        }


        auto pipeline = pipelineFactory.build(device, m_RenderPass);


        m_Pipeline = pipeline->Pipeline;
        m_Layout = pipeline->Layout;


        printf("Pipeline: %lims\n", clock() - StartTime);
    }

    GraphicsPipeline::GraphicsPipeline() {}

    GraphicsPipeline::GraphicsPipeline(GraphicsPipelineConstructor) {
        create(device, swapchain, vShader, fShader, inputData, uniformBuffers);
    }

    VkPipeline GraphicsPipeline::get() { return m_Pipeline; }

    VkPipelineLayout GraphicsPipeline::getLayout() { return m_Layout; }


    vector<VkDescriptorSet> GraphicsPipeline::getDescriptorSet() { return m_Descriptor.Sets; }

    VkRenderPass GraphicsPipeline::getRenderPass() { return m_RenderPass; }

    void GraphicsPipeline::createSampler(ws::Device device) {

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(device.getPhysicalDevice(), &properties);
        VkSamplerCreateInfo samplerInfo = Info::SamplerCreateInfo(properties.limits.maxSamplerAnisotropy);
        auto res = vkCreateSampler(device.get(), &samplerInfo, nullptr, &m_Sampler);

    }

    void GraphicsPipeline::destroy(ws::Device device) {
        vkDestroyPipeline(device.get(), m_Pipeline, nullptr);
        vkDestroyPipelineLayout(device.get(), m_Layout, nullptr);
        vkDestroyRenderPass(device.get(), m_RenderPass, nullptr);
    }

    void GraphicsPipeline::recreate(GraphicsPipelineConstructor) {
        create(device, swapchain, vShader, fShader, inputData, uniformBuffers);
    }


}