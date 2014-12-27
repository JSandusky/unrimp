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
#include "Main.h"

#include <RendererToolkit/Public/RendererToolkit.h>
#include <RendererToolkit/Public/RendererToolkitInstance.h>

#include <fstream>


//[-------------------------------------------------------]
//[ Platform independent program entry point              ]
//[-------------------------------------------------------]
int programEntryPoint()
{
	RendererToolkit::RendererToolkitInstance rendererToolkitInstance;
	RendererToolkit::IRendererToolkit* rendererToolkit = rendererToolkitInstance.getRendererToolkit();
	if (nullptr != rendererToolkit)
	{
		// TODO(co) Compressed output streams

		// Font
		try
		{
			// Input and output
			std::ifstream ifstream("../DataSource/Font/LinBiolinum_R.otf", std::ios::binary);
			std::ofstream ofstream("../DataPc/Font/Default/LinBiolinum_R.font", std::ios::binary);

			// Asset compiler configuration
			std::ifstream jsonConfiguration("../DataSource/Font/LinBiolinum_R.json", std::ios::binary);

			// Compile asset
			RendererToolkit::IAssetCompilerPtr fontAssetCompiler(rendererToolkit->createFontAssetCompiler());
			fontAssetCompiler->compile(ifstream, ofstream, jsonConfiguration);
		}
		catch (const std::exception& e)
		{
			const char* text = e.what();
			int ii = 0;
		}

		// Texture
		try
		{
			// Input and output
			std::ifstream ifstream("../DataSource/Imrod/Imrod_Diffuse.tga", std::ios::binary);
			std::ofstream ofstream("../DataPc/Texture/Imrod/Imrod_Diffuse.dds", std::ios::binary);

			// Asset compiler configuration
			std::ifstream jsonConfiguration("../DataSource/Imrod/Imrod_Diffuse.json", std::ios::binary);

			// Compile asset
			RendererToolkit::IAssetCompilerPtr textureAssetCompiler(rendererToolkit->createTextureAssetCompiler());
			textureAssetCompiler->compile(ifstream, ofstream, jsonConfiguration);
		}
		catch (const std::exception& e)
		{
			const char* text = e.what();
			int ii = 0;
		}

		// Material
		try
		{
			// Input and output
			std::ifstream ifstream("../DataSource/Imrod/Imrod.material", std::ios::binary);
			std::ofstream ofstream("../DataPc/Material/Character/Imrod.material", std::ios::binary);

			// Asset compiler configuration
			std::ifstream jsonConfiguration("../DataSource/Imrod/Imrod.json", std::ios::binary);

			// Compile asset
			RendererToolkit::IAssetCompilerPtr materialAssetCompiler(rendererToolkit->createMaterialAssetCompiler());
			materialAssetCompiler->compile(ifstream, ofstream, jsonConfiguration);
		}
		catch (const std::exception& e)
		{
			const char* text = e.what();
			int ii = 0;
		}

		// Mesh
		try
		{
			// Input and output
			std::ifstream ifstream("../DataSource/Imrod/ImrodLowPoly.obj", std::ios::binary);
			std::ofstream ofstream("../DataPc/Mesh/Character/Imrod.mesh", std::ios::binary);

			// Asset compiler configuration
			std::ifstream jsonConfiguration("../DataSource/Imrod/ImrodLowPoly.json", std::ios::binary);

			// Compile asset
			RendererToolkit::IAssetCompilerPtr meshAssetCompiler(rendererToolkit->createMeshAssetCompiler());
			meshAssetCompiler->compile(ifstream, ofstream, jsonConfiguration);
		}
		catch (const std::exception& e)
		{
			const char* text = e.what();
			int ii = 0;
		}
	}
	

//	RendererToolkit::FontAssetCompiler fontAssetCompiler;


	// No error
	return 0;
}
