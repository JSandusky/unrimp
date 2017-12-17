/*********************************************************\
 * Copyright (c) 2012-2017 The Unrimp Team
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


// TODO(co) Visual Studio 2015 compile settings: For some reasons I need to disable optimization for "ShaderLanguageGlsl.cpp" or else "glslang::TShader::parse()" will output the error "ERROR: 0:1: '€' : unexpected token" (glslang (latest commit 652db16ff114747c216ec631767dfd27e3d3c838 - July 7, 2017))


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "VulkanRenderer/Shader/ShaderLanguageGlsl.h"
#include "VulkanRenderer/Shader/ProgramGlsl.h"
#include "VulkanRenderer/Shader/VertexShaderGlsl.h"
#include "VulkanRenderer/Shader/GeometryShaderGlsl.h"
#include "VulkanRenderer/Shader/FragmentShaderGlsl.h"
#include "VulkanRenderer/Shader/TessellationControlShaderGlsl.h"
#include "VulkanRenderer/Shader/TessellationEvaluationShaderGlsl.h"
#include "VulkanRenderer/VulkanRenderer.h"
#include "VulkanRenderer/VulkanContext.h"

#include <Renderer/ILog.h>
#include <Renderer/IAssert.h>
#include <Renderer/IAllocator.h>

#ifdef VULKANRENDERER_GLSLTOSPIRV
	// Disable warnings in external headers, we can't fix them
	PRAGMA_WARNING_PUSH
		PRAGMA_WARNING_DISABLE_MSVC(4061)	// warning C4061: enumerator '<x>' in switch of enum '<y>' is not explicitly handled by a case label
		PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'argument': conversion from '<x>' to '<y>', signed/unsigned mismatch
		PRAGMA_WARNING_DISABLE_MSVC(4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
		PRAGMA_WARNING_DISABLE_MSVC(4623)	// warning C4623: 'std::_List_node<_Ty,std::_Default_allocator_traits<_Alloc>::void_pointer>': default constructor was implicitly defined as deleted
		PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: '<x>': copy constructor was implicitly defined as deleted
		PRAGMA_WARNING_DISABLE_MSVC(4626)	// warning C4626: 'std::codecvt_base': assignment operator was implicitly defined as deleted
		PRAGMA_WARNING_DISABLE_MSVC(4774)	// warning C4774: 'sprintf_s' : format string expected in argument 3 is not a string literal
		PRAGMA_WARNING_DISABLE_MSVC(5027)	// warning C5027: 'std::_Generic_error_category': move assignment operator was implicitly defined as deleted
		#include <SPIRV/GlslangToSpv.h>
		#include <glslang/MachineIndependent/localintermediate.h>
	PRAGMA_WARNING_POP
#else
	#include <tuple>	// For "std::ignore"
#endif

#include <smol-v/smolv.h>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global variables                                      ]
		//[-------------------------------------------------------]
		#ifdef VULKANRENDERER_GLSLTOSPIRV
			static bool GlslangInitialized = false;

			// Settings from "glslang/StandAlone/ResourceLimits.cpp"
			const TBuiltInResource DefaultTBuiltInResource = {
				32,		///< MaxLights
				6,		///< MaxClipPlanes
				32,		///< MaxTextureUnits
				32,		///< MaxTextureCoords
				64,		///< MaxVertexAttribs
				4096,	///< MaxVertexUniformComponents
				64,		///< MaxVaryingFloats
				32,		///< MaxVertexTextureImageUnits
				80,		///< MaxCombinedTextureImageUnits
				32,		///< MaxTextureImageUnits
				4096,	///< MaxFragmentUniformComponents
				32,		///< MaxDrawBuffers
				128,	///< MaxVertexUniformVectors
				8,		///< MaxVaryingVectors
				16,		///< MaxFragmentUniformVectors
				16,		///< MaxVertexOutputVectors
				15,		///< MaxFragmentInputVectors
				-8,		///< MinProgramTexelOffset
				7,		///< MaxProgramTexelOffset
				8,		///< MaxClipDistances
				65535,	///< MaxComputeWorkGroupCountX
				65535,	///< MaxComputeWorkGroupCountY
				65535,	///< MaxComputeWorkGroupCountZ
				1024,	///< MaxComputeWorkGroupSizeX
				1024,	///< MaxComputeWorkGroupSizeY
				64,		///< MaxComputeWorkGroupSizeZ
				1024,	///< MaxComputeUniformComponents
				16,		///< MaxComputeTextureImageUnits
				8,		///< MaxComputeImageUniforms
				8,		///< MaxComputeAtomicCounters
				1,		///< MaxComputeAtomicCounterBuffers
				60,		///< MaxVaryingComponents
				64,		///< MaxVertexOutputComponents
				64,		///< MaxGeometryInputComponents
				128,	///< MaxGeometryOutputComponents
				128,	///< MaxFragmentInputComponents
				8,		///< MaxImageUnits
				8,		///< MaxCombinedImageUnitsAndFragmentOutputs
				8,		///< MaxCombinedShaderOutputResources
				0,		///< MaxImageSamples
				0,		///< MaxVertexImageUniforms
				0,		///< MaxTessControlImageUniforms
				0,		///< MaxTessEvaluationImageUniforms
				0,		///< MaxGeometryImageUniforms
				8,		///< MaxFragmentImageUniforms
				8,		///< MaxCombinedImageUniforms
				16,		///< MaxGeometryTextureImageUnits
				256,	///< MaxGeometryOutputVertices
				1024,	///< MaxGeometryTotalOutputComponents
				1024,	///< MaxGeometryUniformComponents
				64,		///< MaxGeometryVaryingComponents
				128,	///< MaxTessControlInputComponents
				128,	///< MaxTessControlOutputComponents
				16,		///< MaxTessControlTextureImageUnits
				1024,	///< MaxTessControlUniformComponents
				4096,	///< MaxTessControlTotalOutputComponents
				128,	///< MaxTessEvaluationInputComponents
				128,	///< MaxTessEvaluationOutputComponents
				16,		///< MaxTessEvaluationTextureImageUnits
				1024,	///< MaxTessEvaluationUniformComponents
				120,	///< MaxTessPatchComponents
				32,		///< MaxPatchVertices
				64,		///< MaxTessGenLevel
				16,		///< MaxViewports
				0,		///< MaxVertexAtomicCounters
				0,		///< MaxTessControlAtomicCounters
				0,		///< MaxTessEvaluationAtomicCounters
				0,		///< MaxGeometryAtomicCounters
				8,		///< MaxFragmentAtomicCounters
				8,		///< MaxCombinedAtomicCounters
				1,		///< MaxAtomicCounterBindings
				0,		///< MaxVertexAtomicCounterBuffers
				0,		///< MaxTessControlAtomicCounterBuffers
				0,		///< MaxTessEvaluationAtomicCounterBuffers
				0,		///< MaxGeometryAtomicCounterBuffers
				1,		///< MaxFragmentAtomicCounterBuffers
				1,		///< MaxCombinedAtomicCounterBuffers
				16384,	///< MaxAtomicCounterBufferSize
				4,		///< MaxTransformFeedbackBuffers
				64,		///< MaxTransformFeedbackInterleavedComponents
				8,		///< MaxCullDistances
				8,		///< MaxCombinedClipAndCullDistances
				4,		///< MaxSamples
				{		///< limits
					1,	///< nonInductiveForLoops
					1,	///< whileLoops
					1,	///< doWhileLoops
					1,	///< generalUniformIndexing
					1,	///< generalAttributeMatrixVectorIndexing
					1,	///< generalVaryingIndexing
					1,	///< generalSamplerIndexing
					1,	///< generalVariableIndexing
					1,	///< generalConstantMatrixVectorIndexing
				}
			};
		#endif


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const char* ShaderLanguageGlsl::NAME = "GLSL";


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	VkShaderModule ShaderLanguageGlsl::createVkShaderModuleFromBytecode(VulkanRenderer& vulkanRenderer, const Renderer::ShaderBytecode& shaderBytecode)
	{
		// Decode from SMOL-V: like Vulkan/Khronos SPIR-V, but smaller
		// -> https://github.com/aras-p/smol-v
		// -> http://aras-p.info/blog/2016/09/01/SPIR-V-Compression/
		const size_t spirvOutputBufferSize = smolv::GetDecodedBufferSize(shaderBytecode.getBytecode(), shaderBytecode.getNumberOfBytes());
		// TODO(co) Try to avoid new/delete by trying to use the C-runtime stack if there aren't too many bytes
		const Renderer::Context& context = vulkanRenderer.getContext();
		uint8_t* spirvOutputBuffer = RENDERER_MALLOC_TYPED(context, uint8_t, spirvOutputBufferSize);
		smolv::Decode(shaderBytecode.getBytecode(), shaderBytecode.getNumberOfBytes(), spirvOutputBuffer, spirvOutputBufferSize);

		// Create the Vulkan shader module
		const VkShaderModuleCreateInfo vkShaderModuleCreateInfo =
		{
			VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,			// sType (VkStructureType)
			nullptr,												// pNext (const void*)
			0,														// flags (VkShaderModuleCreateFlags)
			spirvOutputBufferSize,									// codeSize (size_t)
			reinterpret_cast<const uint32_t*>(spirvOutputBuffer)	// pCode (const uint32_t*)
		};
		VkShaderModule vkShaderModule = VK_NULL_HANDLE;
		if (vkCreateShaderModule(vulkanRenderer.getVulkanContext().getVkDevice(), &vkShaderModuleCreateInfo, vulkanRenderer.getVkAllocationCallbacks(), &vkShaderModule) != VK_SUCCESS)
		{
			RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to create the Vulkan shader module")
		}

		// Done
		RENDERER_FREE(context, spirvOutputBuffer);
		return vkShaderModule;
	}

	VkShaderModule ShaderLanguageGlsl::createVkShaderModuleFromSourceCode(VulkanRenderer& vulkanRenderer, VkShaderStageFlagBits vkShaderStageFlagBits, const char* sourceCode, Renderer::ShaderBytecode* shaderBytecode)
	{
		#ifdef VULKANRENDERER_GLSLTOSPIRV
			// Initialize glslang, if necessary
			if (!::detail::GlslangInitialized)
			{
				glslang::InitializeProcess();
				::detail::GlslangInitialized = true;
			}

			// GLSL to intermediate
			// -> OpenGL 4.5
			const int glslVersion = 450;
			EShLanguage shLanguage = EShLangCount;
			if ((vkShaderStageFlagBits & VK_SHADER_STAGE_VERTEX_BIT) != 0)
			{
				shLanguage = EShLangVertex;
			}
			else if ((vkShaderStageFlagBits & VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT) != 0)
			{
				shLanguage = EShLangTessControl;
			}
			else if ((vkShaderStageFlagBits & VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT) != 0)
			{
				shLanguage = EShLangTessEvaluation;
			}
			else if ((vkShaderStageFlagBits & VK_SHADER_STAGE_GEOMETRY_BIT) != 0)
			{
				shLanguage = EShLangGeometry;
			}
			else if ((vkShaderStageFlagBits & VK_SHADER_STAGE_FRAGMENT_BIT) != 0)
			{
				shLanguage = EShLangFragment;
			}
			else
			{
				RENDERER_ASSERT(vulkanRenderer.getContext(), false, "Invalid Vulkan shader stage flag bits")
			}
			glslang::TShader shader(shLanguage);
			shader.setEnvInput(glslang::EShSourceGlsl, shLanguage, glslang::EShClientVulkan, glslVersion);
			shader.setEntryPoint("main");
			{
				const char* sourcePointers[] = { sourceCode };
				shader.setStrings(sourcePointers, 1);
			}
			const EShMessages shMessages = static_cast<EShMessages>(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules);
			if (shader.parse(&::detail::DefaultTBuiltInResource, glslVersion, false, shMessages))
			{
				glslang::TProgram program;
				program.addShader(&shader);
				if (program.link(shMessages))
				{
					// Intermediate to SPIR-V
					const glslang::TIntermediate* intermediate = program.getIntermediate(shLanguage);
					if (nullptr != intermediate)
					{
						std::vector<unsigned int> spirv;
						glslang::GlslangToSpv(*intermediate, spirv);

						// Optional shader bytecode output
						if (nullptr != shaderBytecode)
						{
							// Encode to SMOL-V: like Vulkan/Khronos SPIR-V, but smaller
							// -> https://github.com/aras-p/smol-v
							// -> http://aras-p.info/blog/2016/09/01/SPIR-V-Compression/
							// -> Don't apply "spv::spirvbin_t::remap()" or the SMOL-V result will be bigger
							smolv::ByteArray byteArray;
							smolv::Encode(spirv.data(), sizeof(unsigned int) * spirv.size(), byteArray, smolv::kEncodeFlagStripDebugInfo);

							// Done
							shaderBytecode->setBytecodeCopy(static_cast<uint32_t>(byteArray.size()), reinterpret_cast<uint8_t*>(byteArray.data()));
						}

						// Create the Vulkan shader module
						const VkShaderModuleCreateInfo vkShaderModuleCreateInfo =
						{
							VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,	// sType (VkStructureType)
							nullptr,										// pNext (const void*)
							0,												// flags (VkShaderModuleCreateFlags)
							sizeof(unsigned int) * spirv.size(),			// codeSize (size_t)
							spirv.data()									// pCode (const uint32_t*)
						};
						VkShaderModule vkShaderModule = VK_NULL_HANDLE;
						if (vkCreateShaderModule(vulkanRenderer.getVulkanContext().getVkDevice(), &vkShaderModuleCreateInfo, vulkanRenderer.getVkAllocationCallbacks(), &vkShaderModule) != VK_SUCCESS)
						{
							RENDERER_LOG(vulkanRenderer.getContext(), CRITICAL, "Failed to create the Vulkan shader module")
						}
						return vkShaderModule;
					}
				}
				else
				{
					// Failed to link the program
					if (vulkanRenderer.getContext().getLog().print(Renderer::ILog::Type::CRITICAL, sourceCode, __FILE__, static_cast<uint32_t>(__LINE__), "Failed to link the GLSL program: %s", program.getInfoLog()))
					{
						DEBUG_BREAK;
					}
				}
			}
			else
			{
				// Failed to parse the shader source code
				if (vulkanRenderer.getContext().getLog().print(Renderer::ILog::Type::CRITICAL, sourceCode, __FILE__, static_cast<uint32_t>(__LINE__), "Failed to parse the GLSL shader source code: %s", shader.getInfoLog()))
				{
					DEBUG_BREAK;
				}
			}
		#else
			std::ignore = vulkanRenderer;
			std::ignore = sourceCode;
			std::ignore = shaderBytecode;
		#endif

		// Error!
		return VK_NULL_HANDLE;
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ShaderLanguageGlsl::ShaderLanguageGlsl(VulkanRenderer& vulkanRenderer) :
		IShaderLanguage(vulkanRenderer)
	{
		// Nothing here
	}

	ShaderLanguageGlsl::~ShaderLanguageGlsl()
	{
		// De-initialize glslang, if necessary
		#ifdef VULKANRENDERER_GLSLTOSPIRV
			if (::detail::GlslangInitialized)
			{
				// TODO(co) Fix glslang related memory leaks. See also
				//		    - "Fix a few memory leaks #916" - https://github.com/KhronosGroup/glslang/pull/916
				//		    - "FreeGlobalPools is never called in glslang::FinalizeProcess()'s path. #928" - https://github.com/KhronosGroup/glslang/issues/928
				glslang::FinalizeProcess();
				::detail::GlslangInitialized = false;
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShaderLanguage methods      ]
	//[-------------------------------------------------------]
	const char* ShaderLanguageGlsl::getShaderLanguageName() const
	{
		return NAME;
	}

	Renderer::IVertexShader* ShaderLanguageGlsl::createVertexShaderFromBytecode(const Renderer::VertexAttributes&, const Renderer::ShaderBytecode& shaderBytecode)
	{
		return RENDERER_NEW(getRenderer().getContext(), VertexShaderGlsl)(static_cast<VulkanRenderer&>(getRenderer()), shaderBytecode);
	}

	Renderer::IVertexShader* ShaderLanguageGlsl::createVertexShaderFromSourceCode(const Renderer::VertexAttributes&, const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode* shaderBytecode)
	{
		return RENDERER_NEW(getRenderer().getContext(), VertexShaderGlsl)(static_cast<VulkanRenderer&>(getRenderer()), shaderSourceCode.sourceCode, shaderBytecode);
	}

	Renderer::ITessellationControlShader* ShaderLanguageGlsl::createTessellationControlShaderFromBytecode(const Renderer::ShaderBytecode& shaderBytecode)
	{
		return RENDERER_NEW(getRenderer().getContext(), TessellationControlShaderGlsl)(static_cast<VulkanRenderer&>(getRenderer()), shaderBytecode);
	}

	Renderer::ITessellationControlShader* ShaderLanguageGlsl::createTessellationControlShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode* shaderBytecode)
	{
		return RENDERER_NEW(getRenderer().getContext(), TessellationControlShaderGlsl)(static_cast<VulkanRenderer&>(getRenderer()), shaderSourceCode.sourceCode, shaderBytecode);
	}

	Renderer::ITessellationEvaluationShader* ShaderLanguageGlsl::createTessellationEvaluationShaderFromBytecode(const Renderer::ShaderBytecode& shaderBytecode)
	{
		return RENDERER_NEW(getRenderer().getContext(), TessellationEvaluationShaderGlsl)(static_cast<VulkanRenderer&>(getRenderer()), shaderBytecode);
	}

	Renderer::ITessellationEvaluationShader* ShaderLanguageGlsl::createTessellationEvaluationShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode* shaderBytecode)
	{
		return RENDERER_NEW(getRenderer().getContext(), TessellationEvaluationShaderGlsl)(static_cast<VulkanRenderer&>(getRenderer()), shaderSourceCode.sourceCode, shaderBytecode);
	}

	Renderer::IGeometryShader* ShaderLanguageGlsl::createGeometryShaderFromBytecode(const Renderer::ShaderBytecode& shaderBytecode, Renderer::GsInputPrimitiveTopology gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices)
	{
		return RENDERER_NEW(getRenderer().getContext(), GeometryShaderGlsl)(static_cast<VulkanRenderer&>(getRenderer()), shaderBytecode, gsInputPrimitiveTopology, gsOutputPrimitiveTopology, numberOfOutputVertices);
	}

	Renderer::IGeometryShader* ShaderLanguageGlsl::createGeometryShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::GsInputPrimitiveTopology gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices, Renderer::ShaderBytecode* shaderBytecode)
	{
		return RENDERER_NEW(getRenderer().getContext(), GeometryShaderGlsl)(static_cast<VulkanRenderer&>(getRenderer()), shaderSourceCode.sourceCode, gsInputPrimitiveTopology, gsOutputPrimitiveTopology, numberOfOutputVertices, shaderBytecode);
	}

	Renderer::IFragmentShader* ShaderLanguageGlsl::createFragmentShaderFromBytecode(const Renderer::ShaderBytecode& shaderBytecode)
	{
		return RENDERER_NEW(getRenderer().getContext(), FragmentShaderGlsl)(static_cast<VulkanRenderer&>(getRenderer()), shaderBytecode);
	}

	Renderer::IFragmentShader* ShaderLanguageGlsl::createFragmentShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode* shaderBytecode)
	{
		return RENDERER_NEW(getRenderer().getContext(), FragmentShaderGlsl)(static_cast<VulkanRenderer&>(getRenderer()), shaderSourceCode.sourceCode, shaderBytecode);
	}

	Renderer::IProgram* ShaderLanguageGlsl::createProgram(const Renderer::IRootSignature& rootSignature, const Renderer::VertexAttributes& vertexAttributes, Renderer::IVertexShader* vertexShader, Renderer::ITessellationControlShader* tessellationControlShader, Renderer::ITessellationEvaluationShader* tessellationEvaluationShader, Renderer::IGeometryShader* geometryShader, Renderer::IFragmentShader* fragmentShader)
	{
		VulkanRenderer& vulkanRenderer = static_cast<VulkanRenderer&>(getRenderer());

		// A shader can be a null pointer, but if it's not the shader and program language must match!
		// -> Optimization: Comparing the shader language name by directly comparing the pointer address of
		//    the name is safe because we know that we always reference to one and the same name address
		// TODO(co) Add security check: Is the given resource one of the currently used renderer?
		if (nullptr != vertexShader && vertexShader->getShaderLanguageName() != NAME)
		{
			// Error! Vertex shader language mismatch!
		}
		else if (nullptr != tessellationControlShader && tessellationControlShader->getShaderLanguageName() != NAME)
		{
			// Error! Tessellation control shader language mismatch!
		}
		else if (nullptr != tessellationEvaluationShader && tessellationEvaluationShader->getShaderLanguageName() != NAME)
		{
			// Error! Tessellation evaluation shader language mismatch!
		}
		else if (nullptr != geometryShader && geometryShader->getShaderLanguageName() != NAME)
		{
			// Error! Geometry shader language mismatch!
		}
		else if (nullptr != fragmentShader && fragmentShader->getShaderLanguageName() != NAME)
		{
			// Error! Fragment shader language mismatch!
		}
		else
		{
			return RENDERER_NEW(getRenderer().getContext(), ProgramGlsl)(vulkanRenderer, rootSignature, vertexAttributes, static_cast<VertexShaderGlsl*>(vertexShader), static_cast<TessellationControlShaderGlsl*>(tessellationControlShader), static_cast<TessellationEvaluationShaderGlsl*>(tessellationEvaluationShader), static_cast<GeometryShaderGlsl*>(geometryShader), static_cast<FragmentShaderGlsl*>(fragmentShader));
		}

		// Error! Shader language mismatch!
		// -> Ensure a correct reference counter behaviour, even in the situation of an error
		if (nullptr != vertexShader)
		{
			vertexShader->addReference();
			vertexShader->releaseReference();
		}
		if (nullptr != tessellationControlShader)
		{
			tessellationControlShader->addReference();
			tessellationControlShader->releaseReference();
		}
		if (nullptr != tessellationEvaluationShader)
		{
			tessellationEvaluationShader->addReference();
			tessellationEvaluationShader->releaseReference();
		}
		if (nullptr != geometryShader)
		{
			geometryShader->addReference();
			geometryShader->releaseReference();
		}
		if (nullptr != fragmentShader)
		{
			fragmentShader->addReference();
			fragmentShader->releaseReference();
		}

		// Error!
		return nullptr;
	}


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	void ShaderLanguageGlsl::selfDestruct()
	{
		RENDERER_DELETE(getRenderer().getContext(), ShaderLanguageGlsl, this);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
