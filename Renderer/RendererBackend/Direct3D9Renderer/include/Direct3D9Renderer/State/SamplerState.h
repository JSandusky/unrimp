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
#include <Renderer/State/ISamplerState.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct IDirect3DDevice9;
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
	*    Direct3D 9 sampler state class
	*/
	class SamplerState : public Renderer::ISamplerState
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] direct3D9Renderer
		*    Owner Direct3D 9 renderer instance
		*  @param[in] samplerState
		*    Sampler state to use
		*/
		SamplerState(Direct3D9Renderer& direct3D9Renderer, const Renderer::SamplerState& samplerState);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~SamplerState() override;

		/**
		*  @brief
		*    Set the Direct3D 9 sampler states
		*
		*  @param[in] sampler
		*    Sampler stage index
		*  @param[in] direct3DDevice9
		*    Direct3D 9 device instance to use
		*/
		void setDirect3D9SamplerStates(uint32_t sampler, IDirect3DDevice9& direct3DDevice9) const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	public:
		virtual void setDebugName(const char* name) override;


	//[-------------------------------------------------------]
	//[ Protected virtual Renderer::RefCount methods          ]
	//[-------------------------------------------------------]
	protected:
		virtual void selfDestruct() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit SamplerState(const SamplerState& source) = delete;
		SamplerState& operator =(const SamplerState& source) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		unsigned long mDirect3D9MagFilterMode;	///< Direct3D 9 magnification filter mode (type DWORD not used in here in order to keep the header slim)
		unsigned long mDirect3D9MinFilterMode;	///< Direct3D 9 minification filter mode (type DWORD not used in here in order to keep the header slim)
		unsigned long mDirect3D9MipFilterMode;	///< Direct3D 9 mipmapping filter mode (type DWORD not used in here in order to keep the header slim)
		unsigned long mDirect3D9AddressModeU;	///< Direct3D 9 u address mode (also known as "S") (type DWORD not used in here in order to keep the header slim)
		unsigned long mDirect3D9AddressModeV;	///< Direct3D 9 v address mode (also known as "T") (type DWORD not used in here in order to keep the header slim)
		unsigned long mDirect3D9AddressModeW;	///< Direct3D 9 w address mode (also known as "R") (type DWORD not used in here in order to keep the header slim)
		unsigned long mDirect3D9MipLODBias;		///< Direct3D 9 mipmap LOD bias (type DWORD not used in here in order to keep the header slim)
		unsigned long mDirect3D9MaxAnisotropy;	///< Direct3D 9 maximum anisotropy (type DWORD not used in here in order to keep the header slim)
		unsigned long mDirect3DBorderColor;		///< Direct3D 9 border color (type DWORD not used in here in order to keep the header slim)
		unsigned long mDirect3DMaxMipLevel;		///< Direct3D 9 maximum mipmap level (type DWORD not used in here in order to keep the header slim)


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer
