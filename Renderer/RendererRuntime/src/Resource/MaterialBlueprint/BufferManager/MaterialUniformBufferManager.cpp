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
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/MaterialUniformBufferManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/Listener/IMaterialBlueprintResourceListener.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	// TODO(co) Currently just a shell which needs to be implemented
	void MaterialUniformBufferManager::fillMaterialUniformBuffer()
	{
		MaterialBlueprintResource::UniformBuffer* materialUniformBuffer = mMaterialBlueprintResource.getMaterialUniformBuffer();
		assert(nullptr != materialUniformBuffer);

		const MaterialBlueprintResourceManager& materialBlueprintResourceManager = mMaterialBlueprintResource.getResourceManager<MaterialBlueprintResourceManager>();
		const MaterialProperties& globalMaterialProperties = materialBlueprintResourceManager.getGlobalMaterialProperties();

		IMaterialBlueprintResourceListener& materialBlueprintResourceListener = materialBlueprintResourceManager.getMaterialBlueprintResourceListener();
		materialBlueprintResourceListener.beginFillMaterial();

		// TODO(co) Optimization: Do only update changed materials

		// Update the scratch buffer
		MaterialBlueprintResource::ScratchBuffer& scratchBuffer = materialUniformBuffer->scratchBuffer;
		{
			const MaterialBlueprintResource::UniformBufferElementProperties& uniformBufferElementProperties = materialUniformBuffer->uniformBufferElementProperties;
			const size_t numberOfUniformBufferElementProperties = uniformBufferElementProperties.size();
			const size_t numberOfLinkedMaterialTechniques = mMaterialBlueprintResource.mLinkedMaterialTechniques.size();
			const uint32_t numberOfBytesPerElement = materialUniformBuffer->uniformBufferNumberOfBytes / materialUniformBuffer->numberOfElements;
			for (uint32_t materialIndex = 0; materialIndex < numberOfLinkedMaterialTechniques; ++materialIndex)
			{
				const MaterialTechnique* materialTechnique = mMaterialBlueprintResource.mLinkedMaterialTechniques[materialIndex];
				assert(materialTechnique->getMaterialUniformBufferIndex() == materialIndex);

				const MaterialResource& materialResource = materialTechnique->getMaterialResource();
				uint8_t* scratchBufferPointer = scratchBuffer.data() + numberOfBytesPerElement * materialIndex;

				for (size_t i = 0, numberOfPackageBytes = 0; i < numberOfUniformBufferElementProperties; ++i)
				{
					const MaterialProperty& uniformBufferElementProperty = uniformBufferElementProperties[i];

					// Get value type number of bytes
					const uint32_t valueTypeNumberOfBytes = uniformBufferElementProperty.getValueTypeNumberOfBytes(uniformBufferElementProperty.getValueType());

					// Handling of packing rules for uniform variables (see "Reference for HLSL - Shader Models vs Shader Profiles - Shader Model 4 - Packing Rules for Constant Variables" at https://msdn.microsoft.com/en-us/library/windows/desktop/bb509632%28v=vs.85%29.aspx )
					if (0 != numberOfPackageBytes && numberOfPackageBytes + valueTypeNumberOfBytes > 16)
					{
						// Move the scratch buffer pointer to the location of the next aligned package and restart the package bytes counter
						scratchBufferPointer += 4 * 4 - numberOfPackageBytes;
						numberOfPackageBytes = 0;
					}
					numberOfPackageBytes += valueTypeNumberOfBytes % 16;

					// Copy the property value into the scratch buffer
					const MaterialProperty::Usage usage = uniformBufferElementProperty.getUsage();
					if (MaterialProperty::Usage::MATERIAL_REFERENCE == usage)	// Most likely the case, so check this first
					{
						// Figure out the material property value
						const MaterialProperty* materialProperty = materialResource.getPropertyById(uniformBufferElementProperty.getReferenceValue());
						if (nullptr != materialProperty)
						{
							// TODO(co) Error handling: Usage mismatch, value type mismatch etc.
							memcpy(scratchBufferPointer, materialProperty->getData(), valueTypeNumberOfBytes);
						}
						else if (!materialBlueprintResourceListener.fillMaterialValue(uniformBufferElementProperty.getReferenceValue(), scratchBufferPointer, valueTypeNumberOfBytes))
						{
							// Error, can't resolve reference
							assert(false);
						}
					}
					else if (MaterialProperty::Usage::GLOBAL_REFERENCE == usage)
					{
						// Referencing a global material property inside a material uniform buffer doesn't make really sense performance wise, but don't forbid it
	
						// Figure out the global material property value
						const MaterialProperty* materialProperty = globalMaterialProperties.getPropertyById(uniformBufferElementProperty.getReferenceValue());
						if (nullptr != materialProperty)
						{
							// TODO(co) Error handling: Usage mismatch, value type mismatch etc.
							memcpy(scratchBufferPointer, materialProperty->getData(), valueTypeNumberOfBytes);
						}
						else
						{
							// Error, can't resolve reference
							assert(false);
						}
					}
					else if (!uniformBufferElementProperty.isReferenceUsage())	// TODO(co) Performance: Think about such tests, the toolkit should already take care of this so we have well known verified runtime data
					{
						// Referencing a static material property inside an material uniform buffer doesn't make really sense performance wise, but don't forbid it

						// Just copy over the property value
						memcpy(scratchBufferPointer, uniformBufferElementProperty.getData(), valueTypeNumberOfBytes);
					}
					else
					{
						// Error, invalid property
						assert(false);
					}

					// Next property
					scratchBufferPointer += valueTypeNumberOfBytes;
				}
			}
		}

		// Update the uniform buffer by using our scratch buffer
		materialUniformBuffer->uniformBufferPtr->copyDataFrom(scratchBuffer.size(), scratchBuffer.data());
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
