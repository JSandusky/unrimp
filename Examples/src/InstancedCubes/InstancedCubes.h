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
#ifndef __INSTANCEDCUBES_H__
#define __INSTANCEDCUBES_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Framework/Stopwatch.h"
#include "Framework/IApplicationRendererToolkit.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
class ICubeRenderer;


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Cube instancing application class
*
*  @remarks
*    Demonstrates:
*    - Vertex buffer object (VBO)
*    - Vertex array object (VAO)
*    - Index buffer object (IBO)
*    - Uniform buffer object (UBO)
*    - Texture buffer object (TBO)
*    - 2D texture
*    - 2D texture array
*    - Sampler state object
*    - Depth stencil state object
*    - Blend state object
*    - Vertex shader (VS) and fragment shader (FS)
*    - Instanced arrays (shader model 3 feature, vertex array element advancing per-instance instead of per-vertex)
*    - Draw instanced (shader model 4 feature, build in shader variable holding the current instance ID)
*    - Renderer toolkit fonts
*/
class InstancedCubes : public IApplicationRendererToolkit
{


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
public:
	/**
	*  @brief
	*    Constructor
	*
	*  @param[in] rendererName
	*    Case sensitive ASCII name of the renderer to instance, if null pointer or unknown renderer no renderer will be used.
	*    Example renderer names: "Null", "OpenGL", "Direct3D9", "Direct3D10", "Direct3D11"
	*/
	explicit InstancedCubes(const char *rendererName);

	/**
	*  @brief
	*    Destructor
	*/
	virtual ~InstancedCubes();


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
public:
	virtual void onInitialization() override;
	virtual void onDeinitialization() override;
	virtual void onKeyDown(unsigned int key) override;
	virtual void onUpdate() override;
	virtual void onDraw() override;


//[-------------------------------------------------------]
//[ Private static data                                   ]
//[-------------------------------------------------------]
private:
	static const unsigned int NUMBER_OF_CHANGED_CUBES = 10000;	///< Number of changed cubes on key interaction
	static const unsigned int NUMBER_OF_TEXTURES	  = 8;		///< Number of textures
	static const unsigned int SCENE_RADIUS			  = 10;		///< Scene radius


//[-------------------------------------------------------]
//[ Private data                                          ]
//[-------------------------------------------------------]
private:
	RendererToolkit::IFontPtr  mFont;					///< Font, can be a null pointer
	ICubeRenderer			  *mCubeRenderer;			///< Cube renderer instance, can be a null pointer
	unsigned int			   mNumberOfCubeInstances;	///< Number of cube instances
	// The rest is for timing and statistics
	Stopwatch mStopwatch;			///< Stopwatch instance
	float	  mGlobalTimer;			///< Global timer
	float	  mGlobalScale;			///< Global scale
	bool	  mDisplayStatistics;	///< Display statistics?
	float	  mFPSUpdateTimer;		///< Timer for FPS update
	int		  mFramesSinceCheck;	///< Number of frames since last FPS update
	float	  mFramesPerSecond;		///< Current frames per second


};


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __INSTANCEDCUBES_H__
