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
#include "RendererRuntime/Export.h"
#include "RendererRuntime/Core/StringId.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId AssetId;					///< Asset identifier, internally just a POD "uint32_t", string ID scheme is "<project name>/<asset type>/<asset category>/<asset name>"
	typedef uint32_t FramebufferSignatureId;	///< Framebuffer signature identifier, result of hashing framebuffer properties


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Framebuffer signature
	*/
	class FramebufferSignature
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Default constructor
		*/
		inline FramebufferSignature();

		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] numberOfColorTextures
		*    Number of color textures [0, 7]
		*  @param[in] colorTextureAssetIds
		*    Color texture asset IDs
		*  @param[in] depthStencilTextureAssetId
		*    Depth stencil texture assetID
		*/
		RENDERERRUNTIME_API_EXPORT FramebufferSignature(uint8_t numberOfColorTextures, AssetId colorTextureAssetIds[8], AssetId depthStencilTextureAssetId);

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] framebufferSignature
		*    Framebuffer signature to copy from
		*/
		inline explicit FramebufferSignature(const FramebufferSignature& framebufferSignature);

		/**
		*  @brief
		*    Destructor
		*/
		inline ~FramebufferSignature();

		/**
		*  @brief
		*    Copy operator
		*/
		inline FramebufferSignature& operator=(const FramebufferSignature& framebufferSignature);

		//[-------------------------------------------------------]
		//[ Getter for input data                                 ]
		//[-------------------------------------------------------]
		inline uint8_t getNumberOfColorTextures() const;
		inline AssetId getColorTextureAssetId(uint8_t index) const;
		inline AssetId getDepthStencilTextureAssetId() const;

		//[-------------------------------------------------------]
		//[ Getter for derived data                               ]
		//[-------------------------------------------------------]
		inline FramebufferSignatureId getFramebufferSignatureId() const;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		// Input data
		uint8_t mNumberOfColorTextures;
		AssetId mColorTextureAssetIds[8];
		AssetId mDepthStencilTextureAssetId;
		// Derived data
		FramebufferSignatureId mFramebufferSignatureId;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Core/Renderer/FramebufferSignature.inl"
