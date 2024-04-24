#include <FE/renderer/device.hpp>
#include <FE/renderer/window.hpp>

// std headers
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>




BEGIN_NAMESPACE(FE::renderer)


// local callback functions
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(_MAYBE_UNUSED_ VkDebugUtilsMessageSeverityFlagBitsEXT message_severity_p,
                                                    _MAYBE_UNUSED_ VkDebugUtilsMessageTypeFlagsEXT message_type_p,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* callback_data_p,
                                                    _MAYBE_UNUSED_ void* user_data_p) 
{
    std::cerr << "validation layer: " << callback_data_p->pMessage << std::endl;
    return VK_FALSE;
}

VkResult create_debug_utils_messenger_EXT(VkInstance vulkan_instance_p,
                                          const VkDebugUtilsMessengerCreateInfoEXT* create_info_p,
                                          const VkAllocationCallbacks* allocator_p,
                                          VkDebugUtilsMessengerEXT* debug_messenger_p) 
{
    auto l_func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkan_instance_p, "vkCreateDebugUtilsMessengerEXT");
    if (l_func != nullptr) 
    {
        return l_func(vulkan_instance_p, create_info_p, allocator_p, debug_messenger_p);
    } 
    else 
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroy_debug_utils_messenger_EXT(VkInstance vulkan_instance_p, VkDebugUtilsMessengerEXT debug_messenger_p, const VkAllocationCallbacks* allocator_p) 
{
    auto l_func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkan_instance_p, "vkDestroyDebugUtilsMessengerEXT");

    if (l_func != nullptr) 
    {
        l_func(vulkan_instance_p, debug_messenger_p, allocator_p);
    }
}

// class member functions
device::device(FE::renderer::window& window_p) : m_window{ window_p } 
{
    __create_instance();
    __setup_debug_messenger();
    __create_surface();
    __pick_physical_device();
    __create_logical_device();
    __create_command_pool();
}

device::~device() 
{
    vkDestroyCommandPool(this->m_device, this->m_command_pool, nullptr);
    vkDestroyDevice(this->m_device, nullptr);

    if (this->_are_validation_layers_enabled) 
    {
        destroy_debug_utils_messenger_EXT(this->m_vulkan_instance, this->m_debug_messenger, nullptr);
    }

    vkDestroySurfaceKHR(this->m_vulkan_instance, this->m_surface, nullptr);
    vkDestroyInstance(this->m_vulkan_instance, nullptr);
}

void device::__create_instance() 
{
    FE_EXIT(_are_validation_layers_enabled && !__check_validation_layer_support(), RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "validation layers requested, but not available!");

    VkApplicationInfo l_app_info{};
    l_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    l_app_info.pApplicationName = "Frogman Engine Renderer Test App";
    l_app_info.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);

    l_app_info.pEngineName = "Frogman Engine";
    l_app_info.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
    l_app_info.apiVersion = VK_API_VERSION_1_3;


    VkInstanceCreateInfo l_create_info{};
    l_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    l_create_info.pApplicationInfo = &l_app_info;

    auto l_extensions = __get_required_extensions();
    l_create_info.enabledExtensionCount = static_cast<uint32_t>(l_extensions.size());
    l_create_info.ppEnabledExtensionNames = l_extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT l_debug_create_info;
    if (this->_are_validation_layers_enabled) 
    {
        l_create_info.enabledLayerCount = static_cast<uint32_t>(this->m_validation_layers.size());
        l_create_info.ppEnabledLayerNames = this->m_validation_layers.data();

        __populate_debug_messenger_create_info(l_debug_create_info);
        l_create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&l_debug_create_info;
    } 
    else 
    {
        l_create_info.enabledLayerCount = 0;
        l_create_info.pNext = nullptr;
    }

    FE_EXIT(vkCreateInstance(&l_create_info, nullptr, &m_vulkan_instance) != VK_SUCCESS, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "failed to create m_vulkan_instance!");

    __has_GLFW_required_instance_extensions();
}

void device::__pick_physical_device() 
{
    var::uint32 l_device_count = 0;
    vkEnumeratePhysicalDevices(this->m_vulkan_instance, &l_device_count, nullptr);
    
    FE_EXIT(l_device_count == 0, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "failed to find GPUs with Vulkan support!");
    std::cout << "Device count: " << l_device_count << std::endl;

    std::vector<VkPhysicalDevice> devices(l_device_count);
    vkEnumeratePhysicalDevices(this->m_vulkan_instance, &l_device_count, devices.data());
    
    for (const auto &device : devices) 
    {
        if (__is_device_suitable(device)) 
        {
            this->m_physical_device = device;
            break;
        }
    }
    
    FE_EXIT(m_physical_device == VK_NULL_HANDLE, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "failed to find a suitable GPU!");
    
    vkGetPhysicalDeviceProperties(this->m_physical_device, &_properties);
    std::cout << "physical device: " << this->_properties.deviceName << std::endl;
}

void device::__create_logical_device() 
{
    queue_family_indices l_indices = __find_queue_families(this->m_physical_device);
    
    std::vector<VkDeviceQueueCreateInfo> l_queue_create_infos;
    std::set<uint32_t> l_unique_queue_families = {l_indices._graphics_family, l_indices._present_family};
    
    var::float32 l_queue_priority = 1.0f;
    for (var::uint32 l_queue_family : l_unique_queue_families) 
    {
        VkDeviceQueueCreateInfo l_queue_create_info = {};
        l_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        l_queue_create_info.queueFamilyIndex = l_queue_family;
        l_queue_create_info.queueCount = 1;
        l_queue_create_info.pQueuePriorities = &l_queue_priority;
        l_queue_create_infos.push_back(l_queue_create_info);
    }
    
    VkPhysicalDeviceFeatures l_device_features = {};
    l_device_features.samplerAnisotropy = VK_TRUE;
    
    VkDeviceCreateInfo l_create_info = {};
    l_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    
    l_create_info.queueCreateInfoCount = static_cast<uint32_t>(l_queue_create_infos.size());
    l_create_info.pQueueCreateInfos = l_queue_create_infos.data();
    
    l_create_info.pEnabledFeatures = &l_device_features;
    l_create_info.enabledExtensionCount = static_cast<uint32_t>(this->m_device_extensions.size());
    l_create_info.ppEnabledExtensionNames = this->m_device_extensions.data();
    
    // might not really be necessary anymore because device specific validation layers
    // have been deprecated
    if (this->_are_validation_layers_enabled)
    {
        l_create_info.enabledLayerCount = static_cast<uint32_t>(this->m_validation_layers.size());
        l_create_info.ppEnabledLayerNames = this->m_validation_layers.data();
    } else 
    {
        l_create_info.enabledLayerCount = 0;
    }
    
    FE_EXIT(vkCreateDevice(this->m_physical_device, &l_create_info, nullptr, &this->m_device) != VK_SUCCESS, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "failed to create logical device!")
    
    vkGetDeviceQueue(this->m_device, l_indices._graphics_family, 0, &this->m_graphics_queue);
    vkGetDeviceQueue(this->m_device, l_indices._present_family, 0, &this->m_present_queue);
}

void device::__create_command_pool() 
{
      queue_family_indices l_queue_family_indices = find_physical_queue_families();
    
      VkCommandPoolCreateInfo l_pool_info = {};
      l_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
      l_pool_info.queueFamilyIndex = l_queue_family_indices._graphics_family;
      l_pool_info.flags =
          VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    
      FE_EXIT(vkCreateCommandPool(m_device, &l_pool_info, nullptr, &m_command_pool) != VK_SUCCESS, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "failed to create command pool!");
}

void device::__create_surface() { this->m_window.create_window_surface(this->m_vulkan_instance, &this->m_surface); }

var::boolean device::__is_device_suitable(VkPhysicalDevice device_p) 
{
    queue_family_indices l_indices = __find_queue_families(device_p);
    
    var::boolean l_extensions_supported = __check_device_extension_support(device_p);
    
    var::boolean l_swap_chain_adequate = false;
    if (l_extensions_supported) 
    {
        swap_chain_support_details l_swap_chain_support = __query_swap_chain_support(device_p);
        l_swap_chain_adequate = !l_swap_chain_support._formats.empty() && !l_swap_chain_support._present_modes.empty();
    }
    
    VkPhysicalDeviceFeatures l_supported_features;
    vkGetPhysicalDeviceFeatures(device_p, &l_supported_features);
    
    return l_indices.is_complete() && l_extensions_supported && l_swap_chain_adequate &&
           l_supported_features.samplerAnisotropy;
}

void device::__populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info_p) 
{
    create_info_p = {};
    create_info_p.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info_p.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info_p.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info_p.pfnUserCallback = debug_callback;
    create_info_p.pUserData = nullptr;  // Optional
}

void device::__setup_debug_messenger() 
{
    if (_are_validation_layers_enabled == false) return;

    VkDebugUtilsMessengerCreateInfoEXT l_create_info;
    __populate_debug_messenger_create_info(l_create_info);

    FE_EXIT(create_debug_utils_messenger_EXT(m_vulkan_instance, &l_create_info, nullptr, &m_debug_messenger) != VK_SUCCESS, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "failed to set up debug messenger!");
}

#pragma warning(push)
#pragma warning(disable: 4702)
var::boolean device::__check_validation_layer_support() 
{
    var::uint32 l_layer_count;
    vkEnumerateInstanceLayerProperties(&l_layer_count, nullptr);
    
    std::vector<VkLayerProperties> l_available_layers(l_layer_count);
    vkEnumerateInstanceLayerProperties(&l_layer_count, l_available_layers.data());
    
    const char* l_layer_name = this->m_validation_layers.front();
    
    for (var::size_t i = 0; i < l_layer_count; ++i)
    {
        if (strcmp(l_layer_name, l_available_layers[i].layerName) == 0)
        {
            return true;
        }
    }

    return false;
}
#pragma warning(pop)

std::vector<const char *> device::__get_required_extensions() 
{
    var::uint32 l_GLFW_extension_count = 0;
    const char** l_GLFW_extensions;
    l_GLFW_extensions = glfwGetRequiredInstanceExtensions(&l_GLFW_extension_count);
    
    std::vector<const char *> l_extensions(l_GLFW_extensions, l_GLFW_extensions + l_GLFW_extension_count);
    
    if (_are_validation_layers_enabled)
    {
        l_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    
    return l_extensions;
}

void device::__has_GLFW_required_instance_extensions() 
{
    var::uint32 l_extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &l_extension_count, nullptr);
    std::vector<VkExtensionProperties> l_extensions(l_extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &l_extension_count, l_extensions.data());
    
    std::cout << "available extensions:" << std::endl;
    std::unordered_set<std::string> l_available;
    for (const auto &extension : l_extensions) 
    {
      std::cout << "\t" << extension.extensionName << std::endl;
      l_available.insert(extension.extensionName);
    }
    
    std::cout << "required extensions:" << std::endl;
    auto l_required_extensions = __get_required_extensions();
    for (const auto &required : l_required_extensions) 
    {
        std::cout << "\t" << required << std::endl;
        FE_EXIT(l_available.find(required) == l_available.end(), RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "Missing required glfw extension");
    }
}

var::boolean device::__check_device_extension_support(VkPhysicalDevice device) 
{
    uint32_t l_extension_count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &l_extension_count, nullptr);
    
    std::vector<VkExtensionProperties> l_available_extensions(l_extension_count);
    vkEnumerateDeviceExtensionProperties(
        device,
        nullptr,
        &l_extension_count,
        l_available_extensions.data());
    
    std::set<std::string> l_required_extensions(this->m_device_extensions.begin(), this->m_device_extensions.end());
    
    for (const auto &extension : l_available_extensions) 
    {
        l_required_extensions.erase(extension.extensionName);
    }
    
    return l_required_extensions.empty();
}

queue_family_indices device::__find_queue_families(VkPhysicalDevice device_p) 
{
    queue_family_indices l_indices;
    
    uint32_t l_queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device_p, &l_queue_family_count, nullptr);
    
    std::vector<VkQueueFamilyProperties> l_queue_families(l_queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device_p, &l_queue_family_count, l_queue_families.data());
    
    var::int32 i = 0;
    for (const auto &queue_family : l_queue_families) 
    {
        if (queue_family.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
        {
            l_indices._graphics_family = i;
            l_indices._graphics_family_has_value = true;
        }

        VkBool32 l_present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device_p, i, m_surface, &l_present_support);

        if (queue_family.queueCount > 0 && l_present_support) 
        {
            l_indices._present_family = i;
            l_indices._present_family_has_value = true;
        }
        if (l_indices.is_complete()) 
        {
            break;
        }
        
        i++;
    }
    
    return l_indices;
}

swap_chain_support_details device::__query_swap_chain_support(VkPhysicalDevice device_p) 
{
    swap_chain_support_details l_details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device_p, this->m_surface, &l_details._capabilities);

    var::uint32 l_format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device_p, this->m_surface, &l_format_count, nullptr);

    if (l_format_count != 0) 
    {
        l_details._formats.resize(l_format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device_p, this->m_surface, &l_format_count, l_details._formats.data());
    }

    var::uint32 l_present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device_p, this->m_surface, &l_present_mode_count, nullptr);

    if (l_present_mode_count != 0) 
    {
        l_details._present_modes.resize(l_present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device_p, m_surface, &l_present_mode_count, l_details._present_modes.data());
    }
    return l_details;
}

VkFormat device::find_supported_format(const std::vector<VkFormat>& candidates_p, VkImageTiling tiling_p, VkFormatFeatureFlags features_p) 
{
    for (VkFormat format : candidates_p) 
    {
        VkFormatProperties l_props;
        vkGetPhysicalDeviceFormatProperties(this->m_physical_device, format, &l_props);
        
        if (tiling_p == VK_IMAGE_TILING_LINEAR && (l_props.linearTilingFeatures & features_p) == features_p) 
        {
          return format;
        } 
        else if (tiling_p == VK_IMAGE_TILING_OPTIMAL && (l_props.optimalTilingFeatures & features_p) == features_p) 
        {
          return format;
        }
    }


    FE_EXIT(true, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "failed to find supported format!");
}

var::uint32 device::find_memory_type(uint32 type_filter_p, VkMemoryPropertyFlags properties_p) 
{
    VkPhysicalDeviceMemoryProperties l_mem_properties;
    vkGetPhysicalDeviceMemoryProperties(this->m_physical_device, &l_mem_properties);

    for (uint32_t i = 0; i < l_mem_properties.memoryTypeCount; i++) 
    {
        if ((type_filter_p & (1 << i)) &&
            (l_mem_properties.memoryTypes[i].propertyFlags & properties_p) == properties_p) 
        {
            return i;
        }
    }
    
    FE_EXIT(true, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "failed to find suitable memory type!");
}

void device::create_buffer(VkDeviceSize size_p,
                           VkBufferUsageFlags usage_p,
                           VkMemoryPropertyFlags properties_p,
                           VkBuffer& buffer_p,
                           VkDeviceMemory& buffer_memory_p) 
{
    VkBufferCreateInfo l_buffer_info{};
    l_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    l_buffer_info.size = size_p;
    l_buffer_info.usage = usage_p;
    l_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    FE_EXIT(vkCreateBuffer(this->m_device, &l_buffer_info, nullptr, &buffer_p) != VK_SUCCESS, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN,"failed to create vertex buffer!");
    
    VkMemoryRequirements l_mem_requirements;
    vkGetBufferMemoryRequirements(this->m_device, buffer_p, &l_mem_requirements);
    
    VkMemoryAllocateInfo l_alloc_info{};
    l_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    l_alloc_info.allocationSize = l_mem_requirements.size;
    l_alloc_info.memoryTypeIndex = find_memory_type(l_mem_requirements.memoryTypeBits, properties_p);
    
    FE_EXIT(vkAllocateMemory(this->m_device, &l_alloc_info, nullptr, &buffer_memory_p) != VK_SUCCESS, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "failed to allocate vertex buffer memory!");
    vkBindBufferMemory(this->m_device, buffer_p, buffer_memory_p, 0);
}

VkCommandBuffer device::begin_single_time_commands()
{
    VkCommandBufferAllocateInfo l_alloc_info{};
    l_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    l_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    l_alloc_info.commandPool = this->m_command_pool;
    l_alloc_info.commandBufferCount = 1;
    
    VkCommandBuffer l_command_buffer;
    vkAllocateCommandBuffers(m_device, &l_alloc_info, &l_command_buffer);
    
    VkCommandBufferBeginInfo l_begin_info{};
    l_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    l_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
    vkBeginCommandBuffer(l_command_buffer, &l_begin_info);
    return l_command_buffer;
}

void device::end_single_time_commands(VkCommandBuffer command_buffer_p) 
{
    vkEndCommandBuffer(command_buffer_p);
    
    VkSubmitInfo l_submit_info{};
    l_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    l_submit_info.commandBufferCount = 1;
    l_submit_info.pCommandBuffers = &command_buffer_p;
    
    vkQueueSubmit(this->m_graphics_queue, 1, &l_submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(this->m_graphics_queue);
    
    vkFreeCommandBuffers(this->m_device, this->m_command_pool, 1, &command_buffer_p);
}

void device::copy_buffer(VkBuffer source_buffer_p, VkBuffer dest_buffer_p, VkDeviceSize size_p) 
{
    VkCommandBuffer l_command_buffer = begin_single_time_commands();
    
    VkBufferCopy l_copy_region{};
    l_copy_region.srcOffset = 0;  // Optional
    l_copy_region.dstOffset = 0;  // Optional
    l_copy_region.size = size_p;
    vkCmdCopyBuffer(l_command_buffer, source_buffer_p, dest_buffer_p, 1, &l_copy_region);
    
    end_single_time_commands(l_command_buffer);
}

void device::copyBufferToImage(VkBuffer buffer_p, VkImage image_p, uint32 width_p, uint32 height_p, uint32 layer_count_p) 
{
    VkCommandBuffer l_command_buffer = begin_single_time_commands();
    
    VkBufferImageCopy l_region{};
    l_region.bufferOffset = 0;
    l_region.bufferRowLength = 0;
    l_region.bufferImageHeight = 0;
    
    l_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    l_region.imageSubresource.mipLevel = 0;
    l_region.imageSubresource.baseArrayLayer = 0;
    l_region.imageSubresource.layerCount = layer_count_p;
    
    l_region.imageOffset = {0, 0, 0};
    l_region.imageExtent = {width_p, height_p, 1};
    
    vkCmdCopyBufferToImage(l_command_buffer,
                           buffer_p,
                           image_p,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &l_region
                            );
    end_single_time_commands(l_command_buffer);
}

void device::create_image_with_info(const VkImageCreateInfo& image_info_p,
                                    VkMemoryPropertyFlags properties_p,
                                    VkImage& image_p,
                                    VkDeviceMemory& image_memory_p) 
{
    FE_EXIT(vkCreateImage(this->m_device, &image_info_p, nullptr, &image_p) != VK_SUCCESS, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "failed to create image!");
    
    VkMemoryRequirements l_mem_requirements;
    vkGetImageMemoryRequirements(this->m_device, image_p, &l_mem_requirements);
    
    VkMemoryAllocateInfo l_alloc_info{};
    l_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    l_alloc_info.allocationSize = l_mem_requirements.size;
    l_alloc_info.memoryTypeIndex = find_memory_type(l_mem_requirements.memoryTypeBits, properties_p);
    
    FE_EXIT(vkAllocateMemory(this->m_device, &l_alloc_info, nullptr, &image_memory_p) != VK_SUCCESS, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "failed to allocate image memory!");
    FE_EXIT(vkBindImageMemory(this->m_device, image_p, image_memory_p, 0) != VK_SUCCESS, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "failed to bind image memory!");
}

END_NAMESPACE