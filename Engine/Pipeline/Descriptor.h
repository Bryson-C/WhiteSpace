//
// Created by Owner on 3/16/2022.
//

#ifndef ENGINE_DESCRIPTOR_H
#define ENGINE_DESCRIPTOR_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "../VKInclude/vulkan/vulkan.h"

//https://vkguide.dev/docs/extra-chapter/abstracting_descriptors/
// Didnt want to mess with these yet

class DescriptorAllocator {
public:

    struct PoolSizes {
        std::vector<std::pair<VkDescriptorType,float>> sizes =
                {
                        { VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
                        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f },
                        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.f },
                        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.f },
                        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.f },
                        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.f },
                        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.f },
                        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.f },
                        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f },
                        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.f },
                        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f }
                };
    };

    void reset_pools();
    bool allocate(VkDescriptorSet* set, VkDescriptorSetLayout layout);

    void init(VkDevice newDevice);

    void cleanup();

    VkDevice device;
private:
    VkDescriptorPool grab_pool();

    VkDescriptorPool currentPool{VK_NULL_HANDLE};
    PoolSizes descriptorSizes;
    std::vector<VkDescriptorPool> usedPools;
    std::vector<VkDescriptorPool> freePools;
};

class DescriptorLayoutCache {
public:
    void init(VkDevice newDevice);
    void cleanup();

    VkDescriptorSetLayout create_descriptor_layout(VkDescriptorSetLayoutCreateInfo* info);

    struct DescriptorLayoutInfo {
        //good idea to turn this into a inlined array
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        bool operator==(const DescriptorLayoutInfo& other) const;

        size_t hash() const;
    };



private:

    struct DescriptorLayoutHash		{

        std::size_t operator()(const DescriptorLayoutInfo& k) const{
            return k.hash();
        }
    };

    std::unordered_map<DescriptorLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHash> layoutCache;
    VkDevice device;
};

class DescriptorBuilder {
public:
    static DescriptorBuilder begin(DescriptorLayoutCache* layoutCache, DescriptorAllocator* allocator );

    DescriptorBuilder& bind_buffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo, VkDescriptorType type, VkShaderStageFlags stageFlags);
    DescriptorBuilder& bind_image(uint32_t binding, VkDescriptorImageInfo* imageInfo, VkDescriptorType type, VkShaderStageFlags stageFlags);

    bool build(VkDescriptorSet& set, VkDescriptorSetLayout& layout);
    //[[deprecated]] bool build(VkDescriptorSet& set);
private:

    std::vector<VkWriteDescriptorSet> writes;
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    DescriptorLayoutCache* cache;
    DescriptorAllocator* alloc;
};

struct PipelineDescriptor {
    DescriptorLayoutCache Cache;
    std::vector<VkDescriptorSetLayout> Layouts;
    std::vector<VkDescriptorSet> Sets;
};



#endif //ENGINE_DESCRIPTOR_H
