/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#include "RendererToolkit/RendererToolkitImpl.h"
#include "RendererToolkit/PlatformTypes.h"
#include "RendererToolkit/AssetCompiler/ShaderAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/FontAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/TextureAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/MaterialAssetCompiler.h"
#include "RendererToolkit/AssetCompiler/MeshAssetCompiler.h"


//[-------------------------------------------------------]
//[ Global functions                                      ]
//[-------------------------------------------------------]
// Export the instance creation function
#ifdef RENDERERTOOLKIT_EXPORTS
	#define RENDERERTOOLKIT_API_EXPORT GENERIC_API_EXPORT
#else
	#define RENDERERTOOLKIT_API_EXPORT
#endif
RENDERERTOOLKIT_API_EXPORT RendererToolkit::IRendererToolkit *createRendererToolkitInstance()
{
	return new RendererToolkit::RendererToolkitImpl();
}
#undef RENDERERTOOLKIT_API_EXPORT


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	RendererToolkitImpl::RendererToolkitImpl()
	{
		// Nothing here
	}

	RendererToolkitImpl::~RendererToolkitImpl()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IRendererToolkit methods ]
	//[-------------------------------------------------------]
	IAssetCompiler* RendererToolkitImpl::createShaderAssetCompiler()
	{
		return new ShaderAssetCompiler();
	}

	IAssetCompiler* RendererToolkitImpl::createFontAssetCompiler()
	{
		return new FontAssetCompiler();
	}

	IAssetCompiler* RendererToolkitImpl::createTextureAssetCompiler()
	{
		return new TextureAssetCompiler();
	}

	IAssetCompiler* RendererToolkitImpl::createMaterialAssetCompiler()
	{
		return new MaterialAssetCompiler();
	}

	IAssetCompiler* RendererToolkitImpl::createMeshAssetCompiler()
	{
		return new MeshAssetCompiler();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
