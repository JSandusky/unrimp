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
#include "RendererRuntime/Resource/IResourceManager.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class IRendererRuntime;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Shader resource manager
	*
	*  @remarks
	*    The shader terminology is somewhat tricky.
	*
	*    Sadly, the graphics API designers of "High Level Shading Language" (HLSL) and "OpenGL Shading Language" (GLSL) decided to talk
	*    of a "shader language". Not program language - although in the first OpenGL shader introduction it was called a
	*    "vertex program". Technically speaking, this would be the correct term. Artificially trying to rename it inside our renderer
	*    abstraction would just introduce further terminology confusion, so we stick to the common naming scheme (vertex shader,
	*    fragment shader etc.).
	*
	*    Common names for the medium abstraction level, our "shader resource" is about, are "material", "effect" or "shader". "Material"
	*    would be ok, but then we would need another name for the high abstraction level. "Effect" (e.g. "FX" file format) is more or
	*    less a known name for this concept, but this name sounds funny and does limit the scope somewhat. It's used for everything,
	*    not just "special" effects as the name might indicate to the reader. "Shader" on the other hand is something nearly each artist
	*    understands. So, whenever we talk about "shader" we mean the medium abstraction level, when talking about the low abstraction
	*    level we explicitly talk about vertex shaders, fragment shaders and so on.
	*
	*    A commonly used name for the high abstraction level is "Material", which is just a set of properties. This is what's used and
	*    tweaked by the artists in a large scale. Meaning there are only a few shaders, but tons of materials.
	*/
	class ShaderResourceManager : private IResourceManager
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class RendererRuntimeImpl;


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceManager methods ]
	//[-------------------------------------------------------]
	public:
		virtual void reloadResourceByAssetId(AssetId assetId) override;
		virtual void update() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit ShaderResourceManager(IRendererRuntime& rendererRuntime);
		virtual ~ShaderResourceManager();
		ShaderResourceManager(const ShaderResourceManager&) = delete;
		ShaderResourceManager& operator=(const ShaderResourceManager&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime& mRendererRuntime;	///< Renderer runtime instance, do not destroy the instance


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
