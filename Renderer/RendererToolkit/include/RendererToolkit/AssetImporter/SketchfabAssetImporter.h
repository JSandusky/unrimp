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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererToolkit/AssetImporter/IAssetImporter.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererToolkit
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Sketchfab ( https://sketchfab.com/ ) asset importer
	*
	*  @remarks
	*    Sketchfab gives artists several conventions to work with. Sadly there are downloadable meshes which don't respect the conventions.
	*    As a result, the automatic Sketchfab asset importer doesn't work for all downloadable Sketchfab meshes out-of-the-box without
	*    additional manual asset file adjustments after the import. If an imported mesh asset doesn't look right, please have a look at
	*    the following hints.
	*
	*    If some materials look broken, try adding the following inside the mesh asset file "MeshAssetCompiler"-child: "ImportFlags": "DEFAULT_FLAGS & ~REMOVE_REDUNDANT_MATERIALS"
	*
	*    The Sketchfab asset importer was tested with the following downloadable Sketchfab meshes
	*    - "Spinosaurus" - https://sketchfab.com/models/c230edf4a5cf4a1ab9e34a4a4a04e013
	*
	*  @note
	*    - Has build-in support for texture channel packing "_drgb_nxa" and "_hr_rg_mb_nya"
	*/
	class SketchfabAssetImporter : public IAssetImporter
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		static const AssetImporterTypeId TYPE_ID;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		SketchfabAssetImporter();
		virtual ~SketchfabAssetImporter() override;


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetImporter methods ]
	//[-------------------------------------------------------]
	public:
		virtual AssetImporterTypeId getAssetImporterTypeId() const override;
		virtual void import(const Input& input) override;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
