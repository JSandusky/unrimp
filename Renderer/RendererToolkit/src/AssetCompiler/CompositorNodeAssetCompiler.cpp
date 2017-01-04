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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererToolkit/AssetCompiler/CompositorNodeAssetCompiler.h"
#include "RendererToolkit/Helper/JsonMaterialBlueprintHelper.h"
#include "RendererToolkit/Helper/JsonMaterialHelper.h"
#include "RendererToolkit/Helper/JsonHelper.h"

#include <RendererRuntime/Asset/AssetPackage.h>
#include <RendererRuntime/Resource/Material/MaterialResourceManager.h>
#include <RendererRuntime/Resource/CompositorNode/Loader/CompositorNodeFileFormat.h>
#include <RendererRuntime/Resource/CompositorNode/Pass/Quad/CompositorResourcePassQuad.h>
#include <RendererRuntime/Resource/CompositorNode/Pass/Clear/CompositorResourcePassClear.h>
#include <RendererRuntime/Resource/CompositorNode/Pass/DebugGui/CompositorResourcePassDebugGui.h>
#include <RendererRuntime/Resource/CompositorNode/Pass/ShadowMap/CompositorResourcePassShadowMap.h>
#include <RendererRuntime/Resource/CompositorNode/Pass/ResolveMultisample/CompositorResourcePassResolveMultisample.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'rapidjson::GenericMember<Encoding,Allocator>': copy constructor was implicitly defined as deleted
	#include <rapidjson/document.h>
PRAGMA_WARNING_POP

#include <string>
#include <fstream>
#include <unordered_set>


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		void fillSortedMaterialPropertyVector(const RendererToolkit::IAssetCompiler::Input& input, const std::unordered_set<uint32_t>& renderTargetTextureAssetIds, const rapidjson::Value::ConstMemberIterator& rapidJsonMemberIteratorPasses, RendererRuntime::MaterialProperties::SortedPropertyVector& sortedMaterialPropertyVector)
		{
			// Check whether or not material properties should be set
			if (rapidJsonMemberIteratorPasses->value.HasMember("SetMaterialProperties"))
			{
				const rapidjson::Value& rapidJsonValuePass = rapidJsonMemberIteratorPasses->value;
				if (rapidJsonValuePass.HasMember("MaterialAssetId"))
				{
					RendererToolkit::JsonMaterialHelper::getPropertiesByMaterialAssetId(input, static_cast<uint32_t>(std::atoi(rapidJsonValuePass["MaterialAssetId"].GetString())), sortedMaterialPropertyVector);
				}
				else if (rapidJsonValuePass.HasMember("MaterialBlueprintAssetId"))
				{
					RendererToolkit::JsonMaterialBlueprintHelper::getPropertiesByMaterialBlueprintAssetId(input, static_cast<uint32_t>(std::atoi(rapidJsonValuePass["MaterialBlueprintAssetId"].GetString())), sortedMaterialPropertyVector);
				}
				if (!sortedMaterialPropertyVector.empty())
				{
					// Update material property values were required
					const rapidjson::Value& rapidJsonValueProperties = rapidJsonMemberIteratorPasses->value["SetMaterialProperties"];
					RendererToolkit::JsonMaterialHelper::readMaterialPropertyValues(input, rapidJsonValueProperties, sortedMaterialPropertyVector);

					{ // Need a second round for referenced render target textures so we can write e.g. "ColorMap": "ColorRenderTargetTexture0" ("ColorRenderTargetTexture0" = render target texture)
						// Collect all material property IDs explicitly defined inside the compositor node asset
						typedef std::unordered_map<uint32_t, std::string> DefinedMaterialPropertyIds;	// Key = "RendererRuntime::RendererRuntime::MaterialPropertyId"
						DefinedMaterialPropertyIds definedMaterialPropertyIds;
						for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorProperties = rapidJsonValueProperties.MemberBegin(); rapidJsonMemberIteratorProperties != rapidJsonValueProperties.MemberEnd(); ++rapidJsonMemberIteratorProperties)
						{
							definedMaterialPropertyIds.emplace(RendererRuntime::MaterialPropertyId(rapidJsonMemberIteratorProperties->name.GetString()), rapidJsonMemberIteratorProperties->value.GetString());
						}

						// Mark material properties as overwritten and update texture asset IDs if necessary
						for (RendererRuntime::MaterialProperty& materialProperty : sortedMaterialPropertyVector)
						{
							DefinedMaterialPropertyIds::const_iterator iterator = definedMaterialPropertyIds.find(materialProperty.getMaterialPropertyId());
							if (iterator!= definedMaterialPropertyIds.end())
							{
								materialProperty.setOverwritten(true);
								if (materialProperty.getValueType() == RendererRuntime::MaterialPropertyValue::ValueType::TEXTURE_ASSET_ID)
								{
									const RendererRuntime::AssetId assetId = RendererRuntime::StringId(iterator->second.c_str());
									if (renderTargetTextureAssetIds.find(assetId) != renderTargetTextureAssetIds.end())
									{
										static_cast<RendererRuntime::MaterialPropertyValue&>(materialProperty) = RendererRuntime::MaterialPropertyValue::fromTextureAssetId(assetId);
									}
								}
							}
						}
					}
				}
			}
		}

		void readPassQuad(const RendererToolkit::IAssetCompiler::Input& input, const RendererRuntime::MaterialProperties::SortedPropertyVector& sortedMaterialPropertyVector, const rapidjson::Value& rapidJsonValuePass, bool materialDefinitionMandatory, RendererRuntime::v1CompositorNode::PassQuad& passQuad)
		{
			// Set data
			RendererRuntime::AssetId materialAssetId;
			RendererRuntime::AssetId materialBlueprintAssetId;
			RendererToolkit::JsonHelper::optionalCompiledAssetId(input, rapidJsonValuePass, "MaterialAssetId", materialAssetId);
			RendererToolkit::JsonHelper::optionalStringIdProperty(rapidJsonValuePass, "MaterialTechnique", passQuad.materialTechniqueId);
			RendererToolkit::JsonHelper::optionalCompiledAssetId(input, rapidJsonValuePass, "MaterialBlueprintAssetId", materialBlueprintAssetId);
			passQuad.materialAssetId = materialAssetId;
			passQuad.materialBlueprintAssetId = materialBlueprintAssetId;
			passQuad.numberOfMaterialProperties = sortedMaterialPropertyVector.size();

			// Sanity checks
			if (materialDefinitionMandatory && RendererRuntime::isUninitialized(passQuad.materialAssetId) && RendererRuntime::isUninitialized(passQuad.materialBlueprintAssetId))
			{
				throw std::runtime_error("Material asset ID or material blueprint asset ID must be defined");
			}
			if (RendererRuntime::isInitialized(passQuad.materialAssetId) && RendererRuntime::isInitialized(passQuad.materialBlueprintAssetId))
			{
				throw std::runtime_error("Material asset ID is defined, but material blueprint asset ID is defined as well. Only one asset ID is allowed.");
			}
			if (RendererRuntime::isInitialized(passQuad.materialAssetId) && RendererRuntime::isUninitialized(passQuad.materialTechniqueId))
			{
				throw std::runtime_error("Material asset ID is defined, but material technique is not defined");
			}
			if (RendererRuntime::isInitialized(passQuad.materialBlueprintAssetId) && RendererRuntime::isUninitialized(passQuad.materialTechniqueId))
			{
				passQuad.materialTechniqueId = RendererRuntime::MaterialResourceManager::DEFAULT_MATERIAL_TECHNIQUE_ID;
			}
		}

		void readPassScene(const rapidjson::Value& rapidJsonValuePass, RendererRuntime::v1CompositorNode::PassScene& passScene)
		{
			// Read properties
			RendererToolkit::JsonHelper::optionalByteProperty(rapidJsonValuePass, "MinimumRenderQueueIndex", passScene.minimumRenderQueueIndex);
			RendererToolkit::JsonHelper::optionalByteProperty(rapidJsonValuePass, "MaximumRenderQueueIndex", passScene.maximumRenderQueueIndex);
			RendererToolkit::JsonHelper::optionalBooleanProperty(rapidJsonValuePass, "TransparentPass", passScene.transparentPass);
			RendererToolkit::JsonHelper::mandatoryStringIdProperty(rapidJsonValuePass, "MaterialTechnique", passScene.materialTechniqueId);

			// Sanity check
			if (passScene.maximumRenderQueueIndex < passScene.minimumRenderQueueIndex)
			{
				throw std::runtime_error("The maximum render queue index must be equal or greater as the minimum render queue index");
			}
			if (passScene.maximumRenderQueueIndex < passScene.minimumRenderQueueIndex)
			{
				throw std::runtime_error("The maximum render queue index must be equal or greater as the minimum render queue index");
			}
		}

		uint32_t getRenderTargetTextureSize(const rapidjson::Value& rapidJsonValueRenderTargetTexture, const char* propertyName, const char* defaultValue)
		{
			uint32_t size = RendererRuntime::getUninitialized<uint32_t>();
			if (rapidJsonValueRenderTargetTexture.HasMember(propertyName))
			{
				const char* valueAsString = rapidJsonValueRenderTargetTexture[propertyName].GetString();
				if (strcmp(valueAsString, defaultValue) != 0)
				{
					size = static_cast<uint32_t>(std::atoi(valueAsString));
				}
			}
			return size;
		}


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const AssetCompilerTypeId CompositorNodeAssetCompiler::TYPE_ID("CompositorNode");


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	CompositorNodeAssetCompiler::CompositorNodeAssetCompiler()
	{
		// Nothing here
	}

	CompositorNodeAssetCompiler::~CompositorNodeAssetCompiler()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	AssetCompilerTypeId CompositorNodeAssetCompiler::getAssetCompilerTypeId() const
	{
		return TYPE_ID;
	}

	void CompositorNodeAssetCompiler::compile(const Input& input, const Configuration& configuration, Output& output)
	{
		// Input, configuration and output
		const std::string&			   assetInputDirectory	= input.assetInputDirectory;
		const std::string&			   assetOutputDirectory = input.assetOutputDirectory;
		RendererRuntime::AssetPackage& outputAssetPackage	= *output.outputAssetPackage;

		// Get the JSON asset object
		const rapidjson::Value& rapidJsonValueAsset = configuration.rapidJsonDocumentAsset["Asset"];

		// Read configuration
		// TODO(co) Add required properties
		std::string inputFile;
		{
			// Read compositor node asset compiler configuration
			const rapidjson::Value& rapidJsonValueCompositorNodeAssetCompiler = rapidJsonValueAsset["CompositorNodeAssetCompiler"];
			inputFile = rapidJsonValueCompositorNodeAssetCompiler["InputFile"].GetString();
		}

		// Open the input file
		const std::string inputFilename = assetInputDirectory + inputFile;
		std::ifstream inputFileStream(inputFilename, std::ios::binary);
		const std::string assetName = rapidJsonValueAsset["AssetMetadata"]["AssetName"].GetString();
		const std::string outputAssetFilename = assetOutputDirectory + assetName + ".compositor_node";
		std::ofstream outputFileStream(outputAssetFilename, std::ios::binary);

		{ // Compositor node
			// Parse JSON
			rapidjson::Document rapidJsonDocument;
			JsonHelper::parseDocumentByInputFileStream(rapidJsonDocument, inputFileStream, inputFilename, "CompositorNodeAsset", "1");

			// Mandatory main sections of the compositor node
			const rapidjson::Value& rapidJsonValueCompositorNodeAsset = rapidJsonDocument["CompositorNodeAsset"];
			const rapidjson::Value& rapidJsonValueInputChannels = rapidJsonValueCompositorNodeAsset["InputChannels"];
			const rapidjson::Value& rapidJsonValueTargets = rapidJsonValueCompositorNodeAsset["Targets"];
			const rapidjson::Value& rapidJsonValueOutputChannels = rapidJsonValueCompositorNodeAsset["OutputChannels"];

			{ // Write down the compositor node resource header
				RendererRuntime::v1CompositorNode::Header compositorNodeHeader;
				compositorNodeHeader.formatType					  = RendererRuntime::v1CompositorNode::FORMAT_TYPE;
				compositorNodeHeader.formatVersion				  = RendererRuntime::v1CompositorNode::FORMAT_VERSION;
				compositorNodeHeader.numberOfInputChannels		  = rapidJsonValueInputChannels.MemberCount();
				compositorNodeHeader.numberOfRenderTargetTextures = rapidJsonValueCompositorNodeAsset.HasMember("RenderTargetTextures") ? rapidJsonValueCompositorNodeAsset["RenderTargetTextures"].MemberCount() : 0;
				compositorNodeHeader.numberOfFramebuffers		  = rapidJsonValueCompositorNodeAsset.HasMember("Framebuffers") ? rapidJsonValueCompositorNodeAsset["Framebuffers"].MemberCount() : 0;
				compositorNodeHeader.numberOfTargets			  = rapidJsonValueTargets.MemberCount();
				compositorNodeHeader.numberOfOutputChannels		  = rapidJsonValueOutputChannels.MemberCount();
				outputFileStream.write(reinterpret_cast<const char*>(&compositorNodeHeader), sizeof(RendererRuntime::v1CompositorNode::Header));
			}

			// Write down the compositor resource node input channels
			std::unordered_set<uint32_t> compositorChannelIds;	// "RendererRuntime::CompositorChannelId"-type
			for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorInputChannels = rapidJsonValueInputChannels.MemberBegin(); rapidJsonMemberIteratorInputChannels != rapidJsonValueInputChannels.MemberEnd(); ++rapidJsonMemberIteratorInputChannels)
			{
				RendererRuntime::v1CompositorNode::Channel channel;
				channel.id = RendererRuntime::StringId(rapidJsonMemberIteratorInputChannels->value.GetString());
				outputFileStream.write(reinterpret_cast<const char*>(&channel), sizeof(RendererRuntime::v1CompositorNode::Channel));

				// Remember that there's a compositor channel with this ID
				compositorChannelIds.insert(channel.id);
			}

			// Write down the compositor render target textures
			std::unordered_set<uint32_t> renderTargetTextureAssetIds;	// "RendererRuntime::AssetId"-type
			renderTargetTextureAssetIds.insert(RendererRuntime::StringId("Unrimp/Texture/Dynamic/ImGuiGlyphMap"));	// TODO(co) Make this somehow more generic
			if (rapidJsonValueCompositorNodeAsset.HasMember("RenderTargetTextures"))
			{
				const rapidjson::Value& rapidJsonValueRenderTargetTextures = rapidJsonValueCompositorNodeAsset["RenderTargetTextures"];
				for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorRenderTargetTextures = rapidJsonValueRenderTargetTextures.MemberBegin(); rapidJsonMemberIteratorRenderTargetTextures != rapidJsonValueRenderTargetTextures.MemberEnd(); ++rapidJsonMemberIteratorRenderTargetTextures)
				{
					RendererRuntime::v1CompositorNode::RenderTargetTexture renderTargetTexture;
					renderTargetTexture.assetId = RendererRuntime::StringId(rapidJsonMemberIteratorRenderTargetTextures->name.GetString());
					{ // Render target texture signature
						const rapidjson::Value& rapidJsonValueRenderTargetTexture = rapidJsonMemberIteratorRenderTargetTextures->value;

						// Width and height
						const uint32_t width = ::detail::getRenderTargetTextureSize(rapidJsonValueRenderTargetTexture, "Width", "TARGET_WIDTH");
						const uint32_t height = ::detail::getRenderTargetTextureSize(rapidJsonValueRenderTargetTexture, "Height", "TARGET_HEIGHT");

						// Texture format
						const Renderer::TextureFormat::Enum textureFormat = JsonHelper::mandatoryTextureFormat(rapidJsonValueRenderTargetTexture);

						// Allow multisample?
						bool allowMultisample = false;
						JsonHelper::optionalBooleanProperty(rapidJsonValueRenderTargetTexture, "AllowMultisample", allowMultisample);

						// Allow resolution scale?
						bool allowResolutionScale = true;
						if (RendererRuntime::isInitialized(width) && RendererRuntime::isInitialized(height) && rapidJsonValueRenderTargetTexture.HasMember("AllowResolutionScale"))
						{
							throw std::runtime_error(std::string("Render target texture \"") + rapidJsonMemberIteratorRenderTargetTextures->name.GetString() + "\" has a fixed defined width and height, usage of \"AllowResolutionScale\" is not allowed for this use-case");
						}
						JsonHelper::optionalBooleanProperty(rapidJsonValueRenderTargetTexture, "AllowResolutionScale", allowResolutionScale);

						// Width scale
						float widthScale = 1.0f;
						if (RendererRuntime::isInitialized(width) && rapidJsonValueRenderTargetTexture.HasMember("WidthScale"))
						{
							throw std::runtime_error(std::string("Render target texture \"") + rapidJsonMemberIteratorRenderTargetTextures->name.GetString() + "\" has a fixed defined width, usage of \"WidthScale\" is not allowed for this use-case");
						}
						JsonHelper::optionalFloatProperty(rapidJsonValueRenderTargetTexture, "WidthScale", widthScale);

						// Height scale
						float heightScale = 1.0f;
						if (RendererRuntime::isInitialized(height) && rapidJsonValueRenderTargetTexture.HasMember("HeightScale"))
						{
							throw std::runtime_error(std::string("Render target texture \"") + rapidJsonMemberIteratorRenderTargetTextures->name.GetString() + "\" has a fixed defined height, usage of \"HeightScale\" is not allowed for this use-case");
						}
						JsonHelper::optionalFloatProperty(rapidJsonValueRenderTargetTexture, "HeightScale", heightScale);

						// Ease of use scale for width as well as height
						if (rapidJsonValueRenderTargetTexture.HasMember("Scale") && (rapidJsonValueRenderTargetTexture.HasMember("WidthScale") || rapidJsonValueRenderTargetTexture.HasMember("WidthScale")))
						{
							throw std::runtime_error(std::string("Render target texture \"") + rapidJsonMemberIteratorRenderTargetTextures->name.GetString() + "\" has an already defined width and/or height scale, usage of \"Scale\" is not allowed for this use-case");
						}
						float scale = 1.0f;
						JsonHelper::optionalFloatProperty(rapidJsonValueRenderTargetTexture, "Scale", scale);
						if (!rapidJsonValueRenderTargetTexture.HasMember("WidthScale"))
						{
							widthScale = scale;
						}
						if (!rapidJsonValueRenderTargetTexture.HasMember("HeightScale"))
						{
							heightScale = scale;
						}

						// Write render target texture signature
						// TODO(co) Add sanity checks to be able to detect editing errors (compressed formats are not supported nor unknown formats, check for name conflicts with channels, unused render target textures etc.)
						renderTargetTexture.renderTargetTextureSignature = RendererRuntime::RenderTargetTextureSignature(width, height, textureFormat, allowMultisample, allowResolutionScale, widthScale, heightScale);
					}
					outputFileStream.write(reinterpret_cast<const char*>(&renderTargetTexture), sizeof(RendererRuntime::v1CompositorNode::RenderTargetTexture));

					// Remember that there's a render target texture with this asset ID
					renderTargetTextureAssetIds.insert(renderTargetTexture.assetId);
				}
			}

			// Write down the compositor framebuffers
			std::unordered_set<uint32_t> compositorFramebufferIds;	// "RendererRuntime::CompositorFramebufferId"-type
			if (rapidJsonValueCompositorNodeAsset.HasMember("Framebuffers"))
			{
				const rapidjson::Value& rapidJsonValueFramebuffers = rapidJsonValueCompositorNodeAsset["Framebuffers"];
				for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorFramebuffers = rapidJsonValueFramebuffers.MemberBegin(); rapidJsonMemberIteratorFramebuffers != rapidJsonValueFramebuffers.MemberEnd(); ++rapidJsonMemberIteratorFramebuffers)
				{
					RendererRuntime::v1CompositorNode::Framebuffer framebuffer;
					framebuffer.compositorFramebufferId = RendererRuntime::StringId(rapidJsonMemberIteratorFramebuffers->name.GetString());
					{ // Framebuffer signature
						const rapidjson::Value& rapidJsonValueFramebuffer = rapidJsonMemberIteratorFramebuffers->value;

						// Optional color textures
						uint8_t numberOfColorTextures = 0;
						RendererRuntime::AssetId colorTextureAssetIds[8];
						memset(colorTextureAssetIds, static_cast<int>(RendererRuntime::getUninitialized<RendererRuntime::AssetId>()), sizeof(RendererRuntime::AssetId) * 8);
						if (rapidJsonValueFramebuffer.HasMember("ColorTextures"))
						{
							const rapidjson::Value& rapidJsonValueFramebufferColorTextures = rapidJsonValueFramebuffer["ColorTextures"];
							numberOfColorTextures = static_cast<uint8_t>(rapidJsonValueFramebufferColorTextures.Size());
							for (uint8_t i = 0; i < numberOfColorTextures; ++i)
							{
								colorTextureAssetIds[i] = RendererRuntime::StringId(rapidJsonValueFramebufferColorTextures[i].GetString());
								if (RendererRuntime::isInitialized(colorTextureAssetIds[i]) && renderTargetTextureAssetIds.find(colorTextureAssetIds[i]) == renderTargetTextureAssetIds.end())
								{
									throw std::runtime_error(std::string("Color texture \"") + rapidJsonValueFramebufferColorTextures[i].GetString() + "\" at index " + std::to_string(i) + " of framebuffer \"" + rapidJsonMemberIteratorFramebuffers->name.GetString() + "\" is unknown");
								}
							}
						}

						// Optional depth stencil texture
						const uint32_t depthStencilTexture = rapidJsonValueFramebuffer.HasMember("DepthStencilTexture") ? RendererRuntime::StringId(rapidJsonValueFramebuffer["DepthStencilTexture"].GetString()) : RendererRuntime::getUninitialized<RendererRuntime::AssetId>();
						if (RendererRuntime::isInitialized(depthStencilTexture) && renderTargetTextureAssetIds.find(depthStencilTexture) == renderTargetTextureAssetIds.end())
						{
							throw std::runtime_error(std::string("Depth stencil texture \"") + rapidJsonValueFramebuffer["DepthStencilTexture"].GetString() + "\" of framebuffer \"" + rapidJsonMemberIteratorFramebuffers->name.GetString() + "\" is unknown");
						}

						// Write framebuffer signature
						// TODO(co) Add sanity checks to be able to detect editing errors (check for name conflicts with channels, unused framebuffers etc.)
						framebuffer.framebufferSignature = RendererRuntime::FramebufferSignature(numberOfColorTextures, colorTextureAssetIds, depthStencilTexture);
					}
					outputFileStream.write(reinterpret_cast<const char*>(&framebuffer), sizeof(RendererRuntime::v1CompositorNode::Framebuffer));

					// Remember that there's a compositor framebuffer with this ID
					compositorFramebufferIds.insert(framebuffer.compositorFramebufferId);
				}
			}

			// Write down the compositor resource node targets
			for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorTargets = rapidJsonValueTargets.MemberBegin(); rapidJsonMemberIteratorTargets != rapidJsonValueTargets.MemberEnd(); ++rapidJsonMemberIteratorTargets)
			{
				const rapidjson::Value& rapidJsonValueTarget = rapidJsonMemberIteratorTargets->value;
				const rapidjson::Value& rapidJsonValuePasses = rapidJsonValueTarget["Passes"];

				{ // Write down the compositor resource node target
				  // -> Render target might be compositor channel (external interconnection) or compositor framebuffer (node internal processing)
					const uint32_t id = RendererRuntime::StringId(rapidJsonMemberIteratorTargets->name.GetString());
					RendererRuntime::v1CompositorNode::Target target;
					target.compositorChannelId	   = (compositorChannelIds.find(id) != compositorChannelIds.end()) ? id : RendererRuntime::getUninitialized<uint32_t>();
					target.compositorFramebufferId = (compositorFramebufferIds.find(id) != compositorFramebufferIds.end()) ? id : RendererRuntime::getUninitialized<uint32_t>();
					target.numberOfPasses		   = rapidJsonValuePasses.MemberCount();
					outputFileStream.write(reinterpret_cast<const char*>(&target), sizeof(RendererRuntime::v1CompositorNode::Target));
				}

				// Write down the compositor resource node target passes
				for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorPasses = rapidJsonValuePasses.MemberBegin(); rapidJsonMemberIteratorPasses != rapidJsonValuePasses.MemberEnd(); ++rapidJsonMemberIteratorPasses)
				{
					const rapidjson::Value& rapidJsonValuePass = rapidJsonMemberIteratorPasses->value;
					const RendererRuntime::CompositorPassTypeId compositorPassTypeId = RendererRuntime::StringId(rapidJsonMemberIteratorPasses->name.GetString());

					// Get the compositor resource node target pass type specific data number of bytes
					// TODO(co) Make this more generic via compositor pass factory
					uint32_t numberOfBytes = 0;
					RendererRuntime::MaterialProperties::SortedPropertyVector sortedMaterialPropertyVector;
					if (RendererRuntime::CompositorResourcePassClear::TYPE_ID == compositorPassTypeId)
					{
						numberOfBytes = sizeof(RendererRuntime::v1CompositorNode::PassClear);
					}
					else if (RendererRuntime::CompositorResourcePassScene::TYPE_ID == compositorPassTypeId)
					{
						numberOfBytes = sizeof(RendererRuntime::v1CompositorNode::PassScene);
					}
					else if (RendererRuntime::CompositorResourcePassShadowMap::TYPE_ID == compositorPassTypeId)
					{
						numberOfBytes = sizeof(RendererRuntime::v1CompositorNode::PassShadowMap);
					}
					else if (RendererRuntime::CompositorResourcePassResolveMultisample::TYPE_ID == compositorPassTypeId)
					{
						numberOfBytes = sizeof(RendererRuntime::v1CompositorNode::PassResolveMultisample);
					}
					else if (RendererRuntime::CompositorResourcePassQuad::TYPE_ID == compositorPassTypeId)
					{
						::detail::fillSortedMaterialPropertyVector(input, renderTargetTextureAssetIds, rapidJsonMemberIteratorPasses, sortedMaterialPropertyVector);
						numberOfBytes = sizeof(RendererRuntime::v1CompositorNode::PassQuad) + sizeof(RendererRuntime::MaterialProperty) * sortedMaterialPropertyVector.size();
					}
					else if (RendererRuntime::CompositorResourcePassDebugGui::TYPE_ID == compositorPassTypeId)
					{
						::detail::fillSortedMaterialPropertyVector(input, renderTargetTextureAssetIds, rapidJsonMemberIteratorPasses, sortedMaterialPropertyVector);
						numberOfBytes = sizeof(RendererRuntime::v1CompositorNode::PassDebugGui) + sizeof(RendererRuntime::MaterialProperty) * sortedMaterialPropertyVector.size();
					}

					{ // Write down the compositor resource node target pass header
						RendererRuntime::v1CompositorNode::PassHeader passHeader;
						passHeader.compositorPassTypeId = compositorPassTypeId;
						passHeader.numberOfBytes		= numberOfBytes;
						outputFileStream.write(reinterpret_cast<const char*>(&passHeader), sizeof(RendererRuntime::v1CompositorNode::PassHeader));
					}

					// Write down the compositor resource node target pass type specific data, if there is any
					if (0 != numberOfBytes)
					{
						if (RendererRuntime::CompositorResourcePassClear::TYPE_ID == compositorPassTypeId)
						{
							RendererRuntime::v1CompositorNode::PassClear passClear;

							// Read properties
							JsonHelper::optionalClearFlagsProperty(rapidJsonValuePass, "Flags", passClear.flags);
							JsonHelper::optionalFloatNProperty(rapidJsonValuePass, "Color", passClear.color, 4);
							JsonHelper::optionalFloatProperty(rapidJsonValuePass, "Z", passClear.z);
							JsonHelper::optionalIntegerProperty(rapidJsonValuePass, "Stencil", passClear.stencil);

							// Write down
							outputFileStream.write(reinterpret_cast<const char*>(&passClear), sizeof(RendererRuntime::v1CompositorNode::PassClear));
						}
						else if (RendererRuntime::CompositorResourcePassScene::TYPE_ID == compositorPassTypeId)
						{
							RendererRuntime::v1CompositorNode::PassScene passScene;
							::detail::readPassScene(rapidJsonValuePass, passScene);

							// Write down
							outputFileStream.write(reinterpret_cast<const char*>(&passScene), sizeof(RendererRuntime::v1CompositorNode::PassScene));
						}
						else if (RendererRuntime::CompositorResourcePassShadowMap::TYPE_ID == compositorPassTypeId)
						{
							RendererRuntime::v1CompositorNode::PassShadowMap passShadowMap;
							::detail::readPassScene(rapidJsonValuePass, passShadowMap);
							JsonHelper::mandatoryStringIdProperty(rapidJsonValuePass, "TextureAsset", passShadowMap.textureAssetId);
							renderTargetTextureAssetIds.insert(passShadowMap.textureAssetId);

							// Write down
							outputFileStream.write(reinterpret_cast<const char*>(&passShadowMap), sizeof(RendererRuntime::v1CompositorNode::PassShadowMap));
						}
						else if (RendererRuntime::CompositorResourcePassResolveMultisample::TYPE_ID == compositorPassTypeId)
						{
							RendererRuntime::v1CompositorNode::PassResolveMultisample passResolveMultisample;
							JsonHelper::mandatoryStringIdProperty(rapidJsonValuePass, "SourceMultisampleFramebuffer", passResolveMultisample.sourceMultisampleCompositorFramebufferId);
							if (compositorFramebufferIds.find(passResolveMultisample.sourceMultisampleCompositorFramebufferId) == compositorFramebufferIds.end())
							{
								throw std::runtime_error(std::string("Source multisample framebuffer \"") + rapidJsonValuePass["SourceMultisampleFramebuffer"].GetString() + "\" is unknown");
							}
							outputFileStream.write(reinterpret_cast<const char*>(&passResolveMultisample), sizeof(RendererRuntime::v1CompositorNode::PassResolveMultisample));
						}
						else if (RendererRuntime::CompositorResourcePassQuad::TYPE_ID == compositorPassTypeId)
						{
							RendererRuntime::v1CompositorNode::PassQuad passQuad;
							::detail::readPassQuad(input, sortedMaterialPropertyVector, rapidJsonValuePass, true, passQuad);

							// Write down
							outputFileStream.write(reinterpret_cast<const char*>(&passQuad), sizeof(RendererRuntime::v1CompositorNode::PassQuad));
							if (!sortedMaterialPropertyVector.empty())
							{
								// Write down all material properties
								outputFileStream.write(reinterpret_cast<const char*>(sortedMaterialPropertyVector.data()), sizeof(RendererRuntime::MaterialProperty) * sortedMaterialPropertyVector.size());
							}
						}
						else if (RendererRuntime::CompositorResourcePassDebugGui::TYPE_ID == compositorPassTypeId)
						{
							// The material definition is not mandatory for the debug GUI, if nothing is defined the fixed build in renderer configuration resources will be used instead
							RendererRuntime::v1CompositorNode::PassDebugGui passDebugGui;
							::detail::readPassQuad(input, sortedMaterialPropertyVector, rapidJsonValuePass, false, passDebugGui);

							// Write down
							outputFileStream.write(reinterpret_cast<const char*>(&passDebugGui), sizeof(RendererRuntime::v1CompositorNode::PassDebugGui));
							if (!sortedMaterialPropertyVector.empty())
							{
								// Write down all material properties
								outputFileStream.write(reinterpret_cast<const char*>(sortedMaterialPropertyVector.data()), sizeof(RendererRuntime::MaterialProperty) * sortedMaterialPropertyVector.size());
							}
						}
					}
				}
			}

			// Write down the compositor resource node output channels
			for (rapidjson::Value::ConstMemberIterator rapidJsonMemberIteratorOutputChannels = rapidJsonValueOutputChannels.MemberBegin(); rapidJsonMemberIteratorOutputChannels != rapidJsonValueOutputChannels.MemberEnd(); ++rapidJsonMemberIteratorOutputChannels)
			{
				RendererRuntime::v1CompositorNode::Channel channel;
				channel.id = RendererRuntime::StringId(rapidJsonMemberIteratorOutputChannels->name.GetString());
				outputFileStream.write(reinterpret_cast<const char*>(&channel), sizeof(RendererRuntime::v1CompositorNode::Channel));
			}
		}

		{ // Update the output asset package
			const std::string assetCategory = rapidJsonValueAsset["AssetMetadata"]["AssetCategory"].GetString();
			const std::string assetIdAsString = input.projectName + "/CompositorNode/" + assetCategory + '/' + assetName;

			// Output asset
			RendererRuntime::Asset outputAsset;
			outputAsset.assetId = RendererRuntime::StringId(assetIdAsString.c_str());
			strcpy(outputAsset.assetFilename, outputAssetFilename.c_str());	// TODO(co) Buffer overflow test
			outputAssetPackage.getWritableSortedAssetVector().push_back(outputAsset);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
