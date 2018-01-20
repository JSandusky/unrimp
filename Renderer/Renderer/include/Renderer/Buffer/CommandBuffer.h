/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
#include "Renderer/IRenderer.h"
#include "Renderer/Buffer/IndirectBufferTypes.h"

#include <cassert>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IRenderer;
	class IResource;
	class IVertexArray;
	class IFramebuffer;
	class IRenderTarget;
	class IUniformBuffer;
	class ITextureBuffer;
	class IRootSignature;
	class IPipelineState;
	class IIndirectBuffer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	enum CommandDispatchFunctionIndex : uint8_t
	{
		// Command buffer
		ExecuteCommandBuffer = 0,
		// Graphics root
		SetGraphicsRootSignature,
		SetGraphicsResourceGroup,
		// States
		SetPipelineState,
		// Input-assembler (IA) stage
		SetVertexArray,
		// Rasterizer (RS) stage
		SetViewports,
		SetScissorRectangles,
		// Output-merger (OM) stage
		SetRenderTarget,
		// Operations
		Clear,
		ResolveMultisampleFramebuffer,
		CopyResource,
		// Draw call
		Draw,
		DrawIndexed,
		// Debug
		SetDebugMarker,
		BeginDebugEvent,
		EndDebugEvent,
		// Done
		NumberOfFunctions
	};

	typedef void (*BackendDispatchFunction)(const void*, IRenderer& renderer);
	typedef void* CommandPacket;


	//[-------------------------------------------------------]
	//[ Global functions                                      ]
	//[-------------------------------------------------------]
	namespace CommandPacketHelper
	{
		static const uint32_t OFFSET_NEXT_COMMAND_PACKET_BYTE_INDEX	= 0u;
		static const uint32_t OFFSET_BACKEND_DISPATCH_FUNCTION		= OFFSET_NEXT_COMMAND_PACKET_BYTE_INDEX + sizeof(uint32_t);
		static const uint32_t OFFSET_COMMAND						= OFFSET_BACKEND_DISPATCH_FUNCTION + sizeof(uint32_t);	// Don't use "sizeof(CommandDispatchFunctionIndex)" instead of "sizeof(uint32_t)" so we have a known alignment

		template <typename T>
		uint32_t getNumberOfBytes(uint32_t numberOfAuxiliaryBytes)
		{
			return OFFSET_COMMAND + sizeof(T) + numberOfAuxiliaryBytes;
		};

		inline uint32_t getNextCommandPacketByteIndex(const CommandPacket commandPacket)
		{
			return *reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(commandPacket) + OFFSET_NEXT_COMMAND_PACKET_BYTE_INDEX);
		}

		inline void storeNextCommandPacketByteIndex(const CommandPacket commandPacket, uint32_t nextPacketByteIndex)
		{
			*reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(commandPacket) + OFFSET_NEXT_COMMAND_PACKET_BYTE_INDEX) = nextPacketByteIndex;
		}

		inline CommandDispatchFunctionIndex* getCommandDispatchFunctionIndex(const CommandPacket commandPacket)
		{
			return reinterpret_cast<CommandDispatchFunctionIndex*>(reinterpret_cast<uint8_t*>(commandPacket) + OFFSET_BACKEND_DISPATCH_FUNCTION);
		}

		inline void storeBackendDispatchFunctionIndex(const CommandPacket commandPacket, CommandDispatchFunctionIndex commandDispatchFunctionIndex)
		{
			*getCommandDispatchFunctionIndex(commandPacket) = commandDispatchFunctionIndex;
		}

		inline CommandDispatchFunctionIndex loadCommandDispatchFunctionIndex(const CommandPacket commandPacket)
		{
			return *getCommandDispatchFunctionIndex(commandPacket);
		}

		template <typename T>
		T* getCommand(const CommandPacket commandPacket)
		{
			return reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(commandPacket) + OFFSET_COMMAND);
		}

		inline const void* loadCommand(const CommandPacket commandPacket)
		{
			return reinterpret_cast<uint8_t*>(commandPacket) + OFFSET_COMMAND;
		}

		/**
		*  @brief
		*    Return auxiliary memory address of the given command; returned memory address is considered unstable and might change as soon as another command is added
		*/
		template <typename T>
		uint8_t* getAuxiliaryMemory(T* command)
		{
			return reinterpret_cast<uint8_t*>(command) + sizeof(T);
		}

		/**
		*  @brief
		*    Return auxiliary memory address of the given command; returned memory address is considered unstable and might change as soon as another command is added
		*/
		template <typename T>
		const uint8_t* getAuxiliaryMemory(const T* command)
		{
			return reinterpret_cast<const uint8_t*>(command) + sizeof(T);
		}

	};


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Command buffer
	*
	*  @remarks
	*    Basing on
	*    - http://molecularmusings.wordpress.com/2014/11/06/stateless-layered-multi-threaded-rendering-part-1/ - "Stateless, layered, multi-threaded rendering � Part 1"
	*    - http://molecularmusings.wordpress.com/2014/11/13/stateless-layered-multi-threaded-rendering-part-2-stateless-api-design/ - "Stateless, layered, multi-threaded rendering � Part 2"
	*    - http://molecularmusings.wordpress.com/2014/12/16/stateless-layered-multi-threaded-rendering-part-3-api-design-details/ - "Stateless, layered, multi-threaded rendering � Part 3"
	*    - http://realtimecollisiondetection.net/blog/?p=86 - "Order your graphics draw calls around!"
	*    but without a key inside the more general command buffer. Sorting is a job of a more high level construct like a render queue which also automatically will perform
	*    batching and instancing. Also the memory management is much simplified to be cache friendly.
	*
	*  @note
	*    - The commands are stored as a flat contiguous array to be cache friendly
	*    - Each command can have an additional auxiliary buffer, e.g. to store uniform buffer data to submit to the renderer
	*    - It's valid to record a command buffer only once, and submit it multiple times to the renderer
	*/
	class CommandBuffer final
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Default constructor
		*/
		inline CommandBuffer() :
			mCommandPacketBufferNumberOfBytes(0),
			mCommandPacketBuffer(nullptr),
			mPreviousCommandPacketByteIndex(~0u),
			mCurrentCommandPacketByteIndex(0)
			#ifndef RENDERER_NO_STATISTICS
				, mNumberOfCommands(0)
			#endif
		{
			// Nothing here
		}

		/**
		*  @brief
		*    Destructor
		*/
		inline ~CommandBuffer()
		{
			delete [] mCommandPacketBuffer;
		}

		/**
		*  @brief
		*    Return whether or not the command buffer is empty
		*
		*  @return
		*    "true" if the command buffer is empty, else "false"
		*/
		inline bool isEmpty() const
		{
			return (~0u == mPreviousCommandPacketByteIndex);
		}

		#ifndef RENDERER_NO_STATISTICS
			/**
			*  @brief
			*    Return the number of commands inside the command buffer
			*
			*  @return
			*    The number of commands inside the command buffer
			*
			*  @note
			*    - Counting the number of commands inside the command buffer is only a debugging feature and not used in optimized builds
			*/
			inline uint32_t getNumberOfCommands() const
			{
				return mNumberOfCommands;
			}
		#endif

		/**
		*  @brief
		*    Clear the command buffer
		*/
		inline void clear()
		{
			mPreviousCommandPacketByteIndex = ~0u;
			mCurrentCommandPacketByteIndex = 0;
			#ifndef RENDERER_NO_STATISTICS
				mNumberOfCommands = 0;
			#endif
		}

		/**
		*  @brief
		*    Add command
		*
		*  @param[in] numberOfAuxiliaryBytes
		*    Optional number of auxiliary bytes, e.g. to store uniform buffer data to submit to the renderer
		*
		*  @return
		*    Pointer to the added command, only null pointer in case of apocalypse, don't destroy the memory
		*/
		template <typename U>
		U* addCommand(uint32_t numberOfAuxiliaryBytes = 0)
		{
			// How many command package buffer bytes are consumed by the command to add?
			const uint32_t numberOfCommandBytes = CommandPacketHelper::getNumberOfBytes<U>(numberOfAuxiliaryBytes);

			// 4294967295 is the maximum value of an "uint32_t"-type: Check for overflow
			// -> We use the magic number here to avoid "std::numeric_limits::max()" usage
			assert((static_cast<uint64_t>(mCurrentCommandPacketByteIndex) + numberOfCommandBytes) < 4294967295u);

			// Grow command packet buffer, if required
			if (mCommandPacketBufferNumberOfBytes < mCurrentCommandPacketByteIndex + numberOfCommandBytes)
			{
				// Allocate new memory, grow using a known value but do also add the number of bytes consumed by the current command to add (many auxiliary bytes might be requested)
				const uint32_t newCommandPacketBufferNumberOfBytes = mCommandPacketBufferNumberOfBytes + NUMBER_OF_BYTES_TO_GROW + numberOfCommandBytes;
				uint8_t* newCommandPacketBuffer = new uint8_t[newCommandPacketBufferNumberOfBytes];

				// Copy over current command package buffer content and free it, if required
				if (nullptr != mCommandPacketBuffer)
				{
					memcpy(newCommandPacketBuffer, mCommandPacketBuffer, mCommandPacketBufferNumberOfBytes);
					delete [] mCommandPacketBuffer;
				}

				// Finalize
				mCommandPacketBuffer = newCommandPacketBuffer;
				mCommandPacketBufferNumberOfBytes = newCommandPacketBufferNumberOfBytes;
			}

			// Get command package for the new command
			CommandPacket commandPacket = &mCommandPacketBuffer[mCurrentCommandPacketByteIndex];

			// Setup previous and current command package
			if (~0u != mPreviousCommandPacketByteIndex)
			{
				CommandPacketHelper::storeNextCommandPacketByteIndex(&mCommandPacketBuffer[mPreviousCommandPacketByteIndex], mCurrentCommandPacketByteIndex);
			}
			CommandPacketHelper::storeNextCommandPacketByteIndex(commandPacket, ~0u);
			CommandPacketHelper::storeBackendDispatchFunctionIndex(commandPacket, U::COMMAND_DISPATCH_FUNCTION_INDEX);
			mPreviousCommandPacketByteIndex = mCurrentCommandPacketByteIndex;
			mCurrentCommandPacketByteIndex += numberOfCommandBytes;

			// Done
			#ifndef RENDERER_NO_STATISTICS
				++mNumberOfCommands;
			#endif
			return CommandPacketHelper::getCommand<U>(commandPacket);
		}

		/**
		*  @brief
		*    Submit the command buffer to the renderer without flushing; use this for recording command buffers once and submit them multiple times
		*
		*  @param[in] renderer
		*    Renderer to submit the command buffer to
		*/
		inline void submit(IRenderer& renderer) const
		{
			renderer.submitCommandBuffer(*this);
		}

		/**
		*  @brief
		*    Submit the command buffer to the renderer and clear so the command buffer is empty again
		*
		*  @param[in] renderer
		*    Renderer to submit the command buffer to
		*/
		inline void submitAndClear(IRenderer& renderer)
		{
			renderer.submitCommandBuffer(*this);
			clear();
		}

		//[-------------------------------------------------------]
		//[ Internal                                              ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Return the command packet buffer
		*
		*  @return
		*    The command packet buffer, can be a null pointer, don't destroy the instance
		*
		*  @note
		*    - Don't put this method into the public interface, there's no need for public access
		*/
		const uint8_t* getCommandPacketBuffer() const
		{
			return mCommandPacketBuffer;
		}


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		static const uint32_t NUMBER_OF_BYTES_TO_GROW = 8192;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		// Memory
		uint32_t mCommandPacketBufferNumberOfBytes;
		uint8_t* mCommandPacketBuffer;
		// Current state
		uint32_t mPreviousCommandPacketByteIndex;
		uint32_t mCurrentCommandPacketByteIndex;
		#ifndef RENDERER_NO_STATISTICS
			uint32_t mNumberOfCommands;
		#endif


	};


	//[-------------------------------------------------------]
	//[ Concrete commands                                     ]
	//[-------------------------------------------------------]
	namespace Command
	{


		//[-------------------------------------------------------]
		//[ Command buffer                                        ]
		//[-------------------------------------------------------]
		struct ExecuteCommandBuffer final
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, CommandBuffer* commandBufferToExecute)
			{
				assert(nullptr != commandBufferToExecute);
				*commandBuffer.addCommand<ExecuteCommandBuffer>() = ExecuteCommandBuffer(commandBufferToExecute);
			}
			// Constructor
			inline ExecuteCommandBuffer(CommandBuffer* _commandBufferToExecute) :
				commandBufferToExecute(_commandBufferToExecute)
			{}
			// Data
			CommandBuffer* commandBufferToExecute;
			// Static data
			static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::ExecuteCommandBuffer;
		};

		//[-------------------------------------------------------]
		//[ Graphics root                                         ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Set the used graphics root signature
		*
		*  @param[in] rootSignature
		*    Graphics root signature to use, can be an null pointer (default: "nullptr")
		*/
		struct SetGraphicsRootSignature final
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, IRootSignature* rootSignature)
			{
				*commandBuffer.addCommand<SetGraphicsRootSignature>() = SetGraphicsRootSignature(rootSignature);
			}
			// Constructor
			inline SetGraphicsRootSignature(IRootSignature* _rootSignature) :
				rootSignature(_rootSignature)
			{}
			// Data
			IRootSignature* rootSignature;
			// Static data
			static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::SetGraphicsRootSignature;
		};

		/**
		*  @brief
		*    Set a graphics resource group
		*
		*  @param[in] rootParameterIndex
		*    The root parameter index number for binding
		*  @param[in] resourceGroup
		*    Resource group to set
		*/
		struct SetGraphicsResourceGroup final
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, uint32_t rootParameterIndex, IResourceGroup* resourceGroup)
			{
				*commandBuffer.addCommand<SetGraphicsResourceGroup>() = SetGraphicsResourceGroup(rootParameterIndex, resourceGroup);
			}
			// Constructor
			inline SetGraphicsResourceGroup(uint32_t _rootParameterIndex, IResourceGroup* _resourceGroup) :
				rootParameterIndex(_rootParameterIndex),
				resourceGroup(_resourceGroup)
			{}
			// Data
			uint32_t		rootParameterIndex;
			IResourceGroup*	resourceGroup;
			// Static data
			static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::SetGraphicsResourceGroup;
		};

		//[-------------------------------------------------------]
		//[ States                                                ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Set the used pipeline state
		*
		*  @param[in] pipelineState
		*    Pipeline state to use, can be an null pointer (default: "nullptr")
		*/
		struct SetPipelineState final
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, IPipelineState* pipelineState)
			{
				*commandBuffer.addCommand<SetPipelineState>() = SetPipelineState(pipelineState);
			}
			// Constructor
			inline SetPipelineState(IPipelineState* _pipelineState) :
				pipelineState(_pipelineState)
			{}
			// Data
			IPipelineState* pipelineState;
			// Static data
			static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::SetPipelineState;
		};

		//[-------------------------------------------------------]
		//[ Input-assembler (IA) stage                            ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Set the used vertex array
		*
		*  @param[in] vertexArray
		*    Vertex array to use, can be an null pointer (default: "nullptr")
		*/
		struct SetVertexArray final
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, IVertexArray* vertexArray)
			{
				*commandBuffer.addCommand<SetVertexArray>() = SetVertexArray(vertexArray);
			}
			// Constructor
			inline SetVertexArray(IVertexArray* _vertexArray) :
				vertexArray(_vertexArray)
			{}
			// Data
			IVertexArray* vertexArray;
			// Static data
			static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::SetVertexArray;
		};

		//[-------------------------------------------------------]
		//[ Rasterizer (RS) stage                                 ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Set the viewports
		*
		*  @param[in] numberOfViewports
		*    Number of viewports, if <1 nothing happens, must be <="Renderer::Capabilities::maximumNumberOfViewports"
		*  @param[in] viewports
		*    C-array of viewports, there must be at least "numberOfViewports"-viewports, in case of a null pointer nothing happens
		*
		*  @note
		*    - The current viewport(s) does not affect the clear operation
		*    - Lookout! In Direct3D 12 the scissor test can't be deactivated and hence one always needs to set a valid scissor rectangle.
		*      Use the convenience "Renderer::Command::SetViewportAndScissorRectangle"-command if possible to not walk into this Direct3D 12 trap.
		*/
		struct SetViewports final
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, uint32_t numberOfViewports, const Viewport* viewports)
			{
				*commandBuffer.addCommand<SetViewports>() = SetViewports(numberOfViewports, viewports);
			}
			inline static void create(CommandBuffer& commandBuffer, uint32_t topLeftX, uint32_t topLeftY, uint32_t width, uint32_t height, float minimumDepth = 0.0f, float maximumDepth = 1.0f)
			{
				SetViewports* setViewportsCommand = commandBuffer.addCommand<SetViewports>(sizeof(Viewport));

				// Set command data
				Viewport* viewport = reinterpret_cast<Viewport*>(CommandPacketHelper::getAuxiliaryMemory(setViewportsCommand));
				viewport->topLeftX = static_cast<float>(topLeftX);
				viewport->topLeftY = static_cast<float>(topLeftY);
				viewport->width	   = static_cast<float>(width);
				viewport->height   = static_cast<float>(height);
				viewport->minDepth = minimumDepth;
				viewport->maxDepth = maximumDepth;

				// Finalize command
				setViewportsCommand->numberOfViewports = 1;
				setViewportsCommand->viewports		   = nullptr;
			}
			// Constructor
			inline SetViewports(uint32_t _numberOfViewports, const Viewport* _viewports) :
				numberOfViewports(_numberOfViewports),
				viewports(_viewports)
			{}
			// Data
			uint32_t		numberOfViewports;
			const Viewport* viewports;	///< If null pointer, command auxiliary memory is used instead
			// Static data
			static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::SetViewports;
		};

		/**
		*  @brief
		*    Set the scissor rectangles
		*
		*  @param[in] numberOfScissorRectangles
		*    Number of scissor rectangles, if <1 nothing happens, must be <="Renderer::Capabilities::maximumNumberOfViewports"
		*  @param[in] dcissorRectangles
		*    C-array of scissor rectangles, there must be at least "numberOfScissorRectangles" scissor rectangles, in case of a null pointer nothing happens
		*
		*  @note
		*    - Scissor rectangles are only used when "Renderer::RasterizerState::scissorEnable" is true
		*    - The current scissor rectangle(s) does not affect the clear operation
		*/
		struct SetScissorRectangles final
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, uint32_t numberOfScissorRectangles, const ScissorRectangle* scissorRectangles)
			{
				*commandBuffer.addCommand<SetScissorRectangles>() = SetScissorRectangles(numberOfScissorRectangles, scissorRectangles);
			}
			inline static void create(CommandBuffer& commandBuffer, long topLeftX, long topLeftY, long bottomRightX, long bottomRightY)
			{
				SetScissorRectangles* setScissorRectanglesCommand = commandBuffer.addCommand<SetScissorRectangles>(sizeof(ScissorRectangle));

				// Set command data
				ScissorRectangle* scissorRectangle = reinterpret_cast<ScissorRectangle*>(CommandPacketHelper::getAuxiliaryMemory(setScissorRectanglesCommand));
				scissorRectangle->topLeftX	   = topLeftX;
				scissorRectangle->topLeftY	   = topLeftY;
				scissorRectangle->bottomRightX = bottomRightX;
				scissorRectangle->bottomRightY = bottomRightY;

				// Finalize command
				setScissorRectanglesCommand->numberOfScissorRectangles = 1;
				setScissorRectanglesCommand->scissorRectangles		   = nullptr;
			}
			// Constructor
			inline SetScissorRectangles(uint32_t _numberOfScissorRectangles, const ScissorRectangle* _scissorRectangles) :
				numberOfScissorRectangles(_numberOfScissorRectangles),
				scissorRectangles(_scissorRectangles)
			{}
			// Data
			uint32_t				numberOfScissorRectangles;
			const ScissorRectangle* scissorRectangles;	///< If null pointer, command auxiliary memory is used instead
			// Static data
			static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::SetScissorRectangles;
		};

		/**
		*  @brief
		*    Set viewport and scissor rectangle (convenience method)
		*
		*  @param[in] topLeftX
		*    Top left x
		*  @param[in] topLeftY
		*    Top left y
		*  @param[in] width
		*    Width
		*  @param[in] height
		*    Height
		*  @param[in] minimumDepth
		*    Minimum depth
		*  @param[in] maximumDepth
		*    Maximum depth
		*
		*  @note
		*    - Lookout! In Direct3D 12 the scissor test can't be deactivated and hence one always needs to set a valid scissor rectangle.
		*      Use the convenience "Renderer::Command::SetViewportAndScissorRectangle"-command if possible to not walk into this Direct3D 12 trap.
		*/
		struct SetViewportAndScissorRectangle final
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, uint32_t topLeftX, uint32_t topLeftY, uint32_t width, uint32_t height, float minimumDepth = 0.0f, float maximumDepth = 1.0f)
			{
				// Set the viewport
				SetViewports::create(commandBuffer, topLeftX, topLeftY, width, height, minimumDepth, maximumDepth);

				// Set the scissor rectangle
				SetScissorRectangles::create(commandBuffer, static_cast<long>(topLeftX), static_cast<long>(topLeftY), static_cast<long>(topLeftX + width), static_cast<long>(topLeftY + height));
			}
		};

		//[-------------------------------------------------------]
		//[ Output-merger (OM) stage                              ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Set the render target to render into
		*
		*  @param[in] renderTarget
		*    Render target to render into by binding it to the output-merger state, can be an null pointer to render into the primary window
		*/
		struct SetRenderTarget final
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, IRenderTarget* renderTarget)
			{
				*commandBuffer.addCommand<SetRenderTarget>() = SetRenderTarget(renderTarget);
			}
			// Constructor
			inline SetRenderTarget(IRenderTarget* _renderTarget) :
				renderTarget(_renderTarget)
			{}
			// Data
			IRenderTarget* renderTarget;
			// Static data
			static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::SetRenderTarget;
		};

		//[-------------------------------------------------------]
		//[ Operations                                            ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Clears the viewport to a specified RGBA color, clears the depth buffer,
		*    and erases the stencil buffer
		*
		*  @param[in] flags
		*    Flags that indicate what should be cleared. This parameter can be any
		*    combination of the following flags, but at least one flag must be used:
		*    "Renderer::ClearFlag::COLOR", "Renderer::ClearFlag::DEPTH" and "Renderer::ClearFlag::STENCIL, see "Renderer::ClearFlag"-flags
		*  @param[in] color
		*    RGBA clear color (used if "Renderer::ClearFlag::COLOR" is set)
		*  @param[in] z
		*    Z clear value. (if "Renderer::ClearFlag::DEPTH" is set)
		*    This parameter can be in the range from 0.0 through 1.0. A value of 0.0
		*    represents the nearest distance to the viewer, and 1.0 the farthest distance.
		*  @param[in] stencil
		*    Value to clear the stencil-buffer with. This parameter can be in the range from
		*    0 through 2^n�1, where n is the bit depth of the stencil buffer.
		*
		*  @note
		*    - The current viewport(s) (see "Renderer::IRenderer::rsSetViewports()") does not affect the clear operation
		*    - The current scissor rectangle(s) (see "Renderer::IRenderer::rsSetScissorRectangles()") does not affect the clear operation
		*    - In case there are multiple active render targets, all render targets are cleared
		*/
		struct Clear final
		{
			// Static methods
			// -> z = 0 instead of 1 due to usage of Reversed-Z (see e.g. https://developer.nvidia.com/content/depth-precision-visualized and https://nlguillemot.wordpress.com/2016/12/07/reversed-z-in-opengl/)
			inline static void create(CommandBuffer& commandBuffer, uint32_t flags, const float color[4], float z = 0.0f, uint32_t stencil = 0)
			{
				*commandBuffer.addCommand<Clear>() = Clear(flags, color, z, stencil);
			}
			// Constructor
			inline Clear(uint32_t _flags, const float _color[4], float _z, uint32_t _stencil) :
				flags(_flags),
				color{_color[0], _color[1], _color[2], _color[3]},
				z(_z),
				stencil(_stencil)
			{}
			// Data
			uint32_t flags;
			float	 color[4];
			float	 z;
			uint32_t stencil;
			// Static data
			static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::Clear;
		};

		/**
		*  @brief
		*    Resolve multisample framebuffer
		*
		*  @param[in] destinationRenderTarget
		*    None multisample destination render target
		*  @param[in] sourceMultisampleFramebuffer
		*    Source multisample framebuffer
		*/
		struct ResolveMultisampleFramebuffer final
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, IRenderTarget& destinationRenderTarget, IFramebuffer& sourceMultisampleFramebuffer)
			{
				*commandBuffer.addCommand<ResolveMultisampleFramebuffer>() = ResolveMultisampleFramebuffer(destinationRenderTarget, sourceMultisampleFramebuffer);
			}
			// Constructor
			inline ResolveMultisampleFramebuffer(IRenderTarget& _destinationRenderTarget, IFramebuffer& _sourceMultisampleFramebuffer) :
				destinationRenderTarget(&_destinationRenderTarget),
				sourceMultisampleFramebuffer(&_sourceMultisampleFramebuffer)
			{}
			// Data
			IRenderTarget* destinationRenderTarget;
			IFramebuffer* sourceMultisampleFramebuffer;
			// Static data
			static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::ResolveMultisampleFramebuffer;
		};

		/**
		*  @brief
		*    Copy resource
		*
		*  @param[in] destinationResource
		*    Destination resource
		*  @param[in] sourceResource
		*    Source Resource
		*/
		struct CopyResource final
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, IResource& destinationResource, IResource& sourceResource)
			{
				*commandBuffer.addCommand<CopyResource>() = CopyResource(destinationResource, sourceResource);
			}
			// Constructor
			inline CopyResource(IResource& _destinationResource, IResource& _sourceResource) :
				destinationResource(&_destinationResource),
				sourceResource(&_sourceResource)
			{}
			// Data
			IResource* destinationResource;
			IResource* sourceResource;
			// Static data
			static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::CopyResource;
		};

		//[-------------------------------------------------------]
		//[ Draw call                                             ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Render the specified geometric primitive, based on an array of vertices instancing and indirect draw
		*
		*  @param[in] indirectBuffer
		*    Indirect buffer to use, the indirect buffer must contain at least "numberOfDraws" instances of "Renderer::DrawInstancedArguments" starting at "indirectBufferOffset"
		*  @param[in] indirectBufferOffset
		*    Indirect buffer offset
		*  @param[in] numberOfDraws
		*    Number of draws, can be 0
		*
		*  @note
		*    - Draw instanced is a shader model 4 feature, only supported if "Renderer::Capabilities::drawInstanced" is true
		*    - In Direct3D 9, instanced arrays with hardware support is only possible when drawing indexed primitives, see
		*      "Efficiently Drawing Multiple Instances of Geometry (Direct3D 9)"-article at MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/bb173349%28v=vs.85%29.aspx#Drawing_Non_Indexed_Geometry
		*    - Fails if no vertex array is set
		*    - If the multi-draw indirect feature is not supported this parameter, multiple draw calls are emitted
		*    - If the draw indirect feature is not supported, a software indirect buffer is used and multiple draw calls are emitted
		*/
		struct Draw final
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, const IIndirectBuffer& indirectBuffer, uint32_t indirectBufferOffset = 0, uint32_t numberOfDraws = 1)
			{
				*commandBuffer.addCommand<Draw>() = Draw(indirectBuffer, indirectBufferOffset, numberOfDraws);
			}
			inline static void create(CommandBuffer& commandBuffer, uint32_t vertexCountPerInstance, uint32_t instanceCount = 1, uint32_t startVertexLocation = 0, uint32_t startInstanceLocation = 0)
			{
				Draw* drawCommand = commandBuffer.addCommand<Draw>(sizeof(DrawInstancedArguments));

				// Set command data: The command packet auxiliary memory contains an "Renderer::DrawInstancedArguments"-instance
				const DrawInstancedArguments drawInstancedArguments(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
				memcpy(CommandPacketHelper::getAuxiliaryMemory(drawCommand), &drawInstancedArguments, sizeof(DrawInstancedArguments));

				// Finalize command
				drawCommand->indirectBuffer		  = nullptr;
				drawCommand->indirectBufferOffset = 0;
				drawCommand->numberOfDraws		  = 1;
			}

			// Constructor
			inline Draw(const IIndirectBuffer& _indirectBuffer, uint32_t _indirectBufferOffset, uint32_t _numberOfDraws) :
				indirectBuffer(&_indirectBuffer),
				indirectBufferOffset(_indirectBufferOffset),
				numberOfDraws(_numberOfDraws)
			{}
			// Data
			const IIndirectBuffer* indirectBuffer;	///< If null pointer, command auxiliary memory is used instead
			uint32_t			   indirectBufferOffset;
			uint32_t			   numberOfDraws;
			// Static data
			static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::Draw;
		};

		/**
		*  @brief
		*    Render the specified geometric primitive, based on indexing into an array of vertices, instancing and indirect draw
		*
		*  @param[in] indirectBuffer
		*    Indirect buffer to use, the indirect buffer must contain at least "numberOfDraws" instances of "Renderer::DrawIndexedInstancedArguments" starting at bindirectBufferOffset"
		*  @param[in] indirectBufferOffset
		*    Indirect buffer offset
		*  @param[in] numberOfDraws
		*    Number of draws, can be 0
		*
		*  @note
		*    - Instanced arrays is a shader model 3 feature, only supported if "Renderer::Capabilities::instancedArrays" is true
		*    - Draw instanced is a shader model 4 feature, only supported if "Renderer::Capabilities::drawInstanced" is true
		*    - This method draws indexed primitives from the current set of data input streams
		*    - Fails if no index and/or vertex array is set
		*    - If the multi-draw indirect feature is not supported this parameter, multiple draw calls are emitted
		*    - If the draw indirect feature is not supported, a software indirect buffer is used and multiple draw calls are emitted
		*/
		struct DrawIndexed final
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, const IIndirectBuffer& indirectBuffer, uint32_t indirectBufferOffset = 0, uint32_t numberOfDraws = 1)
			{
				*commandBuffer.addCommand<DrawIndexed>() = DrawIndexed(indirectBuffer, indirectBufferOffset, numberOfDraws);
			}
			inline static void create(CommandBuffer& commandBuffer, uint32_t indexCountPerInstance, uint32_t instanceCount = 1, uint32_t startIndexLocation = 0, int32_t baseVertexLocation = 0, uint32_t startInstanceLocation = 0)
			{
				DrawIndexed* drawCommand = commandBuffer.addCommand<DrawIndexed>(sizeof(DrawIndexedInstancedArguments));

				// Set command data: The command packet auxiliary memory contains an "Renderer::DrawIndexedInstancedArguments"-instance
				const DrawIndexedInstancedArguments drawIndexedInstancedArguments(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
				memcpy(CommandPacketHelper::getAuxiliaryMemory(drawCommand), &drawIndexedInstancedArguments, sizeof(DrawIndexedInstancedArguments));

				// Finalize command
				drawCommand->indirectBuffer		  = nullptr;
				drawCommand->indirectBufferOffset = 0;
				drawCommand->numberOfDraws		  = 1;
			}
			// Constructor
			inline DrawIndexed(const IIndirectBuffer& _indirectBuffer, uint32_t _indirectBufferOffset, uint32_t _numberOfDraws) :
				indirectBuffer(&_indirectBuffer),
				indirectBufferOffset(_indirectBufferOffset),
				numberOfDraws(_numberOfDraws)
			{}
			// Data
			const IIndirectBuffer* indirectBuffer;	///< If null pointer, command auxiliary memory is used instead
			uint32_t			   indirectBufferOffset;
			uint32_t			   numberOfDraws;
			// Static data
			static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::DrawIndexed;
		};

		//[-------------------------------------------------------]
		//[ Debug                                                 ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Set a debug marker
		*
		*  @param[in] name
		*    ASCII name of the debug marker, must be valid (there's no internal null pointer test)
		*
		*  @see
		*    - "isDebugEnabled()"
		*/
		struct SetDebugMarker final
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, const char* name)
			{
				*commandBuffer.addCommand<SetDebugMarker>() = SetDebugMarker(name);
			}
			// Constructor
			inline SetDebugMarker(const char* _name)
			{
				assert(strlen(_name) < 128);
				strncpy(name, _name, 128);
				name[127] = '\0';
			};
			// Data
			char name[128];
			// Static data
			static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::SetDebugMarker;
		};

		/**
		*  @brief
		*    Begin debug event
		*
		*  @param[in] name
		*    ASCII name of the debug event, must be valid (there's no internal null pointer test)
		*
		*  @see
		*    - "isDebugEnabled()"
		*/
		struct BeginDebugEvent final
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, const char* name)
			{
				*commandBuffer.addCommand<BeginDebugEvent>() = BeginDebugEvent(name);
			}
			// Constructor
			inline BeginDebugEvent(const char* _name)
			{
				assert(strlen(_name) < 128);
				strncpy(name, _name, 128);
				name[127] = '\0';
			};
			// Data
			char name[128];
			// Static data
			static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::BeginDebugEvent;
		};

		/**
		*  @brief
		*    End the last started debug event
		*
		*  @see
		*    - "isDebugEnabled()"
		*/
		struct EndDebugEvent final
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer)
			{
				commandBuffer.addCommand<EndDebugEvent>();
			}
			// Static data
			static const CommandDispatchFunctionIndex COMMAND_DISPATCH_FUNCTION_INDEX = CommandDispatchFunctionIndex::EndDebugEvent;
		};


	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Debug                                                 ]
//[-------------------------------------------------------]
// Debug macros
#ifdef RENDERER_DEBUG
	/**
	*  @brief
	*    Set a debug marker
	*
	*  @param[in] commandBuffer
	*    Reference to the renderer instance to use
	*  @param[in] name
	*    ASCII name of the debug marker
	*/
	#define COMMAND_SET_DEBUG_MARKER(commandBuffer, name) Renderer::Command::SetDebugMarker::create(commandBuffer, name);

	/**
	*  @brief
	*    Set a debug marker by using the current function name ("__FUNCTION__") as marker name
	*
	*  @param[in] commandBuffer
	*    Reference to the renderer instance to use
	*/
	#define COMMAND_SET_DEBUG_MARKER_FUNCTION(commandBuffer) Renderer::Command::SetDebugMarker::create(commandBuffer, __FUNCTION__);

	/**
	*  @brief
	*    Begin debug event
	*
	*  @param[in] commandBuffer
	*    Reference to the renderer instance to use
	*  @param[in] name
	*    ASCII name of the debug event
	*/
	#define COMMAND_BEGIN_DEBUG_EVENT(commandBuffer, name) Renderer::Command::BeginDebugEvent::create(commandBuffer, name);

	/**
	*  @brief
	*    Begin debug event by using the current function name ("__FUNCTION__") as event name
	*
	*  @param[in] commandBuffer
	*    Reference to the renderer instance to use
	*/
	#define COMMAND_BEGIN_DEBUG_EVENT_FUNCTION(commandBuffer) Renderer::Command::BeginDebugEvent::create(commandBuffer, __FUNCTION__);

	/**
	*  @brief
	*    End the last started debug event
	*
	*  @param[in] commandBuffer
	*    Reference to the renderer instance to use
	*/
	#define COMMAND_END_DEBUG_EVENT(commandBuffer) Renderer::Command::EndDebugEvent::create(commandBuffer);
#else
	/**
	*  @brief
	*    Set a debug marker
	*
	*  @param[in] commandBuffer
	*    Reference to the renderer instance to use
	*  @param[in] name
	*    ASCII name of the debug marker
	*/
	#define COMMAND_SET_DEBUG_MARKER(commandBuffer, name)

	/**
	*  @brief
	*    Set a debug marker by using the current function name ("__FUNCTION__") as marker name
	*
	*  @param[in] commandBuffer
	*    Reference to the renderer instance to use
	*/
	#define COMMAND_SET_DEBUG_MARKER_FUNCTION(commandBuffer)

	/**
	*  @brief
	*    Begin debug event
	*
	*  @param[in] commandBuffer
	*    Reference to the renderer instance to use
	*  @param[in] name
	*    ASCII name of the debug event
	*/
	#define COMMAND_BEGIN_DEBUG_EVENT(commandBuffer, name)

	/**
	*  @brief
	*    Begin debug event by using the current function name ("__FUNCTION__") as event name
	*
	*  @param[in] commandBuffer
	*    Reference to the renderer instance to use
	*/
	#define COMMAND_BEGIN_DEBUG_EVENT_FUNCTION(commandBuffer)

	/**
	*  @brief
	*    End the last started debug event
	*
	*  @param[in] commandBuffer
	*    Reference to the renderer instance to use
	*/
	#define COMMAND_END_DEBUG_EVENT(commandBuffer)
#endif
