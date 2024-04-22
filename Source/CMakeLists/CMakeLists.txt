CMAKE_MINIMUM_REQUIRED(VERSION 3.24.0...3.27.7)
PROJECT(FE_RENDERER)
SET(CMAKE_CXX_STANDARD 20)

ADD_COMPILE_DEFINITIONS(_CMAKE_PROJECT_DIRECTORY_="${CMAKE_CURRENT_SOURCE_DIR}"
						_HAS_EXCEPTIONS=0 
						BOOST_NO_EXCEPTIONS
						)




INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../Third-Party-Libraries/City-Hash/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../Frogman-Engine-Libraries/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../Third-Party-Libraries/glfw-3.4.bin.WIN64/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../Third-Party-Libraries/glm-1.0.1/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../Third-Party-Libraries/taskflow-3.6.0/include)


SET(_SHADERS_
	${CMAKE_CURRENT_SOURCE_DIR}/../shaders/sample_shader.vert
	${CMAKE_CURRENT_SOURCE_DIR}/../shaders/sample_shader.frag
	)




if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	MESSAGE("Configurating The Build Environment for Linux X86-64 Distributions")

	FIND_PROGRAM(GNU_C NAMES gcc)
	FIND_PROGRAM(CLANG_C NAMES clang)

	if(GNU_C)
		SET(CMAKE_C_COMPILER ${GNU_C})

	elseif(CLANG_C)
		SET(CMAKE_C_COMPILER ${CLANG_C})

	else()
		MESSAGE(FATAL_ERROR "Could Not Find Any of Executable GNU C compilers nor Clang C compilers")
	endif()


	FIND_PROGRAM(GNU_CPP NAMES g++-13 g++-12 g++-11 g++)
	FIND_PROGRAM(CLANG_CPP NAMES clang++-16 clang++-15 clang++-14 clang++-13 clang++-12 clang++)

	if(GNU_CPP)
		SET(CMAKE_CXX_COMPILER ${GNU_CPP})

	elseif(CLANG_CPP)
		SET(CMAKE_CXX_COMPILER ${CLANG_CPP})

	else()
		MESSAGE(FATAL_ERROR "Could Not Find Any of Executable GNU C++ compilers nor Clang C++ compilers")
	endif()

	
	if(GNU_CPP)
		SET(DEBUG_COMPILE_OPTIONS			 -Wall -Wextra -g	-pthread -fno-inline-functions	-fno-unroll-loops	-fno-omit-frame-pointer -fno-exceptions -march=x86-64 -O0)
		SET(RELWITHDEBINFO_COMPILE_OPTIONS	 -Wall -Wextra 		-pthread -finline-functions		-funroll-loops		-fomit-frame-pointer	-fno-exceptions -march=x86-64 -O3)
		SET(RELEASE_COMPILE_OPTIONS			 -Wall -Wextra		-pthread -finline-functions		-funroll-loops		-fomit-frame-pointer	-fno-exceptions -march=x86-64 -O3)
		SET(MINSIZEREL_COMPILE_OPTIONS		 -Wall -Wextra		-pthread -finline-functions		-funroll-loops		-fomit-frame-pointer	-fno-exceptions -march=x86-64 -O3)
		
	elseif(CLANG_CPP)
		SET(DEBUG_COMPILE_OPTIONS			-Wall -Wextra -g	-pthread -fno-inline-functions	-fno-unroll-loops	-fno-omit-frame-pointer -fno-exceptions -march=x86-64 -O0)
		SET(RELWITHDEBINFO_COMPILE_OPTIONS	-Wall -Wextra 		-pthread -finline-functions		-funroll-loops		-fomit-frame-pointer	-fno-exceptions -march=x86-64 -O3)
		SET(RELEASE_COMPILE_OPTIONS			-Wall -Wextra		-pthread -finline-functions		-funroll-loops		-fomit-frame-pointer	-fno-exceptions -march=x86-64 -O3)
		SET(MINSIZEREL_COMPILE_OPTIONS		-Wall -Wextra		-pthread -finline-functions		-funroll-loops		-fomit-frame-pointer	-fno-exceptions -march=x86-64 -O3)
	endif()


	ADD_COMPILE_DEFINITIONS(_LINUX_X86_64_)

elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	MESSAGE("Configurating The Build Environment for Windows X86-64")

	SET(DEBUG_COMPILE_OPTIONS			/Zc:__cplusplus /WX /W4 /MP /Od /Ob0 /Oi /Ot /Oy- /GT /MTd /Gy /GF)
	SET(RELWITHDEBINFO_COMPILE_OPTIONS	/Zc:__cplusplus /WX /W4 /MP /Ox /Ob2 /Oi /Ot /Oy- /GT /MT /Gy /GL /GF)
	SET(RELEASE_COMPILE_OPTIONS			/Zc:__cplusplus /WX /W4 /MP /Ox /Ob2 /Oi /Ot /Oy /GT /MT /Gy /GL /GF)
	SET(MINSIZEREL_COMPILE_OPTIONS		/Zc:__cplusplus /WX /W4 /MP /Ox /Ob2 /Oi /Os /Oy /GT /MT /Gy /GL /GF)

	
	SET(FE_CORE_AVX_WINDOWS 
		$<$<CONFIG:DEBUG>:			${CMAKE_CURRENT_SOURCE_DIR}/../../Frogman-Engine-Libraries/lib/FE_CORE_AVX/Windows/debug/lib/FE_CORE_AVX.lib>
		$<$<CONFIG:RELWITHDEBINFO>: ${CMAKE_CURRENT_SOURCE_DIR}/../../Frogman-Engine-Libraries/lib/FE_CORE_AVX/Windows/release_with_debug_info/lib/FE_CORE_AVX.lib>
		$<$<CONFIG:RELEASE>:		${CMAKE_CURRENT_SOURCE_DIR}/../../Frogman-Engine-Libraries/lib/FE_CORE_AVX/Windows/release/lib/FE_CORE_AVX.lib>
		$<$<CONFIG:MINSIZEREL>:		${CMAKE_CURRENT_SOURCE_DIR}/../../Frogman-Engine-Libraries/lib/FE_CORE_AVX/Windows/minimally_sized_release/lib/FE_CORE_AVX.lib>
	)

	SET(FE_CORE_FRAMEWORK_WINDOWS 
		$<$<CONFIG:DEBUG>:			${CMAKE_CURRENT_SOURCE_DIR}/../../Frogman-Engine-Libraries/lib/FE_CORE_FRAMEWORK/Windows/debug/lib/FE_CORE_FRAMEWORK.lib>
		$<$<CONFIG:RELWITHDEBINFO>: ${CMAKE_CURRENT_SOURCE_DIR}/../../Frogman-Engine-Libraries/lib/FE_CORE_FRAMEWORK/Windows/release_with_debug_info/lib/FE_CORE_FRAMEWORK.lib>
		$<$<CONFIG:RELEASE>:		${CMAKE_CURRENT_SOURCE_DIR}/../../Frogman-Engine-Libraries/lib/FE_CORE_FRAMEWORK/Windows/release/lib/FE_CORE_FRAMEWORK.lib>
		$<$<CONFIG:MINSIZEREL>:		${CMAKE_CURRENT_SOURCE_DIR}/../../Frogman-Engine-Libraries/lib/FE_CORE_FRAMEWORK/Windows/minimally_sized_release/lib/FE_CORE_FRAMEWORK.lib>
	)

	SET(GLFW_WINDOWS 
		$<$<CONFIG:DEBUG>:			${CMAKE_CURRENT_SOURCE_DIR}/../../Third-Party-Libraries/glfw-3.4.bin.WIN64/lib-vc2022/glfw3_mt.lib>
		$<$<CONFIG:RELWITHDEBINFO>: ${CMAKE_CURRENT_SOURCE_DIR}/../../Third-Party-Libraries/glfw-3.4.bin.WIN64/lib-vc2022/glfw3_mt.lib>
		$<$<CONFIG:RELEASE>:		${CMAKE_CURRENT_SOURCE_DIR}/../../Third-Party-Libraries/glfw-3.4.bin.WIN64/lib-vc2022/glfw3_mt.lib>
		$<$<CONFIG:MINSIZEREL>:		${CMAKE_CURRENT_SOURCE_DIR}/../../Third-Party-Libraries/glfw-3.4.bin.WIN64/lib-vc2022/glfw3_mt.lib>
	)
	
	ADD_COMPILE_DEFINITIONS(_WINDOWS_X86_64_)

	SET(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.280.0")
	SET(GLSL_COMPILER_WINDOWS $ENV{VULKAN_SDK}/Bin/glslc.exe)

	SET(_SHADER_COMPILER_SCRIPT_
	${CMAKE_CURRENT_SOURCE_DIR}/../shaders/bin/compile-glsl.bat
	)

endif()




FIND_PACKAGE(Vulkan REQUIRED)
INCLUDE_DIRECTORIES(${Vulkan_INCLUDE_DIRS})
MESSAGE("\n\n\nDetected GLSL compiler at: " ${GLSL_COMPILER_WINDOWS})


# GLSL Compiler Script
SET(GLSL_COMPILER_OUTPUT)
SET(GLSL_COMPILER_EXIT_CODE)
SET(CMAKE_EXECUTE_PROCESS_COMMAND_ECHO STDERR)
MESSAGE("\n----------------------------------------GLSL-COMPILER-OUTPUTS----------------------------------------")
EXECUTE_PROCESS(WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../shaders/bin
				RESULT_VARIABLE GLSL_COMPILER_EXIT_CODE
				COMMAND compile-glsl.bat ${GLSL_COMPILER_WINDOWS} OUTPUT_VARIABLE GLSL_COMPILER_OUTPUT
				)
if(GLSL_COMPILER_EXIT_CODE EQUAL 1)
MESSAGE("\n")
MESSAGE(FATAL_ERROR "Build failed due to GLSL compilation failure!\n----------------------------------------------------------------------------------------------------")
endif()
MESSAGE("\n----------------------------------------GLSL-COMPILER-INPUTS----------------------------------------" ${GLSL_COMPILER_OUTPUT} "----------------------------------------------------------------------------------------------------\n\n\n")
# GLSL Compiler Script



#ADD_SUBDIRECTORY(FE_RENDERER)
ADD_SUBDIRECTORY(FE_RENDERER_TEST)
#ADD_DEPENDENCIES("FE_RENDERER_TEST" "FE_RENDERER")
SET_PROPERTY(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "FE_RENDERER_TEST")
