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
if(RENDERER_OPENGL_NO_STATE_CLEANUP)
	add_definitions(-DOPENGLRENDERER_NO_STATE_CLEANUP)
endif()
if(RENDERER_OPENGL_NO_RENDERERMATCHCHECK)
	add_definitions(-DOPENGLRENDERER_NO_RENDERERMATCHCHECK)
endif()


##################################################
## Includes
##################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Renderer/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../External/OpenGL/include)


##################################################
## Source codes
##################################################
# Cross-platform source codes
set(SOURCE_CODES
	src/Extensions.cpp
	src/Framebuffer.cpp
	src/FramebufferDsa.cpp
	src/FramebufferBind.cpp
	src/IContext.cpp
	src/IndexBuffer.cpp
	src/IndexBufferBind.cpp
	src/IndexBufferDsa.cpp
	src/Mapping.cpp
	src/OpenGLRuntimeLinking.cpp
	src/PipelineState.cpp
	src/OpenGLRenderer.cpp
	src/SamplerState.cpp
	src/SamplerStateBind.cpp
	src/SamplerStateDsa.cpp
	src/SamplerStateSo.cpp
	src/SwapChain.cpp
	src/Texture2D.cpp
	src/Texture2DArray.cpp
	src/Texture2DArrayBind.cpp
	src/Texture2DArrayDsa.cpp
	src/Texture2DBind.cpp
	src/Texture2DDsa.cpp
	src/TextureBuffer.cpp
	src/TextureBufferBind.cpp
	src/TextureBufferDsa.cpp
	src/VertexArray.cpp
	src/VertexArrayNoVao.cpp
	src/VertexArrayVao.cpp
	src/VertexArrayVaoBind.cpp
	src/VertexArrayVaoDsa.cpp
	src/VertexBuffer.cpp
	src/VertexBufferBind.cpp
	src/VertexBufferDsa.cpp
	src/UniformBuffer.cpp
	src/UniformBuffer.cpp
	src/UniformBufferBind.cpp
	src/UniformBufferDsa.cpp
	src/State/BlendState.cpp
	src/State/DepthStencilState.cpp
	src/State/RasterizerState.cpp
	src/Shader/Monolithic/FragmentShaderMonolithic.cpp
	src/Shader/Monolithic/GeometryShaderMonolithic.cpp
	src/Shader/Monolithic/ProgramMonolithic.cpp
	src/Shader/Monolithic/ProgramMonolithicDsa.cpp
	src/Shader/Monolithic/ShaderLanguageMonolithic.cpp
	src/Shader/Monolithic/TessellationControlShaderMonolithic.cpp
	src/Shader/Monolithic/TessellationEvaluationShaderMonolithic.cpp
	src/Shader/Monolithic/VertexShaderMonolithic.cpp
)
if(WIN32)
	# Windows source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		src/Windows/ContextWindows.cpp
		src/Windows/ExtensionsWindows.cpp
	)
endif()
if(UNIX)
	# Linux source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		src/Linux/ContextLinux.cpp
		src/Linux/ExtensionsLinux.cpp
	)
endif()


##################################################
## Libraries
##################################################
if(STATIC_LIBRARY)
	add_library(OpenGLRendererStatic STATIC ${SOURCE_CODES})
endif()
if(SHARED_LIBRARY)
	add_library(OpenGLRenderer SHARED ${SOURCE_CODES})
	set_target_properties(OpenGLRenderer PROPERTIES COMPILE_FLAGS "-DSHARED_LIBRARIES -DOPENGLRENDERER_EXPORTS")
endif()


##################################################
## Post build
##################################################
if(STATIC_LIBRARY)
	# Copy static library
	add_custom_command(TARGET OpenGLRendererStatic
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}OpenGLRendererStatic${CMAKE_STATIC_LIBRARY_SUFFIX}" ${OUTPUT_LIB_DIR}
	)
endif()
if(SHARED_LIBRARY)
	# Copy shared library
	add_custom_command(TARGET OpenGLRenderer
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}OpenGLRenderer${CMAKE_SHARED_LIBRARY_SUFFIX}" ${OUTPUT_BIN_DIR}
	)
	if(WIN32)
		add_custom_command(TARGET OpenGLRenderer
			COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/OpenGLRenderer.lib" ${OUTPUT_LIB_DIR}
		)
	endif()
endif()

##################################################
## Post build
##################################################
# Copy header
file(MAKE_DIRECTORY ${OUTPUT_INCLUDE_DIR}/../OpenGLRenderer/)
