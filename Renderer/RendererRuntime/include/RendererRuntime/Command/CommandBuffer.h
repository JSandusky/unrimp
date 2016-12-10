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
#include "RendererRuntime/Export.h"

#include <Renderer/Public/Renderer.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	typedef void (*BackendDispatchFunction)(const void*, Renderer::IRenderer& renderer);
	typedef void* CommandPacket;


	namespace CommandPacketHelper
	{
		static const uint32_t OFFSET_NEXT_COMMAND_PACKET_BYTE_INDEX	= 0u;
		static const uint32_t OFFSET_BACKEND_DISPATCH_FUNCTION		= OFFSET_NEXT_COMMAND_PACKET_BYTE_INDEX + sizeof(uint32_t);
		static const uint32_t OFFSET_COMMAND						= OFFSET_BACKEND_DISPATCH_FUNCTION + sizeof(BackendDispatchFunction);

		template <typename T>
		uint32_t getNumberOfBytes(uint32_t numberOfAuxiliaryBytes)
		{
			return OFFSET_COMMAND + sizeof(T) + numberOfAuxiliaryBytes;
		};

		inline uint32_t getNextCommandPacketByteIndex(const CommandPacket packet)
		{
			return *reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(packet) + OFFSET_NEXT_COMMAND_PACKET_BYTE_INDEX);
		}

		inline void storeNextCommandPacketByteIndex(const CommandPacket packet, uint32_t nextPacketByteIndex)
		{
			*reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(packet) + OFFSET_NEXT_COMMAND_PACKET_BYTE_INDEX) = nextPacketByteIndex;
		}

		inline BackendDispatchFunction* getBackendDispatchFunction(const CommandPacket packet)
		{
			return reinterpret_cast<BackendDispatchFunction*>(reinterpret_cast<uint8_t*>(packet) + OFFSET_BACKEND_DISPATCH_FUNCTION);
		}

		inline void storeBackendDispatchFunction(const CommandPacket packet, BackendDispatchFunction dispatchFunction)
		{
			*CommandPacketHelper::getBackendDispatchFunction(packet) = dispatchFunction;
		}

		inline const BackendDispatchFunction loadBackendDispatchFunction(const CommandPacket packet)
		{
			return *getBackendDispatchFunction(packet);
		}

		template <typename T>
		T* getCommand(const CommandPacket packet)
		{
			return reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(packet) + OFFSET_COMMAND);
		}

		inline const void* loadCommand(const CommandPacket packet)
		{
			return reinterpret_cast<uint8_t*>(packet) + OFFSET_COMMAND;
		}

		template <typename T>
		uint8_t* getAuxiliaryMemory(T* command)
		{
			return reinterpret_cast<uint8_t*>(command) + sizeof(T);
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
	*    - http://molecularmusings.wordpress.com/2014/11/06/stateless-layered-multi-threaded-rendering-part-1/ - "Stateless, layered, multi-threaded rendering – Part 1"
	*    - http://molecularmusings.wordpress.com/2014/11/13/stateless-layered-multi-threaded-rendering-part-2-stateless-api-design/ - "Stateless, layered, multi-threaded rendering – Part 2"
	*    - http://molecularmusings.wordpress.com/2014/12/16/stateless-layered-multi-threaded-rendering-part-3-api-design-details/ - "Stateless, layered, multi-threaded rendering – Part 3"
	*    - http://realtimecollisiondetection.net/blog/?p=86 - "Order your graphics draw calls around!"
	*    but without a key inside the more general command buffer. Sorting is a job of a more high level construct like a render queue which also automatically will perform
	*    batching and instancing. Also the memory management is much simplified to be cache friendly.
	*
	*  @note
	*    - The commands are stored as a flat contiguous array to be cache friendly
	*    - Each command can have an additional auxiliary buffer, e.g. to store uniform buffer data to submit to the renderer
	*    - It's valid to record a command buffer only once, and submit it multiple times to the renderer
	*/
	class CommandBuffer
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
		{
			// Nothing here
		}

		/**
		*  @brief
		*    Destructor
		*/
		inline ~CommandBuffer()
		{
			if (nullptr != mCommandPacketBuffer)
			{
				delete [] mCommandPacketBuffer;
			}
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

		/**
		*  @brief
		*    Clear the command buffer
		*/
		inline void clear()
		{
			mPreviousCommandPacketByteIndex = ~0u;
			mCurrentCommandPacketByteIndex = 0;
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
			assert(numberOfCommandBytes < NUMBER_OF_BYTES_TO_GROW);

			// Grow command packet buffer, if required
			if (mCommandPacketBufferNumberOfBytes < mCurrentCommandPacketByteIndex + numberOfCommandBytes)
			{
				// Allocate new memory, grow using a known value
				const uint32_t newCommandPacketBufferNumberOfBytes = mCommandPacketBufferNumberOfBytes + NUMBER_OF_BYTES_TO_GROW;
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
			CommandPacketHelper::storeBackendDispatchFunction(commandPacket, U::DISPATCH_FUNCTION);
			mPreviousCommandPacketByteIndex = mCurrentCommandPacketByteIndex;
			mCurrentCommandPacketByteIndex += numberOfCommandBytes;

			// Done
			return CommandPacketHelper::getCommand<U>(commandPacket);
		}

		/**
		*  @brief
		*    Submit the command buffer to the renderer without flushing; use this for recording command buffers once and submit them multiple times
		*
		*  @param[in] renderer
		*    Renderer to submit the command buffer to
		*/
		inline void submit(Renderer::IRenderer& renderer) const
		{
			CommandPacket commandPacket = mCommandPacketBuffer;
			while (nullptr != commandPacket)
			{
				submitCommandPacket(commandPacket, renderer);
				const uint32_t nextCommandPacketByteIndex = CommandPacketHelper::getNextCommandPacketByteIndex(commandPacket);
				commandPacket = (~0u != nextCommandPacketByteIndex) ? &mCommandPacketBuffer[nextCommandPacketByteIndex] : nullptr;
			}
		}

		/**
		*  @brief
		*    Submit the command buffer to the renderer and clear so the command buffer is empty again
		*
		*  @param[in] renderer
		*    Renderer to submit the command buffer to
		*/
		inline void submitAndClear(Renderer::IRenderer& renderer)
		{
			submit(renderer);
			clear();
		}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		inline void submitCommandPacket(const CommandPacket packet, Renderer::IRenderer& renderer) const
		{
			const BackendDispatchFunction backendDispatchFunction = CommandPacketHelper::loadBackendDispatchFunction(packet);
			const void* command = CommandPacketHelper::loadCommand(packet);
			backendDispatchFunction(command, renderer);
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
		uint32_t mCurrentCommandPacketByteIndex;
		uint32_t mPreviousCommandPacketByteIndex;


	};




	namespace Command
	{
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
		struct SetGraphicsRootSignature
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, Renderer::IRootSignature* rootSignature)
			{
				*commandBuffer.addCommand<SetGraphicsRootSignature>() = SetGraphicsRootSignature(rootSignature);
			}
			// Constructor
			inline SetGraphicsRootSignature(Renderer::IRootSignature* _rootSignature) :
				rootSignature(_rootSignature)
			{};
			// Data
			Renderer::IRootSignature* rootSignature;
			// Static data
			RENDERERRUNTIME_API_EXPORT static const BackendDispatchFunction DISPATCH_FUNCTION;
		};

		/**
		*  @brief
		*    Set the graphics descriptor table
		*
		*  @param[in] rootParameterIndex
		*    The slot number for binding
		*  @param[in] resource
		*    Resource to bind
		*/
		struct SetGraphicsRootDescriptorTable
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, uint32_t rootParameterIndex, Renderer::IResource* resource)
			{
				*commandBuffer.addCommand<SetGraphicsRootDescriptorTable>() = SetGraphicsRootDescriptorTable(rootParameterIndex, resource);
			}
			// Constructor
			inline SetGraphicsRootDescriptorTable(uint32_t _rootParameterIndex, Renderer::IResource* _resource) :
				rootParameterIndex(_rootParameterIndex),
				resource(_resource)
			{};
			// Data
			uint32_t			 rootParameterIndex;
			Renderer::IResource* resource;
			// Static data
			RENDERERRUNTIME_API_EXPORT static const BackendDispatchFunction DISPATCH_FUNCTION;
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
		struct SetPipelineState
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, Renderer::IPipelineState* pipelineState)
			{
				*commandBuffer.addCommand<SetPipelineState>() = SetPipelineState(pipelineState);
			}
			// Constructor
			inline SetPipelineState(Renderer::IPipelineState* _pipelineState) :
				pipelineState(_pipelineState)
			{};
			// Data
			Renderer::IPipelineState* pipelineState;
			// Static data
			RENDERERRUNTIME_API_EXPORT static const BackendDispatchFunction DISPATCH_FUNCTION;
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
		struct SetVertexArray
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, Renderer::IVertexArray* vertexArray)
			{
				*commandBuffer.addCommand<SetVertexArray>() = SetVertexArray(vertexArray);
			}
			// Constructor
			inline SetVertexArray(Renderer::IVertexArray* _vertexArray) :
				vertexArray(_vertexArray)
			{};
			// Data
			Renderer::IVertexArray* vertexArray;
			// Static data
			RENDERERRUNTIME_API_EXPORT static const BackendDispatchFunction DISPATCH_FUNCTION;
		};

		/**
		*  @brief
		*    Set the primitive topology used for draw calls
		*
		*  @param[in] primitiveTopology
		*    Member of the primitive topology enumerated type, describing the type of primitive to render (default: "Renderer::PrimitiveTopology::UNKNOWN")
		*/
		struct SetPrimitiveTopology
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, Renderer::PrimitiveTopology primitiveTopology)
			{
				*commandBuffer.addCommand<SetPrimitiveTopology>() = SetPrimitiveTopology(primitiveTopology);
			}
			// Constructor
			inline SetPrimitiveTopology(Renderer::PrimitiveTopology _primitiveTopology) :
				primitiveTopology(_primitiveTopology)
			{};
			// Data
			Renderer::PrimitiveTopology primitiveTopology;
			// Static data
			RENDERERRUNTIME_API_EXPORT static const BackendDispatchFunction DISPATCH_FUNCTION;
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
		*      Use the convenience method "Renderer::rsSetViewportAndScissorRectangle()" if possible to not walk into this Direct3D 12 trap.
		*/
		struct SetViewports
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, uint32_t numberOfViewports, const Renderer::Viewport* viewports)
			{
				*commandBuffer.addCommand<SetViewports>() = SetViewports(numberOfViewports, viewports);
			}
			// Constructor
			inline SetViewports(uint32_t _numberOfViewports, const Renderer::Viewport* _viewports) :
				numberOfViewports(_numberOfViewports),
				viewports(_viewports)
			{};
			// Data
			uint32_t				  numberOfViewports;
			const Renderer::Viewport* viewports;
			// Static data
			RENDERERRUNTIME_API_EXPORT static const BackendDispatchFunction DISPATCH_FUNCTION;
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
		struct SetScissorRectangles
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, uint32_t numberOfScissorRectangles, const Renderer::ScissorRectangle* scissorRectangles)
			{
				*commandBuffer.addCommand<SetScissorRectangles>() = SetScissorRectangles(numberOfScissorRectangles, scissorRectangles);
			}
			// Constructor
			inline SetScissorRectangles(uint32_t _numberOfScissorRectangles, const Renderer::ScissorRectangle* _scissorRectangles) :
				numberOfScissorRectangles(_numberOfScissorRectangles),
				scissorRectangles(_scissorRectangles)
			{};
			// Data
			uint32_t						  numberOfScissorRectangles;
			const Renderer::ScissorRectangle* scissorRectangles;
			// Static data
			RENDERERRUNTIME_API_EXPORT static const BackendDispatchFunction DISPATCH_FUNCTION;
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
		*    width
		*  @param[in] height
		*    height
		*
		*  @note
		*    - Lookout! In Direct3D 12 the scissor test can't be deactivated and hence one always needs to set a valid scissor rectangle.
		*      Use the convenience method "Renderer::rsSetViewportAndScissorRectangle()" if possible to not walk into this Direct3D 12 trap.
		*/
		struct SetViewportAndScissorRectangle
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, uint32_t topLeftX, uint32_t topLeftY, uint32_t width, uint32_t height)
			{
				{ // Set the viewport
					SetViewports* setViewportsCommand = commandBuffer.addCommand<SetViewports>(sizeof(Renderer::Viewport));

					// Set command data
					Renderer::Viewport* viewport = reinterpret_cast<Renderer::Viewport*>(CommandPacketHelper::getAuxiliaryMemory(setViewportsCommand));
					viewport->topLeftX = static_cast<float>(topLeftX);
					viewport->topLeftY = static_cast<float>(topLeftY);
					viewport->width	   = static_cast<float>(width);
					viewport->height   = static_cast<float>(height);
					viewport->minDepth = 0.0f;
					viewport->maxDepth = 1.0f;

					// Finalize command
					setViewportsCommand->numberOfViewports = 1;
					setViewportsCommand->viewports		   = viewport;
				}

				{ // Set the scissor rectangle
					SetScissorRectangles* setScissorRectanglesCommand = commandBuffer.addCommand<SetScissorRectangles>(sizeof(Renderer::ScissorRectangle));

					// Set command data
					Renderer::ScissorRectangle* scissorRectangle = reinterpret_cast<Renderer::ScissorRectangle*>(CommandPacketHelper::getAuxiliaryMemory(setScissorRectanglesCommand));
					scissorRectangle->topLeftX	   = static_cast<long>(topLeftX);
					scissorRectangle->topLeftY	   = static_cast<long>(topLeftY);
					scissorRectangle->bottomRightX = static_cast<long>(width);
					scissorRectangle->bottomRightY = static_cast<long>(height);

					// Finalize command
					setScissorRectanglesCommand->numberOfScissorRectangles = 1;
					setScissorRectanglesCommand->scissorRectangles		   = scissorRectangle;
				}
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
		struct SetRenderTarget
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, Renderer::IRenderTarget* renderTarget)
			{
				*commandBuffer.addCommand<SetRenderTarget>() = SetRenderTarget(renderTarget);
			}
			// Constructor
			inline SetRenderTarget(Renderer::IRenderTarget* _renderTarget) :
				renderTarget(_renderTarget)
			{};
			// Data
			Renderer::IRenderTarget* renderTarget;
			// Static data
			RENDERERRUNTIME_API_EXPORT static const BackendDispatchFunction DISPATCH_FUNCTION;
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
		*    0 through 2^n–1, where n is the bit depth of the stencil buffer.
		*
		*  @note
		*    - The current viewport(s) (see "Renderer::IRenderer::rsSetViewports()") does not affect the clear operation
		*    - The current scissor rectangle(s) (see "Renderer::IRenderer::rsSetScissorRectangles()") does not affect the clear operation
		*    - In case there are multiple active render targets, all render targets are cleared
		*/
		struct Clear
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, uint32_t flags, const float color[4], float z, uint32_t stencil)
			{
				*commandBuffer.addCommand<Clear>() = Clear(flags, color, z, stencil);
			}
			// Constructor
			inline Clear(uint32_t _flags, const float _color[4], float _z, uint32_t _stencil) :
				flags(_flags),
				color{_color[0], _color[1], _color[2], _color[3]},
				z(_z),
				stencil(_stencil)
			{};
			// Data
			uint32_t flags;
			float	 color[4];
			float	 z;
			uint32_t stencil;
			// Static data
			RENDERERRUNTIME_API_EXPORT static const BackendDispatchFunction DISPATCH_FUNCTION;
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
		struct Draw
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, const Renderer::IIndirectBuffer& indirectBuffer, uint32_t indirectBufferOffset = 0, uint32_t numberOfDraws = 1)
			{
				*commandBuffer.addCommand<Draw>() = Draw(indirectBuffer, indirectBufferOffset, numberOfDraws);
			}
			inline static void create(CommandBuffer& commandBuffer, uint32_t vertexCountPerInstance, uint32_t instanceCount = 1, uint32_t startVertexLocation = 0, uint32_t startInstanceLocation = 0)
			{
				Draw* drawCommand = commandBuffer.addCommand<Draw>(sizeof(Renderer::IndirectBuffer));

				// TODO(co) I'am sure there's a more elegant way to do this
				Renderer::IndirectBuffer* indirectBuffer = reinterpret_cast<Renderer::IndirectBuffer*>(CommandPacketHelper::getAuxiliaryMemory(drawCommand));
				Renderer::IndirectBuffer indirectBufferData(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
				memcpy(indirectBuffer, &indirectBufferData, sizeof(Renderer::IndirectBuffer));

				*drawCommand = Draw(*indirectBuffer, 0, 1);
			}

			// Constructor
			inline Draw(const Renderer::IIndirectBuffer& _indirectBuffer, uint32_t _indirectBufferOffset, uint32_t _numberOfDraws) :
				indirectBuffer(&_indirectBuffer),
				indirectBufferOffset(_indirectBufferOffset),
				numberOfDraws(_numberOfDraws)
			{};
			// Data
			const Renderer::IIndirectBuffer* indirectBuffer;
			uint32_t						 indirectBufferOffset;
			uint32_t						 numberOfDraws;
			// Static data
			RENDERERRUNTIME_API_EXPORT static const BackendDispatchFunction DISPATCH_FUNCTION;
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
		struct DrawIndexed
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, const Renderer::IIndirectBuffer& indirectBuffer, uint32_t indirectBufferOffset = 0, uint32_t numberOfDraws = 1)
			{
				*commandBuffer.addCommand<DrawIndexed>() = DrawIndexed(indirectBuffer, indirectBufferOffset, numberOfDraws);
			}
			inline static void create(CommandBuffer& commandBuffer, uint32_t indexCountPerInstance, uint32_t instanceCount = 1, uint32_t startIndexLocation = 0, int32_t baseVertexLocation = 0, uint32_t startInstanceLocation = 0)
			{
				DrawIndexed* drawCommand = commandBuffer.addCommand<DrawIndexed>(sizeof(Renderer::IndexedIndirectBuffer));

				// TODO(co) I'am sure there's a more elegant way to do this
				Renderer::IndexedIndirectBuffer* indexedIndirectBuffer = reinterpret_cast<Renderer::IndexedIndirectBuffer*>(CommandPacketHelper::getAuxiliaryMemory(drawCommand));
				Renderer::IndexedIndirectBuffer indexedIndirectBufferData(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
				memcpy(indexedIndirectBuffer, &indexedIndirectBufferData, sizeof(Renderer::IndexedIndirectBuffer));

				*drawCommand = DrawIndexed(*indexedIndirectBuffer, 0, 1);
			}
			// Constructor
			inline DrawIndexed(const Renderer::IIndirectBuffer& _indirectBuffer, uint32_t _indirectBufferOffset, uint32_t _numberOfDraws) :
				indirectBuffer(&_indirectBuffer),
				indirectBufferOffset(_indirectBufferOffset),
				numberOfDraws(_numberOfDraws)
			{};
			// Data
			const Renderer::IIndirectBuffer* indirectBuffer;
			uint32_t						 indirectBufferOffset;
			uint32_t						 numberOfDraws;
			// Static data
			RENDERERRUNTIME_API_EXPORT static const BackendDispatchFunction DISPATCH_FUNCTION;
		};

		//[-------------------------------------------------------]
		//[ TODO(co)                                              ]
		//[-------------------------------------------------------]
		struct CopyUniformBufferData
		{
			Renderer::IUniformBuffer* uniformBufferDynamicVs;
			uint32_t size;
			void* data;
			// Static data
			RENDERERRUNTIME_API_EXPORT static const BackendDispatchFunction DISPATCH_FUNCTION;
		};

		//[-------------------------------------------------------]
		//[ Debug                                                 ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Set a debug marker
		*
		*  @param[in] name
		*    Unicode name of the debug marker, must be valid (there's no internal null pointer test)
		*
		*  @see
		*    - "isDebugEnabled()"
		*/
		struct SetDebugMarker
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, const wchar_t* name)
			{
				*commandBuffer.addCommand<SetDebugMarker>() = SetDebugMarker(name);
			}
			// Constructor
			inline SetDebugMarker(const wchar_t* _name)
			{
				assert(wcslen(_name) < 64);
				wcsncpy(name, _name, 64);
				name[63] = '\0';
			};
			// Data
			wchar_t name[64];
			// Static data
			RENDERERRUNTIME_API_EXPORT static const BackendDispatchFunction DISPATCH_FUNCTION;
		};

		/**
		*  @brief
		*    Begin debug event
		*
		*  @param[in] name
		*    Unicode name of the debug event, must be valid (there's no internal null pointer test)
		*
		*  @see
		*    - "isDebugEnabled()"
		*/
		struct BeginDebugEvent
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer, const wchar_t* name)
			{
				*commandBuffer.addCommand<BeginDebugEvent>() = BeginDebugEvent(name);
			}
			// Constructor
			inline BeginDebugEvent(const wchar_t* _name)
			{
				assert(wcslen(_name) < 64);
				wcsncpy(name, _name, 64);
				name[63] = '\0';
			};
			// Data
			wchar_t name[64];
			// Static data
			RENDERERRUNTIME_API_EXPORT static const BackendDispatchFunction DISPATCH_FUNCTION;
		};

		/**
		*  @brief
		*    End the last started debug event
		*
		*  @see
		*    - "isDebugEnabled()"
		*/
		struct EndDebugEvent
		{
			// Static methods
			inline static void create(CommandBuffer& commandBuffer)
			{
				commandBuffer.addCommand<EndDebugEvent>();
			}
			// Static data
			RENDERERRUNTIME_API_EXPORT static const BackendDispatchFunction DISPATCH_FUNCTION;
		};


	}


	// TODO(co) Move the following macros into the right place when finishing the command buffer implementation
	#define RENDERER_SET_DEBUG_MARKER2(commandBuffer, name) Renderer::Command::SetDebugMarker::create(commandBuffer, name);
	#define RENDERER_SET_DEBUG_MARKER_FUNCTION2(commandBuffer) Renderer::Command::SetDebugMarker::create(commandBuffer, RENDERER_INTERNAL__WFUNCTION__);
	#define RENDERER_BEGIN_DEBUG_EVENT2(commandBuffer, name) Renderer::Command::BeginDebugEvent::create(commandBuffer, name);
	#define RENDERER_BEGIN_DEBUG_EVENT_FUNCTION2(commandBuffer) Renderer::Command::BeginDebugEvent::create(commandBuffer, RENDERER_INTERNAL__WFUNCTION__);
	#define RENDERER_END_DEBUG_EVENT2(commandBuffer) Renderer::Command::EndDebugEvent::create(commandBuffer);


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer
