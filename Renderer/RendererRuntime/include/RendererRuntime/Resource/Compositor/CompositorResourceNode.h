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
#include "RendererRuntime/Resource/Compositor/CompositorResourceTarget.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId CompositorResourceNodeId;	///< Compositor resource node identifier, internally just a POD "uint32_t"


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class CompositorResourceNode : protected NonCopyable
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class CompositorInstance;	// TODO(co) Just a first test, remove this
		friend class CompositorResource;	// TODO(co) Just a first test, remove this


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline CompositorResourceNodeId getCompositorResourceNodeId() const;

		inline void setNumberOfInputChannels(uint32_t numberOfInputChannels);
		inline void addInputChannel(CompositorChannelId compositorChannelId);

		inline void setNumberOfCompositorResourceTargets(uint32_t numberOfCompositorResourceTargets);
		inline CompositorResourceTarget& addCompositorResourceTarget(CompositorChannelId compositorChannelId);

		inline void setNumberOfOutputChannels(uint32_t numberOfOutputChannels);
		inline void addOutputChannel(CompositorChannelId compositorChannelId);


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		explicit CompositorResourceNode(CompositorResourceNodeId compositorResourceNodeId);
		virtual ~CompositorResourceNode();
		CompositorResourceNode(const CompositorResourceNode&) = delete;
		CompositorResourceNode& operator=(const CompositorResourceNode&) = delete;


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		typedef std::vector<CompositorChannelId>	  CompositorChannels;	// TODO(co) Get rid of "std::vector" and dynamic memory handling in here? (need to introduce a maximum number of input channels for this)
		typedef std::vector<CompositorResourceTarget> CompositorResourceTargets;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		CompositorResourceNodeId  mCompositorResourceNodeId;
		CompositorChannels		  mInputChannels;
		CompositorResourceTargets mCompositorResourceTargets;
		CompositorChannels		  mOutputChannels;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Compositor/CompositorResourceNode.inl"
