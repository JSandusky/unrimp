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


// Public comfort header putting everything within a single header


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once
#ifndef __RENDERERTOOLKIT_H__
#define __RENDERERTOOLKIT_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/Public/Renderer.h>

#include <iosfwd>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererToolkit
{
	class IAssetCompiler;
	class IRendererToolkit;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Types                                                 ]
	//[-------------------------------------------------------]
	// RendererToolkit/IRendererToolkit.h
	class IRendererToolkit : public Renderer::RefCount<IRendererToolkit>
	{
	public:
		virtual ~IRendererToolkit();
	public:
		virtual IAssetCompiler* createFontAssetCompiler() = 0;
		virtual IAssetCompiler* createTextureAssetCompiler() = 0;
		virtual IAssetCompiler* createMaterialAssetCompiler() = 0;
		virtual IAssetCompiler* createMeshAssetCompiler() = 0;
	protected:
		IRendererToolkit();
		explicit IRendererToolkit(const IRendererToolkit &source);
		IRendererToolkit &operator =(const IRendererToolkit &source);
	};
	typedef Renderer::SmartRefCount<IRendererToolkit> IRendererToolkitPtr;

	// RendererToolkit/AssetCompiler/IAssetCompiler.h
	class IAssetCompiler : public Renderer::RefCount<IAssetCompiler>
	{
	public:
		~IAssetCompiler() {};
	public:
		virtual bool compile(std::istream& istream, std::ostream& ostream, std::istream& jsonConfiguration) = 0;
	protected:
		IAssetCompiler() {};
		explicit IAssetCompiler(const IAssetCompiler &source);
		IAssetCompiler &operator =(const IAssetCompiler &source);
	};
	typedef Renderer::SmartRefCount<IAssetCompiler> IAssetCompilerPtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERERTOOLKIT_H__
