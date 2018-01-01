/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Renderer/SmartRefCount.h"
#include "Renderer/Buffer/BufferTypes.h"
#include "Renderer/Shader/ShaderTypes.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IBuffer;
	class IProgram;
	class IRenderer;
	class IVertexShader;
	class IUniformBuffer;
	class IRootSignature;
	class IGeometryShader;
	class IFragmentShader;
	struct VertexAttributes;
	class ITessellationControlShader;
	class ITessellationEvaluationShader;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Abstract shader language interface
	*/
	class IShaderLanguage : public RefCount<IShaderLanguage>
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Optimization level
		*/
		enum class OptimizationLevel
		{
			Debug = 0,	///< No optimization and debug features enabled, usually only used for debugging
			None,		///< No optimization, usually only used for debugging
			Low,		///< Low optimization
			Medium,		///< Medium optimization
			High,		///< High optimization
			Ultra		///< Ultra optimization
		};


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~IShaderLanguage() override;

		/**
		*  @brief
		*    Return the owner renderer instance
		*
		*  @return
		*    The owner renderer instance, do not release the returned instance unless you added an own reference to it
		*/
		inline IRenderer& getRenderer() const;

		/**
		*  @brief
		*    Return the optimization level
		*
		*  @return
		*    The optimization level
		*/
		inline OptimizationLevel getOptimizationLevel() const;

		/**
		*  @brief
		*    Set the optimization level
		*
		*  @param[in] optimizationLevel
		*    The optimization level
		*/
		inline void setOptimizationLevel(OptimizationLevel optimizationLevel);

		/**
		*  @brief
		*    Create a program and assigns a vertex and fragment shader to it
		*
		*  @param[in] rootSignature
		*    Root signature
		*  @param[in] vertexAttributes
		*    Vertex attributes ("vertex declaration" in Direct3D 9 terminology, "input layout" in Direct3D 10 & 11 terminology)
		*  @param[in] vertexShader
		*    Vertex shader the program is using, can be a null pointer, vertex shader and program language must match!
		*  @param[in] fragmentShader
		*    Fragment shader the program is using, can be a null pointer, fragment shader and program language must match!
		*
		*  @return
		*    The created program, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - The program keeps a reference to the provided shaders and releases it when no longer required
		*    - It's valid that a program implementation is adding a reference and releasing it again at once
		*      (this means that in the case of not having any more references, a shader might get destroyed when calling this method)
		*    - Comfort method
		*/
		inline IProgram* createProgram(const IRootSignature& rootSignature, const VertexAttributes& vertexAttributes, IVertexShader* vertexShader, IFragmentShader* fragmentShader);

		/**
		*  @brief
		*    Create a program and assigns a vertex, geometry and fragment shader to it
		*
		*  @param[in] rootSignature
		*    Root signature
		*  @param[in] vertexAttributes
		*    Vertex attributes ("vertex declaration" in Direct3D 9 terminology, "input layout" in Direct3D 10 & 11 terminology)
		*  @param[in] vertexShader
		*    Vertex shader the program is using, can be a null pointer, vertex shader and program language must match!
		*  @param[in] geometryShader
		*    Geometry shader the program is using, can be a null pointer, geometry shader and program language must match!
		*  @param[in] fragmentShader
		*    Fragment shader the program is using, can be a null pointer, fragment shader and program language must match!
		*
		*  @return
		*    The created program, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - The program keeps a reference to the provided shaders and releases it when no longer required
		*    - It's valid that a program implementation is adding a reference and releasing it again at once
		*      (this means that in the case of not having any more references, a shader might get destroyed when calling this method)
		*    - Comfort method
		*/
		inline IProgram* createProgram(const IRootSignature& rootSignature, const VertexAttributes& vertexAttributes, IVertexShader* vertexShader, IGeometryShader* geometryShader, IFragmentShader* fragmentShader);

		/**
		*  @brief
		*    Create a program and assigns a vertex, tessellation control, tessellation evaluation and fragment shader to it
		*
		*  @param[in] rootSignature
		*    Root signature
		*  @param[in] vertexAttributes
		*    Vertex attributes ("vertex declaration" in Direct3D 9 terminology, "input layout" in Direct3D 10 & 11 terminology)
		*  @param[in] vertexShader
		*    Vertex shader the program is using, can be a null pointer, vertex shader and program language must match!
		*  @param[in] tessellationControlShader
		*    Tessellation control shader the program is using, can be a null pointer, tessellation control shader and program language must match!
		*  @param[in] tessellationEvaluationShader
		*    Tessellation evaluation shader the program is using, can be a null pointer, tessellation evaluation shader and program language must match!
		*  @param[in] fragmentShader
		*    Fragment shader the program is using, can be a null pointer, fragment shader and program language must match!
		*
		*  @return
		*    The created program, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - The program keeps a reference to the provided shaders and releases it when no longer required
		*    - It's valid that a program implementation is adding a reference and releasing it again at once
		*      (this means that in the case of not having any more references, a shader might get destroyed when calling this method)
		*    - Comfort method
		*/
		inline IProgram* createProgram(const IRootSignature& rootSignature, const VertexAttributes& vertexAttributes, IVertexShader* vertexShader, ITessellationControlShader* tessellationControlShader, ITessellationEvaluationShader* tessellationEvaluationShader, IFragmentShader* fragmentShader);


	//[-------------------------------------------------------]
	//[ Public virtual IShaderLanguage methods                ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Return the name of the shader language
		*
		*  @return
		*    The ASCII name of the shader language (for example "GLSL" or "HLSL"), never a null pointer
		*
		*  @note
		*    - Do not free the memory the returned pointer is pointing to
		*/
		virtual const char* getShaderLanguageName() const = 0;

		/**
		*  @brief
		*    Create a vertex shader from shader bytecode
		*
		*  @param[in] vertexAttributes
		*    Vertex attributes ("vertex declaration" in Direct3D 9 terminology, "input layout" in Direct3D 10 & 11 terminology)
		*  @param[in] shaderBytecode
		*    Shader bytecode
		*
		*  @return
		*    The created vertex shader, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::vertexShader" is "true"
		*    - The data the given pointers are pointing to is internally copied and you have to free your memory if you no longer need it
		*/
		virtual IVertexShader* createVertexShaderFromBytecode(const VertexAttributes& vertexAttributes, const ShaderBytecode& shaderBytecode) = 0;

		/**
		*  @brief
		*    Create a vertex shader from shader source code
		*
		*  @param[in] vertexAttributes
		*    Vertex attributes ("vertex declaration" in Direct3D 9 terminology, "input layout" in Direct3D 10 & 11 terminology)
		*  @param[in] shaderSourceCode
		*    Shader source code
		*  @param[out] shaderBytecode
		*    If not a null pointer, receives the shader bytecode in case the used renderer API supports this feature
		*
		*  @return
		*    The created vertex shader, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @remarks
		*    "profile" is not supported by each shader-API and is in general shader-API dependent. GLSL doesn't have such
		*    profiles, just something named "version" - one has to directly write into the shader. But even when this information
		*    is not used for compiling the GLSL shader, we highly recommend to provide GLSL version information in the form of e.g.
		*    "130" for OpenGL 3.0 shaders ("#version 130").
		*    Please note that the profile is just a hint, if necessary, the implementation is free to choose another profile.
		*    In general, be carefully when explicitly setting a profile.
		*
		*   "entry" is not supported by each shader-API. GLSL doesn't have such an user defined entry point and the main
		*   function must always be "main".
		*
		*   Look out! When working with shaders you have to be prepared that a shader may work on one system, but fails to even
		*   compile on another one. Sadly, even if there are e.g. official GLSL specifications, you can't be sure that every
		*   GPU driver is implementing them in detail. Here are some pitfalls which already produced some headaches...
		*
		*   When using GLSL, don't forget to provide the #version directive! Quote from
		*     "The OpenGL® Shading Language - Language Version: 3.30 - Document Revision: 6 - 11-Mar-2010" Page 14
		*       "Version 1.10 of the language does not require shaders to include this directive, and shaders that do not include
		*        a #version directive will be treated as targeting version 1.10."
		*   It looks like that AMD/ATI drivers ("AMD Catalyst™ 11.3") are NOT using just version 1.10 if there's no #version directive, but a higher
		*   version... so don't trust your GPU driver when your GLSL code, using modern language features, also works for you without
		*   #version directive, because it may not on other systems! OpenGL version and GLSL version can be a bit confusing, so
		*   here's a version table:
		*     GLSL #version    OpenGL version    Some comments
		*     110              2.0
		*     120              2.1
		*     130              3.0               Precision qualifiers added
		*                                        "attribute" deprecated; linkage between a vertex shader and OpenGL for per-vertex data -> use "in" instead
		*                                        "varying"/"centroid varying" deprecated; linkage between a vertex shader and a fragment shader for interpolated data -> use "in"/"out" instead
		*     140              3.1
		*     150              3.2               Almost feature-identical to Direct3D Shader Model 4.0 (Direct3D version 10), geometry shader added
		*     330              3.3               Equivalent to Direct3D Shader Model 4.0 (Direct3D version 10)
		*     400              4.0               Tessellation control ("Hull"-Shader in Direct3D 11) and evaluation ("Domain"-Shader in Direct3D 11) shaders added
		*     410              4.1
		*     420              4.2               Equivalent to Direct3D Shader Model 5.0 (Direct3D version 11)
		*  #version must occur before any other statement in the program as stated within:
		*    "The OpenGL® Shading Language - Language Version: 3.30 - Document Revision: 6 - 11-Mar-2010" Page 15
		*      "The #version directive must occur in a shader before anything else, except for comments and white space."
		*  ... sadly, this time NVIDIA (driver: "266.58 WHQL") is not implementing the specification in detail and while on AMD/ATI drivers ("AMD Catalyst™ 11.3")
		*  you get the error message "error(#105) #version must occur before any other statement in the program" when breaking specification,
		*  NVIDIA just accepts it without any error.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::vertexShader" is "true"
		*    - The data the given pointers are pointing to is internally copied and you have to free your memory if you no longer need it
		*/
		virtual IVertexShader* createVertexShaderFromSourceCode(const VertexAttributes& vertexAttributes, const ShaderSourceCode& shaderSourceCode, ShaderBytecode* shaderBytecode = nullptr) = 0;

		/**
		*  @brief
		*    Create a tessellation control shader ("hull shader" in Direct3D terminology) from shader bytecode
		*
		*  @param[in] shaderBytecode
		*    Shader bytecode
		*
		*  @return
		*    The created tessellation control shader, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::maximumNumberOfPatchVertices" is not 0
		*    - The data the given pointers are pointing to is internally copied and you have to free your memory if you no longer need it
		*/
		virtual ITessellationControlShader* createTessellationControlShaderFromBytecode(const ShaderBytecode& shaderBytecode) = 0;

		/**
		*  @brief
		*    Create a tessellation control shader ("hull shader" in Direct3D terminology) from shader source code
		*
		*  @param[in] shaderSourceCode
		*    Shader source code
		*  @param[out] shaderBytecode
		*    If not a null pointer, receives the shader bytecode in case the used renderer API supports this feature
		*
		*  @return
		*    The created tessellation control shader, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::maximumNumberOfPatchVertices" is not 0
		*    - The data the given pointers are pointing to is internally copied and you have to free your memory if you no longer need it
		*
		*  @see
		*    - "Renderer::IShaderLanguage::createVertexShader()" for more information
		*/
		virtual ITessellationControlShader* createTessellationControlShaderFromSourceCode(const ShaderSourceCode& shaderSourceCode, ShaderBytecode* shaderBytecode = nullptr) = 0;

		/**
		*  @brief
		*    Create a tessellation evaluation shader ("domain shader" in Direct3D terminology) from shader bytecode
		*
		*  @param[in] shaderBytecode
		*    Shader bytecode
		*
		*  @return
		*    The created tessellation evaluation shader, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::maximumNumberOfPatchVertices" is not 0
		*    - The data the given pointers are pointing to is internally copied and you have to free your memory if you no longer need it
		*/
		virtual ITessellationEvaluationShader* createTessellationEvaluationShaderFromBytecode(const ShaderBytecode& shaderBytecode) = 0;

		/**
		*  @brief
		*    Create a tessellation evaluation shader ("domain shader" in Direct3D terminology) from shader source code
		*
		*  @param[in] shaderSourceCode
		*    Shader source code
		*  @param[out] shaderBytecode
		*    If not a null pointer, receives the shader bytecode in case the used renderer API supports this feature
		*
		*  @return
		*    The created tessellation evaluation shader, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::maximumNumberOfPatchVertices" is not 0
		*    - The data the given pointers are pointing to is internally copied and you have to free your memory if you no longer need it
		*
		*  @see
		*    - "Renderer::IShaderLanguage::createVertexShader()" for more information
		*/
		virtual ITessellationEvaluationShader* createTessellationEvaluationShaderFromSourceCode(const ShaderSourceCode& shaderSourceCode, ShaderBytecode* shaderBytecode = nullptr) = 0;

		/**
		*  @brief
		*    Create a geometry shader from shader bytecode
		*
		*  @param[in] shaderBytecode
		*    Shader bytecode
		*  @param[in] gsInputPrimitiveTopology
		*    Geometry shader input primitive topology
		*  @param[in] gsOutputPrimitiveTopology
		*    Geometry shader output primitive topology
		*  @param[in] numberOfOutputVertices
		*    Number of output vertices, maximum is "Renderer::Capabilities::maximumNumberOfGsOutputVertices"
		*
		*  @return
		*    The created geometry shader, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::maximumNumberOfGsOutputVertices" is not 0
		*    - The data the given pointers are pointing to is internally copied and you have to free your memory if you no longer need it
		*    - Please note that not each internal implementation may actually need information like "gsInputPrimitiveTopology", but it's
		*      highly recommended to provide this information anyway to be able to switch the internal implementation (e.g. using
		*      OpenGL instead of Direct3D)
		*/
		virtual IGeometryShader* createGeometryShaderFromBytecode(const ShaderBytecode& shaderBytecode, GsInputPrimitiveTopology gsInputPrimitiveTopology, GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices) = 0;

		/**
		*  @brief
		*    Create a geometry shader from shader source code
		*
		*  @param[in] shaderSourceCode
		*    Shader source code
		*  @param[in] gsInputPrimitiveTopology
		*    Geometry shader input primitive topology
		*  @param[in] gsOutputPrimitiveTopology
		*    Geometry shader output primitive topology
		*  @param[in] numberOfOutputVertices
		*    Number of output vertices, maximum is "Renderer::Capabilities::maximumNumberOfGsOutputVertices"
		*  @param[out] shaderBytecode
		*    If not a null pointer, receives the shader bytecode in case the used renderer API supports this feature
		*
		*  @return
		*    The created geometry shader, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::maximumNumberOfGsOutputVertices" is not 0
		*    - The data the given pointers are pointing to is internally copied and you have to free your memory if you no longer need it
		*    - Please note that not each internal implementation may actually need information like "gsInputPrimitiveTopology", but it's
		*      highly recommended to provide this information anyway to be able to switch the internal implementation (e.g. using
		*      OpenGL instead of Direct3D)
		*
		*  @see
		*    - "Renderer::IShaderLanguage::createVertexShader()" for more information
		*/
		virtual IGeometryShader* createGeometryShaderFromSourceCode(const ShaderSourceCode& shaderSourceCode, GsInputPrimitiveTopology gsInputPrimitiveTopology, GsOutputPrimitiveTopology gsOutputPrimitiveTopology, uint32_t numberOfOutputVertices, ShaderBytecode* shaderBytecode = nullptr) = 0;

		/**
		*  @brief
		*    Create a fragment shader from shader bytecode
		*
		*  @param[in] shaderBytecode
		*    Shader bytecode
		*
		*  @return
		*    The created fragment shader, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::fragmentShader" is "true"
		*    - The data the given pointers are pointing to is internally copied and you have to free your memory if you no longer need it
		*/
		virtual IFragmentShader* createFragmentShaderFromBytecode(const ShaderBytecode& shaderBytecode) = 0;

		/**
		*  @brief
		*    Create a fragment shader from shader source code
		*
		*  @param[in] shaderSourceCode
		*    Shader source code
		*  @param[out] shaderBytecode
		*    If not a null pointer, receives the shader bytecode in case the used renderer API supports this feature
		*
		*  @return
		*    The created fragment shader, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - Only supported if "Renderer::Capabilities::fragmentShader" is "true"
		*    - The data the given pointers are pointing to is internally copied and you have to free your memory if you no longer need it
		*
		*  @see
		*    - "Renderer::IShaderLanguage::createVertexShader()" for more information
		*/
		virtual IFragmentShader* createFragmentShaderFromSourceCode(const ShaderSourceCode& shaderSourceCode, ShaderBytecode* shaderBytecode = nullptr) = 0;

		/**
		*  @brief
		*    Create a program and assigns a vertex, tessellation control, tessellation evaluation, geometry and fragment shader to it
		*
		*  @param[in] rootSignature
		*    Root signature
		*  @param[in] vertexAttributes
		*    Vertex attributes ("vertex declaration" in Direct3D 9 terminology, "input layout" in Direct3D 10 & 11 terminology)
		*  @param[in] vertexShader
		*    Vertex shader the program is using, can be a null pointer, vertex shader and program language must match!
		*  @param[in] tessellationControlShader
		*    Tessellation control shader the program is using, can be a null pointer, tessellation control shader and program language must match!
		*  @param[in] tessellationEvaluationShader
		*    Tessellation evaluation shader the program is using, can be a null pointer, tessellation evaluation shader and program language must match!
		*  @param[in] geometryShader
		*    Geometry shader the program is using, can be a null pointer, geometry shader and program language must match!
		*  @param[in] fragmentShader
		*    Fragment shader the program is using, can be a null pointer, fragment shader and program language must match!
		*
		*  @return
		*    The created program, a null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - The program keeps a reference to the provided shaders and releases it when no longer required,
		*      so it's safe to directly hand over a fresh created resource without releasing it manually
		*    - It's valid that a program implementation is adding a reference and releasing it again at once
		*      (this means that in the case of not having any more references, a shader might get destroyed when calling this method)
		*/
		virtual IProgram* createProgram(const IRootSignature& rootSignature, const VertexAttributes& vertexAttributes, IVertexShader* vertexShader, ITessellationControlShader* tessellationControlShader, ITessellationEvaluationShader* tessellationEvaluationShader, IGeometryShader* geometryShader, IFragmentShader* fragmentShader) = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] renderer
		*    Owner renderer instance
		*/
		inline explicit IShaderLanguage(IRenderer& renderer);

		explicit IShaderLanguage(const IShaderLanguage& source) = delete;
		IShaderLanguage& operator =(const IShaderLanguage& source) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRenderer*		  mRenderer;			///< The owner renderer instance, always valid
		OptimizationLevel mOptimizationLevel;	///< Optimization level


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef SmartRefCount<IShaderLanguage> IShaderLanguagePtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/Shader/IShaderLanguage.inl"
