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


// TODO(co) Work in progress


// Basing on
// http://molecularmusings.wordpress.com/2014/11/06/stateless-layered-multi-threaded-rendering-part-1/ - "Stateless, layered, multi-threaded rendering – Part 1"
// http://molecularmusings.wordpress.com/2014/11/13/stateless-layered-multi-threaded-rendering-part-2-stateless-api-design/ - "Stateless, layered, multi-threaded rendering – Part 2"
// http://molecularmusings.wordpress.com/2014/12/16/stateless-layered-multi-threaded-rendering-part-3-api-design-details/ - "Stateless, layered, multi-threaded rendering – Part 3"
// http://realtimecollisiondetection.net/blog/?p=86 - "Order your graphics draw calls around!"
// but without a key inside the more general command bucket. Sorting is a job of a more high level construct like a render queue which also automatically will perform batching and instancing.

#include "RendererRuntime/Export.h"
#include "RendererRuntime/Command/Material.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	typedef void (*BackendDispatchFunction)(const void*, Renderer::IRenderer& renderer);
	typedef void* CommandPacket;


	namespace commandPacket
	{
		static const size_t OFFSET_NEXT_COMMAND_PACKET = 0u;
		static const size_t OFFSET_BACKEND_DISPATCH_FUNCTION = OFFSET_NEXT_COMMAND_PACKET + sizeof(CommandPacket);
		static const size_t OFFSET_COMMAND = OFFSET_BACKEND_DISPATCH_FUNCTION + sizeof(BackendDispatchFunction);
 
		template <typename T>
		CommandPacket Create(size_t auxMemorySize)
		{
			// TODO(co) Memory management: Only one large chunk of continuous and reused memory
			return ::operator new(GetSize<T>(auxMemorySize));
		}

		template <typename T>
		size_t GetSize(size_t auxMemorySize)
		{
			return OFFSET_COMMAND + sizeof(T) + auxMemorySize;
		};

		inline CommandPacket* GetNextCommandPacket(CommandPacket packet)
		{
			return reinterpret_cast<CommandPacket*>(reinterpret_cast<char*>(packet) + OFFSET_NEXT_COMMAND_PACKET);
		}

		template <typename T>
		CommandPacket* GetNextCommandPacket(T* command)
		{
			return reinterpret_cast<CommandPacket*>(reinterpret_cast<char*>(command) - OFFSET_COMMAND + OFFSET_NEXT_COMMAND_PACKET);
		}

		inline BackendDispatchFunction* GetBackendDispatchFunction(CommandPacket packet)
		{
			return reinterpret_cast<BackendDispatchFunction*>(reinterpret_cast<char*>(packet) + OFFSET_BACKEND_DISPATCH_FUNCTION);
		}

		template <typename T>
		T* GetCommand(CommandPacket packet)
		{
			return reinterpret_cast<T*>(reinterpret_cast<char*>(packet) + OFFSET_COMMAND);
		}

		template <typename T>
		char* GetAuxiliaryMemory(T* command)
		{
			return reinterpret_cast<char*>(command) + sizeof(T);
		}

		inline void StoreNextCommandPacket(CommandPacket packet, CommandPacket nextPacket)
		{
			*commandPacket::GetNextCommandPacket(packet) = nextPacket;
		}

		template <typename T>
		void StoreNextCommandPacket(T* command, CommandPacket nextPacket)
		{
			*commandPacket::GetNextCommandPacket<T>(command) = nextPacket;
		}

		inline void StoreBackendDispatchFunction(CommandPacket packet, BackendDispatchFunction dispatchFunction)
		{
			*commandPacket::GetBackendDispatchFunction(packet) = dispatchFunction;
		}

		inline const CommandPacket LoadNextCommandPacket(const CommandPacket packet)
		{
			return *GetNextCommandPacket(packet);
		}

		inline const BackendDispatchFunction LoadBackendDispatchFunction(const  CommandPacket packet)
		{
			return *GetBackendDispatchFunction(packet);
		}

		inline const void* LoadCommand(const CommandPacket packet)
		{
			return reinterpret_cast<char*>(packet) + OFFSET_COMMAND;
		}
	};




	class CommandBucket
	{
	public:
		inline CommandBucket(uint16_t maximumNumberOfCommandPackages) :
			mMaximumNumberOfCommandPackages(maximumNumberOfCommandPackages),
			mCommandPackets(new CommandPacket[mMaximumNumberOfCommandPackages]),
			mCurrentNumberOfCommandPackages(0)
		{
		}
		inline ~CommandBucket()
		{
			delete [] mCommandPackets;
		}
		template <typename U>
		U* addCommand(size_t auxMemorySize = 0)
		{
			CommandPacket packet = commandPacket::Create<U>(auxMemorySize);

			// store pointer to the data
			{
				// TODO: add some kind of lock or atomic operation here
				const unsigned int current = mCurrentNumberOfCommandPackages++;
				mCommandPackets[current] = packet;
			}

			commandPacket::StoreNextCommandPacket(packet, nullptr);
			commandPacket::StoreBackendDispatchFunction(packet, U::DISPATCH_FUNCTION);

			return commandPacket::GetCommand<U>(packet);
		}
		inline void submit(Renderer::IRenderer& renderer)
		{
			for (unsigned int i=0; i < mCurrentNumberOfCommandPackages; ++i)
			{
				CommandPacket packet = mCommandPackets[i];
				do
				{
					submitPacket(packet, renderer);
					CommandPacket previousPacket = packet;
					packet = commandPacket::LoadNextCommandPacket(packet);

					// TODO(co) Memory management: Only one large chunk of continuous and reused memory
					delete previousPacket;

				} while (packet != nullptr);
			}

			mCurrentNumberOfCommandPackages = 0;
		}
	private:
		inline void submitPacket(const CommandPacket packet, Renderer::IRenderer& renderer)
		{
			const BackendDispatchFunction function = commandPacket::LoadBackendDispatchFunction(packet);
			const void* command = commandPacket::LoadCommand(packet);
			function(command, renderer);
		}
	private:
		uint16_t	   mMaximumNumberOfCommandPackages;
		CommandPacket* mCommandPackets;
		uint16_t	   mCurrentNumberOfCommandPackages;

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
			inline static void create(CommandBucket& commandBucket, Renderer::IRootSignature* rootSignature)
			{
				*commandBucket.addCommand<SetGraphicsRootSignature>() = SetGraphicsRootSignature(rootSignature);
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
			inline static void create(CommandBucket& commandBucket, Renderer::IPipelineState* pipelineState)
			{
				*commandBucket.addCommand<SetPipelineState>() = SetPipelineState(pipelineState);
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
			inline static void create(CommandBucket& commandBucket, Renderer::IVertexArray* vertexArray)
			{
				*commandBucket.addCommand<SetVertexArray>() = SetVertexArray(vertexArray);
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
			inline static void create(CommandBucket& commandBucket, Renderer::PrimitiveTopology primitiveTopology)
			{
				*commandBucket.addCommand<SetPrimitiveTopology>() = SetPrimitiveTopology(primitiveTopology);
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
			inline static void create(CommandBucket& commandBucket, uint32_t flags, const float color[4], float z, uint32_t stencil)
			{
				*commandBucket.addCommand<Clear>() = Clear(flags, color, z, stencil);
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
			inline static void create(CommandBucket& commandBucket, const Renderer::IIndirectBuffer& indirectBuffer, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
			{
				*commandBucket.addCommand<Draw>() = Draw(indirectBuffer, indirectBufferOffset, numberOfDraws);
			}
			inline static void create(CommandBucket& commandBucket, uint32_t vertexCountPerInstance, uint32_t instanceCount = 1, uint32_t startVertexLocation = 0, uint32_t startInstanceLocation = 0)
			{
				Draw* drawCommand = commandBucket.addCommand<Draw>(sizeof(Renderer::IndirectBuffer));

				// TODO(co) I'am sure there's a more elegant way to do this
				Renderer::IndirectBuffer* indirectBuffer = reinterpret_cast<Renderer::IndirectBuffer*>(commandPacket::GetAuxiliaryMemory(drawCommand));
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
			inline static void create(CommandBucket& commandBucket, const Renderer::IIndirectBuffer& indirectBuffer, uint32_t indirectBufferOffset, uint32_t numberOfDraws)
			{
				*commandBucket.addCommand<DrawIndexed>() = DrawIndexed(indirectBuffer, indirectBufferOffset, numberOfDraws);
			}
			inline static void create(CommandBucket& commandBucket, uint32_t indexCountPerInstance, uint32_t instanceCount = 1, uint32_t startIndexLocation = 0, int32_t baseVertexLocation = 0, uint32_t startInstanceLocation = 0)
			{
				DrawIndexed* drawCommand = commandBucket.addCommand<DrawIndexed>(sizeof(Renderer::IndexedIndirectBuffer));

				// TODO(co) I'am sure there's a more elegant way to do this
				Renderer::IndexedIndirectBuffer* indexedIndirectBuffer = reinterpret_cast<Renderer::IndexedIndirectBuffer*>(commandPacket::GetAuxiliaryMemory(drawCommand));
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
			inline static void create(CommandBucket& commandBucket, const wchar_t* name)
			{
				*commandBucket.addCommand<SetDebugMarker>() = SetDebugMarker(name);
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
			inline static void create(CommandBucket& commandBucket, const wchar_t* name)
			{
				*commandBucket.addCommand<BeginDebugEvent>() = BeginDebugEvent(name);
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
			inline static void create(CommandBucket& commandBucket)
			{
				commandBucket.addCommand<EndDebugEvent>();
			}
			// Static data
			RENDERERRUNTIME_API_EXPORT static const BackendDispatchFunction DISPATCH_FUNCTION;
		};


	}


	// TODO(co) Move the following macros into the right place when finishing the command bucket implementation
	#define RENDERER_SET_DEBUG_MARKER2(commandBucket, name) RendererRuntime::Command::SetDebugMarker::create(commandBucket, name);
	#define RENDERER_SET_DEBUG_MARKER_FUNCTION2(commandBucket) RendererRuntime::Command::SetDebugMarker::create(commandBucket, RENDERER_INTERNAL__WFUNCTION__);
	#define RENDERER_BEGIN_DEBUG_EVENT2(commandBucket, name) RendererRuntime::Command::BeginDebugEvent::create(commandBucket, name);
	#define RENDERER_BEGIN_DEBUG_EVENT_FUNCTION2(commandBucket) RendererRuntime::Command::BeginDebugEvent::create(commandBucket, RENDERER_INTERNAL__WFUNCTION__);
	#define RENDERER_END_DEBUG_EVENT2(commandBucket) RendererRuntime::Command::EndDebugEvent::create(commandBucket);


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
