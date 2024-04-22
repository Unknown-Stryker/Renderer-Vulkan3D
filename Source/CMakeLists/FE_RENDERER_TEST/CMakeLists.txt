CMAKE_MINIMUM_REQUIRED(VERSION 3.24.0...3.27.7)
SET(FE_RENDERER_TEST "FE_RENDERER_TEST")




SOURCE_GROUP("shaders" FILES ${_SHADERS_})
SOURCE_GROUP("shader compiler script" FILES ${_SHADER_COMPILER_SCRIPT_})

#SET(_TEST_CODE_
#	${CMAKE_CURRENT_SOURCE_DIR}
#	)
#SOURCE_GROUP("Tests" FILES ${_TEST_CODE_})


ADD_EXECUTABLE(	${FE_RENDERER_TEST} 
#				${_TEST_CODE_}
				main.cpp

				${_SHADERS_}
				${_SHADER_COMPILER_SCRIPT_}

				${CMAKE_CURRENT_SOURCE_DIR}/../../FE/renderer/window.hpp
				${CMAKE_CURRENT_SOURCE_DIR}/../../FE/renderer/private/window.cpp
				${CMAKE_CURRENT_SOURCE_DIR}/../../FE/renderer/pipeline.hpp
				${CMAKE_CURRENT_SOURCE_DIR}/../../FE/renderer/private/pipeline.cpp
				${CMAKE_CURRENT_SOURCE_DIR}/../../FE/renderer/device.hpp
				${CMAKE_CURRENT_SOURCE_DIR}/../../FE/renderer/private/device.cpp
				)




if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	MESSAGE("Configurating The Build Environment for Linux X86-64 Distributions")


	if(GNU_CPP)
		TARGET_COMPILE_OPTIONS(${FE_RENDERER_TEST} PRIVATE
			$<$<CONFIG:DEBUG>:			${DEBUG_COMPILE_OPTIONS} 			-mavx>
			$<$<CONFIG:RELWITHDEBINFO>: ${RELWITHDEBINFO_COMPILE_OPTIONS}	-mavx>
			$<$<CONFIG:RELEASE>:		${RELEASE_COMPILE_OPTIONS} 			-mavx>
			$<$<CONFIG:MINSIZEREL>:		${MINSIZEREL_COMPILE_OPTIONS} 		-mavx>
		)
	elseif(CLANG_CPP)
		TARGET_COMPILE_OPTIONS(${FE_RENDERER_TEST} PRIVATE
			$<$<CONFIG:DEBUG>:			${DEBUG_COMPILE_OPTIONS} 			-mavx>
			$<$<CONFIG:RELWITHDEBINFO>: ${RELWITHDEBINFO_COMPILE_OPTIONS} 	-mavx>
			$<$<CONFIG:RELEASE>:		${RELEASE_COMPILE_OPTIONS} 			-mavx>
			$<$<CONFIG:MINSIZEREL>:		${MINSIZEREL_COMPILE_OPTIONS} 		-mavx>
		)
	endif()


	SET_TARGET_PROPERTIES(${FE_RENDERER_TEST} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG 			${CMAKE_CURRENT_SOURCE_DIR}/Linux/debug/a
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/Linux/release_with_debug_info/a
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE 		${CMAKE_CURRENT_SOURCE_DIR}/Linux/release/a
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL 	${CMAKE_CURRENT_SOURCE_DIR}/Linux/minimally_sized_release/a
    )

#	SET(UNIT_TEST_LINUX			
#		$<$<CONFIG:DEBUG>:			${CMAKE_CURRENT_SOURCE_DIR}/../../../Frogman-Engine-Tests/Google-Test/linux/libgtest.a>
#		$<$<CONFIG:RELWITHDEBINFO>: ${CMAKE_CURRENT_SOURCE_DIR}/../../../Frogman-Engine-Tests/Google-Test/linux/libgtest.a>
#		$<$<CONFIG:RELEASE>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Frogman-Engine-Tests/Google-Test/linux/libgtest.a>
#		$<$<CONFIG:MINSIZEREL>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Frogman-Engine-Tests/Google-Test/linux/libgtest.a>
#	)

#	SET(BENCHMARK_TEST_LINUX			
#		$<$<CONFIG:DEBUG>:			${CMAKE_CURRENT_SOURCE_DIR}/../../../Frogman-Engine-Tests/Benchmark/linux/libbenchmark.a>
#		$<$<CONFIG:RELWITHDEBINFO>: ${CMAKE_CURRENT_SOURCE_DIR}/../../../Frogman-Engine-Tests/Benchmark/linux/libbenchmark.a>
#		$<$<CONFIG:RELEASE>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Frogman-Engine-Tests/Benchmark/linux/libbenchmark.a>
#		$<$<CONFIG:MINSIZEREL>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Frogman-Engine-Tests/Benchmark/linux/libbenchmark.a>
#	)

	TARGET_LINK_LIBRARIES(${FE_RENDERER_TEST} 
		stdc++fs 
		
#		${UNIT_TEST_LINUX}
#		${BENCHMARK_TEST_LINUX}
		
		${FE_CORE_FRAMEWORK_LINUX}
	)




elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	MESSAGE("Configurating The Build Environment for Windows X86-64 Distributions")

	TARGET_COMPILE_OPTIONS(${FE_RENDERER_TEST} PRIVATE
        $<$<CONFIG:DEBUG>:			${DEBUG_COMPILE_OPTIONS} 			/arch:AVX>
        $<$<CONFIG:RELWITHDEBINFO>:	${RELWITHDEBINFO_COMPILE_OPTIONS} 	/arch:AVX>
        $<$<CONFIG:RELEASE>:		${RELEASE_COMPILE_OPTIONS} 			/arch:AVX>
        $<$<CONFIG:MINSIZEREL>:		${MINSIZEREL_COMPILE_OPTIONS} 		/arch:AVX>
    )


	SET_TARGET_PROPERTIES(${FE_RENDERER_TEST} PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY_DEBUG 			${CMAKE_CURRENT_SOURCE_DIR}/Windows/debug
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/Windows/release_with_debug_info
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE 		${CMAKE_CURRENT_SOURCE_DIR}/Windows/release
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL 	${CMAKE_CURRENT_SOURCE_DIR}/Windows/minimally_sized_release
    )


#	SET(UNIT_TEST_WINDOWS			
#		$<$<CONFIG:DEBUG>:			${CMAKE_CURRENT_SOURCE_DIR}/../../../Frogman-Engine-Tests/Google-Test/windows/Debug/gtest.lib>
#		$<$<CONFIG:RELWITHDEBINFO>: ${CMAKE_CURRENT_SOURCE_DIR}/../../../Frogman-Engine-Tests/Google-Test/windows/RelWithDebInfo/gtest.lib>
#		$<$<CONFIG:RELEASE>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Frogman-Engine-Tests/Google-Test/windows/Release/gtest.lib>
#		$<$<CONFIG:MINSIZEREL>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Frogman-Engine-Tests/Google-Test/windows/MinSizeRel/gtest.lib>
#	)

#	SET(BENCHMARK_TEST_WINDOWS			
#		$<$<CONFIG:DEBUG>:			${CMAKE_CURRENT_SOURCE_DIR}/../../../Frogman-Engine-Tests/Benchmark/windows/Debug/benchmark.lib>
#		$<$<CONFIG:RELWITHDEBINFO>: ${CMAKE_CURRENT_SOURCE_DIR}/../../../Frogman-Engine-Tests/Benchmark/windows/RelWithDebInfo/benchmark.lib>
#		$<$<CONFIG:RELEASE>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Frogman-Engine-Tests/Benchmark/windows/Release/benchmark.lib>
#		$<$<CONFIG:MINSIZEREL>:		${CMAKE_CURRENT_SOURCE_DIR}/../../../Frogman-Engine-Tests/Benchmark/windows/MinSizeRel/benchmark.lib>
#	)


	TARGET_LINK_LIBRARIES(${FE_RENDERER_TEST} 
		${Vulkan_LIBRARIES}
		${GLFW_WINDOWS}

		shlwapi.lib
	#	${UNIT_TEST_WINDOWS}
	#	${BENCHMARK_TEST_WINDOWS}

		${FE_CORE_AVX_WINDOWS}
		${FE_CORE_FRAMEWORK_WINDOWS}
	)


endif()




#// Copyright © from 2023 to current, UNKNOWN STRYKER. All Rights Reserved.
#// To enable or disable a certain feature, remove the macro.
#// 
#// Modifying the macro will not immediately affect your project.
#// The binary files e.g. (.lib, .a, .dll, or .so) need to be rebuilt to apply changes that have been made.

#// Frogman Engine Memory Tracker generates a memory utilization report over frames at the end of an app process.(Planned)
#// The output file can be visualized as a graph image via Frogman Engine Memory Utilization Analyzer.
#// Memory tracking routines involve atomic integral sum and add operations, and its tracking frequency can be dynamically adjusted without recompiling the Engine binary.
#// Excluding this feature can improve an app performance.

#// FE_ASSERT can be disabled by removing the corresponding macro to eliminate error-checking overhead.
#// Please NOTE that FE_ASSERT aborts when circumstances are unrecoverable and critical.

ADD_COMPILE_DEFINITIONS(BENCHMARK_STATIC_DEFINE)


TARGET_COMPILE_DEFINITIONS(${FE_RENDERER_TEST} PRIVATE
	$<$<CONFIG:DEBUG>:			_DEBUG_					_ENABLE_ASSERT_ _ENABLE_ABORT_IF_ _ENABLE_LOG_ _ENABLE_EXIT_ _ENABLE_FE_CHECK_ONCE_> # Additonal Options: _ENABLE_MEMORY_TRACKER_ _MEMORY_POOL_FE_STRING_
	$<$<CONFIG:RELWITHDEBINFO>:	_RELWITHDEBINFO_		_ENABLE_ASSERT_ _ENABLE_ABORT_IF_ _ENABLE_LOG_ _ENABLE_EXIT_ _ENABLE_FE_CHECK_ONCE_>
	$<$<CONFIG:RELEASE>:		_RELEASE_				_ENABLE_EXIT_>
	$<$<CONFIG:MINSIZEREL>:		_RELEASE_ _MINSIZEREL_	_ENABLE_EXIT_>
)