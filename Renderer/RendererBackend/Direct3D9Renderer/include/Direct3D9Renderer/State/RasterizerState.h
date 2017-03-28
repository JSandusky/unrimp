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
#include "Direct3D9Renderer/State/IState.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct IDirect3DDevice9;
namespace Renderer
{
	struct RasterizerState;
}
namespace Direct3D9Renderer
{
	class Direct3D9Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D9Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 9 rasterizer state class
	*/
	class RasterizerState : public IState
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] rasterizerState
		*    Rasterizer state to use
		*/
		RasterizerState(const Renderer::RasterizerState &rasterizerState);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~RasterizerState();

		/**
		*  @brief
		*    Set the Direct3D 9 rasterizer states
		*
		*  @param[in] direct3DDevice9
		*    Direct3D 9 device instance to use
		*/
		void setDirect3D9RasterizerStates(IDirect3DDevice9 &direct3DDevice9) const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		unsigned long mDirect3DFillMode;				///< Direct3D 9 fill mode (type DWORD not used in here in order to keep the header slim)
		unsigned long mDirect3DCullMode;				///< Direct3D 9 cull mode (type DWORD not used in here in order to keep the header slim)
		unsigned long mDirect3DDepthBias;				///< Direct3D 9 depth bias (type DWORD not used in here in order to keep the header slim)
		unsigned long mDirect3DSlopeScaledDepthBias;	///< Direct3D 9 slope scaled depth bias (type DWORD not used in here in order to keep the header slim)
		unsigned long mDirect3DScissorEnable;			///< Direct3D 9 scissor enable (type DWORD not used in here in order to keep the header slim)
		unsigned long mDirect3DMultisampleEnable;		///< Direct3D 9 multisample enable (type DWORD not used in here in order to keep the header slim)
		unsigned long mDirect3DAntialiasedLineEnable;	///< Direct3D 9 antialiased line enable (type DWORD not used in here in order to keep the header slim)


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
