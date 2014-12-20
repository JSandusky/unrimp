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


// TODO(co) Work in progress


// Basing on
// http://molecularmusings.wordpress.com/2014/11/06/stateless-layered-multi-threaded-rendering-part-1/ - "Stateless, layered, multi-threaded rendering – Part 1"
// http://molecularmusings.wordpress.com/2014/11/06/stateless-layered-multi-threaded-rendering-part-2/ - "Stateless, layered, multi-threaded rendering – Part 2"
// http://molecularmusings.wordpress.com/2014/11/06/stateless-layered-multi-threaded-rendering-part-3/ - "Stateless, layered, multi-threaded rendering – Part 3"
// http://realtimecollisiondetection.net/blog/?p=86 - "Order your graphics draw calls around!"


#include "Material.h"


namespace RendererToolkit
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




	namespace Command
	{
		struct Draw
		{
			static const BackendDispatchFunction DISPATCH_FUNCTION;

			// Input-assembler (IA) stage
			Renderer::IVertexArray*			  iaVertexArray;
			Renderer::PrimitiveTopology::Enum iaPrimitiveTopology;
			// Material
			Material* material;
			// Draw call specific
			uint32_t startVertexLocation;
			uint32_t numberOfVertices;
		};
		static_assert(std::is_pod<Draw>::value == true, "Draw must be a POD");

		struct DrawIndexed
		{
			static const BackendDispatchFunction DISPATCH_FUNCTION;

			// Input-assembler (IA) stage
			Renderer::IVertexArray*			  iaVertexArray;
			Renderer::PrimitiveTopology::Enum iaPrimitiveTopology;
			// Material
			Material* material;
			// Draw call specific
			uint32_t startIndexLocation;
			uint32_t numberOfIndices;
			uint32_t baseVertexLocation;
		};
		static_assert(std::is_pod<DrawIndexed>::value == true, "DrawIndexed must be a POD");

		struct CopyUniformBufferData
		{
			static const BackendDispatchFunction DISPATCH_FUNCTION;

			Renderer::IUniformBuffer* uniformBufferDynamicVs;
			uint32_t size;
			void* data;
		};
		static_assert(std::is_pod<CopyUniformBufferData>::value == true, "CopyUniformBufferData must be a POD");

	}









	template <typename T>
	class CommandBucket
	{
		typedef T Key;
	public:
		CommandBucket(uint16_t maximumNumberOfCommandPackages) :
			mMaximumNumberOfCommandPackages(maximumNumberOfCommandPackages),
			mKeys(new T[mMaximumNumberOfCommandPackages]),
			mCommandPackets(new CommandPacket[mMaximumNumberOfCommandPackages]),
			mCurrentNumberOfCommandPackages(0)
		{
		}
		~CommandBucket()
		{
			delete [] mKeys;
			delete [] mCommandPackets;
		}
		template <typename U>
		U* addCommand(Key key, size_t auxMemorySize = 0)
		{
			CommandPacket packet = commandPacket::Create<U>(auxMemorySize);

			// store key and pointer to the data
			{
				// TODO: add some kind of lock or atomic operation here
				const unsigned int current = mCurrentNumberOfCommandPackages++;
				mKeys[current] = key;
				mCommandPackets[current] = packet;
			}

			commandPacket::StoreNextCommandPacket(packet, nullptr);
			commandPacket::StoreBackendDispatchFunction(packet, U::DISPATCH_FUNCTION);

			return commandPacket::GetCommand<U>(packet);
		}
		template <typename U, typename V>
		U* appendCommand(V* command, size_t auxMemorySize = 0)
		{
			CommandPacket packet = commandPacket::Create<U>(auxMemorySize);

			// append this command to the given one
			commandPacket::StoreNextCommandPacket<V>(command, packet);

			commandPacket::StoreNextCommandPacket(packet, nullptr);
			commandPacket::StoreBackendDispatchFunction(packet, U::DISPATCH_FUNCTION);

			return commandPacket::GetCommand<U>(packet);
		}
		void sort()
		{
		}
		void submit(Renderer::IRenderer& renderer)
		{
			/*
			SetViewMatrix();
			SetProjectionMatrix();
			SetRenderTargets();
			*/

			for (unsigned int i=0; i < mCurrentNumberOfCommandPackages; ++i)
			{
				CommandPacket packet = mCommandPackets[i];
				do
				{
					submitPacket(packet, renderer);
					CommandPacket previousPacket = packet;
					packet = commandPacket::LoadNextCommandPacket(packet);

					// TODO Memory management
					delete previousPacket;

				} while (packet != nullptr);
			}

			mCurrentNumberOfCommandPackages = 0;
		}
	private:
		void submitPacket(const CommandPacket packet, Renderer::IRenderer& renderer)
		{
			const BackendDispatchFunction function = commandPacket::LoadBackendDispatchFunction(packet);
			const void* command = commandPacket::LoadCommand(packet);
			function(command, renderer);
		}
	private:
		uint16_t	   mMaximumNumberOfCommandPackages;
		Key*		   mKeys;
		CommandPacket* mCommandPackets;
		uint16_t	   mCurrentNumberOfCommandPackages;

	};





}
