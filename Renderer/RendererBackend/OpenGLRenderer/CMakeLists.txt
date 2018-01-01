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
if(RENDERER_OPENGL_NO_STATE_CLEANUP)
	add_definitions(-DOPENGLRENDERER_NO_STATE_CLEANUP)
endif()
if(RENDERER_OPENGL_GLSLTOSPIRV)
	add_definitions(-DOPENGLRENDERER_GLSLTOSPIRV)
endif()


##################################################
## Includes
##################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Renderer/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../External/OpenGL/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../External/smol-v/include)
if(RENDERER_OPENGL_GLSLTOSPIRV)
	set(GLSLANG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../External/glslang/src)
	include_directories(${GLSLANG_DIR})
endif()


##################################################
## Source codes
##################################################
# Cross-platform source codes
set(SOURCE_CODES
	${CMAKE_CURRENT_SOURCE_DIR}/../../../External/smol-v/src/smolv.cpp
	src/Extensions.cpp
	src/IOpenGLContext.cpp
	src/Mapping.cpp
	src/OpenGLRenderer.cpp
	src/OpenGLRuntimeLinking.cpp
	src/RootSignature.cpp
	src/ResourceGroup.cpp
	src/Buffer/BufferManager.cpp
	src/Buffer/IndexBuffer.cpp
	src/Buffer/IndexBufferBind.cpp
	src/Buffer/IndexBufferDsa.cpp
	src/Buffer/IndirectBuffer.cpp
	src/Buffer/IndirectBufferBind.cpp
	src/Buffer/IndirectBufferDsa.cpp
	src/Buffer/TextureBuffer.cpp
	src/Buffer/TextureBufferBind.cpp
	src/Buffer/TextureBufferDsa.cpp
	src/Buffer/UniformBuffer.cpp
	src/Buffer/UniformBufferBind.cpp
	src/Buffer/UniformBufferDsa.cpp
	src/Buffer/VertexArray.cpp
	src/Buffer/VertexArrayNoVao.cpp
	src/Buffer/VertexArrayVao.cpp
	src/Buffer/VertexArrayVaoBind.cpp
	src/Buffer/VertexArrayVaoDsa.cpp
	src/Buffer/VertexBuffer.cpp
	src/Buffer/VertexBufferBind.cpp
	src/Buffer/VertexBufferDsa.cpp
	src/RenderTarget/RenderPass.cpp
	src/RenderTarget/Framebuffer.cpp
	src/RenderTarget/FramebufferBind.cpp
	src/RenderTarget/FramebufferDsa.cpp
	src/RenderTarget/SwapChain.cpp
	src/Shader/Monolithic/FragmentShaderMonolithic.cpp
	src/Shader/Monolithic/GeometryShaderMonolithic.cpp
	src/Shader/Monolithic/ProgramMonolithic.cpp
	src/Shader/Monolithic/ProgramMonolithicDsa.cpp
	src/Shader/Monolithic/ShaderLanguageMonolithic.cpp
	src/Shader/Monolithic/TessellationControlShaderMonolithic.cpp
	src/Shader/Monolithic/TessellationEvaluationShaderMonolithic.cpp
	src/Shader/Monolithic/VertexShaderMonolithic.cpp
	src/Shader/Separate/FragmentShaderSeparate.cpp
	src/Shader/Separate/GeometryShaderSeparate.cpp
	src/Shader/Separate/ProgramSeparate.cpp
	src/Shader/Separate/ProgramSeparateDsa.cpp
	src/Shader/Separate/ShaderLanguageSeparate.cpp
	src/Shader/Separate/TessellationControlShaderSeparate.cpp
	src/Shader/Separate/TessellationEvaluationShaderSeparate.cpp
	src/Shader/Separate/VertexShaderSeparate.cpp
	src/State/BlendState.cpp
	src/State/DepthStencilState.cpp
	src/State/PipelineState.cpp
	src/State/RasterizerState.cpp
	src/State/SamplerState.cpp
	src/State/SamplerStateBind.cpp
	src/State/SamplerStateDsa.cpp
	src/State/SamplerStateSo.cpp
	src/Texture/Texture1D.cpp
	src/Texture/Texture1DBind.cpp
	src/Texture/Texture1DDsa.cpp
	src/Texture/Texture2D.cpp
	src/Texture/Texture2DBind.cpp
	src/Texture/Texture2DDsa.cpp
	src/Texture/Texture2DArray.cpp
	src/Texture/Texture2DArrayBind.cpp
	src/Texture/Texture2DArrayDsa.cpp
	src/Texture/Texture3D.cpp
	src/Texture/Texture3DBind.cpp
	src/Texture/Texture3DDsa.cpp
	src/Texture/TextureCube.cpp
	src/Texture/TextureCubeBind.cpp
	src/Texture/TextureCubeDsa.cpp
	src/Texture/TextureManager.cpp
)
if(WIN32)
	# Windows source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		src/Windows/OpenGLContextWindows.cpp
		src/Windows/ExtensionsWindows.cpp
	)
endif()
if(UNIX)
	# Linux source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		src/Linux/OpenGLContextLinux.cpp
		src/Linux/ExtensionsLinux.cpp
	)
endif()
if(RENDERER_OPENGL_GLSLTOSPIRV)
	set(GLSLANG_SOURCE_CODES
		${GLSLANG_DIR}/glslang/GenericCodeGen/CodeGen.cpp
		${GLSLANG_DIR}/glslang/GenericCodeGen/Link.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/preprocessor/Pp.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/preprocessor/PpAtom.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/preprocessor/PpContext.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/preprocessor/PpScanner.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/preprocessor/PpTokens.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/Constant.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/glslang_tab.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/InfoSink.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/Initialize.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/Intermediate.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/intermOut.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/IntermTraverse.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/iomapper.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/limits.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/linkValidate.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/ParseContextBase.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/ParseHelper.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/PoolAlloc.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/propagateNoContraction.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/reflection.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/RemoveTree.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/Scan.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/ShaderLang.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/SymbolTable.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/Versions.cpp
		${GLSLANG_DIR}/glslang/MachineIndependent/parseConst.cpp
		${GLSLANG_DIR}/OGLCompilersDLL/InitializeDll.cpp
		${GLSLANG_DIR}/SPIRV/GlslangToSpv.cpp
		${GLSLANG_DIR}/SPIRV/InReadableOrder.cpp
		${GLSLANG_DIR}/SPIRV/Logger.cpp
		${GLSLANG_DIR}/SPIRV/SpvBuilder.cpp
	)
	if(WIN32)
		# Windows source codes
		set(GLSLANG_SOURCE_CODES
			${GLSLANG_SOURCE_CODES}
			${GLSLANG_DIR}/glslang/OSDependent/Windows/ossource.cpp
		)
	endif()
	if(UNIX)
		# Linux source codes
		set(GLSLANG_SOURCE_CODES
			${GLSLANG_SOURCE_CODES}
			${GLSLANG_DIR}/glslang/OSDependent/Unix/ossource.cpp
		)
	endif()
	
	# Append GLSLANG sources to SOURCE_CODES variable
	set(SOURCE_CODES
		${SOURCE_CODES}
		${GLSLANG_SOURCE_CODES}
	)
	
	# Disable some warnings for the glslang code
	set_source_files_properties( ${GLSLANG_SOURCE_CODES} PROPERTIES COMPILE_FLAGS "-Wno-missing-field-initializers -Wno-reorder" )
endif()

# Disable some warnings for smol-v code
set_source_files_properties( ${CMAKE_CURRENT_SOURCE_DIR}/../../../External/smol-v/src/smolv.cpp PROPERTIES COMPILE_FLAGS "-Wno-sign-compare" )


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
