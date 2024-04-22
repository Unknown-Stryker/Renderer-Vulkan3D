// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/renderer/window.hpp>

#include <FE/core/fstring.hxx>




BEGIN_NAMESPACE(FE::renderer)


window::window(uint32 width_p, uint32 height_p, const char* title_p) noexcept : m_width(width_p), m_height(height_p), m_window(), m_title(title_p), m_pipeline()
{
	FE_ASSERT(title_p == nullptr, "The window title cannot be nullptr.");
	FE_ASSERT(width_p == 0, "width cannot be zero.");
	FE_ASSERT(height_p == 0, "height cannot be zero.");

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);	

	this->m_window = glfwCreateWindow(this->m_width, this->m_height, this->m_title.c_str(), nullptr, nullptr);
	FE_ASSERT(this->m_window == nullptr, "Failed to create window.");
	
	constexpr const char* l_vert_shader_subdirectory = 
#ifdef  _WINDOWS_X86_64_
		"\\..\\shaders\\bin\\sample_shader.vert.spv";
#else
		"/../shaders/bin/sample_shader.vert.spv";
#endif
	constexpr length_t l_path_to_vert_shader_len = algorithm::string::length(_CMAKE_PROJECT_DIRECTORY_) + algorithm::string::length(l_vert_shader_subdirectory);
	FE::fstring<l_path_to_vert_shader_len> l_path_to_vert_shader(_CMAKE_PROJECT_DIRECTORY_);

	constexpr const char* l_frag_shader_subdirectory = 
#ifdef  _WINDOWS_X86_64_
		"\\..\\shaders\\bin\\sample_shader.frag.spv";
#else
		"/../shaders/bin/sample_shader.frag.spv";
#endif
	constexpr length_t l_path_to_frag_shader_len = algorithm::string::length(_CMAKE_PROJECT_DIRECTORY_) + algorithm::string::length(l_frag_shader_subdirectory);
	FE::fstring<l_path_to_frag_shader_len> l_path_to_frag_shader(_CMAKE_PROJECT_DIRECTORY_);

	l_path_to_vert_shader += l_vert_shader_subdirectory;
	l_path_to_frag_shader += l_frag_shader_subdirectory;

	this->m_pipeline.create_pipeline(l_path_to_vert_shader.c_str(), l_path_to_frag_shader.c_str());
}

window::~window() noexcept
{
	glfwDestroyWindow(this->m_window);
	glfwTerminate();
}


END_NAMESPACE