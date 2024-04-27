#include <FE/renderer/pipeline.hpp>
#include <FE/renderer/device.hpp>

// FE
#include <FE/core/fstream_guard.hxx>

// std
#include <fstream>




BEGIN_NAMESPACE(FE::renderer)


pipeline::pipeline(device& device_p, FE::unique_ptr<pipeline_config_info> pipeline_config_info_p) noexcept : m_device(device_p), m_graphics_pipeline(), m_vert_shader_module(), m_frag_shader_module(), m_pipeline_config_info(std::move(pipeline_config_info_p))
{
}

void pipeline::create_pipeline(const char* const vert_file_path_p, const char* const frag_file_path_p) noexcept
{
	FE_ASSERT(this->m_pipeline_config_info->_pipeline_layout != VK_NULL_HANDLE, "Unable to create a graphics pipeline: _pipeline_layout is nullptr.");
	FE_ASSERT(this->m_pipeline_config_info->_render_pass != VK_NULL_HANDLE, "Unable to create a graphics pipeline: _render_pass is nullptr.");

	FE::string l_vert_shader = __read_file(vert_file_path_p);
	FE::string l_frag_shader = __read_file(frag_file_path_p);

	create_shader_module(l_vert_shader, &this->m_vert_shader_module);
	create_shader_module(l_frag_shader, &this->m_frag_shader_module);

	VkPipelineShaderStageCreateInfo l_vert_shader_stage_info{};
	l_vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	l_vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	l_vert_shader_stage_info.module = this->m_vert_shader_module;
	l_vert_shader_stage_info.pName = "main";
	l_vert_shader_stage_info.flags = 0;
	l_vert_shader_stage_info.pNext = nullptr;
	l_vert_shader_stage_info.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo l_frag_shader_stage_info{};
	l_frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	l_frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	l_frag_shader_stage_info.module = this->m_frag_shader_module;
	l_frag_shader_stage_info.pName = "main";
	l_frag_shader_stage_info.flags = 0;
	l_frag_shader_stage_info.pNext = nullptr;
	l_frag_shader_stage_info.pSpecializationInfo = nullptr;


	VkPipelineVertexInputStateCreateInfo l_vertex_input_info{};
	l_vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	l_vertex_input_info.vertexAttributeDescriptionCount = 0;
	l_vertex_input_info.vertexBindingDescriptionCount = 0;
	l_vertex_input_info.pVertexAttributeDescriptions = nullptr;
	l_vertex_input_info.pVertexBindingDescriptions = nullptr;
	
	VkGraphicsPipelineCreateInfo l_pipeline_info{};
	l_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	l_pipeline_info.stageCount = 2;
	l_pipeline_info.pStages = &l_vert_shader_stage_info;
	l_pipeline_info.pVertexInputState = &l_vertex_input_info;
	l_pipeline_info.pInputAssemblyState = &this->m_pipeline_config_info->_input_assembly_info;
	l_pipeline_info.pViewportState = &this->m_pipeline_config_info->_viewport_info;
	l_pipeline_info.pRasterizationState = &this->m_pipeline_config_info->_rasterization_info;
	l_pipeline_info.pMultisampleState = &this->m_pipeline_config_info->_multi_sample_info;
	l_pipeline_info.pColorBlendState = &this->m_pipeline_config_info->_color_blend_info;
	l_pipeline_info.pDepthStencilState = &this->m_pipeline_config_info->_depth_stencil_info;
	l_pipeline_info.pDynamicState = nullptr;

	l_pipeline_info.layout = this->m_pipeline_config_info->_pipeline_layout;
	l_pipeline_info.renderPass = this->m_pipeline_config_info->_render_pass;
	l_pipeline_info.subpass = this->m_pipeline_config_info->_subpass;

	l_pipeline_info.basePipelineIndex = -1;
	l_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;


	FE_EXIT(vkCreateGraphicsPipelines(this->m_device.get_device(), VK_NULL_HANDLE, 1, &l_pipeline_info, nullptr, &this->m_graphics_pipeline) != VK_SUCCESS, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "Failed to create graphics pipeline.");
}

void pipeline::destroy_pipeline() noexcept
{
	vkDestroyShaderModule(this->m_device.get_device(), this->m_vert_shader_module, nullptr);
	vkDestroyShaderModule(this->m_device.get_device(), this->m_frag_shader_module, nullptr);
	vkDestroyPipeline(this->m_device.get_device(), this->m_graphics_pipeline, nullptr);
}

void pipeline::create_shader_module(const FE::string& code_p, VkShaderModule* const shader_module_p) noexcept
{
	VkShaderModuleCreateInfo l_create_info{};
	l_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	l_create_info.codeSize = code_p.length();
	l_create_info.pCode = reinterpret_cast<const uint32*>(code_p.data());

	FE_EXIT(vkCreateShaderModule(this->m_device.get_device(), &l_create_info, nullptr, shader_module_p) != VK_SUCCESS, RENDERER_ERROR_TYPE::ERROR_FROM_VULKAN, "Failed to create shader module.");
}

FE::unique_ptr<pipeline_config_info> pipeline::default_pipeline_config_info(_MAYBE_UNUSED_ uint32 width_p, _MAYBE_UNUSED_ uint32 height_p) noexcept
{
	FE::unique_ptr<pipeline_config_info> l_info = FE::make_unique<pipeline_config_info>();
	l_info->_input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	l_info->_input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	l_info->_input_assembly_info.primitiveRestartEnable = VK_FALSE;

	l_info->_viewport.x = 0.0f;
	l_info->_viewport.y = 0.0f;
	l_info->_viewport.width = static_cast<float>(width_p);
	l_info->_viewport.height = static_cast<float>(height_p);
	l_info->_viewport.minDepth = 0.0f;
	l_info->_viewport.maxDepth = 1.0f;

	l_info->_scissor.offset = { 0, 0 };
	l_info->_scissor.extent = { width_p, height_p };

	l_info->_viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	l_info->_viewport_info.viewportCount = 1;
	l_info->_viewport_info.pViewports = &l_info->_viewport;
	l_info->_viewport_info.scissorCount = 1;
	l_info->_viewport_info.pScissors = &l_info->_scissor;

	l_info->_rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	l_info->_rasterization_info.depthClampEnable = VK_FALSE;
	l_info->_rasterization_info.rasterizerDiscardEnable = VK_FALSE;
	l_info->_rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
	l_info->_rasterization_info.lineWidth = 1.0f;
	l_info->_rasterization_info.cullMode = VK_CULL_MODE_NONE;
	l_info->_rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
	l_info->_rasterization_info.depthBiasEnable = VK_FALSE;
	l_info->_rasterization_info.depthBiasConstantFactor = 0.0f;  // Optional
	l_info->_rasterization_info.depthBiasClamp = 0.0f;           // Optional
	l_info->_rasterization_info.depthBiasSlopeFactor = 0.0f;     // Optional

	l_info->_multi_sample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	l_info->_multi_sample_info.sampleShadingEnable = VK_FALSE;
	l_info->_multi_sample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	l_info->_multi_sample_info.minSampleShading = 1.0f;           // Optional
	l_info->_multi_sample_info.pSampleMask = nullptr;             // Optional
	l_info->_multi_sample_info.alphaToCoverageEnable = VK_FALSE;  // Optional
	l_info->_multi_sample_info.alphaToOneEnable = VK_FALSE;       // Optional

	l_info->_color_blend_attachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	l_info->_color_blend_attachment.blendEnable = VK_FALSE;
	l_info->_color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
	l_info->_color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
	l_info->_color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
	l_info->_color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
	l_info->_color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
	l_info->_color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

	l_info->_color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	l_info->_color_blend_info.logicOpEnable = VK_FALSE;
	l_info->_color_blend_info.logicOp = VK_LOGIC_OP_COPY;  // Optional
	l_info->_color_blend_info.attachmentCount = 1;
	l_info->_color_blend_info.pAttachments = &l_info->_color_blend_attachment;
	l_info->_color_blend_info.blendConstants[0] = 0.0f;  // Optional
	l_info->_color_blend_info.blendConstants[1] = 0.0f;  // Optional
	l_info->_color_blend_info.blendConstants[2] = 0.0f;  // Optional
	l_info->_color_blend_info.blendConstants[3] = 0.0f;  // Optional

	l_info->_depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	l_info->_depth_stencil_info.depthTestEnable = VK_TRUE;
	l_info->_depth_stencil_info.depthWriteEnable = VK_TRUE;
	l_info->_depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
	l_info->_depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
	l_info->_depth_stencil_info.minDepthBounds = 0.0f;  // Optional
	l_info->_depth_stencil_info.maxDepthBounds = 1.0f;  // Optional
	l_info->_depth_stencil_info.stencilTestEnable = VK_FALSE;
	l_info->_depth_stencil_info.front = {};  // Optional
	l_info->_depth_stencil_info.back = {};   // Optional

	return std::move(l_info);
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