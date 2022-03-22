//
// Created by Owner on 2/18/2022.
//

#include "Instance.h"


namespace ws {
                    Instance::Instance(const char *name, bool debug) : m_InstanceName(name), m_Debug(debug) {
        auto StartTime = clock();
        Initalize();
        m_Instance = CreateInstance(debug);
        printf("Instance: %lims\n", clock() - StartTime);
    }
    VkInstance      Instance::get() { return m_Instance; }
    const char      *Instance::getName() { return m_InstanceName; }
}