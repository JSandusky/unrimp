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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererToolkit/AssetCompiler/TextureAssetCompiler.h"
#include "RendererToolkit/PlatformTypes.h"

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4005)	// warning C4005: '<x>': macro redefinition
	#pragma warning(disable: 4365)	// warning C4365: 'argument': conversion from '<x>' to '<y>', signed/unsigned mismatch
	#pragma warning(disable: 4548)	// warning C4548: expression before comma has no effect; expected expression with side-effect
	#pragma warning(disable: 4619)	// warning C4619: #pragma warning: there is no warning number '<x>'
	#pragma warning(disable: 4668)	// warning C4668: '<x>' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#include <nvtt/nvtt.h>
	#include <nvcore/Debug.h>
	#include <nvcore/StdStream.h>
#pragma warning(pop)

// Disable warnings in external headers, we can't fix them
#pragma warning(push)
	#pragma warning(disable: 4127)	// warning C4127: conditional expression is constant
	#pragma warning(disable: 4244)	// warning C4244: 'argument': conversion from '<x>' to '<y>', possible loss of data
	#pragma warning(disable: 4266)	// warning C4266: '<x>': no override available for virtual member function from base '<y>'; function is hidden
	#pragma warning(disable: 4365)	// warning C4365: 'return': conversion from '<x>' to '<y>', signed/unsigned mismatch
	#pragma warning(disable: 4548)	// warning C4548: expression before comma has no effect; expected expression with side-effect
	#pragma warning(disable: 4571)	// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
	#pragma warning(disable: 4619)	// warning C4619: #pragma warning: there is no warning number '<x>'
	#pragma warning(disable: 4668)	// warning C4668: '<x>' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	#include <Poco/JSON/Parser.h>
#pragma warning(pop)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Classes                                               ]
		//[-------------------------------------------------------]
		class NvttMessageHandler : public nv::MessageHandler
		{


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			NvttMessageHandler()
			{
				nv::debug::setMessageHandler(this);
			}

			virtual ~NvttMessageHandler()
			{
				nv::debug::resetMessageHandler();
			}


		//[-------------------------------------------------------]
		//[ Public virtual nv::MessageHandler methods             ]
		//[-------------------------------------------------------]
		public:
			virtual void log(const char* str, va_list arg) override
			{
				RENDERERTOOLKIT_OUTPUT_DEBUG_PRINTF(str, arg);
			}


		};

		class NvttAssertHandler : public nv::AssertHandler
		{


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			NvttAssertHandler()
			{
				nv::debug::setAssertHandler(this);
				nv::debug::enableSigHandler(true);
			}

			virtual ~NvttAssertHandler()
			{
				nv::debug::resetAssertHandler();
				nv::debug::disableSigHandler();
			}


		//[-------------------------------------------------------]
		//[ Public virtual nv::AssertHandler methods              ]
		//[-------------------------------------------------------]
		public:
			virtual int assertion(const char* exp, const char* file, int line, const char* func, const char* msg, va_list arg) override
			{
				RENDERERTOOLKIT_OUTPUT_DEBUG_PRINTF("NVTT assertion failed: \"%s\"\nFile \"%s\" line %d\nFunction \"%s\"\n", exp, file, line, func);
				RENDERERTOOLKIT_OUTPUT_DEBUG_PRINTF(msg, arg);
				nv::debug::dumpInfo();
				exit(1);
			}


		};

		class NvStream : public nv::Stream
		{


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			NvStream(std::istream& istream) :
				mIstream(istream)
			{
				// Nothing here
			}

			virtual ~NvStream()
			{
				// Nothing here
			}


		//[-------------------------------------------------------]
		//[ Public virtual nv::Stream methods                     ]
		//[-------------------------------------------------------]
		public:
			virtual uint serialize(void* data, uint len) override
			{
				const std::streampos previousPosition = mIstream.tellg();
				mIstream.read(static_cast<char*>(data), len);
				return static_cast<uint>(mIstream.tellg() - previousPosition);
			}

			virtual void seek(uint pos) override
			{
				mIstream.seekg(pos);
			}

			virtual uint tell() const override
			{
				return static_cast<uint>(mIstream.tellg());
			}

			virtual uint size() const override
			{
				const std::streampos currentPosition = mIstream.tellg();
				mIstream.seekg(0, std::istream::end);
				const uint endPosition = static_cast<uint>(mIstream.tellg());
				mIstream.seekg(currentPosition, std::istream::beg);

				return endPosition;
			}

			virtual bool isError() const override
			{
				return false;
			}

			virtual void clearError() override
			{
				// Nothing here
			}

			virtual bool isAtEnd() const override
			{
				const std::streampos currentPosition = mIstream.tellg();
				mIstream.seekg(0, std::istream::end);
				const std::streampos endPosition = mIstream.tellg();
				mIstream.seekg(currentPosition, std::istream::beg);

				return (endPosition == currentPosition);
			}

			virtual bool isSeekable() const override
			{
				return true;
			}

			virtual bool isLoading() const override
			{
				return true;
			}

			virtual bool isSaving() const override
			{
				return false;
			}


		//[-------------------------------------------------------]
		//[ Private methods                                       ]
		//[-------------------------------------------------------]
		private:
			explicit NvStream(NvStream& other) :
				mIstream(other.mIstream)
			{
				// Nothing here
			}

			NvStream& operator=(NvStream&)
			{
				// Nothing here
			}


		//[-------------------------------------------------------]
		//[ Private data                                          ]
		//[-------------------------------------------------------]
		private:
			std::istream& mIstream;


		};

		class NvttOutputHandler : public nvtt::OutputHandler
		{


		//[-------------------------------------------------------]
		//[ Public methods                                        ]
		//[-------------------------------------------------------]
		public:
			explicit NvttOutputHandler(std::ostream& ostream) :
				mOstream(ostream)
			{
				// Nothing here
			}

			virtual ~NvttOutputHandler()
			{
				// Nothing here
			}


		//[-------------------------------------------------------]
		//[ Public virtual nv::OutputHandler methods              ]
		//[-------------------------------------------------------]
		public:
			virtual void beginImage(int, int, int, int, int, int) override
			{
				// Nothing here
			}

			virtual bool writeData(const void* data, int size) override
			{
				mOstream.write(static_cast<const char*>(data), size);
				return true;
			}

			virtual void endImage() override
			{
				// Nothing here
			}


		//[-------------------------------------------------------]
		//[ Private methods                                       ]
		//[-------------------------------------------------------]
		private:
			explicit NvttOutputHandler(NvttOutputHandler& other) :
				mOstream(other.mOstream)
			{
				// Nothing here
			}

			NvttOutputHandler& operator=(NvttOutputHandler&)
			{
				// Nothing here
			}


		//[-------------------------------------------------------]
		//[ Private data                                          ]
		//[-------------------------------------------------------]
		private:
			std::ostream& mOstream;


		};



	}


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TextureAssetCompiler::TextureAssetCompiler()
	{
		// Nothing here
	}

	TextureAssetCompiler::~TextureAssetCompiler()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	bool TextureAssetCompiler::compile(std::istream& istream, std::ostream& ostream, std::istream& jsonConfiguration)
	{
		// Read configuration
		// TODO(co) Add required properties
		uint32_t test = 0;
		{
			// Parse JSON
			Poco::JSON::Parser jsonParser;
			jsonParser.parse(jsonConfiguration);
			Poco::JSON::Object::Ptr jsonRootObject = jsonParser.result().extract<Poco::JSON::Object::Ptr>();
		
			{ // Check whether or not the configuration format matches
				Poco::JSON::Object::Ptr jsonFormatObject = jsonRootObject->get("Format").extract<Poco::JSON::Object::Ptr>();
				if (jsonFormatObject->get("Type").convert<std::string>() != "Asset")
				{
					throw std::exception("Invalid JSON format type, must be \"Asset\"");
				}
				if (jsonFormatObject->get("Version").convert<uint32_t>() != 1)
				{
					throw std::exception("Invalid JSON format version, must be 1");
				}
			}

			// Read configuration
			Poco::JSON::Object::Ptr jsonConfigurationObject = jsonRootObject->get("TextureAssetCompiler").extract<Poco::JSON::Object::Ptr>();
			test = jsonConfigurationObject->optValue<uint32_t>("Test", test);
		}

		// Initialize context, no CUDA support at the moment because it makes things more complicated to build
		nvtt::Context context;

		// Custom NVTT message and assert handler
		detail::NvttMessageHandler nvttMessageHandler;
		detail::NvttAssertHandler nvttAssertHandler;

		// Create the surface we'll work with
		nvtt::Surface image;
		detail::NvStream nvStream(istream);
		if (!image.load("", nvStream))
		{
			throw std::exception("Could not load input texture");
		}

		// DDS textures will be y-flipped
		// -> We don't want to care about something like this during runtime, so take care of it during asset compile time
		image.flipY();

		// Setup compression options, use the highest quality which makes sense
		// -> NVTT documentation says:
		//   "
		//   Quality_Highest is a brute force compressor. In some cases, depending on the size of the search space, this compressor will be extremely slow.
		//   Use this only for testing purposes, to determine how much room is left for improvement in the regular compressors.
		//   "
		// -> According to https://code.google.com/p/nvidia-texture-tools/wiki/ApiDocumentation#Compression_Quality "nvtt::Quality_Highest" is not supported anyway
		nvtt::CompressionOptions compressionOptions;
		compressionOptions.setQuality(nvtt::Quality_Production);
		compressionOptions.setFormat(nvtt::Format_BC1);

		// Setup output options
		detail::NvttOutputHandler nvttOutputHandler(ostream);
		nvtt::OutputOptions outputOptions;
		outputOptions.setOutputHandler(&nvttOutputHandler);

		// Output compressed image
		bool generateMipmaps = true;
		if (!context.outputHeader(image, generateMipmaps ? image.countMipmaps() : 1, compressionOptions, outputOptions))
		{
			throw std::exception("Could not set output header");
		}

		// Output first mipmap
		context.compress(image, 0, 0, compressionOptions, outputOptions);

		// Generate mipmaps
		const int imageWidth = image.width();
		const int imageHeight = image.height();
		int numberOfImageMipmaps = 0;
		if (generateMipmaps)
		{
			// Estimate original coverage
			// -> See http://the-witness.net/news/2010/09/computing-alpha-mipmaps/ for background information about alpha test coverage
			// -> For terrain materials, we put the height map into the alpha channel of the diffuse map. We noticed an pretty odd mipmap issue which
			//    could also be seen when reviewing the mipmaps inside an image editing application. The first mipmap looked fine, while the next was suddenly
			//    quite darker. "scale alpha to coverage" feature needs to be deactivated if the alpha channel does not store information used for alpha rejection.
			// -> After switching to the material-texture-compositing, the famous "trees look now odd and one can see the polygons"-issue (TM) came up. Also,
			//    when moving the camera away from stuff using alpha maps, the alpha pretty fast disappeared and a solid polyon remained. Turned out to be
			//    "scale alpha to coverage" and when deactivating the feature all looks ok, again.
			// -> We don't use "scale alpha to coverage" during material-texture-compositing because it has to many pitfalls
			// -> Don't delete this comment so we don't step into this issue, again
		//	const float alphaRef = getConfig().alphaCoverageThreshold;
		//	const float coverage = hasAlpha ? image.alphaTestCoverage(alphaRef) : 0.0f;

			// Build mipmaps
			numberOfImageMipmaps = 1;
			while (image.buildNextMipmap(nvtt::MipmapFilter_Kaiser))
			{
				nvtt::Surface tempImage = image;

				// Build mipmaps and scale alpha to preserve original coverage
				// -> See "Estimate original coverage"-comment above for the background
				// -> Don't delete this comment so we don't step into this issue, again
			//	if (hasAlpha)
			//	{
			//		tempImage.scaleAlphaToCoverage(coverage, alphaRef);
			//	}

				context.compress(tempImage, 0, numberOfImageMipmaps, compressionOptions, outputOptions);
				++numberOfImageMipmaps;
			}
		}

		// Done
		return true;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
