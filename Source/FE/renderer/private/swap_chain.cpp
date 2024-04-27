#include <FE/renderer/swap_chain.hpp>

// std
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>




BEGIN_NAMESPACE(FE::renderer)


swap_chain::swap_chain(FE::renderer::device& device_p, VkExtent2D window_extent_p) : m_device{device_p}, m_window_extent{window_extent_p}
{
    __create_swap_chain();
    __create_image_views();
    __create_render_pass();
    __create_depth_resources();
    __create_frame_buffers();
    __create_sync_objects();
}

swap_chain::~swap_chain() 
{
    for (auto image_view : this->m_swap_chain_image_views) 
    {
        vkDestroyImageView(this->m_device.get_device(), image_view, nullptr);
    }
    this->m_swap_chain_image_views.clear();
    
    if (this->m_swap_chain != nullptr) 
    {
        vkDestroySwapchainKHR(this->m_device.get_device(), this->m_swap_chain, nullptr);
        this->m_swap_chain = nullptr;
    }
    
    for (var::int32 i = 0; i < this->m_depth_images.size(); ++i) 
    {
        vkDestroyImageView(this->m_device.get_device(), this->m_depth_image_views[i], nullptr);
        vkDestroyImage(this->m_device.get_device(), this->m_depth_images[i], nullptr);
        vkFreeMemory(this->m_device.get_device(), this->m_depth_image_memorys[i], nullptr);
    }
    
    for (auto frame_buffer : this->m_swap_chain_frame_buffers)
    {
        vkDestroyFramebuffer(this->m_device.get_device(), frame_buffer, nullptr);
    }
    
    vkDestroyRenderPass(this->m_device.get_device(), this->m_render_pass, nullptr);
    
    // cleanup synchronization objects
    for (var::size_t i = 0; i < max_frames_in_flight; ++i) 
    {
        vkDestroySemaphore(this->m_device.get_device(), this->m_render_finished_semaphores[i], nullptr);
        vkDestroySemaphore(this->m_device.get_device(), this->m_image_available_semaphores[i], nullptr);
        vkDestroyFence(this->m_device.get_device(), this->m_in_flight_fences[i], nullptr);
    }
}

VkResult swap_chain::acquire_next_image(var::uint32* image_index_p) 
{
  vkWaitForFences(  this->m_device.get_device(), 
                    1, 
                    &this->m_in_flight_fences[this->m_current_frame], 
                    VK_TRUE, 
                    std::numeric_limits<uint64_t>::max());

  VkResult l_result = vkAcquireNextImageKHR(  this->m_device.get_device(), 
                                            this->m_swap_chain, 
                                            std::numeric_limits<uint64_t>::max(), 
                                            this->m_image_available_semaphores[this->m_current_frame],  // must be a not signaled semaphore
                                            VK_NULL_HANDLE,
                                            image_index_p);

  return l_result;
}

VkResult swap_chain::submit_command_buffers(const VkCommandBuffer *buffers, uint32* image_index_p)
{
    if (this->m_images_in_flight[*image_index_p] != VK_NULL_HANDLE)
    {
        vkWaitForFences(this->m_device.get_device(), 1, &this->m_images_in_flight[*image_index_p], VK_TRUE, UINT64_MAX);
    }
    this->m_images_in_flight[*image_index_p] = this->m_in_flight_fences[this->m_current_frame];

    VkSubmitInfo l_submit_info = {};
    l_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore l_wait_semaphores[] = { this->m_image_available_semaphores[this->m_current_frame]};
    VkPipelineStageFlags l_wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    l_submit_info.waitSemaphoreCount = 1;
    l_submit_info.pWaitSemaphores = l_wait_semaphores;
    l_submit_info.pWaitDstStageMask = l_wait_stages;

    l_submit_info.commandBufferCount = 1;
    l_submit_info.pCommandBuffers = buffers;

    VkSemaphore l_signal_semaphores[] = {this->m_render_finished_semaphores[this->m_current_frame]};
    l_submit_info.signalSemaphoreCount = 1;
    l_submit_info.pSignalSemaphores = l_signal_semaphores;

    vkResetFences(this->m_device.get_device(), 1, &this->m_in_flight_fences[this->m_current_frame]);

    FE_EXIT(vkQueueSubmit(this->m_device.get_graphics_queue(), 1, &l_submit_info, this->m_in_flight_fences[this->m_current_frame]) != VK_SUCCESS, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "Failed to submit draw command buffer!");

    VkPresentInfoKHR l_present_info = {};
    l_present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    l_present_info.waitSemaphoreCount = 1;
    l_present_info.pWaitSemaphores = l_signal_semaphores;

    VkSwapchainKHR l_swap_chains[] = {this->m_swap_chain};
    l_present_info.swapchainCount = 1;
    l_present_info.pSwapchains = l_swap_chains;

    l_present_info.pImageIndices = image_index_p;

    auto l_result = vkQueuePresentKHR(this->m_device.get_present_queue(), &l_present_info);

    this->m_current_frame = (this->m_current_frame + 1) % max_frames_in_flight;

    return l_result;
}

void swap_chain::__create_swap_chain()
{
    FE::unique_ptr<swap_chain_support_details> l_swap_chain_support = this->m_device.get_swap_chain_support();

    VkSurfaceFormatKHR l_surface_format = __choose_swap_surface_format(l_swap_chain_support->_formats);
    VkPresentModeKHR l_present_mode = __choose_swap_present_mode(l_swap_chain_support->_present_modes);
    VkExtent2D l_extent = __choose_swap_extent(l_swap_chain_support->_capabilities);
    
    var::uint32 l_image_count = l_swap_chain_support->_capabilities.minImageCount + 1;
    if (l_swap_chain_support->_capabilities.maxImageCount > 0 &&
        l_image_count > l_swap_chain_support->_capabilities.maxImageCount) 
    {
        l_image_count = l_swap_chain_support->_capabilities.maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR l_create_info = {};
    l_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    l_create_info.surface = this->m_device.get_surface();
    
    l_create_info.minImageCount = l_image_count;
    l_create_info.imageFormat = l_surface_format.format;
    l_create_info.imageColorSpace = l_surface_format.colorSpace;
    l_create_info.imageExtent = l_extent;
    l_create_info.imageArrayLayers = 1;
    l_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    queue_family_indices l_indices = this->m_device.find_physical_queue_families();
    var::uint32 l_queue_family_indices[] = {l_indices._graphics_family, l_indices._present_family};
    
    if (l_indices._graphics_family != l_indices._present_family)
    {
        l_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        l_create_info.queueFamilyIndexCount = 2;
        l_create_info.pQueueFamilyIndices = l_queue_family_indices;
    } 
    else 
    {
        l_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        l_create_info.queueFamilyIndexCount = 0;      // Optional
        l_create_info.pQueueFamilyIndices = nullptr;  // Optional
    }
    
    l_create_info.preTransform = l_swap_chain_support->_capabilities.currentTransform;
    l_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    
    l_create_info.presentMode = l_present_mode;
    l_create_info.clipped = VK_TRUE;
    
    l_create_info.oldSwapchain = VK_NULL_HANDLE;
    
    FE_EXIT(vkCreateSwapchainKHR(this->m_device.get_device(), &l_create_info, nullptr, &this->m_swap_chain) != VK_SUCCESS, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "Failed to create swap chain!");
    
    // we only specified a minimum number of images in the swap chain, so the implementation is
    // allowed to create a swap chain with more. That's why we'll first query the final number of
    // images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
    // retrieve the handles.
    vkGetSwapchainImagesKHR(this->m_device.get_device(), this->m_swap_chain, &l_image_count, nullptr);
    this->m_swap_chain_images.resize(l_image_count);
    vkGetSwapchainImagesKHR(this->m_device.get_device(), this->m_swap_chain, &l_image_count, this->m_swap_chain_images.data());
    
    this->m_swap_chain_image_format = l_surface_format.format;
    this->m_swap_chain_extent = l_extent;
}

void swap_chain::__create_image_views() 
{
    this->m_swap_chain_image_views.resize(this->m_swap_chain_images.size());
    VkImageViewCreateInfo l_view_info{};
    l_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    l_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    l_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    l_view_info.subresourceRange.baseMipLevel = 0;
    l_view_info.subresourceRange.levelCount = 1;
    l_view_info.subresourceRange.baseArrayLayer = 0;
    l_view_info.subresourceRange.layerCount = 1;

    for (var::size_t i = 0; i < this->m_swap_chain_images.size(); ++i)
    {
        l_view_info.image = this->m_swap_chain_images[i];
        l_view_info.format = this->m_swap_chain_image_format;
    
        FE_EXIT(vkCreateImageView(this->m_device.get_device(), &l_view_info, nullptr, &this->m_swap_chain_image_views[i]) != VK_SUCCESS, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "Failed to create texture image view!");
    }
}

void swap_chain::__create_render_pass()
{
    VkAttachmentDescription l_depth_attachment{};
    l_depth_attachment.format = find_depth_format();
    l_depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    l_depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    l_depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    l_depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    l_depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    l_depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    l_depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
    VkAttachmentReference l_depth_attachment_ref{};
    l_depth_attachment_ref.attachment = 1;
    l_depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
    VkAttachmentDescription l_color_attachment{};
    l_color_attachment.format = get_swapC_chain_image_format();
    l_color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    l_color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    l_color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    l_color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    l_color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    l_color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    l_color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
    VkAttachmentReference l_color_attachment_ref{};
    l_color_attachment_ref.attachment = 0;
    l_color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    
    VkSubpassDescription l_subpass = {};
    l_subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    l_subpass.colorAttachmentCount = 1;
    l_subpass.pColorAttachments = &l_color_attachment_ref;
    l_subpass.pDepthStencilAttachment = &l_depth_attachment_ref;
    
    VkSubpassDependency l_dependency{};
    l_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    l_dependency.srcAccessMask = 0;
    l_dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    l_dependency.dstSubpass = 0;
    l_dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    l_dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    
    std::array<VkAttachmentDescription, 2> l_attachments = {l_color_attachment, l_depth_attachment};
    VkRenderPassCreateInfo l_render_pass_info{};
    l_render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    l_render_pass_info.attachmentCount = static_cast<uint32_t>(l_attachments.size());
    l_render_pass_info.pAttachments = l_attachments.data();
    l_render_pass_info.subpassCount = 1;
    l_render_pass_info.pSubpasses = &l_subpass;
    l_render_pass_info.dependencyCount = 1;
    l_render_pass_info.pDependencies = &l_dependency;
    
    FE_EXIT(vkCreateRenderPass(this->m_device.get_device(), &l_render_pass_info, nullptr, &this->m_render_pass) != VK_SUCCESS, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "Failed to create render pass!");
}

void swap_chain::__create_frame_buffers() 
{
    this->m_swap_chain_frame_buffers.resize(image_count());
    VkFramebufferCreateInfo l_frame_buffer_info{};
    l_frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    l_frame_buffer_info.layers = 1;

    for (var::size_t i = 0; i < image_count(); ++i)
    {
        std::array<VkImageView, 2> l_attachments = {this->m_swap_chain_image_views[i], this->m_depth_image_views[i]};
        
        VkExtent2D l_swap_chain_extent = get_swap_chain_extent();
        l_frame_buffer_info.renderPass = this->m_render_pass;
        l_frame_buffer_info.attachmentCount = static_cast<uint32>(l_attachments.size());
        l_frame_buffer_info.pAttachments = l_attachments.data();
        l_frame_buffer_info.width = l_swap_chain_extent.width;
        l_frame_buffer_info.height = l_swap_chain_extent.height;

        FE_EXIT(vkCreateFramebuffer(this->m_device.get_device(), &l_frame_buffer_info, nullptr, &this->m_swap_chain_frame_buffers[i]) != VK_SUCCESS, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "Failed to create frame buffer!");
    }
}

void swap_chain::__create_depth_resources() 
{
    VkFormat l_depth_format = find_depth_format();
    VkExtent2D l_swap_chain_extent = get_swap_chain_extent();

    this->m_depth_images.resize(image_count());
    this->m_depth_image_memorys.resize(image_count());
    this->m_depth_image_views.resize(image_count());
    
    VkImageCreateInfo l_image_info{};
    l_image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    l_image_info.imageType = VK_IMAGE_TYPE_2D;
    l_image_info.extent.depth = 1;
    l_image_info.mipLevels = 1;
    l_image_info.arrayLayers = 1;
    l_image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    l_image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    l_image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    l_image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    l_image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    l_image_info.flags = 0;

    VkImageViewCreateInfo l_view_info{};
    l_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    l_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    l_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    l_view_info.subresourceRange.baseMipLevel = 0;
    l_view_info.subresourceRange.levelCount = 1;
    l_view_info.subresourceRange.baseArrayLayer = 0;
    l_view_info.subresourceRange.layerCount = 1;

    for (int i = 0; i < this->m_depth_images.size(); i++) 
    {
        l_image_info.extent.width = l_swap_chain_extent.width;
        l_image_info.extent.height = l_swap_chain_extent.height;
        l_image_info.format = l_depth_format;
        this->m_device.create_image_with_info(  l_image_info,
                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                this->m_depth_images[i],
                                                this->m_depth_image_memorys[i]
                                                );
        
    
        l_view_info.image = this->m_depth_images[i];
        l_view_info.format = l_depth_format;

        FE_EXIT(vkCreateImageView(this->m_device.get_device(), &l_view_info, nullptr, &this->m_depth_image_views[i]) != VK_SUCCESS, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "Failed to create texture image view!");
    }
}

void swap_chain::__create_sync_objects()
{
    this->m_image_available_semaphores.resize(max_frames_in_flight);
    this->m_render_finished_semaphores.resize(max_frames_in_flight);
    this->m_in_flight_fences.resize(max_frames_in_flight);
    this->m_images_in_flight.resize(image_count(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo l_semaphore_info = {};
    l_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo l_fence_info = {};
    l_fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    l_fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (var::size_t i = 0; i < max_frames_in_flight; ++i)
    {
        FE_EXIT(vkCreateSemaphore(this->m_device.get_device(), &l_semaphore_info, nullptr, &this->m_image_available_semaphores[i]) != VK_SUCCESS
            ||
            vkCreateSemaphore(this->m_device.get_device(), &l_semaphore_info, nullptr, &this->m_render_finished_semaphores[i]) != VK_SUCCESS
            ||
            vkCreateFence(this->m_device.get_device(), &l_fence_info, nullptr, &this->m_in_flight_fences[i]) != VK_SUCCESS, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "Failed to create synchronization objects for a frame!");
    }
}

VkSurfaceFormatKHR swap_chain::__choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> &available_formats_p)
{
    for (const auto &available_format : available_formats_p) 
    {
        if (available_format.format == VK_FORMAT_B8G8R8A8_UNORM &&
            available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return available_format;
        }
    }

    return available_formats_p[0];
}

VkPresentModeKHR swap_chain::__choose_swap_present_mode(const std::vector<VkPresentModeKHR> &available_present_modes_p) 
{
    for (const auto &available_present_mode : available_present_modes_p)
    {
        if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) 
        {
            std::cout << "Present mode: Mailbox" << std::endl;
            return available_present_mode;
        }
    }

    // for (const auto &available_present_mode : available_present_modes_p) {
    //   if (available_present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
    //     std::cout << "Present mode: Immediate" << std::endl;
    //     return available_present_mode;
    //   }
    // }

    std::cout << "Present mode: V-Sync" << std::endl;
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D swap_chain::__choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities_p) 
{
    if (capabilities_p.currentExtent.width != std::numeric_limits<uint32>::max()) 
    {
        return capabilities_p.currentExtent;
    } 
    else
    {
        VkExtent2D l_actual_extent = this->m_window_extent;
        l_actual_extent.width = std::max(capabilities_p.minImageExtent.width, std::min(capabilities_p.maxImageExtent.width, l_actual_extent.width));
        l_actual_extent.height = std::max(capabilities_p.minImageExtent.height, std::min(capabilities_p.maxImageExtent.height, l_actual_extent.height));

        return l_actual_extent;
    }
}

VkFormat swap_chain::find_depth_format() 
{
    return this->m_device.find_supported_format(    {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                                                    VK_IMAGE_TILING_OPTIMAL,
                                                    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
                                                    );
}


END_NAMESPACE