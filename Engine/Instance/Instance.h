//
// Created by Owner on 2/18/2022.
//

#ifndef ENGINE_INSTANCE_H
#define ENGINE_INSTANCE_H

#include "../../VKInclude/vulkan/vulkan.h"
#include "../Infos/Infos.hpp"

#define ENGINE_NAME "WhiteSpace"

namespace ws {
    // Instance Class Is A Wrapper For The Initial Vulkan State
    class Instance {
    private:
        // Instance Object
        VkInstance m_Instance;
        // Instance Name
        const char *m_InstanceName;
        // Use Validation And Debug Features
        bool m_Debug;
    public:
        // Initializes Instance @EngineName Set The Name Of The Instance, @debug Enables Debug For The Vulkan Instance
        Instance(const char* name = ENGINE_NAME, bool debug = false);
        // Returns The VkInstance Object
        VkInstance get();
        // Returns Name Of Instance
        const char* getName();



    };
}

#endif //ENGINE_INSTANCE_H
