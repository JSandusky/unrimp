/*********************************************************\
 * Copyright (c) 2012-2013 Christian Ofenberg
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
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Destructor
	*/
	inline IShaderLanguage::~IShaderLanguage()
	{
		// Nothing to do in here
	}

	/**
	*  @brief
	*    Return the owner renderer instance
	*/
	inline IRenderer &IShaderLanguage::getRenderer() const
	{
		return *mRenderer;
	}

	/**
	*  @brief
	*    Create a program and assigns a vertex and fragment shader to it
	*/
	inline IProgram *IShaderLanguage::createProgram(IVertexShader *vertexShader, IFragmentShader *fragmentShader)
	{
		return createProgram(vertexShader, nullptr, nullptr, nullptr, fragmentShader);
	}

	/**
	*  @brief
	*    Create a program and assigns a vertex, geometry and fragment shader to it
	*/
	inline IProgram *IShaderLanguage::createProgram(IVertexShader *vertexShader, IGeometryShader *geometryShader, IFragmentShader *fragmentShader)
	{
		return createProgram(vertexShader, nullptr, nullptr, geometryShader, fragmentShader);
	}

	/**
	*  @brief
	*    Create a program and assigns a vertex, tessellation control, tessellation evaluation and fragment shader to it
	*/
	inline IProgram *IShaderLanguage::createProgram(IVertexShader *vertexShader, ITessellationControlShader *tessellationControlShader, ITessellationEvaluationShader *tessellationEvaluationShader, IFragmentShader *fragmentShader)
	{
		return createProgram(vertexShader, tessellationControlShader, tessellationEvaluationShader, nullptr, fragmentShader);
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Constructor
	*/
	inline IShaderLanguage::IShaderLanguage(IRenderer &renderer) :
		mRenderer(&renderer)
	{
		// Nothing to do in here
	}

	/**
	*  @brief
	*    Copy constructor
	*/
	inline IShaderLanguage::IShaderLanguage(const IShaderLanguage &source) :
		mRenderer(&source.getRenderer())
	{
		// Not supported
	}

	/**
	*  @brief
	*    Copy operator
	*/
	inline IShaderLanguage &IShaderLanguage::operator =(const IShaderLanguage &)
	{
		// Not supported
		return *this;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
