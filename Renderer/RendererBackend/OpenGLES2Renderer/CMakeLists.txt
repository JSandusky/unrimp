#/*********************************************************\
# * Copyright (c) 2012-2016 Christian Ofenberg
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
if(RENDERER_OPENGLES2_NO_STATE_CLEANUP)
	add_definitions(-DOPENGLES2RENDERER_NO_STATE_CLEANUP)
endif()
if(RENDERER_OPENGLES2_NO_RENDERERMATCHCHECK)
	add_definitions(-DOPENGLES2RENDERER_NO_RENDERERMATCHCHECK)
endif()


##################################################
## Includes
##################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Renderer/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../External/OpenGLES2/include)


##################################################
## Source codes
##################################################
# Cross-platform source codes
set(SOURCE_CODES
	src/ContextRuntimeLinking.cpp
	src/ExtensionsRuntimeLinking.cpp
	src/Framebuffer.cpp
	src/IContext.cpp
	src/IExtensions.cpp
	src/IndexBuffer.cpp
	src/Mapping.cpp
	src/OpenGLES2Renderer.cpp
	src/PipelineState.cpp
	src/SamplerState.cpp
	src/SwapChain.cpp
	src/Texture2D.cpp
	src/Texture2DArray.cpp
	src/VertexArray.cpp
	src/VertexArrayNoVao.cpp
	src/VertexArrayVao.cpp
	src/VertexBuffer.cpp
	src/State/BlendState.cpp
	src/State/DepthStencilState.cpp
	src/State/RasterizerState.cpp
	src/Shader/FragmentShader.cpp
	src/Shader/FragmentShaderGlsl.cpp
	src/Shader/Program.cpp
	src/Shader/ProgramGlsl.cpp
	src/Shader/ShaderLanguage.cpp
	src/Shader/ShaderLanguageGlsl.cpp
	src/Shader/VertexShader.cpp
	src/Shader/VertexShaderGlsl.cpp
)


##################################################
## Libraries
##################################################
if(STATIC_LIBRARY)
	add_library(OpenGLES2RendererStatic STATIC ${SOURCE_CODES})
endif()
if(SHARED_LIBRARY)
	add_library(OpenGLES2Renderer SHARED ${SOURCE_CODES})
	set_target_properties(OpenGLES2Renderer PROPERTIES COMPILE_FLAGS "-DSHARED_LIBRARIES -DOPENGLES2RENDERER_EXPORTS")
endif()


##################################################
## Post build
##################################################
if(STATIC_LIBRARY)
	# Copy static library
	add_custom_command(TARGET OpenGLES2RendererStatic
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}OpenGLES2RendererStatic${CMAKE_STATIC_LIBRARY_SUFFIX}" ${OUTPUT_LIB_DIR}
	)
endif()
if(SHARED_LIBRARY)
	# Copy shared library
	add_custom_command(TARGET OpenGLES2Renderer
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}OpenGLES2Renderer${CMAKE_SHARED_LIBRARY_SUFFIX}" ${OUTPUT_BIN_DIR}
	)
	if(WIN32)
		add_custom_command(TARGET OpenGLES2Renderer
			COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/OpenGLES2Renderer.lib" ${OUTPUT_LIB_DIR}
		)
	endif()
endif()
