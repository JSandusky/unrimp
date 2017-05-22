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
#include "RendererToolkit/AssetCompiler/IAssetCompiler.h"


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
	*    Texture asset compiler
	*
	*  @remarks
	*    == Texture Semantics ==
	*    Overview of recommended source texture semantics to use while authoring texture source assets:
	*    --------------------------------------------------------------
	*    Semantic               Postfix    Data         sRGB    Comment
	*    --------------------------------------------------------------
	*    Diffuse map            _d         rgb          Yes     Also known as base color or albedo. Raw color with no lighting information. Small amount of ambient occlusion can be baked in if using it for micro-surface occlusion. For a metal-rough-worfkow, the color range for dark values should stay within 30-50 RGB. Never have dark values below 30 RGB. The brightest color value should not go above 240 RGB. With metal/rough, the areas indicated as metal in the metallic map have a corresponding metal reflectance value in the base color map. The metal reflectance value in the base color needs to be a measured real-world value. Transitional areas in the metal map (not raw metal 1.0 white) need to have the metal reflectance value lowered to indicate that its reflectance value is not raw metal.
	*    Alpha map              _a         luminance    No      8-bit-alpha as some artists might call it
	*    Normal map             _n         rgb          No      Tangent space normal map
	*    Emissive map           _e         rgb          Yes
	*    Roughness map          _r         luminance    No      Roughness map = 1 - gloss map. Metal-rough-worfkow: Describes the microsurface of the object. White 1.0 is rough and black 0.0 is smooth. The microsurface if rough can cause the light rays to scatter and make the highlight appear dimmer and more broad. The same amount of light energy is reflected going out as coming into the surface. This map has the most artistic freedom. There is no wrong answers here. This map gives the asset the most character as it truly describes the surface e.g. scratches, fingerprints, smudges, grime etc.
	*    Metallic map           _m         luminance    No      Metal-rough-worfkow: Tells the shader if something is metal or not. Raw Metal = 1.0 white and non metal = 0.0 black. There can be transitional gray values that indicate something covering the raw metal such as dirt. With metal/rough, you only have control over metal reflectance values. The dielectric values are set to 0.04 or 4% which is most dielectric materials.
	*    Height map             _h         luminance    No
	*    Tint map               _t         luminance    No
	*    Ambient occlusion map  _ao        luminance    No
	*    Reflection 2D map      _r2d       rgb          Yes
	*    Reflection cube map    _rcube     rgb          Yes
	*/
	class TextureAssetCompiler : public IAssetCompiler
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		static const AssetCompilerTypeId TYPE_ID;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		TextureAssetCompiler();
		virtual ~TextureAssetCompiler();


	//[-------------------------------------------------------]
	//[ Public virtual RendererToolkit::IAssetCompiler methods ]
	//[-------------------------------------------------------]
	public:
		virtual AssetCompilerTypeId getAssetCompilerTypeId() const override;
		virtual void compile(const Input& input, const Configuration& configuration, Output& output) override;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererToolkit
