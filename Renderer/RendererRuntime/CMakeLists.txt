#/*********************************************************\
# * Copyright (c) 2012-2017 The Unrimp Team
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


cmake_minimum_required(VERSION 3.2.2)


##################################################
## Preprocessor definitions
##################################################
add_definitions(-DGLM_FORCE_RADIANS -DGLM_FORCE_LEFT_HANDED -DGLM_FORCE_DEPTH_ZERO_TO_ONE)
if(NOT RENDERER_DEBUG)
	add_definitions(-DRENDERER_NO_DEBUG)
endif()
if(UNIX)
	# Needed for OpenVR
	add_definitions(-DCOMPILER_GCC)
endif()


##################################################
## Includes
##################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Renderer/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/ImGui/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/ImGuizmo/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/OpenVR/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/crunch/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/glm/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/lz4/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../External/mikktspace/include/)


##################################################
## Source codes
##################################################
# Cross-platform source codes
set(SOURCE_CODES
	${CMAKE_CURRENT_SOURCE_DIR}/../../External/ImGui/src/imgui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../External/ImGui/src/imgui_draw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../External/ImGuizmo/src/ImGuizmo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../External/lz4/src/lz4.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../External/lz4/src/lz4frame.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../External/lz4/src/lz4hc.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../External/lz4/src/xxhash.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../External/mikktspace/src/mikktspace.c
	src/Asset/AssetManager.cpp
	src/Asset/AssetPackage.cpp
	src/Asset/Serializer/AssetPackageSerializer.cpp
	src/Backend/RendererRuntimeImpl.cpp
	src/Core/Math/Math.cpp
	src/Core/Math/EulerAngles.cpp
	src/Core/Math/Transform.cpp
	src/Core/Platform/PlatformManager.cpp
	src/Core/File/MemoryFile.cpp
	src/Core/Time/TimeManager.cpp
	src/Core/Time/Stopwatch.cpp
	src/Core/Renderer/FramebufferManager.cpp
	src/Core/Renderer/FramebufferSignature.cpp
	src/Core/Renderer/RenderTargetTextureManager.cpp
	src/Core/Renderer/RenderTargetTextureSignature.cpp
	src/Core/String.cpp
	src/DebugGui/DebugGuiManager.cpp
	src/DebugGui/DebugGuiHelper.cpp
	src/RenderQueue/IndirectBufferManager.cpp
	src/RenderQueue/Renderable.cpp
	src/RenderQueue/RenderableManager.cpp
	src/RenderQueue/RenderQueue.cpp
	src/Resource/CompositorNode/CompositorNodeInstance.cpp
	src/Resource/CompositorNode/CompositorNodeResource.cpp
	src/Resource/CompositorNode/CompositorNodeResourceManager.cpp
	src/Resource/CompositorNode/CompositorTarget.cpp
	src/Resource/CompositorNode/Loader/CompositorNodeResourceLoader.cpp
	src/Resource/CompositorNode/Pass/CompositorPassFactory.cpp
	src/Resource/CompositorNode/Pass/Clear/CompositorInstancePassClear.cpp
	src/Resource/CompositorNode/Pass/Clear/CompositorResourcePassClear.cpp
	src/Resource/CompositorNode/Pass/VrHiddenAreaMesh/CompositorInstancePassVrHiddenAreaMesh.cpp
	src/Resource/CompositorNode/Pass/VrHiddenAreaMesh/CompositorResourcePassVrHiddenAreaMesh.cpp
	src/Resource/CompositorNode/Pass/Scene/CompositorInstancePassScene.cpp
	src/Resource/CompositorNode/Pass/Scene/CompositorResourcePassScene.cpp
	src/Resource/CompositorNode/Pass/ShadowMap/CompositorInstancePassShadowMap.cpp
	src/Resource/CompositorNode/Pass/ShadowMap/CompositorResourcePassShadowMap.cpp
	src/Resource/CompositorNode/Pass/ResolveMultisample/CompositorInstancePassResolveMultisample.cpp
	src/Resource/CompositorNode/Pass/ResolveMultisample/CompositorResourcePassResolveMultisample.cpp
	src/Resource/CompositorNode/Pass/Copy/CompositorInstancePassCopy.cpp
	src/Resource/CompositorNode/Pass/Copy/CompositorResourcePassCopy.cpp
	src/Resource/CompositorNode/Pass/Quad/CompositorInstancePassQuad.cpp
	src/Resource/CompositorNode/Pass/Quad/CompositorResourcePassQuad.cpp
	src/Resource/CompositorNode/Pass/DebugGui/CompositorInstancePassDebugGui.cpp
	src/Resource/CompositorNode/Pass/DebugGui/CompositorResourcePassDebugGui.cpp
	src/Resource/CompositorWorkspace/CompositorWorkspaceInstance.cpp
	src/Resource/CompositorWorkspace/CompositorWorkspaceResourceManager.cpp
	src/Resource/CompositorWorkspace/Loader/CompositorWorkspaceResourceLoader.cpp
	src/Resource/Detail/IResource.cpp
	src/Resource/Detail/ResourceStreamer.cpp
	src/Resource/IResourceListener.cpp
	src/Resource/MaterialBlueprint/BufferManager/InstanceBufferManager.cpp
	src/Resource/MaterialBlueprint/BufferManager/LightBufferManager.cpp
	src/Resource/MaterialBlueprint/BufferManager/MaterialBufferManager.cpp
	src/Resource/MaterialBlueprint/BufferManager/MaterialBufferSlot.cpp
	src/Resource/MaterialBlueprint/BufferManager/PassBufferManager.cpp
	src/Resource/MaterialBlueprint/Cache/PipelineStateCacheManager.cpp
	src/Resource/MaterialBlueprint/Cache/PipelineStateCompiler.cpp
	src/Resource/MaterialBlueprint/Cache/PipelineStateSignature.cpp
	src/Resource/MaterialBlueprint/Cache/ProgramCacheManager.cpp
	src/Resource/MaterialBlueprint/Listener/MaterialBlueprintResourceListener.cpp
	src/Resource/MaterialBlueprint/Loader/MaterialBlueprintResourceLoader.cpp
	src/Resource/MaterialBlueprint/MaterialBlueprintResource.cpp
	src/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.cpp
	src/Resource/Material/Loader/MaterialResourceLoader.cpp
	src/Resource/Material/MaterialProperties.cpp
	src/Resource/Material/MaterialPropertyValue.cpp
	src/Resource/Material/MaterialResource.cpp
	src/Resource/Material/MaterialResourceManager.cpp
	src/Resource/Material/MaterialTechnique.cpp
	src/Resource/Mesh/Loader/IMeshResourceLoader.cpp
	src/Resource/Mesh/Loader/MeshResourceLoader.cpp
	src/Resource/Mesh/MeshResourceManager.cpp
	src/Resource/Mesh/MeshResource.cpp
	src/Resource/Scene/Factory/SceneFactory.cpp
	src/Resource/Scene/Item/Camera/CameraSceneItem.cpp
	src/Resource/Scene/Item/Light/LightSceneItem.cpp
	src/Resource/Scene/Item/Light/SunLightSceneItem.cpp
	src/Resource/Scene/Item/Mesh/MeshSceneItem.cpp
	src/Resource/Scene/Item/Mesh/SkeletonMeshSceneItem.cpp
	src/Resource/Scene/Item/Sky/SkyboxSceneItem.cpp
	src/Resource/Scene/Loader/SceneResourceLoader.cpp
	src/Resource/Scene/SceneNode.cpp
	src/Resource/Scene/SceneResource.cpp
	src/Resource/Scene/SceneResourceManager.cpp
	src/Resource/ShaderBlueprint/Cache/Preprocessor/mojoshader_common.cpp
	src/Resource/ShaderBlueprint/Cache/Preprocessor/mojoshader_lexer.cpp
	src/Resource/ShaderBlueprint/Cache/Preprocessor/mojoshader_preprocessor.cpp
	src/Resource/ShaderBlueprint/Cache/Preprocessor/Preprocessor.cpp
	src/Resource/ShaderBlueprint/Cache/ShaderBuilder.cpp
	src/Resource/ShaderBlueprint/Cache/ShaderCacheManager.cpp
	src/Resource/ShaderBlueprint/Cache/ShaderProperties.cpp
	src/Resource/ShaderBlueprint/Loader/ShaderBlueprintResourceLoader.cpp
	src/Resource/ShaderBlueprint/ShaderBlueprintResourceManager.cpp
	src/Resource/ShaderPiece/Loader/ShaderPieceResourceLoader.cpp
	src/Resource/ShaderPiece/ShaderPieceResourceManager.cpp
	src/Resource/Skeleton/Loader/SkeletonResourceLoader.cpp
	src/Resource/Skeleton/SkeletonResource.cpp
	src/Resource/Skeleton/SkeletonResourceManager.cpp
	src/Resource/SkeletonAnimation/Loader/SkeletonAnimationResourceLoader.cpp
	src/Resource/SkeletonAnimation/SkeletonAnimationResourceManager.cpp
	src/Resource/SkeletonAnimation/SkeletonAnimationEvaluator.cpp
	src/Resource/SkeletonAnimation/SkeletonAnimationController.cpp
	src/Resource/Texture/Loader/ITextureResourceLoader.cpp
	src/Resource/Texture/Loader/CrnTextureResourceLoader.cpp
	src/Resource/Texture/Loader/DdsTextureResourceLoader.cpp
	src/Resource/Texture/Loader/KtxTextureResourceLoader.cpp
	src/Resource/Texture/TextureResourceManager.cpp
	src/Resource/VertexAttributes/Loader/VertexAttributesResourceLoader.cpp
	src/Resource/VertexAttributes/VertexAttributesResourceManager.cpp
)

if(NOT ANDROID)
	# Not Android source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		src/Vr/OpenVR/OpenVRRuntimeLinking.cpp
		src/Vr/OpenVR/VrManagerOpenVR.cpp
		src/Vr/OpenVR/Loader/OpenVRMeshResourceLoader.cpp
		src/Vr/OpenVR/Loader/OpenVRTextureResourceLoader.cpp
	)
endif()

if(UNIX)
	# Windows source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		src/DebugGui/Detail/DebugGuiManagerLinux.cpp
	)
endif()

if(WIN32)
	# Windows source codes
	set(SOURCE_CODES
		${SOURCE_CODES}
		src/DebugGui/Detail/DebugGuiManagerWindows.cpp
		src/PrecompiledHeader.cpp
	)
endif()


##################################################
## Libraries
##################################################
if(STATIC_LIBRARY)
	add_library(RendererRuntimeStatic STATIC ${SOURCE_CODES})
endif()
if(SHARED_LIBRARY)
	add_library(RendererRuntime SHARED ${SOURCE_CODES})
	set_target_properties(RendererRuntime PROPERTIES COMPILE_FLAGS "-DSHARED_LIBRARIES -DRENDERERRUNTIME_EXPORTS")
	if (ANDROID)
		target_link_libraries(RendererRuntime dl ${ANDROID_CXX_STANDARD_LIBRARIES})
	elseif(UNIX)
		target_link_libraries(RendererRuntime dl pthread)
	endif()
endif()


##################################################
## Post build
##################################################
if(STATIC_LIBRARY)
	# Copy static library
	add_custom_command(TARGET RendererRuntimeStatic
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}RendererRuntimeStatic${CMAKE_STATIC_LIBRARY_SUFFIX}" ${OUTPUT_LIB_DIR}
	)
endif()
if(SHARED_LIBRARY)
	# Copy shared library
	add_custom_command(TARGET RendererRuntime
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}RendererRuntime${CMAKE_SHARED_LIBRARY_SUFFIX}" ${OUTPUT_BIN_DIR}
	)
	if(WIN32)
		add_custom_command(TARGET RendererRuntime
			COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/RendererRuntime.lib" ${OUTPUT_LIB_DIR}
		)
	endif()
endif()


##################################################
## Post build
##################################################
# Copy header
if(SHARED_LIBRARY)
	add_custom_command(TARGET RendererRuntime
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/include/RendererRuntime/Public/RendererRuntimeInstance.h" ${OUTPUT_INCLUDE_DIR}
	)
elseif(STATIC_LIBRARY)
	add_custom_command(TARGET RendererRuntimeStatic
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/include/RendererRuntime/Public/RendererRuntimeInstance.h" ${OUTPUT_INCLUDE_DIR}
	)
endif()
