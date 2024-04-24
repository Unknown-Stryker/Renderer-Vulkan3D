#ifndef _FE_RENDERER_SETTINGS_H_
#define _FE_RENDERER_SETTINGS_H_
#pragma warning(disable: 4819)
#include <FE/core/prerequisites.h>


/*
To find a string within the current file, press Ctrl + F.
To find and replace a string in the current file, press Ctrl + H.
*/


BEGIN_NAMESPACE(FE::renderer)


enum struct RENDERER_ERROR_TYPE
{
	ERROR_FROM_VULKAN = static_cast<::FE::var::uint16>('v'),
	ERROR_FROM_GLFW = static_cast<::FE::var::uint16>('g'),
};


END_NAMESPACE
#endif