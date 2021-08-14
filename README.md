# WhiteSpace
Personal Vulkan
 - Made To Prove A Point (I Can Respect OpenGL But I Dislike Using It)
 
 - TODO: Draw Multiple Objects Easier
 
 ## Creating Vulkan Objects
 ``` c++
 auto Instance = WS::createInstance(false); // Returns VkInstnace Type, Parameter 1 of createInstance is For Validation Layers
 
 auto SurfaceObjects = WS::createWindow(title, width, height, Instance) // SurfaceObjects is a struct containing VkSurfaceKHR and GLFWwindow*
 
 auto PhysicalDevice = WS::findPhysicalDevice(Instance); // Returns VkPhysicalDevice Type
 
 auto QueueList = WS::findQueues(PhysicalDevice); // Returns a Vector Of WS::Queue's
 
 auto Device = WS::createDevice(PhysicalDevice, QueueList); // Returns VkDevice Type
 
 WS::Queue GraphicsQueue;
 WS::Queue PresentQueue;
 
VkFormat ImageFormat;
VkExtent2D Extent;
auto Swapchain = WS::createSwapchain(Devices, SurfaceObjects, QueueList, ImageFormat, Extent, gQueue.Index, pQueue.Index); // Returns VkSwapchainKHR
auto Images = WS::getSwapchainImages(Device, Swapchain); // Returns a Vector Of Swapchain Images
auto ImageViews = WS::createImageViews(Images, Device, ImageFormat); // Returns a Vector Of Image Views

 ```
 
 
