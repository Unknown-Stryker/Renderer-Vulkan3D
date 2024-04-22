// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
//#include <gtest/gtest.h>
//#include <benchmark/benchmark.h>

// Frogman Engine Libraries
#include <FE/core/prerequisites.h>
#include <FE/framework/framework.hpp>
#include <FE/core/pool.hxx>

#include <FE/renderer/settings.h>
#include <FE/renderer/window.hpp>

#include <FE/miscellaneous/private/macro_restrictions.h>



// FE.log does not print messages to a file when it is operating outside of its development project or an app entry C++ file
//https://learn.microsoft.com/en-us/cpp/build/profile-guided-optimizations?view=msvc-170
// Profile Guided Optimization
class vulkan_renderer final : public FE::framework::application
{
	FE::renderer::window m_window;

public:
	vulkan_renderer() noexcept : m_window(800, 800, "Vulkan Renderer")
	{
	}

private:
	virtual void set_up(_MAYBE_UNUSED_ int argc_p, _MAYBE_UNUSED_ char** argv_p) override final
	{
	}
	
	virtual int run(_MAYBE_UNUSED_ int argc_p, _MAYBE_UNUSED_ char** argv_p) override final
	{
		while (this->m_window.should_window_be_closed() == false)
		{
			glfwPollEvents();
		}
		return 0;
	}

	virtual void clean_up() override final
	{

	}

public:
	~vulkan_renderer()
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