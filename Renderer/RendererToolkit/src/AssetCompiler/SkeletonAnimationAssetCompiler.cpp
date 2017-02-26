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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererToolkit/AssetCompiler/SkeletonAnimationAssetCompiler.h"
#include "RendererToolkit/Helper/StringHelper.h"
#include "RendererToolkit/Helper/JsonHelper.h"

#include <RendererRuntime/Asset/AssetPackage.h>
#include <RendererRuntime/Core/GetUninitialized.h>
#include <RendererRuntime/Resource/SkeletonAnimation/SkeletonAnimationResource.h>
#include <RendererRuntime/Resource/SkeletonAnimation/Loader/SkeletonAnimationFileFormat.h>

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4061)	// warning C4061: enumerator 'FORCE_32BIT' in switch of enum 'aiMetadataType' is not explicitly handled by a case label
	#include <assimp/scene.h>
	#include <assimp/Importer.hpp>
	#include <assimp/postprocess.h>
PRAGMA_WARNING_POP

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4668)	// warning C4668: '__GNUC__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
	PRAGMA_WARNING_DISABLE_MSVC(4365)	// warning C4365: '=': conversion from 'int' to 'rapidjson::internal::BigInteger::Type', signed/unsigned mismatch
	PRAGMA_WARNING_DISABLE_MSVC(4625)	// warning C4625: 'rapidjson::GenericMember<Encoding,Allocator>': copy constructor was implicitly defined as deleted
	#include <rapidjson/document.h>
PRAGMA_WARNING_POP

#include <fstream>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const AssetCompilerTypeId SkeletonAnimationAssetCompiler::TYPE_ID("SkeletonAnimation");


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	SkeletonAnimationAssetCompiler::SkeletonAnimationAssetCompiler()
	{
		// Nothing here
	}

	SkeletonAnimationAssetCompiler::~SkeletonAnimationAssetCompiler()
	{
		// Nothing here
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	AssetCompilerTypeId SkeletonAnimationAssetCompiler::getAssetCompilerTypeId() const
	{
		return TYPE_ID;
	}

	void SkeletonAnimationAssetCompiler::compile(const Input& input, const Configuration& configuration, Output& output)
	{
		// Input, configuration and output
		const std::string&			   assetInputDirectory	= input.assetInputDirectory;
		const std::string&			   assetOutputDirectory	= input.assetOutputDirectory;
		RendererRuntime::AssetPackage& outputAssetPackage	= *output.outputAssetPackage;

		// Get the JSON asset object
		const rapidjson::Value& rapidJsonValueAsset = configuration.rapidJsonDocumentAsset["Asset"];

		// Read configuration
		std::string inputFile;
		uint32_t animationIndex = RendererRuntime::getUninitialized<uint32_t>();
		{
			// Read skeleton animation asset compiler configuration
			const rapidjson::Value& rapidJsonValueSkeletonAnimationAssetCompiler = rapidJsonValueAsset["SkeletonAnimationAssetCompiler"];
			inputFile = rapidJsonValueSkeletonAnimationAssetCompiler["InputFile"].GetString();
			JsonHelper::optionalIntegerProperty(rapidJsonValueSkeletonAnimationAssetCompiler, "AnimationIndex", animationIndex);
		}

		// Open the input file
		std::ifstream inputFileStream(assetInputDirectory + inputFile, std::ios::binary);
		const std::string assetName = rapidJsonValueAsset["AssetMetadata"]["AssetName"].GetString();
		const std::string outputAssetFilename = assetOutputDirectory + assetName + ".skeleton_animation";
		std::ofstream outputFileStream(outputAssetFilename, std::ios::binary);

		// Create an instance of the Assimp importer class
		Assimp::Importer assimpImporter;

		// Load the given mesh
		// -> "aiProcess_MakeLeftHanded" is added because the rasterizer states directly map to Direct3D
		const std::string absoluteFilename = assetInputDirectory + inputFile;
		const aiScene* assimpScene = assimpImporter.ReadFile(absoluteFilename, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_MakeLeftHanded);
		if (nullptr != assimpScene && nullptr != assimpScene->mRootNode)
		{
			// Get the Assimp animation instance to import
			// -> In case there are multiple animations stored inside the imported skeleton animation we must
			//    insist that the skeleton animation compiler gets supplied with the animation index to use
			// -> One skeleton animation assets contains one skeleton animation, everything else would make things more complicated in high-level animation systems
			if (!assimpScene->HasAnimations())
			{
				throw std::runtime_error("The input file \"" + absoluteFilename + "\" contains no animations");
			}
			if (assimpScene->mNumAnimations > 1)
			{
				if (RendererRuntime::isUninitialized(animationIndex))
				{
					throw std::runtime_error("The input file \"" + absoluteFilename + "\" contains multiple animations, but the skeleton animation compiler wasn't provided with an animation index");
				}
			}
			else
			{
				// "When there's only one candidate, there's only one choice" (Monkey Island 1 quote)
				animationIndex = 0;
			}
			const aiAnimation* assimpAnimation = assimpScene->mAnimations[animationIndex];

			// Calculate the number of bytes required to store the complete animation data
			std::vector<uint32_t> channelByteOffsets;
			channelByteOffsets.resize(assimpAnimation->mNumChannels);
			uint32_t numberOfChannelDataBytes = 0;
			for (unsigned int channel = 0; channel < assimpAnimation->mNumChannels; ++channel)
			{
				const aiNodeAnim* assimpNodeAnim = assimpAnimation->mChannels[channel];
				channelByteOffsets[channel] = numberOfChannelDataBytes;
				numberOfChannelDataBytes += sizeof(RendererRuntime::SkeletonAnimationResource::ChannelHeader);
				numberOfChannelDataBytes += sizeof(RendererRuntime::SkeletonAnimationResource::Vector3Key) * assimpNodeAnim->mNumPositionKeys;
				numberOfChannelDataBytes += sizeof(RendererRuntime::SkeletonAnimationResource::QuaternionKey) * assimpNodeAnim->mNumRotationKeys;
				numberOfChannelDataBytes += sizeof(RendererRuntime::SkeletonAnimationResource::Vector3Key) * assimpNodeAnim->mNumScalingKeys;
			}

			{ // Skeleton animation header
				RendererRuntime::v1SkeletonAnimation::Header skeletonAnimationHeader;
				skeletonAnimationHeader.formatType				 = RendererRuntime::v1SkeletonAnimation::FORMAT_TYPE;
				skeletonAnimationHeader.formatVersion			 = RendererRuntime::v1SkeletonAnimation::FORMAT_VERSION;
				skeletonAnimationHeader.numberOfChannels		 = static_cast<uint8_t>(assimpAnimation->mNumChannels);
				skeletonAnimationHeader.durationInTicks			 = static_cast<float>(assimpAnimation->mDuration);
				skeletonAnimationHeader.ticksPerSecond			 = static_cast<float>(assimpAnimation->mTicksPerSecond);
				skeletonAnimationHeader.numberOfChannelDataBytes = numberOfChannelDataBytes;

				// Write down the skeleton animation header
				outputFileStream.write(reinterpret_cast<const char*>(&skeletonAnimationHeader), sizeof(RendererRuntime::v1SkeletonAnimation::Header));
			}

			// Write down the channel byte offsets
			outputFileStream.write(reinterpret_cast<const char*>(channelByteOffsets.data()), static_cast<std::streamsize>(sizeof(uint32_t) * channelByteOffsets.size()));

			// Bone channels, all the skeleton animation data in one big chunk
			for (unsigned int channel = 0; channel < assimpAnimation->mNumChannels; ++channel)
			{
				const aiNodeAnim* assimpNodeAnim = assimpAnimation->mChannels[channel];

				{ // Bone channel header
					RendererRuntime::SkeletonAnimationResource::ChannelHeader channelHeader;
					channelHeader.boneId			   = RendererRuntime::StringId(assimpNodeAnim->mNodeName.C_Str());
					channelHeader.numberOfPositionKeys = assimpNodeAnim->mNumPositionKeys;
					channelHeader.numberOfRotationKeys = assimpNodeAnim->mNumRotationKeys;
					channelHeader.numberOfScaleKeys	   = assimpNodeAnim->mNumScalingKeys;

					// Write down the bone channel header
					outputFileStream.write(reinterpret_cast<const char*>(&channelHeader), sizeof(RendererRuntime::SkeletonAnimationResource::ChannelHeader));
				}

				{ // Write bone channel position data
					std::vector<RendererRuntime::SkeletonAnimationResource::Vector3Key> positionKeys;
					positionKeys.resize(assimpNodeAnim->mNumPositionKeys);
					for (unsigned int i = 0; i < assimpNodeAnim->mNumPositionKeys; ++i)
					{
						const aiVectorKey& assimpVectorKey = assimpNodeAnim->mPositionKeys[i];
						RendererRuntime::SkeletonAnimationResource::Vector3Key& vector3Key = positionKeys[i];
						vector3Key.timeInTicks = static_cast<float>(assimpVectorKey.mTime);
						vector3Key.value.x	   = assimpVectorKey.mValue.x;
						vector3Key.value.y	   = assimpVectorKey.mValue.y;
						vector3Key.value.z	   = assimpVectorKey.mValue.z;
					}
					outputFileStream.write(reinterpret_cast<const char*>(positionKeys.data()), static_cast<std::streamsize>(sizeof(RendererRuntime::SkeletonAnimationResource::Vector3Key) * assimpNodeAnim->mNumPositionKeys));
				}

				{ // Write bone channel rotation data
				  // -> Some Assimp importers like the MD5 one compensate coordinate system differences by setting a root node transform, so we need to take this into account
					std::vector<RendererRuntime::SkeletonAnimationResource::QuaternionKey> rotationKeys;
					rotationKeys.resize(assimpNodeAnim->mNumRotationKeys);
					const aiQuaternion assimpQuaternionOffset(aiMatrix3x3(assimpScene->mRootNode->mTransformation));
					for (unsigned int i = 0; i < assimpNodeAnim->mNumRotationKeys; ++i)
					{
						const aiQuatKey& assimpQuatKey = assimpNodeAnim->mRotationKeys[i];
						RendererRuntime::SkeletonAnimationResource::QuaternionKey& quaternionKey = rotationKeys[i];
						const aiQuaternion assimpQuaternion = (0 == channel) ? (assimpQuaternionOffset * assimpQuatKey.mValue) : assimpQuatKey.mValue;
						quaternionKey.timeInTicks = static_cast<float>(assimpQuatKey.mTime);
						quaternionKey.value.x	  = assimpQuaternion.x;
						quaternionKey.value.y	  = assimpQuaternion.y;
						quaternionKey.value.z	  = assimpQuaternion.z;
						quaternionKey.value.w	  = assimpQuaternion.w;
					}
					outputFileStream.write(reinterpret_cast<const char*>(rotationKeys.data()), static_cast<std::streamsize>(sizeof(RendererRuntime::SkeletonAnimationResource::QuaternionKey) * assimpNodeAnim->mNumRotationKeys));
				}

				{ // Write bone channel scale data
					std::vector<RendererRuntime::SkeletonAnimationResource::Vector3Key> scaleKeys;
					scaleKeys.resize(assimpNodeAnim->mNumScalingKeys);
					for (unsigned int i = 0; i < assimpNodeAnim->mNumScalingKeys; ++i)
					{
						const aiVectorKey& assimpVectorKey = assimpNodeAnim->mScalingKeys[i];
						RendererRuntime::SkeletonAnimationResource::Vector3Key& vector3Key = scaleKeys[i];
						vector3Key.timeInTicks = static_cast<float>(assimpVectorKey.mTime);
						vector3Key.value.x	   = assimpVectorKey.mValue.x;
						vector3Key.value.y	   = assimpVectorKey.mValue.y;
						vector3Key.value.z	   = assimpVectorKey.mValue.z;
					}
					outputFileStream.write(reinterpret_cast<const char*>(scaleKeys.data()), static_cast<std::streamsize>(sizeof(RendererRuntime::SkeletonAnimationResource::Vector3Key) * assimpNodeAnim->mNumScalingKeys));
				}
			}
		}

		{ // Update the output asset package
			const std::string assetCategory = rapidJsonValueAsset["AssetMetadata"]["AssetCategory"].GetString();
			const std::string assetIdAsString = input.projectName + "/SkeletonAnimation/" + assetCategory + '/' + assetName;

			// Output asset
			RendererRuntime::Asset outputAsset;
			outputAsset.assetId = StringHelper::getAssetIdByString(assetIdAsString.c_str());
			strcpy(outputAsset.assetFilename, outputAssetFilename.c_str());	// TODO(co) Buffer overflow test
			outputAssetPackage.getWritableSortedAssetVector().push_back(outputAsset);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
