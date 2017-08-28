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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/Shader/IProgram.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IRootSignature;
	struct VertexAttributes;
}
namespace VulkanRenderer
{
	class VulkanRenderer;
	class VertexShaderGlsl;
	class GeometryShaderGlsl;
	class FragmentShaderGlsl;
	class TessellationControlShaderGlsl;
	class TessellationEvaluationShaderGlsl;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    GLSL program class
	*/
	class ProgramGlsl : public Renderer::IProgram
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] vulkanRenderer
		*    Owner Vulkan renderer instance
		*  @param[in] rootSignature
		*    Root signature
		*  @param[in] vertexAttributes
		*    Vertex attributes ("vertex declaration" in Direct3D 9 terminology, "input layout" in Direct3D 10 & 11 terminology)
		*  @param[in] vertexShaderGlsl
		*    Vertex shader the program is using, can be a null pointer
		*  @param[in] tessellationControlShaderGlsl
		*    Tessellation control shader the program is using, can be a null pointer
		*  @param[in] tessellationEvaluationShaderGlsl
		*    Tessellation evaluation shader the program is using, can be a null pointer
		*  @param[in] geometryShaderGlsl
		*    Geometry shader the program is using, can be a null pointer
		*  @param[in] fragmentShaderGlsl
		*    Fragment shader the program is using, can be a null pointer
		*
		*  @note
		*    - The program keeps a reference to the provided shaders and releases it when no longer required
		*/
		ProgramGlsl(VulkanRenderer& vulkanRenderer, const Renderer::IRootSignature& rootSignature, const Renderer::VertexAttributes& vertexAttributes, VertexShaderGlsl *vertexShaderGlsl, TessellationControlShaderGlsl *tessellationControlShaderGlsl, TessellationEvaluationShaderGlsl *tessellationEvaluationShaderGlsl, GeometryShaderGlsl *geometryShaderGlsl, FragmentShaderGlsl *fragmentShaderGlsl);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~ProgramGlsl() override;

		/**
		*  @brief
		*    Return the GLSL vertex shader the program is using
		*
		*  @return
		*    The GLSL vertex shader the program is using, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*/
		inline VertexShaderGlsl* getVertexShaderGlsl() const;

		/**
		*  @brief
		*    Return the GLSL tessellation control shader the program is using
		*
		*  @return
		*    The GLSL tessellation control shader the program is using, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*/
		inline TessellationControlShaderGlsl* getTessellationControlShaderGlsl() const;

		/**
		*  @brief
		*    Return the GLSL tessellation evaluation shader the program is using
		*
		*  @return
		*    The GLSL tessellation evaluation shader the program is using, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*/
		inline TessellationEvaluationShaderGlsl* getTessellationEvaluationShaderGlsl() const;

		/**
		*  @brief
		*    Return the GLSL geometry shader the program is using
		*
		*  @return
		*    The GLSL geometry shader the program is using, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*/
		inline GeometryShaderGlsl* getGeometryShaderGlsl() const;

		/**
		*  @brief
		*    Return the GLSL fragment shader the program is using
		*
		*  @return
		*    The GLSL fragment shader the program is using, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*/
		inline FragmentShaderGlsl* getFragmentShaderGlsl() const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit ProgramGlsl(const ProgramGlsl& source) = delete;
		ProgramGlsl& operator =(const ProgramGlsl& source) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		VertexShaderGlsl*				  mVertexShaderGlsl;					///< Vertex shader the program is using (we keep a reference to it), can be a null pointer
		TessellationControlShaderGlsl*	  mTessellationControlShaderGlsl;		///< Tessellation control shader the program is using (we keep a reference to it), can be a null pointer
		TessellationEvaluationShaderGlsl* mTessellationEvaluationShaderGlsl;	///< Tessellation evaluation shader the program is using (we keep a reference to it), can be a null pointer
		GeometryShaderGlsl*				  mGeometryShaderGlsl;					///< Geometry shader the program is using (we keep a reference to it), can be a null pointer
		FragmentShaderGlsl*				  mFragmentShaderGlsl;					///< Fragment shader the program is using (we keep a reference to it), can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "VulkanRenderer/Shader/ProgramGlsl.inl"
