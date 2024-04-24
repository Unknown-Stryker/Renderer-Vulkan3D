// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/renderer/pipeline.hpp>
#include <FE/renderer/device.hpp>

// FE
#include <FE/core/fstream_guard.hxx>

// std
#include <fstream>




BEGIN_NAMESPACE(FE::renderer)


pipeline::pipeline(device& device_p, const pipeline_config_info& pipeline_config_info_p) noexcept : m_device(device_p), m_graphics_pipeline(), m_vert_shader_module(), m_frag_shader_module(), m_pipeline_config_info(pipeline_config_info_p)
{
}

void pipeline::create_pipeline(const char* const vert_file_path_p, const char* const frag_file_path_p) noexcept
{
	FE::string l_vert_shader = __read_file(vert_file_path_p);
	FE::string l_frag_shader = __read_file(frag_file_path_p);

	std::cout << l_vert_shader.length() << std::endl;
	std::cout << l_frag_shader.length() << std::endl;
}

void pipeline::create_shader_module(const FE::string& code_p, VkShaderModule* const shader_module_p) noexcept
{
	VkShaderModuleCreateInfo l_create_info{};
	l_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	l_create_info.codeSize = code_p.length();
	l_create_info.pCode = reinterpret_cast<const uint32*>(code_p.data());

	FE_EXIT(vkCreateShaderModule(this->m_device.get_device(), &l_create_info, nullptr, shader_module_p) != VK_SUCCESS, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "Failed to create shader module.");
}

pipeline_config_info pipeline::default_pipeline_config_info(_MAYBE_UNUSED_ uint32 width_p, _MAYBE_UNUSED_ uint32 height_p) noexcept
{
	return pipeline_config_info();
}


FE::string pipeline::__read_file(const char* const file_path_p) noexcept
{
	std::ifstream l_file(file_path_p, std::ios::ate | std::ios::binary);
	FE::ifstream_guard l_guard(l_file);

	FE_ASSERT((l_file.is_open() == false), "Failed to open a shader file.");

	size_t l_file_size = static_cast<size_t>(l_file.tellg());
	FE::string l_buffer(l_file_size);

	l_file.seekg(0);
	l_file.read(l_buffer.data(), l_file_size);

	return std::move(l_buffer);
}

END_NAMESPACE