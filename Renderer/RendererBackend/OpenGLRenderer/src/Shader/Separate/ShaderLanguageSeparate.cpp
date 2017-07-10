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


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/Shader/Separate/ShaderLanguageSeparate.h"
#include "OpenGLRenderer/Shader/Separate/ProgramSeparate.h"
#include "OpenGLRenderer/Shader/Separate/ProgramSeparateDsa.h"
#include "OpenGLRenderer/Shader/Separate/VertexShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/GeometryShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/FragmentShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/TessellationControlShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/TessellationEvaluationShaderSeparate.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/OpenGLRenderer.h"

#include <Renderer/ILog.h>

#ifdef OPENGLRENDERER_GLSLTOSPIRV
	// Disable warnings in external headers, we can't fix them
	PRAGMA_WARNING_PUSH
		PRAGMA_WARNING_DISABLE_MSVC(4061)	// warning C4061: enumerator '<x>' in switch of enum '<y>' is not explicitly handled by a case label
		PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'argument': conversion from '<x>' to '<y>', signed/unsigned mismatch
		PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: '<x>': copy constructor was implicitly defined as deleted
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
		#ifdef OPENGLRENDERER_GLSLTOSPIRV
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
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		void printOpenGLShaderProgramInformationIntoLog(OpenGLRenderer::OpenGLRenderer& openGLRenderer, GLuint openGLObject)
		{
			// Get the length of the information (including a null termination)
			GLint informationLength = 0;
			OpenGLRenderer::glGetObjectParameterivARB(openGLObject, GL_OBJECT_INFO_LOG_LENGTH_ARB, &informationLength);
			if (informationLength > 1)
			{
				// Allocate memory for the information
				char* informationLog = new char[static_cast<uint32_t>(informationLength)];

				// Get the information
				OpenGLRenderer::glGetInfoLogARB(openGLObject, informationLength, nullptr, informationLog);

				// Output the debug string
				RENDERER_LOG(openGLRenderer.getContext(), CRITICAL, informationLog)

				// Cleanup information memory
				delete [] informationLog;
			}
		}


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const char* ShaderLanguageSeparate::NAME = "GLSL";


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	uint32_t ShaderLanguageSeparate::loadShaderFromBytecode(uint32_t shaderType, const Renderer::ShaderBytecode& shaderBytecode)
	{
		// Create the shader object
		const GLuint openGLShader = glCreateShaderObjectARB(shaderType);

		// Load the SPIR-V module into the shader object
		// -> "glShaderBinary" is OpenGL 4.1
		{
			// Decode from SMOL-V: like Vulkan/Khronos SPIR-V, but smaller
			// -> https://github.com/aras-p/smol-v
			// -> http://aras-p.info/blog/2016/09/01/SPIR-V-Compression/
			const size_t spirvOutputBufferSize = smolv::GetDecodedBufferSize(shaderBytecode.getBytecode(), shaderBytecode.getNumberOfBytes());
			uint8_t* spirvOutputBuffer = new uint8_t[spirvOutputBufferSize];
			smolv::Decode(shaderBytecode.getBytecode(), shaderBytecode.getNumberOfBytes(), spirvOutputBuffer, spirvOutputBufferSize);
			glShaderBinary(1, &openGLShader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, spirvOutputBuffer, static_cast<GLsizei>(spirvOutputBufferSize));
			delete [] spirvOutputBuffer;
		}

		// Done
		return openGLShader;
	}

	uint32_t ShaderLanguageSeparate::loadShaderProgramFromBytecode(OpenGLRenderer& openGLRenderer, uint32_t shaderType, const Renderer::ShaderBytecode& shaderBytecode)
	{
		// Create and load the shader object
		const GLuint openGLShader = loadShaderFromBytecode(shaderType, shaderBytecode);

		// Specialize the shader
		// -> Before this shader the isn't compiled, after this shader is supposed to be compiled
		glSpecializeShaderARB(openGLShader, "main", 0, nullptr, nullptr);

		// Check the compile status
		GLint compiled = GL_FALSE;
		glGetObjectParameterivARB(openGLShader, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
		if (GL_TRUE == compiled)
		{
			// All went fine, create and return the program
			const GLuint openGLProgram = glCreateProgramObjectARB();
			glProgramParameteri(openGLProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);
			glAttachObjectARB(openGLProgram, openGLShader);
			glLinkProgramARB(openGLProgram);
			glDetachObjectARB(openGLProgram, openGLShader);
			glDeleteObjectARB(openGLShader);

			// Check the link status
			GLint linked = GL_FALSE;
			glGetObjectParameterivARB(openGLProgram, GL_OBJECT_LINK_STATUS_ARB, &linked);
			if (GL_TRUE != linked)
			{
				// Error, program link failed!
				::detail::printOpenGLShaderProgramInformationIntoLog(openGLRenderer, openGLProgram);
			}

			// Done
			return openGLProgram;
		}
		else
		{
			// Error, failed to compile the shader!
			::detail::printOpenGLShaderProgramInformationIntoLog(openGLRenderer, openGLShader);

			// Destroy the OpenGL shader
			// -> A value of 0 for shader will be silently ignored
			glDeleteObjectARB(openGLShader);

			// Error!
			return 0;
		}
	}

	uint32_t ShaderLanguageSeparate::loadShaderProgramFromSourceCode(OpenGLRenderer& openGLRenderer, uint32_t shaderType, const char* sourceCode)
	{
		// Create the shader program
		const GLuint openGLProgram = glCreateShaderProgramv(shaderType, 1, &sourceCode);

		// Check the link status
		GLint linked = GL_FALSE;
		glGetObjectParameterivARB(openGLProgram, GL_LINK_STATUS, &linked);
		if (GL_TRUE == linked)
		{
			// All went fine, return the program
			return openGLProgram;
		}
		else
		{
			// Error, failed to compile the shader!
			::detail::printOpenGLShaderProgramInformationIntoLog(openGLRenderer, openGLProgram);

			// Destroy the program
			// -> A value of 0 for shader will be silently ignored
			glDeleteProgram(openGLProgram);

			// Error!
			return 0;
		}
	}

	void ShaderLanguageSeparate::shaderSourceCodeToShaderBytecode(uint32_t shaderType, const char* sourceCode, Renderer::ShaderBytecode& shaderBytecode)
	{
		#ifdef OPENGLRENDERER_GLSLTOSPIRV
			// Initialize glslang, if necessary
			if (!::detail::GlslangInitialized)
			{
				glslang::InitializeProcess();
				::detail::GlslangInitialized = true;
			}

			// GLSL to intermediate
			// -> OpenGL 4.1 (the best OpenGL version Mac OS X 10.11 supports, so lowest version we have to support)
			EShLanguage shLanguage = EShLangCount;
			switch (shaderType)
			{
				case GL_VERTEX_SHADER_ARB:
					shLanguage = EShLangVertex;
					break;

				case GL_TESS_CONTROL_SHADER:
					shLanguage = EShLangTessControl;
					break;

				case GL_TESS_EVALUATION_SHADER:
					shLanguage = EShLangTessEvaluation;
					break;

				case GL_GEOMETRY_SHADER_ARB:
					shLanguage = EShLangGeometry;
					break;

				case GL_FRAGMENT_SHADER_ARB:
					shLanguage = EShLangFragment;
					break;
			}
			glslang::TShader shader(shLanguage);
			shader.setEntryPoint("main");
			{
				const char* sourcePointers[] = { sourceCode };
				shader.setStrings(sourcePointers, 1);
			}
			shader.parse(&::detail::DefaultTBuiltInResource, 410, false, EShMsgDefault);
			glslang::TProgram program;
			program.addShader(&shader);
			program.link(EShMsgDefault);

			// Intermediate to SPIR-V
			const glslang::TIntermediate* intermediate = program.getIntermediate(shLanguage);
			if (nullptr != intermediate)
			{
				std::vector<unsigned int> spirv;
				glslang::GlslangToSpv(*intermediate, spirv);

				// Encode to SMOL-V: like Vulkan/Khronos SPIR-V, but smaller
				// -> https://github.com/aras-p/smol-v
				// -> http://aras-p.info/blog/2016/09/01/SPIR-V-Compression/
				// -> Don't apply "spv::spirvbin_t::remap()" or the SMOL-V result will be bigger
				smolv::ByteArray byteArray;
				smolv::Encode(spirv.data(), sizeof(unsigned int) * spirv.size(), byteArray, smolv::kEncodeFlagStripDebugInfo);

				// Done
				shaderBytecode.setBytecodeCopy(static_cast<uint32_t>(byteArray.size()), reinterpret_cast<uint8_t*>(byteArray.data()));
			}
		#else
			std::ignore = shaderType;
			std::ignore = sourceCode;
			std::ignore = shaderBytecode;
		#endif
	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ShaderLanguageSeparate::ShaderLanguageSeparate(OpenGLRenderer& openGLRenderer) :
		IShaderLanguage(openGLRenderer)
	{
		// Nothing here
	}

	ShaderLanguageSeparate::~ShaderLanguageSeparate()
	{
		// De-initialize glslang, if necessary
		#ifdef OPENGLRENDERER_GLSLTOSPIRV
			if (::detail::GlslangInitialized)
			{
				glslang::FinalizeProcess();
				::detail::GlslangInitialized = false;
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShaderLanguage methods      ]
	//[-------------------------------------------------------]
	const char* ShaderLanguageSeparate::getShaderLanguageName() const
	{
		return NAME;
	}

	Renderer::IVertexShader* ShaderLanguageSeparate::createVertexShaderFromBytecode(const Renderer::VertexAttributes& vertexAttributes, const Renderer::ShaderBytecode& shaderBytecode)
	{
		// Check whether or not there's vertex shader support
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		const Extensions& extensions = openGLRenderer.getExtensions();
		if (extensions.isGL_ARB_vertex_shader() && extensions.isGL_ARB_gl_spirv())
		{
			return new VertexShaderSeparate(openGLRenderer, vertexAttributes, shaderBytecode);
		}
		else
		{
			// Error! There's no vertex shader support or no decent shader bytecode support!
			return nullptr;
		}
	}

	Renderer::IVertexShader* ShaderLanguageSeparate::createVertexShaderFromSourceCode(const Renderer::VertexAttributes& vertexAttributes, const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode* shaderBytecode)
	{
		// Check whether or not there's vertex shader support
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		const Extensions& extensions = openGLRenderer.getExtensions();
		if (extensions.isGL_ARB_vertex_shader())
		{
			return new VertexShaderSeparate(openGLRenderer, vertexAttributes, shaderSourceCode.sourceCode, extensions.isGL_ARB_gl_spirv() ? shaderBytecode : nullptr);
		}
		else
		{
			// Error! There's no vertex shader support!
			return nullptr;
		}
	}

	Renderer::ITessellationControlShader* ShaderLanguageSeparate::createTessellationControlShaderFromBytecode(const Renderer::ShaderBytecode& shaderBytecode)
	{
		// Check whether or not there's tessellation support
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		const Extensions& extensions = openGLRenderer.getExtensions();
		if (extensions.isGL_ARB_tessellation_shader() && extensions.isGL_ARB_gl_spirv())
		{
			return new TessellationControlShaderSeparate(openGLRenderer, shaderBytecode);
		}
		else
		{
			// Error! There's no tessellation support or no decent shader bytecode support!
			return nullptr;
		}
	}

	Renderer::ITessellationControlShader* ShaderLanguageSeparate::createTessellationControlShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode* shaderBytecode)
	{
		// Check whether or not there's tessellation support
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		const Extensions& extensions = openGLRenderer.getExtensions();
		if (extensions.isGL_ARB_tessellation_shader())
		{
			return new TessellationControlShaderSeparate(openGLRenderer, shaderSourceCode.sourceCode, extensions.isGL_ARB_gl_spirv() ? shaderBytecode : nullptr);
		}
		else
		{
			// Error! There's no tessellation support!
			return nullptr;
		}
	}

	Renderer::ITessellationEvaluationShader* ShaderLanguageSeparate::createTessellationEvaluationShaderFromBytecode(const Renderer::ShaderBytecode& shaderBytecode)
	{
		// Check whether or not there's tessellation support
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		const Extensions& extensions = openGLRenderer.getExtensions();
		if (extensions.isGL_ARB_tessellation_shader() && extensions.isGL_ARB_gl_spirv())
		{
			return new TessellationEvaluationShaderSeparate(openGLRenderer, shaderBytecode);
		}
		else
		{
			// Error! There's no tessellation support or no decent shader bytecode support!
			return nullptr;
		}
	}

	Renderer::ITessellationEvaluationShader* ShaderLanguageSeparate::createTessellationEvaluationShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode* shaderBytecode)
	{
		// Check whether or not there's tessellation support
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		const Extensions& extensions = openGLRenderer.getExtensions();
		if (extensions.isGL_ARB_tessellation_shader())
		{
			return new TessellationEvaluationShaderSeparate(openGLRenderer, shaderSourceCode.sourceCode, extensions.isGL_ARB_gl_spirv() ? shaderBytecode : nullptr);
		}
		else
		{
			// Error! There's no tessellation support!
			return nullptr;
		}
	}

	Renderer::IGeometryShader* ShaderLanguageSeparate::createGeometryShaderFromBytecode(const Renderer::ShaderBytecode& shaderBytecode, Renderer::GsInputPrimitiveTopology gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices)
	{
		// Check whether or not there's geometry shader support
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		const Extensions& extensions = openGLRenderer.getExtensions();
		if (extensions.isGL_ARB_geometry_shader4() && extensions.isGL_ARB_gl_spirv())
		{
			// In modern GLSL, "geometry shader input primitive topology" & "geometry shader output primitive topology" & "number of output vertices" can be directly set within GLSL by writing e.g.
			//   "layout(triangles) in;"
			//   "layout(triangle_strip, max_vertices = 3) out;"
			// -> To be able to support older GLSL versions, we have to provide this information also via OpenGL API functions
			return new GeometryShaderSeparate(openGLRenderer, shaderBytecode, gsInputPrimitiveTopology, gsOutputPrimitiveTopology, numberOfOutputVertices);
		}
		else
		{
			// Error! There's no geometry shader support or no decent shader bytecode support!
			return nullptr;
		}
	}

	Renderer::IGeometryShader* ShaderLanguageSeparate::createGeometryShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::GsInputPrimitiveTopology gsInputPrimitiveTopology, Renderer::GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices, Renderer::ShaderBytecode* shaderBytecode)
	{
		// Check whether or not there's geometry shader support
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		const Extensions& extensions = openGLRenderer.getExtensions();
		if (extensions.isGL_ARB_geometry_shader4())
		{
			// In modern GLSL, "geometry shader input primitive topology" & "geometry shader output primitive topology" & "number of output vertices" can be directly set within GLSL by writing e.g.
			//   "layout(triangles) in;"
			//   "layout(triangle_strip, max_vertices = 3) out;"
			// -> To be able to support older GLSL versions, we have to provide this information also via OpenGL API functions
			return new GeometryShaderSeparate(openGLRenderer, shaderSourceCode.sourceCode, gsInputPrimitiveTopology, gsOutputPrimitiveTopology, numberOfOutputVertices, extensions.isGL_ARB_gl_spirv() ? shaderBytecode : nullptr);
		}
		else
		{
			// Error! There's no geometry shader support!
			return nullptr;
		}
	}

	Renderer::IFragmentShader* ShaderLanguageSeparate::createFragmentShaderFromBytecode(const Renderer::ShaderBytecode& shaderBytecode)
	{
		// Check whether or not there's fragment shader support
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		const Extensions& extensions = openGLRenderer.getExtensions();
		if (extensions.isGL_ARB_fragment_shader() && extensions.isGL_ARB_gl_spirv())
		{
			return new FragmentShaderSeparate(openGLRenderer, shaderBytecode);
		}
		else
		{
			// Error! There's no fragment shader support or no decent shader bytecode support!
			return nullptr;
		}
	}

	Renderer::IFragmentShader* ShaderLanguageSeparate::createFragmentShaderFromSourceCode(const Renderer::ShaderSourceCode& shaderSourceCode, Renderer::ShaderBytecode* shaderBytecode)
	{
		// Check whether or not there's fragment shader support
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());
		const Extensions& extensions = openGLRenderer.getExtensions();
		if (extensions.isGL_ARB_fragment_shader())
		{
			return new FragmentShaderSeparate(openGLRenderer, shaderSourceCode.sourceCode, extensions.isGL_ARB_gl_spirv() ? shaderBytecode : nullptr);
		}
		else
		{
			// Error! There's no fragment shader support!
			return nullptr;
		}
	}

	Renderer::IProgram* ShaderLanguageSeparate::createProgram(const Renderer::IRootSignature& rootSignature, const Renderer::VertexAttributes&, Renderer::IVertexShader* vertexShader, Renderer::ITessellationControlShader* tessellationControlShader, Renderer::ITessellationEvaluationShader* tessellationEvaluationShader, Renderer::IGeometryShader* geometryShader, Renderer::IFragmentShader* fragmentShader)
	{
		OpenGLRenderer& openGLRenderer = static_cast<OpenGLRenderer&>(getRenderer());

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

		// Is "GL_EXT_direct_state_access" there?
		else if (openGLRenderer.getExtensions().isGL_EXT_direct_state_access() || openGLRenderer.getExtensions().isGL_ARB_direct_state_access())
		{
			// Effective direct state access (DSA)
			return new ProgramSeparateDsa(openGLRenderer, rootSignature, static_cast<VertexShaderSeparate*>(vertexShader), static_cast<TessellationControlShaderSeparate*>(tessellationControlShader), static_cast<TessellationEvaluationShaderSeparate*>(tessellationEvaluationShader), static_cast<GeometryShaderSeparate*>(geometryShader), static_cast<FragmentShaderSeparate*>(fragmentShader));
		}
		else
		{
			// Traditional bind version
			return new ProgramSeparate(openGLRenderer, rootSignature, static_cast<VertexShaderSeparate*>(vertexShader), static_cast<TessellationControlShaderSeparate*>(tessellationControlShader), static_cast<TessellationEvaluationShaderSeparate*>(tessellationEvaluationShader), static_cast<GeometryShaderSeparate*>(geometryShader), static_cast<FragmentShaderSeparate*>(fragmentShader));
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
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
