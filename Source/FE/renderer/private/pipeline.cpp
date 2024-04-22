// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/renderer/pipeline.hpp>

// FE
#include <FE/core/fstream_guard.hxx>

// std
#include <fstream>




BEGIN_NAMESPACE(FE::renderer)


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

void pipeline::create_pipeline(const char* const vert_file_path_p, const char* const frag_file_path_p) noexcept
{
	FE::string l_vert_shader = __read_file(vert_file_path_p);
	FE::string l_frag_shader = __read_file(frag_file_path_p);

	std::cout << l_vert_shader.length() << std::endl;
	std::cout << l_frag_shader.length() << std::endl;
}


END_NAMESPACE