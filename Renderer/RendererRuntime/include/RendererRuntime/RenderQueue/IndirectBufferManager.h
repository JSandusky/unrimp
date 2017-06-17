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
#include "RendererRuntime/Core/Manager.h"

#include <vector>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IBufferManager;
	class IIndirectBuffer;
}
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
	*    Indirect buffer manager
	*/
	class IndirectBufferManager : private Manager
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] rendererRuntime
		*    Renderer runtime instance to use, must stay valid as long as the indirect buffer manager instance exists
		*/
		explicit IndirectBufferManager(const IRendererRuntime& rendererRuntime);

		/**
		*  @brief
		*    Return the renderer runtime instance to use
		*
		*  @return
		*    The renderer runtime instance to use
		*/
		inline const IRendererRuntime& getRendererRuntime() const;

		/**
		*  @brief
		*    Destructor
		*/
		~IndirectBufferManager();

		/**
		*  @brief
		*    Return a indirect buffer
		*
		*  @param[in] maximumNumberOfDrawCalls
		*    Maximum number of draw calls ("Renderer::DrawIndexedInstancedArguments") the indirect buffer must be able to hold
		*
		*  @return
		*    The requested indirect buffer, don't destroy the instance, null pointer in case of a horrible nightmare apocalypse scenario
		*/
		Renderer::IIndirectBuffer* getIndirectBuffer(uint32_t maximumNumberOfDrawCalls);

		/**
		*  @brief
		*    Called post command buffer execution
		*/
		void onPostCommandBufferExecution();


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit IndirectBufferManager(const IndirectBufferManager&) = delete;
		IndirectBufferManager& operator=(const IndirectBufferManager&) = delete;


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		struct IndirectBuffer
		{
			Renderer::IIndirectBuffer* indirectBuffer;
			uint32_t				   numberOfBytes;
			IndirectBuffer(Renderer::IIndirectBuffer* _indirectBuffer, uint32_t _numberOfBytes) :
				indirectBuffer(_indirectBuffer),
				numberOfBytes(_numberOfBytes)
			{}
		};
		typedef std::vector<IndirectBuffer> IndirectBuffers;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		const IRendererRuntime&   mRendererRuntime;
		Renderer::IBufferManager& mBufferManager;
		IndirectBuffers			  mFreeIndirectBuffers;
		IndirectBuffers			  mUsedIndirectBuffers;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/RenderQueue/IndirectBufferManager.inl"
