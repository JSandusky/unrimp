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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline bool TextureResource::isRgbHardwareGammaCorrection() const
	{
		return mRgbHardwareGammaCorrection;
	}

	inline Renderer::ITexturePtr TextureResource::getTexture() const
	{
		return mTexture;
	}

	inline void TextureResource::setTexture(Renderer::ITexture* texture)
	{
		// Sanity check
		assert((LoadingState::LOADED == getLoadingState() || LoadingState::UNLOADED == getLoadingState()) && "Texture resource change while in-flight inside the resource streamer");

		// Set new renderer texture
		if (nullptr != mTexture)
		{
			setLoadingState(LoadingState::UNLOADED);
		}
		mTexture = texture;
		setLoadingState(LoadingState::LOADED);
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	inline TextureResource::TextureResource() :
		mRgbHardwareGammaCorrection(false)
	{
		// Nothing here
	}

	inline TextureResource::~TextureResource()
	{
		// Sanity checks
		assert(nullptr == mTexture.getPointer());
	}

	inline TextureResource& TextureResource::operator=(TextureResource&& textureResource)
	{
		// Call base implementation
		IResource::operator=(std::move(textureResource));

		// Swap data
		std::swap(mRgbHardwareGammaCorrection, textureResource.mRgbHardwareGammaCorrection);
		std::swap(mTexture,					   textureResource.mTexture);

		// Done
		return *this;
	}

	inline void TextureResource::initializeElement(TextureResourceId textureResourceId)
	{
		// Sanity checks
		assert(nullptr == mTexture.getPointer());

		// Call base implementation
		IResource::initializeElement(textureResourceId);
	}

	inline void TextureResource::deinitializeElement()
	{
		// Reset everything
		mTexture = nullptr;

		// Call base implementation
		IResource::deinitializeElement();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
