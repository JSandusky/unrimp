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
#include <Renderer/Shader/IProgram.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct IDirect3DDevice9;
struct ID3DXConstantTable;
namespace Direct3D9Renderer
{
	class VertexShaderHlsl;
	class Direct3D9Renderer;
	class FragmentShaderHlsl;
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
	*    HLSL program class
	*/
	class ProgramHlsl : public Renderer::IProgram
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
		*  @param[in] vertexShaderHlsl
		*    Vertex shader the program is using, can be a null pointer
		*  @param[in] fragmentShaderHlsl
		*    Fragment shader the program is using, can be a null pointer
		*
		*  @note
		*    - The program keeps a reference to the provided shaders and releases it when no longer required
		*/
		ProgramHlsl(Direct3D9Renderer &rendererDirect3D9, VertexShaderHlsl *vertexShaderHlsl, FragmentShaderHlsl *fragmentShaderHlsl);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~ProgramHlsl();

		/**
		*  @brief
		*    Return the HLSL vertex shader the program is using
		*
		*  @return
		*    The HLSL vertex shader the program is using, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*/
		inline VertexShaderHlsl *getVertexShaderHlsl() const;

		/**
		*  @brief
		*    Return the HLSL fragment shader the program is using
		*
		*  @return
		*    The HLSL fragment shader the program is using, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*/
		inline FragmentShaderHlsl *getFragmentShaderHlsl() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	public:
		virtual void setDebugName(const char *name) override;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IProgram methods             ]
	//[-------------------------------------------------------]
	public:
		virtual handle getUniformHandle(const char *uniformName) override;
		virtual void setUniform1f(handle uniformHandle, float value) override;
		virtual void setUniform2fv(handle uniformHandle, const float *value) override;
		virtual void setUniform3fv(handle uniformHandle, const float *value) override;
		virtual void setUniform4fv(handle uniformHandle, const float *value) override;
		virtual void setUniformMatrix3fv(handle uniformHandle, const float *value) override;
		virtual void setUniformMatrix4fv(handle uniformHandle, const float *value) override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit ProgramHlsl(const ProgramHlsl& source) = delete;
		ProgramHlsl& operator =(const ProgramHlsl& source) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		Direct3D9Renderer  *mDirect3D9Renderer;		///< Owner Direct3D 9 renderer instance, always valid
		VertexShaderHlsl   *mVertexShaderHlsl;		///< Vertex shader the program is using (we keep a reference to it), can be a null pointer
		FragmentShaderHlsl *mFragmentShaderHlsl;	///< Fragment shader the program is using (we keep a reference to it), can be a null pointer
		IDirect3DDevice9   *mDirect3DDevice9;		///< The Direct3D 9 device instance (we keep a reference to it), can be a null pointer
		ID3DXConstantTable *mD3DXConstantTable;		/**< The Direct3D 9 constant table instance (we keep a reference to it), null pointer on horrible error (so we don't check).
														 I noticed that as soon as working with "D3DXHANDLE", we no longer need to make a difference between vertex/pixel shaders.
														 I was unable to find this behaviour within the documentation, but it simplifies the implementation so I exploid it in here. */


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D9Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D9Renderer/Shader/ProgramHlsl.inl"
