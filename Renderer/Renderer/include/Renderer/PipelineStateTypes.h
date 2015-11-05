/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "Renderer/VertexArrayTypes.h"
#include "Renderer/RasterizerStateTypes.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IProgram;
	class IRootSignature;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Definitions                                           ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Primitive topology type
	*
	*  @note
	*    - These constants directly map to Direct3D 12 constants, do not change them
	*
	*  @see
	*    - "D3D12_PRIMITIVE_TOPOLOGY_TYPE"-documentation for details
	*/
	struct PrimitiveTopologyType
	{
		enum Enum
		{
			UNDEFINED	= 0,	///< The shader has not been initialized with an input primitive type
			POINT		= 1,	///< Interpret the input primitive as a point
			LINE		= 2,	///< Interpret the input primitive as a line
			TRIANGLE	= 3,	///< Interpret the input primitive as a triangle
			PATCH		= 4		///< Interpret the input primitive as a control point patch
		};
	};

	/**
	*  @brief
	*    Pipeline state
	*
	*  @todo
	*    - TODO(co) Under construction
	*/
	struct PipelineState
	{
		IRootSignature*				rootSignature;			///< Root signature (pipeline state instances keep a reference to the program), must be valid
		IProgram*					program;				///< Program used by the pipeline state (pipeline state instances keep a reference to the program), must be valid
		VertexAttributes			vertexAttributes;		///< Vertex attributes
		PrimitiveTopologyType::Enum	primitiveTopologyType;	///< Primitive topology type
		RasterizerState				rasterizerState;		///< Rasterizer state
	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
