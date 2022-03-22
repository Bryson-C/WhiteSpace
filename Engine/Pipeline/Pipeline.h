//
// Created by Owner on 2/18/2022.
//

#ifndef ENGINE_PIPELINE_H
#define ENGINE_PIPELINE_H

#include "../../VKInclude/vulkan/vulkan.h"
#include "../Infos/Infos.hpp"
#include "../Device/Device.h"
#include "../Swapchain/Swapchain.h"
#include "../Buffers/Buffers.h"
#include "Descriptor.h"
#include <array>

namespace ws {




    class PipelineFactory {
    private:
        PipelineObject m_Pipelines{};
        std::vector <Shader> m_ShaderStages{};
        VkPipelineVertexInputStateCreateInfo m_VertexInfo{};
        VkPipelineInputAssemblyStateCreateInfo m_InputAssembly{};
        VkViewport m_Viewport;
        VkRect2D m_Scissor;
        VkPipelineRasterizationStateCreateInfo m_Rasterization{};
        VkPipelineMultisampleStateCreateInfo m_Multisample{};
        vector <VkPipelineColorBlendAttachmentState> colorBlendAttachment;
        vector <VkPushConstantRange> m_PushConstants{};
        vector <VkDescriptorSetLayout> m_DescriptorLayouts{};
    public:
        // Adds A Shader To Be Processed Later During [build] Function
        void addShaderComponent(Shader shader);

        void addShaderComponent(std::initializer_list <Shader> shaders);

        // Adds A VertexInputState To Be Processed Later During [build] Function
        void addVertexInputComponent(VertexInputData vertexData);

        // Adds A InputAssemblyState To Be Processed Later During [build] Function
        void addInputAssemblyComponent(VkPrimitiveTopology topology);

        //Adds A ViewportState To Be Processed Later During [build] Function
        void addViewportComponent(VkExtent2D extent);

        void addViewportComponent(Window window);

        //Adds A RasterizationState To Be Processed Later During [build] Function
        void addRasterizationComponent(VkPolygonMode drawMode,
                                       VkCullModeFlags cullmode = VK_CULL_MODE_BACK_BIT,
                                       VkFrontFace frontface = VK_FRONT_FACE_COUNTER_CLOCKWISE);

        //Adds A MultisampleState To Be Processed Later During [build] Function
        void addMultiSampleComponent(VkSampleCountFlagBits samples);

        //Adds A ColorBlendState To Be Processed Later During [build] Function
        void addColorBlendComponent(vector <VkPipelineColorBlendAttachmentState> attachments = {});

        //Adds A Set Amount Of PushConstantRanges To Be Processed Later During [build] Function
        void addPushConstants(std::vector<VkPushConstantRange> pushConstants);

        //Adds A Set Amount Of Descriptor Objects To Be Processed Later During [build] Function
        void addLayoutObjects(std::vector<VkDescriptorSetLayout> layouts);

        //WIP
        void addSampler();

        // Final Processing Of The Pipeline, Returns A Pipeline Structure Containing Vital Information
        PipelineObject *build(ws::Device device, VkRenderPass renderPass);
    };




// I made this simply because I dont want to keep writing it
#define GraphicsPipelineConstructor \
                        ws::Device device, ws::Swapchain swapchain, \
                        Shader vShader, Shader fShader, VertexInputData inputData, \
                        std::vector<ws::UniformBuffer> uniformBuffers

    class GraphicsPipeline {
    private:

        VkPipeline m_Pipeline;
        VkPipelineLayout m_Layout;
        VkRenderPass m_RenderPass;

        PipelineDescriptor m_Descriptor;
        VkSampler m_Sampler;

        void createSampler(ws::Device device);

    public:
        void create(GraphicsPipelineConstructor);

        GraphicsPipeline();

        // Creates Graphics Pipeline And Related Objects
        GraphicsPipeline(GraphicsPipelineConstructor);

        // Returns Pipeline Object
        VkPipeline get();

        // Returns Pipeline Layout Object
        VkPipelineLayout getLayout();

        // Returns List Of Descriptor Sets
        vector <VkDescriptorSet> getDescriptorSet();

        // Returns Render Pass Object
        VkRenderPass getRenderPass();

        void destroy(ws::Device device);

        void recreate(GraphicsPipelineConstructor);
    };
}

[[deprecated("Not Being Used, Considering Later Usages")]] ws::GraphicsPipeline CreatePipelineDefaultRectangleRenderer();








#endif //ENGINE_PIPELINE_H
