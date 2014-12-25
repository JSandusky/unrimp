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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Framework/IApplicationRenderer.h"

#include <RendererRuntime/Public/RendererRuntime.h>


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Renderer runtime application interface
*/
class IApplicationRendererRuntime : public IApplicationRenderer
{


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Destructor
	*/
	virtual ~IApplicationRendererRuntime();

	/**
	*  @brief
	*    Return the renderer runtime instance
	*
	*  @remarks
	*    The renderer runtime instance, can be a null pointer
	*/
	inline RendererRuntime::IRendererRuntime *getRendererRuntime() const;


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
public:
	virtual void onInitialization() override;
	virtual void onDeinitialization() override;


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
protected:
	/**
	*  @brief
	*    Constructor
	*
	*  @param[in] rendererName
	*    Case sensitive ASCII name of the renderer to instance, if null pointer or unknown renderer no renderer will be used.
	*    Example renderer names: "Null", "OpenGL", "OpenGLES2", "Direct3D9", "Direct3D10", "Direct3D11"
	*/
	explicit IApplicationRendererRuntime(const char *rendererName);


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
private:
	/**
	*  @brief
	*    Create a renderer runtime instance
	*
	*  @param[in] renderer
	*    Renderer instance to use
	*
	*  @return
	*    The created renderer runtime instance, null pointer on error
	*
	*  @note
	*    - The renderer runtime keeps a reference to the provided renderer instance
	*/
	RendererRuntime::IRendererRuntime *createRendererRuntimeInstance(Renderer::IRenderer &renderer);


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	void								  *mRendererRuntimeSharedLibrary;	///< Renderer runtime shared library, can be a null pointer
	RendererRuntime::IRendererRuntimePtr   mRendererRuntime;				///< Renderer runtime instance, can be a null pointer


};


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "IApplicationRendererRuntime.inl"
