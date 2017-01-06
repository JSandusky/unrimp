/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
#include "Framework/IApplicationFrontend.h"

#include <Renderer/Public/Renderer.h>


//[-------------------------------------------------------]
//[ Definitions                                           ]
//[-------------------------------------------------------]
#ifdef RENDERER_ONLY_NULL
	#define RENDERER_NO_OPENGL
	#define RENDERER_NO_OPENGLES2
	#define RENDERER_NO_DIRECT3D9
	#define RENDERER_NO_DIRECT3D10
	#define RENDERER_NO_DIRECT3D11
#elif RENDERER_ONLY_OPENGL
	#define RENDERER_NO_NULL
	#define RENDERER_NO_OPENGLES2
	#define RENDERER_NO_DIRECT3D9
	#define RENDERER_NO_DIRECT3D10
	#define RENDERER_NO_DIRECT3D11
#elif RENDERER_ONLY_OPENGLES2
	#define RENDERER_NO_NULL
	#define RENDERER_NO_OPENGL
	#define RENDERER_NO_DIRECT3D9
	#define RENDERER_NO_DIRECT3D10
	#define RENDERER_NO_DIRECT3D11
#elif RENDERER_ONLY_DIRECT3D9
	#define RENDERER_NO_NULL
	#define RENDERER_NO_OPENGL
	#define RENDERER_NO_OPENGLES2
	#define RENDERER_NO_DIRECT3D10
	#define RENDERER_NO_DIRECT3D11
#elif RENDERER_ONLY_DIRECT3D10
	#define RENDERER_NO_NULL
	#define RENDERER_NO_OPENGL
	#define RENDERER_NO_OPENGLES2
	#define RENDERER_NO_DIRECT3D9
	#define RENDERER_NO_DIRECT3D11
#elif RENDERER_ONLY_DIRECT3D11
	#define RENDERER_NO_NULL
	#define RENDERER_NO_OPENGL
	#define RENDERER_NO_OPENGLES2
	#define RENDERER_NO_DIRECT3D9
	#define RENDERER_NO_DIRECT3D10
#endif


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class RendererInstance;
}


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Example base class
*/
class ExampleBase
{


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Destructor
	*/
	virtual ~ExampleBase();

	/**
	*  @brief
	*    Return the renderer instance
	*
	*  @return
	*    The renderer instance, can be a null pointer, do not release the returned instance unless you added an own reference to it
	*/
	inline Renderer::IRenderer *getRenderer() const;

	/**
	*  @brief
	*    Return the main renderer target
	*
	*  @return
	*    The main renderer target instance, can be a null pointer, do not release the returned instance unless you added an own reference to it
	*/
	inline Renderer::IRenderTarget *getMainRenderTarget() const;

	void initialize();
	void deInitialize();
	void draw();

	void setApplicationFrontend(IApplicationFrontend* applicationFrontend);


//[-------------------------------------------------------]
//[ Public virtual ExampleBase methods                    ]
//[-------------------------------------------------------]
public:
	virtual void onInitialization();
	virtual void onDeinitialization();
	virtual void onDraw();


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
protected:
	/**
	*  @brief
	*    Constructor
	*/
	explicit ExampleBase();


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	IApplicationFrontend*		mApplicationFrontend;	///< Renderer instance, can be a null pointer, do not destroy the instance
	Renderer::CommandBuffer		mCommandBuffer;			///< Command buffer


};


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "ExampleBase.inl"
