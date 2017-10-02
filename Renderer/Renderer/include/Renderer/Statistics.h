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
#include "Renderer/PlatformTypes.h"

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: 'return': conversion from 'int' to 'std::char_traits<wchar_t>::int_type', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4623)	// warning C4623: 'std::_List_node<_Ty,std::_Default_allocator_traits<_Alloc>::void_pointer>': default constructor was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4626)	// warning C4626: 'std::_Compressed_pair<glslang::pool_allocator<char>,std::_String_val<std::_Simple_types<_Ty>>,false>': assignment operator was implicitly defined as deleted
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '_M_HYBRID_X86_ARM64' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(5027)	// warning C5027: 'std::_Compressed_pair<glslang::pool_allocator<char>,std::_String_val<std::_Simple_types<_Ty>>,false>': move assignment operator was implicitly defined as deleted
	#include <atomic>	// For "std::atomic<>"
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class Context;
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
		std::atomic<uint32_t> currentNumberOfRootSignatures;				///< Current number of root signature instances
		std::atomic<uint32_t> numberOfCreatedRootSignatures;				///< Number of created root signature instances
		std::atomic<uint32_t> currentNumberOfResourceGroups;				///< Current number of resource group instances
		std::atomic<uint32_t> numberOfCreatedResourceGroups;				///< Number of created resource group instances
		std::atomic<uint32_t> currentNumberOfPrograms;						///< Current number of program instances
		std::atomic<uint32_t> numberOfCreatedPrograms;						///< Number of created program instances
		std::atomic<uint32_t> currentNumberOfVertexArrays;					///< Current number of vertex array object (VAO, input-assembler (IA) stage) instances
		std::atomic<uint32_t> numberOfCreatedVertexArrays;					///< Number of created vertex array object (VAO, input-assembler (IA) stage) instances
		std::atomic<uint32_t> currentNumberOfRenderPasses;					///< Current number of render pass instances
		std::atomic<uint32_t> numberOfCreatedRenderPasses;					///< Number of created render pass instances
		// IRenderTarget
		std::atomic<uint32_t> currentNumberOfSwapChains;					///< Current number of swap chain instances
		std::atomic<uint32_t> numberOfCreatedSwapChains;					///< Number of created swap chain instances
		std::atomic<uint32_t> currentNumberOfFramebuffers;					///< Current number of framebuffer object (FBO) instances
		std::atomic<uint32_t> numberOfCreatedFramebuffers;					///< Number of created framebuffer object (FBO) instances
		// IBuffer
		std::atomic<uint32_t> currentNumberOfIndexBuffers;					///< Current number of index buffer object (IBO, input-assembler (IA) stage) instances
		std::atomic<uint32_t> numberOfCreatedIndexBuffers;					///< Number of created index buffer object (IBO, input-assembler (IA) stage) instances
		std::atomic<uint32_t> currentNumberOfVertexBuffers;					///< Current number of vertex buffer object (VBO, input-assembler (IA) stage) instances
		std::atomic<uint32_t> numberOfCreatedVertexBuffers;					///< Number of created vertex buffer object (VBO, input-assembler (IA) stage) instances
		std::atomic<uint32_t> currentNumberOfUniformBuffers;				///< Current number of uniform buffer object (UBO, "constant buffer" in Direct3D terminology) instances
		std::atomic<uint32_t> numberOfCreatedUniformBuffers;				///< Number of created uniform buffer object (UBO, "constant buffer" in Direct3D terminology) instances
		std::atomic<uint32_t> currentNumberOfTextureBuffers;				///< Current number of texture buffer object (TBO) instances
		std::atomic<uint32_t> numberOfCreatedTextureBuffers;				///< Number of created texture buffer object (TBO) instances
		std::atomic<uint32_t> currentNumberOfIndirectBuffers;				///< Current number of indirect buffer object instances
		std::atomic<uint32_t> numberOfCreatedIndirectBuffers;				///< Number of created indirect buffer object instances
		// ITexture
		std::atomic<uint32_t> currentNumberOfTexture1Ds;					///< Current number of texture 1D instances
		std::atomic<uint32_t> numberOfCreatedTexture1Ds;					///< Number of created texture 1D instances
		std::atomic<uint32_t> currentNumberOfTexture2Ds;					///< Current number of texture 2D instances
		std::atomic<uint32_t> numberOfCreatedTexture2Ds;					///< Number of created texture 2D instances
		std::atomic<uint32_t> currentNumberOfTexture2DArrays;				///< Current number of texture 2D array instances
		std::atomic<uint32_t> numberOfCreatedTexture2DArrays;				///< Number of created texture 2D array instances
		std::atomic<uint32_t> currentNumberOfTexture3Ds;					///< Current number of texture 3D instances
		std::atomic<uint32_t> numberOfCreatedTexture3Ds;					///< Number of created texture 3D instances
		std::atomic<uint32_t> currentNumberOfTextureCubes;					///< Current number of texture cube instances
		std::atomic<uint32_t> numberOfCreatedTextureCubes;					///< Number of created texture cube instances
		// IState
		std::atomic<uint32_t> currentNumberOfPipelineStates;				///< Current number of pipeline state (PSO) instances
		std::atomic<uint32_t> numberOfCreatedPipelineStates;				///< Number of created pipeline state (PSO) instances
		std::atomic<uint32_t> currentNumberOfSamplerStates;					///< Current number of sampler state instances
		std::atomic<uint32_t> numberOfCreatedSamplerStates;					///< Number of created sampler state instances
		// IShader
		std::atomic<uint32_t> currentNumberOfVertexShaders;					///< Current number of vertex shader (VS) instances
		std::atomic<uint32_t> numberOfCreatedVertexShaders;					///< Number of created vertex shader (VS) instances
		std::atomic<uint32_t> currentNumberOfTessellationControlShaders;	///< Current number of tessellation control shader (TCS, "hull shader" in Direct3D terminology) instances
		std::atomic<uint32_t> numberOfCreatedTessellationControlShaders;	///< Number of created tessellation control shader (TCS, "hull shader" in Direct3D terminology) instances
		std::atomic<uint32_t> currentNumberOfTessellationEvaluationShaders;	///< Current number of tessellation evaluation shader (TES, "domain shader" in Direct3D terminology) instances
		std::atomic<uint32_t> numberOfCreatedTessellationEvaluationShaders;	///< Number of created tessellation evaluation shader (TES, "domain shader" in Direct3D terminology) instances
		std::atomic<uint32_t> currentNumberOfGeometryShaders;				///< Current number of geometry shader (GS) instances
		std::atomic<uint32_t> numberOfCreatedGeometryShaders;				///< Number of created geometry shader (GS) instances
		std::atomic<uint32_t> currentNumberOfFragmentShaders;				///< Current number of fragment shader (FS, "pixel shader" in Direct3D terminology) instances
		std::atomic<uint32_t> numberOfCreatedFragmentShaders;				///< Number of created fragment shader (FS, "pixel shader" in Direct3D terminology) instances


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
		inline uint32_t getNumberOfCurrentResources() const;

		/**
		*  @brief
		*    Use debug output to show the current number of resource instances
		*
		*  @param[in] context
		*    The renderer context to use
		*
		*  @note
		*    - Primarily for debugging
		*    - Not for the public "Renderer.h"-interface
		*/
		inline void debugOutputCurrentResouces(const Context& context) const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit Statistics(const Statistics& source) = delete;
		Statistics& operator =(const Statistics& source) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/Statistics.inl"
