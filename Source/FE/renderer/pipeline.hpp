#ifndef _FE_RENDERER_PIPELINE_HPP_
#define _FE_RENDERER_PIPELINE_HPP_
#include <FE/core/prerequisites.h>
#include <FE/renderer/settings.h>

// Frogman Engine Libraries
#define _MEMORY_POOL_FE_STRING_
#include <FE/core/string.hxx>
#include <FE/core/smart_pointers/unique_ptr.hxx>

// Renderer Libraries
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>




BEGIN_NAMESPACE(FE::renderer)


class device;


struct pipeline_config_info 
{
	VkViewport _viewport;
	VkRect2D _scissor;
	VkPipelineLayout _pipeline_layout = nullptr;
	VkRenderPass _render_pass = nullptr;
	var::uint32 _subpass = 0;
	VkPipelineColorBlendAttachmentState _color_blend_attachment;

	VkPipelineInputAssemblyStateCreateInfo _input_assembly_info;
	VkPipelineViewportStateCreateInfo _viewport_info;
	VkPipelineRasterizationStateCreateInfo _rasterization_info;
	VkPipelineMultisampleStateCreateInfo _multi_sample_info;
	VkPipelineColorBlendStateCreateInfo _color_blend_info;
	VkPipelineDepthStencilStateCreateInfo _depth_stencil_info;
};


class pipeline
{
	device& m_device;
	VkPipeline m_graphics_pipeline;
	VkShaderModule m_vert_shader_module;
	VkShaderModule m_frag_shader_module;
	FE::owner_ptr<pipeline_config_info> m_pipeline_config_info;

public:
	pipeline(device& device_p, FE::owner_ptr<pipeline_config_info> pipeline_config_info_p) noexcept;
	~pipeline() noexcept = default;
	void create_pipeline(const char* const vert_file_path_p, const char* const frag_file_path_p, FE::safe_ptr<pipeline_config_info> config_info_p) noexcept;
	void destroy_pipeline() noexcept;
	void create_shader_module(const FE::string& code_p, VkShaderModule* const shader_module_p) noexcept;
	FE::safe_ptr<pipeline_config_info> get_pipeline_config_info() noexcept { return this->m_pipeline_config_info; };
	static FE::owner_ptr<pipeline_config_info> default_pipeline_config_info(uint32 width_p, uint32 height_p) noexcept;

private:
	static FE::string __read_file(const char* const file_path_p) noexcept;

	pipeline(const pipeline& pipeline_p) = delete;
	pipeline(pipeline&& pipeline_p) = delete;
	pipeline& operator=(const pipeline& pipeline_p) = delete;
	pipeline& operator=(pipeline&& pipeline_p) = delete;
};

 
END_NAMESPACE
#endif