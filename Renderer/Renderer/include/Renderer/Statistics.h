/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __RENDERER_STATISTICS_H__
#define __RENDERER_STATISTICS_H__


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
	*    Statistics class
	*
	*  @note
	*    - The data is public by intent in order to make it easier to use this class,
	*      no issues involved because the user only gets a constant instance
	*/
	class Statistics
	{


	//[-------------------------------------------------------]
	//[ Public data                                           ]
	//[-------------------------------------------------------]
	public:
		//[-------------------------------------------------------]
		//[ Resources                                             ]
		//[-------------------------------------------------------]
		unsigned int currentNumberOfPrograms;						/**< Current number of program instances */
		unsigned int numberOfCreatedPrograms;						/**< Number of created program instances */
		unsigned int currentNumberOfVertexArrays;					/**< Current number of vertex array object (VAO, input-assembler (IA) stage) instances */
		unsigned int numberOfCreatedVertexArrays;					/**< Number of created vertex array object (VAO, input-assembler (IA) stage) instances */
		// IRenderTarget
		unsigned int currentNumberOfSwapChains;						/**< Current number of swap chain instances */
		unsigned int numberOfCreatedSwapChains;						/**< Number of created swap chain instances */
		unsigned int currentNumberOfFramebuffers;					/**< Current number of framebuffer object (FBO) instances */
		unsigned int numberOfCreatedFramebuffers;					/**< Number of created framebuffer object (FBO) instances */
		// IBuffer
		unsigned int currentNumberOfIndexBuffers;					/**< Current number of index buffer object (IBO, input-assembler (IA) stage) instances */
		unsigned int numberOfCreatedIndexBuffers;					/**< Number of created index buffer object (IBO, input-assembler (IA) stage) instances */
		unsigned int currentNumberOfVertexBuffers;					/**< Current number of vertex buffer object (VBO, input-assembler (IA) stage) instances */
		unsigned int numberOfCreatedVertexBuffers;					/**< Number of created vertex buffer object (VBO, input-assembler (IA) stage) instances */
		unsigned int currentNumberOfUniformBuffers;					/**< Current number of uniform buffer object (UBO, "constant buffer" in Direct3D terminology) instances */
		unsigned int numberOfCreatedUniformBuffers;					/**< Number of created uniform buffer object (UBO, "constant buffer" in Direct3D terminology) instances */
		// ITexture
		unsigned int currentNumberOfTextureBuffers;					/**< Current number of texture buffer object (TBO) instances */
		unsigned int numberOfCreatedTextureBuffers;					/**< Number of created texture buffer object (TBO) instances */
		unsigned int currentNumberOfTexture2Ds;						/**< Current number of texture 2D instances */
		unsigned int numberOfCreatedTexture2Ds;						/**< Number of created texture 2D instances */
		unsigned int currentNumberOfTexture2DArrays;				/**< Current number of texture 2D array instances */
		unsigned int numberOfCreatedTexture2DArrays;				/**< Number of created texture 2D array instances */
		// IState
		unsigned int currentNumberOfRasterizerStates;				/**< Current number of rasterizer state (rasterizer stage (RS)) instances */
		unsigned int numberOfCreatedRasterizerStates;				/**< Number of created rasterizer state (rasterizer stage (RS)) instances */
		unsigned int currentNumberOfDepthStencilStates;				/**< Current number of depth stencil state (output-merger (OM) stage) instances */
		unsigned int numberOfCreatedDepthStencilStates;				/**< Number of created depth stencil state (output-merger (OM) stage) instances */
		unsigned int currentNumberOfBlendStates;					/**< Current number of blend state (output-merger (OM) stage) instances */
		unsigned int numberOfCreatedBlendStates;					/**< Number of created blend state (output-merger (OM) stage) instances */
		unsigned int currentNumberOfSamplerStates;					/**< Current number of sampler state instances */
		unsigned int numberOfCreatedSamplerStates;					/**< Number of created sampler state instances */
		// IShader
		unsigned int currentNumberOfVertexShaders;					/**< Current number of vertex shader (VS) instances */
		unsigned int numberOfCreatedVertexShaders;					/**< Number of created vertex shader (VS) instances */
		unsigned int currentNumberOfTessellationControlShaders;		/**< Current number of tessellation control shader (TCS, "hull shader" in Direct3D terminology) instances */
		unsigned int numberOfCreatedTessellationControlShaders;		/**< Number of created tessellation control shader (TCS, "hull shader" in Direct3D terminology) instances */
		unsigned int currentNumberOfTessellationEvaluationShaders;	/**< Current number of tessellation evaluation shader (TES, "domain shader" in Direct3D terminology) instances */
		unsigned int numberOfCreatedTessellationEvaluationShaders;	/**< Number of created tessellation evaluation shader (TES, "domain shader" in Direct3D terminology) instances */
		unsigned int currentNumberOfGeometryShaders;				/**< Current number of geometry shader (GS) instances */
		unsigned int numberOfCreatedGeometryShaders;				/**< Number of created geometry shader (GS) instances */
		unsigned int currentNumberOfFragmentShaders;				/**< Current number of gragment shader (FS, "pixel shader" in Direct3D terminology) instances */
		unsigned int numberOfCreatedFragmentShaders;				/**< Number of created gragment shader (FS, "pixel shader" in Direct3D terminology) instances */
		// ICollection
		unsigned int currentNumberOfTextureCollections;				/**< Current number of texture collection instances */
		unsigned int numberOfCreatedTextureCollections;				/**< Number of created texture collection instances */
		unsigned int currentNumberOfSamplerStateCollections;		/**< Current number of sampler state collection instances */
		unsigned int numberOfCreatedSamplerStateCollections;		/**< Number of created sampler state collection instances */


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Default constructor
		*/
		inline Statistics();

		/**
		*  @brief
		*    Destructor
		*/
		inline ~Statistics();

		/**
		*  @brief
		*    Return the number of current resource instances
		*
		*  @return
		*    The number of current resource instances
		*
		*  @note
		*    - Primarily for debugging
		*    - Not for the public "Renderer.h"-interface
		*    - The result is calculated by using the current statistics, do only call this method if you have to
		*/
		inline unsigned int getNumberOfCurrentResources() const;

		/**
		*  @brief
		*    Use debug output to show the current number of resource instances
		*
		*  @note
		*    - Primarily for debugging
		*    - Not for the public "Renderer.h"-interface
		*/
		inline void debugOutputCurrentResouces() const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		inline explicit Statistics(const Statistics &source);

		/**
		*  @brief
		*    Copy operator
		*
		*  @param[in] source
		*    Source to copy from
		*
		*  @return
		*    Reference to this instance
		*/
		inline Statistics &operator =(const Statistics &source);


	};


	//[-------------------------------------------------------]
	//[ Namespace                                             ]
	//[-------------------------------------------------------]
	} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/Statistics.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERER_STATISTICS_H__
