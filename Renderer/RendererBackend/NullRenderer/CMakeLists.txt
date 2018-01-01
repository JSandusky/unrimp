#/*********************************************************\
# * Copyright (c) 2012-2018 The Unrimp Team
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


cmake_minimum_required(VERSION 3.3.0)


##################################################
## Preprocessor definitions
##################################################
if(RENDERER_DEBUG)
	add_definitions(-DRENDERER_DEBUG)
else()
	add_definitions(-D_DEBUG)
endif()


##################################################
## Includes
##################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Renderer/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


##################################################
## Source codes
##################################################
# Cross-platform source codes
set(SOURCE_CODES
	src/Buffer/BufferManager.cpp
	src/Buffer/IndexBuffer.cpp
	src/Buffer/IndirectBuffer.cpp
	src/Buffer/TextureBuffer.cpp
	src/Buffer/UniformBuffer.cpp
	src/Buffer/VertexArray.cpp
	src/Buffer/VertexBuffer.cpp
	src/RenderTarget/RenderPass.cpp
	src/RenderTarget/Framebuffer.cpp
	src/RenderTarget/SwapChain.cpp
	src/Shader/FragmentShader.cpp
	src/Shader/GeometryShader.cpp
	src/Shader/Program.cpp
	src/Shader/ShaderLanguage.cpp
	src/Shader/TessellationControlShader.cpp
	src/Shader/TessellationEvaluationShader.cpp
	src/Shader/VertexShader.cpp
	src/State/PipelineState.cpp
	src/State/SamplerState.cpp
	src/Texture/Texture1D.cpp
	src/Texture/Texture2D.cpp
	src/Texture/Texture2DArray.cpp
	src/Texture/Texture3D.cpp
	src/Texture/TextureCube.cpp
	src/Texture/TextureManager.cpp
	src/NullRenderer.cpp
	src/RootSignature.cpp
	src/ResourceGroup.cpp
)


##################################################
## Libraries
##################################################
if(STATIC_LIBRARY)
	add_library(NullRendererStatic STATIC ${SOURCE_CODES})
endif()
if(SHARED_LIBRARY)
	add_library(NullRenderer SHARED ${SOURCE_CODES})
	set_target_properties(NullRenderer PROPERTIES COMPILE_FLAGS "-DSHARED_LIBRARIES -DNULLRENDERER_EXPORTS")
endif()


##################################################
## Post build
##################################################
if(STATIC_LIBRARY)
	# Copy static library
	add_custom_command(TARGET NullRendererStatic
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}NullRendererStatic${CMAKE_STATIC_LIBRARY_SUFFIX}" ${OUTPUT_LIB_DIR}
	)
endif()
if(SHARED_LIBRARY)
	# Copy shared library
	add_custom_command(TARGET NullRenderer
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}NullRenderer${CMAKE_SHARED_LIBRARY_SUFFIX}" ${OUTPUT_BIN_DIR}
	)
	if(WIN32)
		add_custom_command(TARGET NullRenderer
			COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/NullRenderer.lib" ${OUTPUT_LIB_DIR}
		)
	endif()
endif()
