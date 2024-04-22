#ifndef _FE_RENDERER_WINDOW_HPP_
#define _FE_RENDERER_WINDOW_HPP_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/renderer/settings.h>
#include <FE/renderer/pipeline.hpp>

// Frogman Engine Libraries
#include <FE/core/string.hxx>

// Renderer Libraries
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>




BEGIN_NAMESPACE(FE::renderer)


class window
{
	uint32 m_width;
	uint32 m_height;

	GLFWwindow* m_window;
	FE::string m_title;
	FE::renderer::pipeline m_pipeline;

public:
	window(uint32 width_p, uint32 height_p, const char* title_p) noexcept;
	~window() noexcept;

	window(const window&) = delete;
	window(window&&) = delete;
	window& operator=(const window&) = delete;
	window& operator=(window&&) = delete;

	_FORCE_INLINE_ boolean should_window_be_closed() const noexcept { return glfwWindowShouldClose(this->m_window); }
};


END_NAMESPACE
#endif
