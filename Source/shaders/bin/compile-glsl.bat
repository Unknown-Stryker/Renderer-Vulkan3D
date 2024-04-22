set path_to_vulkanSDK=%1

%path_to_vulkanSDK% ..\sample_shader.vert -o sample_shader.vert.spv
%path_to_vulkanSDK% ..\sample_shader.frag -o sample_shader.frag.spv