// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
//#include <gtest/gtest.h>
//#include <benchmark/benchmark.h>

// Frogman Engine Libraries
#include <FE/core/prerequisites.h>
#include <FE/framework/framework.hpp>
#include <FE/core/algorithm/string.hxx>
#include <FE/core/pool.hxx>

#include <FE/renderer/settings.h>
#include <FE/renderer/window.hpp>
#include <FE/renderer/device.hpp>
#include <FE/renderer/pipeline.hpp>

#include <FE/miscellaneous/private/macro_restrictions.h>




// FE.log does not print messages to a file when it is operating outside of its development project or an app entry C++ file
//https://learn.microsoft.com/en-us/cpp/build/profile-guided-optimizations?view=msvc-170
// Profile Guided Optimization
class vulkan_renderer final : public FE::framework::application
{
	FE::renderer::window m_window;
	FE::renderer::device m_device;
	FE::renderer::pipeline m_pipeline;


public:
	vulkan_renderer() noexcept : m_window(800, 800, "Vulkan Renderer"), m_device(m_window), m_pipeline(m_device, FE::renderer::pipeline::default_pipeline_config_info(m_window._width, m_window._height))
	{
		using namespace FE;

		constexpr const char* l_vert_shader_subdirectory =
#ifdef  _WINDOWS_X86_64_
			"\\..\\shaders\\bin\\sample_shader.vert.spv";
#else
			"/../shaders/bin/sample_shader.vert.spv";
#endif
		constexpr length_t l_path_to_vert_shader_len = algorithm::string::length(_CMAKE_PROJECT_DIRECTORY_) + algorithm::string::length(l_vert_shader_subdirectory);
		FE::string l_path_to_vert_shader;
		l_path_to_vert_shader.reserve(l_path_to_vert_shader_len);
		l_path_to_vert_shader = _CMAKE_PROJECT_DIRECTORY_;
		l_path_to_vert_shader += l_vert_shader_subdirectory;


		constexpr const char* l_frag_shader_subdirectory =
#ifdef  _WINDOWS_X86_64_
			"\\..\\shaders\\bin\\sample_shader.frag.spv";
#else
			"/../shaders/bin/sample_shader.frag.spv";
#endif
		constexpr length_t l_path_to_frag_shader_len = algorithm::string::length(_CMAKE_PROJECT_DIRECTORY_) + algorithm::string::length(l_frag_shader_subdirectory);
		FE::string l_path_to_frag_shader;
		l_path_to_frag_shader.reserve(l_path_to_frag_shader_len);
		l_path_to_frag_shader = _CMAKE_PROJECT_DIRECTORY_;
		l_path_to_frag_shader += l_frag_shader_subdirectory;

		this->m_pipeline.create_pipeline(l_path_to_vert_shader.c_str(), l_path_to_frag_shader.c_str());
	}

private:
	virtual void set_up(_MAYBE_UNUSED_ int argc_p, _MAYBE_UNUSED_ char** argv_p) noexcept override final
	{
	}
	
	virtual int run(_MAYBE_UNUSED_ int argc_p, _MAYBE_UNUSED_ char** argv_p) noexcept override final
	{
		while (this->m_window.should_window_be_closed() == false)
		{
			glfwPollEvents();
		}
		return 0;
	}

	virtual void clean_up() noexcept override final
	{

	}

public:
	~vulkan_renderer() noexcept
	{
	}
};




FE::framework::application::initializer_t g_initializer = FE::framework::application::create_application
(
	[]()
	{
		return new vulkan_renderer();
	}
);