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


// TODO(co) Due to the HLSL compiler usage, this is currently MS Windows only (maybe there are Linux HLSL cross-compilers?) (see also "RendererToolkit::RendererToolkitImpl::createShaderAssetCompiler()")


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererToolkit/AssetCompiler/ShaderAssetCompiler.h"
#include "RendererToolkit/PlatformTypes.h"

#include <RendererRuntime/Manager/Asset/AssetPackage.h>

#define DIRECT3D11_DEFINERUNTIMELINKING
#include <Direct3D11Renderer/D3D11.h>

#define EXCLUDE_PSTDINT
#include <HLSLCrossCompiler/hlslcc.hpp>

#include <memory>
#include <fstream>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
typedef __interface ID3D10Blob *LPD3D10BLOB;	// "__interface" is no keyword of the ISO C++ standard, shouldn't be a problem because this in here is MS Windows only and it's also within the Direct3D headers we have to use
typedef ID3D10Blob ID3DBlob;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	namespace detail
	{
		void *malloc_hook(size_t size)
		{
			return malloc(size);
		}
		void *calloc_hook(size_t num, size_t size)
		{
			return calloc(num,size);
		}
		void *realloc_hook(void *p, size_t size)
		{
			return realloc(p,size);
		}
		void free_hook(void *p)
		{
			free(p);
		}
	}


	//[-------------------------------------------------------]
	//[ D3DCompiler functions                                 ]
	//[-------------------------------------------------------]
	#ifdef DIRECT3D11_DEFINERUNTIMELINKING
		#define FNDEF_D3DCOMPILER(retType, funcName, args) retType (WINAPI *funcPtr_##funcName) args
	#else
		#define FNDEF_D3DCOMPILER(retType, funcName, args) extern retType (WINAPI *funcPtr_##funcName) args
	#endif
	FNDEF_D3DCOMPILER(HRESULT,	D3DCompile,	(LPCSTR, SIZE_T, LPCSTR, CONST D3D_SHADER_MACRO *, ID3DInclude *, LPCSTR, LPCSTR, UINT, UINT, ID3D10Blob **, ID3D10Blob **, HRESULT *));


	//[-------------------------------------------------------]
	//[ Macros & definitions                                  ]
	//[-------------------------------------------------------]
	#ifndef FNPTR
		#define FNPTR(name) funcPtr_##name
	#endif

	// Redirect D3DCompiler* function calls to funcPtr_D3DCompiler*

	// D3DCompiler
	#define D3DCompile	FNPTR(D3DCompile)


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	ShaderAssetCompiler::ShaderAssetCompiler() :
		mD3DCompilerSharedLibrary(nullptr)
	{
		// Load the shared library
		mD3DCompilerSharedLibrary = ::LoadLibraryExA("D3DCompiler_47.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (nullptr == mD3DCompilerSharedLibrary)
		{
			RENDERERTOOLKIT_OUTPUT_DEBUG_STRING("Direct3D 11 error: Failed to load in the shared library \"D3DCompiler_47.dll\"\n")
		}

		{ // Load the required D3DCompiler shader compiler entry point
			bool result = true;	// Success by default

			// Define a helper macro
			#define IMPORT_FUNC(funcName)																																							\
				if (result)																																											\
				{																																													\
					void *symbol = ::GetProcAddress(static_cast<HMODULE>(mD3DCompilerSharedLibrary), #funcName);																					\
					if (nullptr != symbol)																																							\
					{																																												\
						*(reinterpret_cast<void**>(&(funcName))) = symbol;																															\
					}																																												\
					else																																											\
					{																																												\
						wchar_t moduleFilename[MAX_PATH];																																			\
						moduleFilename[0] = '\0';																																					\
						::GetModuleFileNameW(static_cast<HMODULE>(mD3DCompilerSharedLibrary), moduleFilename, MAX_PATH);																			\
						RENDERERTOOLKIT_OUTPUT_DEBUG_PRINTF("Direct3D 11 error: Failed to locate the entry point \"%s\" within the Direct3D 11 shared library \"%s\"", #funcName, moduleFilename)	\
						result = false;																																								\
					}																																												\
				}

			// Load the entry points
			IMPORT_FUNC(D3DCompile);

			// Undefine the helper macro
			#undef IMPORT_FUNC
		}

		// Tell the HLSL cross compiler about memory handling functions to use
		HLSLcc_SetMemoryFunctions(detail::malloc_hook, detail::calloc_hook, detail::free_hook, detail::realloc_hook);
	}

	ShaderAssetCompiler::~ShaderAssetCompiler()
	{
		// Destroy the shared library instances
		if (nullptr != mD3DCompilerSharedLibrary)
		{
			::FreeLibrary(static_cast<HMODULE>(mD3DCompilerSharedLibrary));
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	void ShaderAssetCompiler::compile(const Input& input, const Configuration& configuration, Output& output)
	{
		// Input, configuration and output
		const std::string&			   assetInputDirectory	= input.assetInputDirectory;
		const std::string&			   assetOutputDirectory	= input.assetOutputDirectory;
		Poco::JSON::Object::Ptr		   jsonAssetRootObject	= configuration.jsonAssetRootObject;
		RendererRuntime::AssetPackage& outputAssetPackage	= *output.outputAssetPackage;

		// Get the JSON asset object
		Poco::JSON::Object::Ptr jsonAssetObject = jsonAssetRootObject->get("Asset").extract<Poco::JSON::Object::Ptr>();

		// Read configuration
		// TODO(co) Add required properties
		std::string inputFile;
		std::string entryPoint = "main";
		std::string shaderModel = "vs_5_0";
		{
			// Read shader asset compiler configuration
			Poco::JSON::Object::Ptr jsonConfigurationObject = jsonAssetObject->get("ShaderAssetCompiler").extract<Poco::JSON::Object::Ptr>();
			inputFile   = jsonConfigurationObject->getValue<std::string>("InputFile");
			entryPoint  = jsonConfigurationObject->optValue<std::string>("EntryPoint", entryPoint);
			shaderModel = jsonConfigurationObject->optValue<std::string>("ShaderModel", shaderModel);
		}

		// Open the input file
		std::ifstream ifstream(assetInputDirectory + inputFile, std::ios::binary);
		const std::string assetName = jsonAssetObject->get("AssetMetadata").extract<Poco::JSON::Object::Ptr>()->getValue<std::string>("AssetName");
		const std::string assetFilename = assetOutputDirectory + assetName + ".shader";
		std::ofstream ofstream(assetFilename, std::ios::binary);

		{ // Shader
			// Parse JSON
			Poco::JSON::Parser jsonParser;
			jsonParser.parse(ifstream);
			Poco::JSON::Object::Ptr jsonRootObject = jsonParser.result().extract<Poco::JSON::Object::Ptr>();
		
			{ // Check whether or not the file format matches
				Poco::JSON::Object::Ptr jsonFormatObject = jsonRootObject->get("Format").extract<Poco::JSON::Object::Ptr>();
				if (jsonFormatObject->get("Type").convert<std::string>() != "ShaderAsset")
				{
					throw std::exception("Invalid JSON format type, must be \"ShaderAsset\"");
				}
				if (jsonFormatObject->get("Version").convert<uint32_t>() != 1)
				{
					throw std::exception("Invalid JSON format version, must be 1");
				}
			}

			Poco::JSON::Object::Ptr jsonShaderObject = jsonRootObject->get("ShaderAsset").extract<Poco::JSON::Object::Ptr>();
			Poco::JSON::Object::Ptr jsonShaderPropertiesObject = jsonShaderObject->get("Properties").extract<Poco::JSON::Object::Ptr>();
			Poco::JSON::Object::Ptr jsonShaderTechniqueObject = jsonShaderObject->get("Technique").extract<Poco::JSON::Object::Ptr>();
			Poco::JSON::Object::Ptr jsonShaderTechniqueTagsObject = jsonShaderTechniqueObject->get("Tags").extract<Poco::JSON::Object::Ptr>();
			std::string tagTest = jsonShaderTechniqueTagsObject->optValue<std::string>("Test", "");
			std::string sourceCode = jsonShaderTechniqueObject->optValue<std::string>("SourceCode", "");
			NOP;

		}




		entryPoint = "VertexShaderMain";
		shaderModel = "vs_5_0";


		// Load in the shader source code
		std::unique_ptr<char[]> buffer;
		ifstream.seekg(0, std::ifstream::end);
		const size_t numberOfBytes = static_cast<size_t>(ifstream.tellg());
		ifstream.seekg(0, std::ifstream::beg);
		buffer = std::unique_ptr<char[]>(new char[numberOfBytes]);
		ifstream.read(buffer.get(), numberOfBytes);

		// TODO(co)
		/*
		// Compile the HLSL source code
		// TODO(co) Cleanup
		ID3DBlob* d3dBlobVertexShader = nullptr;
		{
			HRESULT hr = S_OK;

			DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
			#if defined( DEBUG ) || defined( _DEBUG )
				// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
				// Setting this flag improves the shader debugging experience, but still allows 
				// the shaders to be optimized and to run exactly the way they will run in 
				// the release configuration of this program.
		//		shaderFlags |= D3DCOMPILE_DEBUG;
			#endif

			ID3DBlob *errorBlob;
			hr = D3DCompile(buffer.get(), numberOfBytes, nullptr, nullptr, nullptr, entryPoint.c_str(), shaderModel.c_str(), 
				shaderFlags, 0, &d3dBlobVertexShader, &errorBlob, nullptr );

			if (FAILED(hr))
			{
				if( errorBlob != nullptr )
					OutputDebugStringA( (char*)errorBlob->GetBufferPointer() );
				if( errorBlob ) errorBlob->Release();
				// TODO(co) Better error handling
				throw std::exception("Shader asset compiler failed to compile the shader");
			}
			if (nullptr != errorBlob ) errorBlob->Release();
		}
		
		// Compiled HLSL
//		ofstream.write(static_cast<char*>(d3dBlobVertexShader->GetBufferPointer()), d3dBlobVertexShader->GetBufferSize());

		GlExtensions ext;
		ext.ARB_explicit_attrib_location = 0;
		ext.ARB_explicit_uniform_location = 0;
		ext.ARB_shading_language_420pack = 0;
		GLSLShader result;
		TranslateHLSLFromMem(static_cast<const char*>(d3dBlobVertexShader->GetBufferPointer()), 0, LANG_150, &ext, nullptr, &result);
		ofstream.write(result.sourceCode, strlen(result.sourceCode));
		FreeGLSLShader(&result);

		// Release the Direct3D 11 shader binary large object
		d3dBlobVertexShader->Release();
		*/

		// TODO(co) Implement me


		{ // Update the output asset package
			const std::string assetCategory = jsonAssetObject->get("AssetMetadata").extract<Poco::JSON::Object::Ptr>()->getValue<std::string>("AssetCategory");
			const std::string assetIdAsString = input.projectName + "/Shader/" + assetCategory + '/' + assetName;

			// Output asset
			RendererRuntime::Asset outputAsset;
			outputAsset.assetId = RendererRuntime::StringId(assetIdAsString.c_str());
			strcpy(outputAsset.assetFilename, assetFilename.c_str());	// TODO(co) Buffer overflow test
			outputAssetPackage.getWritableSortedAssetVector().push_back(outputAsset);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
