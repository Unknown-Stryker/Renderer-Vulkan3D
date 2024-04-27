// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/renderer/window.hpp>




BEGIN_NAMESPACE(FE::renderer)


window::window(uint32 width_p, uint32 height_p, const char* title_p) noexcept : m_width(width_p), m_height(height_p), m_window(), m_title(title_p)
{
	FE_ASSERT(width_p == 0, "width cannot be zero.");
	FE_ASSERT(height_p == 0, "height cannot be zero.");
	FE_ASSERT(title_p == nullptr, "The window title cannot be nullptr.");

	FE_EXIT(glfwInit() == GLFW_FALSE, RENDERER_ERROR_TYPE::ERROR_FROM_GLFW, "Failed to initialize GLFW library");
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);	

	this->m_window = glfwCreateWindow(this->m_width, this->m_height, this->m_title.c_str(), nullptr, nullptr);
	FE_EXIT(this->m_window == nullptr, RENDERER_ERROR_TYPE::ERROR_FROM_GLFW, "Failed to create a window.");
}

window::~window() noexcept
{
	glfwDestroyWindow(this->m_window);
	glfwTerminate();
}


END_NAMESPACE