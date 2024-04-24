#ifndef _FE_RENDERER_WINDOW_HPP_
#define _FE_RENDERER_WINDOW_HPP_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/renderer/settings.h>

// Frogman Engine Libraries
#define _MEMORY_POOL_FE_STRING_
#include <FE/core/string.hxx>

// Renderer Libraries
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>




BEGIN_NAMESPACE(FE::renderer)


class window
{
	GLFWwindow* m_window;
	FE::string m_title;

public:
	window(uint32 width_p, uint32 height_p, const char* title_p) noexcept;
	~window() noexcept;

	_FORCE_INLINE_ boolean should_window_be_closed() const noexcept { return glfwWindowShouldClose(this->m_window); }
	_FORCE_INLINE_ const char* get_title() const noexcept { return this->m_title.c_str(); }

	_FORCE_INLINE_ void create_window_surface(VkInstance instance_p, VkSurfaceKHR* surface_p) const noexcept
	{
		FE_EXIT(glfwVulkanSupported() == GLFW_FALSE, RENDERER_ERROR_TYPE::ERROR_FROM_GLFW, "GLFW on your pc does not support your vulkan version");
		auto l_vk_error_code = glfwCreateWindowSurface(instance_p, this->m_window, nullptr, surface_p);
		FE_EXIT(l_vk_error_code != VK_SUCCESS, l_vk_error_code, "Failed to create window surface.");
	}


	uint32 _width;
	uint32 _height;


	window(const window&) = delete;
	window(window&&) = delete;
	window& operator=(const window&) = delete;
	window& operator=(window&&) = delete;

};


END_NAMESPACE
#endif
