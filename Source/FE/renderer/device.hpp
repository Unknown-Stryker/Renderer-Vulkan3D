#ifndef _FE_RENDERER_DEVICE_HPP_
#define _FE_RENDERER_DEVICE_HPP_
#include <FE/core/prerequisites.h>
#include <FE/renderer/settings.h>

// std
#include <string>
#include <vector>

// Renderer Libraries
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>




BEGIN_NAMESPACE(FE::renderer)


class window;


struct swap_chain_support_details 
{
    VkSurfaceCapabilitiesKHR _capabilities;
    std::vector<VkSurfaceFormatKHR> _formats;
    std::vector<VkPresentModeKHR> _present_modes;
};


struct queue_family_indices 
{
    var::uint32 _graphics_family;
    var::uint32 _present_family;
    var::boolean _graphics_family_has_value = false;
    var::boolean _present_family_has_value = false;

    _FORCE_INLINE_ var::boolean is_complete() noexcept { return _graphics_family_has_value && _present_family_has_value; }
};


class device 
{
    VkInstance m_vulkan_instance;
    VkDebugUtilsMessengerEXT m_debug_messenger;
    VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
    FE::renderer::window& m_window;
    VkCommandPool m_command_pool;

    VkDevice m_device;
    VkSurfaceKHR m_surface;
    VkQueue m_graphics_queue;
    VkQueue m_present_queue;

    const std::vector<const char*> m_validation_layers = { "VK_LAYER_KHRONOS_validation" };
    const std::vector<const char*> m_device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

public:
     boolean _are_validation_layers_enabled =
#if !defined(_DEBUG_)
false;
#else
true;
#endif

    device(FE::renderer::window &window_p);
    ~device();

    _FORCE_INLINE_ VkCommandPool get_command_pool() noexcept { return this->m_command_pool; }
    _FORCE_INLINE_ VkDevice get_device() noexcept { return this->m_device; }
    _FORCE_INLINE_ VkSurfaceKHR get_surface() noexcept { return this->m_surface; }
    _FORCE_INLINE_ VkQueue get_graphics_queue() noexcept { return this->m_graphics_queue; }
    _FORCE_INLINE_ VkQueue get_present_queue() noexcept { return this->m_present_queue; }
    
    _FORCE_INLINE_ swap_chain_support_details get_swap_chain_support() { return __query_swap_chain_support(this->m_physical_device); }

    uint32_t find_memory_type(uint32 type_filter_p, VkMemoryPropertyFlags properties_p);

    _FORCE_INLINE_ queue_family_indices find_physical_queue_families() { return __find_queue_families(this->m_physical_device); }

    VkFormat find_supported_format(const std::vector<VkFormat>& candidates_p, VkImageTiling tiling_p, VkFormatFeatureFlags features_p);
    
    // Buffer Helper Functions
    void create_buffer(VkDeviceSize size_p, VkBufferUsageFlags usage_p, VkMemoryPropertyFlags properties_p, VkBuffer& buffer_p, VkDeviceMemory& buffer_memory_p);

    VkCommandBuffer begin_single_time_commands();

    void end_single_time_commands(VkCommandBuffer command_buffer_p);

    void copy_buffer(VkBuffer source_buffer_p, VkBuffer dest_buffer_p, VkDeviceSize size_p);

    void copyBufferToImage(VkBuffer buffer_p, VkImage image_p, uint32_t width_p, uint32_t height_p, uint32_t layer_count_p);

    void create_image_with_info(const VkImageCreateInfo& image_info_p, VkMemoryPropertyFlags properties_p, VkImage& image_p, VkDeviceMemory& image_memory_p);
    
    VkPhysicalDeviceProperties _properties;
    
private:
    void __create_instance();
    void __setup_debug_messenger();
    void __create_surface();
    void __pick_physical_device();
    void __create_logical_device();
    void __create_command_pool();
    
    // helper functions
    var::boolean __is_device_suitable(VkPhysicalDevice device_p);
    std::vector<const char*> __get_required_extensions();
    var::boolean __check_validation_layer_support();
    queue_family_indices __find_queue_families(VkPhysicalDevice device_p);
    void __populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info_p);
    void __has_GLFW_required_instance_extensions();
    var::boolean __check_device_extension_support(VkPhysicalDevice device_p);
    swap_chain_support_details __query_swap_chain_support(VkPhysicalDevice device_p);
};

}  // namespace lve
#endif