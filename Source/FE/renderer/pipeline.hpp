#ifndef _FE_RENDERER_PIPELINE_HPP_
#define _FE_RENDERER_PIPELINE_HPP_
// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#include <FE/core/prerequisites.h>
#include <FE/renderer/settings.h>

// FE
#define _MEMORY_POOL_FE_STRING_
#include <FE/core/string.hxx>




BEGIN_NAMESPACE(FE::renderer)


class pipeline
{
public:
	pipeline() noexcept = default;
	void create_pipeline(const char* const vert_file_path_p, const char* const frag_file_path_p) noexcept;

private:
	static FE::string __read_file(const char* const file_path_p) noexcept;
};

 
END_NAMESPACE
#endif