#ifndef _FE_RENDERER_PIPELINE_HPP_
#define _FE_RENDERER_PIPELINE_HPP_
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


class device;


struct pipeline_config_info {};


class pipeline
{
	device& m_device;
	VkPipeline m_graphics_pipeline;
	VkShaderModule m_vert_shader_module;
	VkShaderModule m_frag_shader_module;
	pipeline_config_info m_pipeline_config_info;

public:
	pipeline(device& device_p, const pipeline_config_info& pipeline_config_info_p) noexcept;
	~pipeline() noexcept = default;
	void create_pipeline(const char* const vert_file_path_p, const char* const frag_file_path_p) noexcept;
	void create_shader_module(const FE::string& code_p, VkShaderModule* const shader_module_p) noexcept;
	static pipeline_config_info default_pipeline_config_info(uint32 width_p, uint32 height_p) noexcept;

private:
	static FE::string __read_file(const char* const file_path_p) noexcept;

	pipeline(const pipeline& pipeline_p) = delete;
	pipeline(pipeline&& pipeline_p) = delete;
	pipeline& operator=(const pipeline& pipeline_p) = delete;
	pipeline& operator=(pipeline&& pipeline_p) = delete;
};

 
END_NAMESPACE
#endif