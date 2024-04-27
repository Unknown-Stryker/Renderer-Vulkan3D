#ifndef _FE_RENDERER_SWAP_CHAIN_HPP_
#include <FE/core/prerequisites.h>
#include <FE/renderer/settings.h>

#include <FE/renderer/device.hpp>

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>




BEGIN_NAMESPACE(FE::renderer)


class swap_chain 
{
    VkFormat m_swap_chain_image_format;
    VkExtent2D m_swap_chain_extent;

    std::vector<VkFramebuffer> m_swap_chain_frame_buffers;
    VkRenderPass m_render_pass;

    std::vector<VkImage> m_depth_images;
    std::vector<VkDeviceMemory> m_depth_image_memorys;
    std::vector<VkImageView> m_depth_image_views;
    std::vector<VkImage> m_swap_chain_images;
    std::vector<VkImageView> m_swap_chain_image_views;

    device& m_device;
    VkExtent2D m_window_extent;

    VkSwapchainKHR m_swap_chain;

    std::vector<VkSemaphore> m_image_available_semaphores;
    std::vector<VkSemaphore> m_render_finished_semaphores;
    std::vector<VkFence> m_in_flight_fences;
    std::vector<VkFence> m_images_in_flight;
    var::size_t m_current_frame = 0;

public:
    static constexpr int32 max_frames_in_flight = 2;
    
    swap_chain(device& device_p, VkExtent2D window_extent_p);
    ~swap_chain();
    
    swap_chain(const swap_chain &) = delete;
    swap_chain(swap_chain&&) = delete;
    void operator=(const swap_chain &) = delete;
    void operator=(swap_chain&&) = delete;
    
    _FORCE_INLINE_ VkFramebuffer get_frame_buffer(int32 index_p) { return this->m_swap_chain_frame_buffers[index_p]; }
    _FORCE_INLINE_ VkRenderPass get_render_pass() { return this->m_render_pass; }
    _FORCE_INLINE_ VkImageView get_image_view(int32 index_p) { return this->m_swap_chain_image_views[index_p]; }
    _FORCE_INLINE_ var::size_t image_count() { return this->m_swap_chain_images.size(); }
    _FORCE_INLINE_ VkFormat get_swapC_chain_image_format() { return m_swap_chain_image_format; }
    _FORCE_INLINE_ VkExtent2D get_swap_chain_extent() { return this->m_swap_chain_extent; }
    _FORCE_INLINE_ var::uint32 get_width() { return this->m_swap_chain_extent.width; }
    _FORCE_INLINE_ var::uint32 get_height() { return this->m_swap_chain_extent.height; }
    _FORCE_INLINE_ var::float32 extent_aspect_ratio() { return static_cast<float>(this->m_swap_chain_extent.width) / static_cast<float>(this->m_swap_chain_extent.height); }

    VkFormat find_depth_format();
    VkResult acquire_next_image(var::uint32* image_index_p);
    VkResult submit_command_buffers(const VkCommandBuffer* buffers_p, uint32* image_index_p);
    
private:
    void __create_swap_chain();
    void __create_image_views();
    void __create_depth_resources();
    void __create_render_pass();
    void __create_frame_buffers();
    void __create_sync_objects();

    // Helper functions
    VkSurfaceFormatKHR __choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats_p);
    VkPresentModeKHR __choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes_p);
    VkExtent2D __choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities_p);
};


END_NAMESPACE
#endif 