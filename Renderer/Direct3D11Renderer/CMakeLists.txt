#/*********************************************************\
# * Copyright (c) 2012-2013 Christian Ofenberg
# *
# * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
# * and associated documentation files (the "Software"), to deal in the Software without
# * restriction, including without limitation the rights to use, copy, modify, merge, publish,
# * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
# * Software is furnished to do so, subject to the following conditions:
# *
# * The above copyright notice and this permission notice shall be included in all copies or
# * substantial portions of the Software.
# *
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
# * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#\*********************************************************/


cmake_minimum_required(VERSION 2.8.8)


##################################################
## Preprocessor definitions
##################################################
if(NOT RENDERER_DEBUG)
	add_definitions(-DRENDERER_NO_DEBUG)
endif()
if(NOT RENDERER_DIRECT3D11_DEBUG)
	add_definitions(-DRENDERER_DIRECT3D11_NO_DEBUG)
endif()
if(NOT CG_SUPPORT)
	add_definitions(-DDIRECT3D11RENDERER_NO_CG)
endif()


##################################################
## Includes
##################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Renderer/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
if(CG_SUPPORT)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/Direct3D11Renderer)	# Required, enables the Cg Direct3D 11 header to find our "D3D11.h"
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/Cg/include)
endif()


##################################################
## Source codes
##################################################
# Cross-platform source codes
set(SOURCE_CODES
	src/BlendState.cpp
	src/DepthStencilState.cpp
	src/Direct3D11RuntimeLinking.cpp
	src/FragmentShader.cpp
	src/FragmentShaderHlsl.cpp
	src/Framebuffer.cpp
	src/GeometryShader.cpp
	src/GeometryShaderHlsl.cpp
	src/Guid.cpp
	src/IndexBuffer.cpp
	src/Mapping.cpp
	src/Program.cpp
	src/ProgramHlsl.cpp
	src/RasterizerState.cpp
	src/Direct3D11Renderer.cpp
	src/SamplerState.cpp
	src/SamplerStateCollection.cpp
	src/ShaderLanguage.cpp
	src/ShaderLanguageHlsl.cpp
	src/SwapChain.cpp
	src/TessellationControlShader.cpp
	src/TessellationControlShaderHlsl.cpp
	src/TessellationEvaluationShader.cpp
	src/TessellationEvaluationShaderHlsl.cpp
	src/Texture2D.cpp
	src/Texture2DArray.cpp
	src/TextureBuffer.cpp
	src/TextureCollection.cpp
	src/UniformBuffer.cpp
	src/VertexArray.cpp
	src/VertexBuffer.cpp
	src/VertexShader.cpp
	src/VertexShaderHlsl.cpp
)
if(NOT DIRECT3D11RENDERER_NO_DEBUG)
	# Support Direct3D 9 PIX functions (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box) debug features
	set(SOURCE_CODES
		${SOURCE_CODES}
		src/Direct3D9RuntimeLinking.cpp
	)
endif()
if(CG_SUPPORT)
	# Cg support source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		src/CgRuntimeLinking.cpp
		src/ShaderLanguageCg.cpp
		src/VertexShaderCg.cpp
		src/TessellationControlShaderCg.cpp
		src/TessellationEvaluationShaderCg.cpp
		src/GeometryShaderCg.cpp
		src/FragmentShaderCg.cpp
		src/ProgramCg.cpp
	)
endif()


##################################################
## Libraries
##################################################
if(STATIC_LIBRARY)
	add_library(Direct3D11RendererStatic STATIC ${SOURCE_CODES})
endif()
if(SHARED_LIBRARY)
	add_library(Direct3D11Renderer SHARED ${SOURCE_CODES})
	set_target_properties(Direct3D11Renderer PROPERTIES COMPILE_FLAGS "-D_USRDLL -DDIRECT3D11RENDERER_EXPORTS")
endif()


##################################################
## Post build
##################################################
if(STATIC_LIBRARY)
	# Copy static library
	add_custom_command(TARGET Direct3D11RendererStatic
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}Direct3D11RendererStatic${CMAKE_STATIC_LIBRARY_SUFFIX}" ${OUTPUT_LIB_DIR}
	)
	if(CG_SUPPORT)
		# Copy Cg shared libraries
		add_custom_command(TARGET Direct3D11RendererStatic
			COMMAND ${CMAKE_COMMAND} -E copy "${OUTPUT_EXTERNAL_OS_ARCHITECTURE_DIR}/Cg/bin/${CMAKE_SHARED_LIBRARY_PREFIX}cg${CMAKE_SHARED_LIBRARY_SUFFIX}" ${OUTPUT_BIN_DIR}
		)
		add_custom_command(TARGET Direct3D11RendererStatic
			COMMAND ${CMAKE_COMMAND} -E copy "${OUTPUT_EXTERNAL_OS_ARCHITECTURE_DIR}/Cg/bin/${CMAKE_SHARED_LIBRARY_PREFIX}cgD3D10${CMAKE_SHARED_LIBRARY_SUFFIX}" ${OUTPUT_BIN_DIR}
		)
	endif()
endif()
if(SHARED_LIBRARY)
	# Copy shared library
	add_custom_command(TARGET Direct3D11Renderer
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}Direct3D11Renderer${CMAKE_SHARED_LIBRARY_SUFFIX}" ${OUTPUT_BIN_DIR}
	)
	if(WIN32)
		add_custom_command(TARGET Direct3D11Renderer
			COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Direct3D11Renderer.lib" ${OUTPUT_LIB_DIR}
		)
	endif()
	if(CG_SUPPORT)
		# Copy Cg shared libraries
		add_custom_command(TARGET Direct3D11Renderer
			COMMAND ${CMAKE_COMMAND} -E copy "${OUTPUT_EXTERNAL_OS_ARCHITECTURE_DIR}/Cg/bin/${CMAKE_SHARED_LIBRARY_PREFIX}cg${CMAKE_SHARED_LIBRARY_SUFFIX}" ${OUTPUT_BIN_DIR}
		)
		add_custom_command(TARGET Direct3D11Renderer
			COMMAND ${CMAKE_COMMAND} -E copy "${OUTPUT_EXTERNAL_OS_ARCHITECTURE_DIR}/Cg/bin/${CMAKE_SHARED_LIBRARY_PREFIX}cgD3D11${CMAKE_SHARED_LIBRARY_SUFFIX}" ${OUTPUT_BIN_DIR}
		)
	endif()
endif()
