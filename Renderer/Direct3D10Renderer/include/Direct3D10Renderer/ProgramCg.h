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
#ifndef __DIRECT3D10RENDERER_PROGRAM_CG_H__
#define __DIRECT3D10RENDERER_PROGRAM_CG_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Direct3D10Renderer/Program.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{
	class VertexShaderCg;
	class GeometryShaderCg;
	class FragmentShaderCg;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Cg program class
	*/
	class ProgramCg : public Program
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] direct3D10Renderer
		*    Owner Direct3D 10 renderer instance
		*  @param[in] vertexShaderCg
		*    Vertex shader the program is using, can be a null pointer
		*  @param[in] geometryShaderCg
		*    Geometry shader the program is using, can be a null pointer
		*  @param[in] fragmentShaderCg
		*    Fragment shader the program is using, can be a null pointer
		*
		*  @note
		*    - The program keeps a reference to the provided shaders and releases it when no longer required
		*/
		ProgramCg(Direct3D10Renderer &direct3D10Renderer, VertexShaderCg *vertexShaderCg, GeometryShaderCg *geometryShaderCg, FragmentShaderCg *fragmentShaderCg);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~ProgramCg();

		/**
		*  @brief
		*    Return the Cg vertex shader the program is using
		*
		*  @return
		*    The Cg vertex shader the program is using, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*/
		inline VertexShaderCg *getVertexShaderCg() const;

		/**
		*  @brief
		*    Return the Cg geometry shader the program is using
		*
		*  @return
		*    The Cg geometry shader the program is using, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*/
		inline GeometryShaderCg *getGeometryShaderCg() const;

		/**
		*  @brief
		*    Return the Cg fragment shader the program is using
		*
		*  @return
		*    The Cg fragment shader the program is using, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*/
		inline FragmentShaderCg *getFragmentShaderCg() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IProgram methods             ]
	//[-------------------------------------------------------]
	public:
		virtual Renderer::IVertexArray *createVertexArray(unsigned int numberOfAttributes, const Renderer::VertexArrayAttribute *attributes, Renderer::IIndexBuffer *indexBuffer = nullptr) override;
		virtual int getAttributeLocation(const char *attributeName) override;
		virtual unsigned int getUniformBlockIndex(const char *uniformBlockName, unsigned int defaultIndex) override;
		virtual handle getUniformHandle(const char *uniformName) override;
		virtual void setUniform1f(handle uniformHandle, float value) override;
		virtual void setUniform2fv(handle uniformHandle, const float *value) override;
		virtual void setUniform3fv(handle uniformHandle, const float *value) override;
		virtual void setUniform4fv(handle uniformHandle, const float *value) override;
		virtual void setUniformMatrix3fv(handle uniformHandle, const float *value) override;
		virtual void setUniformMatrix4fv(handle uniformHandle, const float *value) override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		Direct3D10Renderer *mDirect3D10Renderer;	///< Owner Direct3D 10 renderer instance, always valid
		VertexShaderCg	   *mVertexShaderCg;		///< Cg vertex shader the program is using (we keep a reference to it), can be a null pointer
		GeometryShaderCg   *mGeometryShaderCg;		///< Cg geometry shader the program is using (we keep a reference to it), can be a null pointer
		FragmentShaderCg   *mFragmentShaderCg;		///< Cg gragment shader the program is using (we keep a reference to it), can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D10Renderer/ProgramCg.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __DIRECT3D10RENDERER_PROGRAM_CG_H__
