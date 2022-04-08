# WhiteSpace
Personal Vulkan Engine
 - Made To Prove A Point (I Can Respect OpenGL But I Dislike Using It)
 - Uses GLFW As A Window System
 - Currently Uses Spaghetti Code
 
 ## TODO
 [] Move Away From C++ Standard As It Can Get Quite Messy
 [] Re-Create Time Functions And Classes
 [] Add Built In Frame Syncronization Into The Device Class As This Is What Class Interacts With Queue Submission
 
 ## Updates
 ### March 22 2022
 - Added Many Abstractions
 - Split Contents Of Engine.cpp And Engine.hpp Into Seperate File Named Accordingly
 - Added Inline Command Buffers To Device Object
 - Added New Class `WSInstance` Which Is A Compilation Of The Previous Main Function
 - Remade Main Function To Use `WSInstance` Class (Old Main Exists As `void main2()` still)
 - Changes To Infos.cpp And Infos.hpp Files To Include More Optional Function Parameters
 - Other Changes I may Have Forgotten
 ### Feburary 15 2022
 - Added PipelineFactory Class (For Building Graphics Pipelines)
 - Rebuilt GraphicsPipeline Class To Use PipelineFactory Class For Creation
 - New Functions In Infos/Infos.hpp + Infos/Infos.cpp
 - (**If You Use This Renderer I Pity You**)
 - (**No Safety Features Have Been Added!**)
 ### January 24 2022
 - Added Timers To Creation Of Objects
 - Changed Example 
 - Added New Functions To Engine/Infos.cpp + Engine/Infos.hpp
 
 
