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
#include "OpenGLRenderer/TessellationControlShaderCg.h"
#include "OpenGLRenderer/CgRuntimeLinking.h"
#include "OpenGLRenderer/ShaderLanguageCg.h"

#include <Renderer/PlatformTypes.h>	// For "RENDERER_OUTPUT_DEBUG_STRING()"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	TessellationControlShaderCg::TessellationControlShaderCg(OpenGLRenderer &openGLRenderer, _CGcontext &cgContext, const char *sourceCode, const char *profile) :
		TessellationControlShader(openGLRenderer),
		mCgProgram(nullptr)
	{
		// Get the profile from a user given string
		// -> TODO(co) "February 2012 version of Cg 3.1": No GLSL tessellation control shader support, at least one that's not using special NVIDIA-only extensions
		CGprofile cgProfile = (nullptr != profile) ? cgGetProfile(profile) : cgGLGetLatestProfile(CG_GL_TESSELLATION_CONTROL);
	//	CGprofile cgProfile = (nullptr != profile) ? cgGetProfile(profile) : cgGetProfile("?"); // We're using a GLSL profile as default so ATI users have reasonable shader support when using Cg

		// On unknown or invalid profile, choose a fallback profile
		// TODO(co) GLSL tessellation control program: "February 2012 version of Cg 3.1": No GLSL tessellation control shader support, at least one that's not using special NVIDIA-only extensions
		// -> Currently, Cg has only "CG_PROFILE_GP5TCP" (NV_tessellation_program5 tessellation control program)
	//	if (CG_PROFILE_UNKNOWN == cgProfile || (!cgGetProfileProperty(cgProfile, CG_IS_TESSELLATION_CONTROL_PROFILE) && ? != cgProfile))
	//	{
	//		cgProfile = cgGLGetLatestProfile(CG_GL_TESSELLATION_CONTROL);
	//		if (CG_PROFILE_UNKNOWN == cgProfile && cgGLIsProfileSupported(?))
	//			cgProfile = ?;
	//	}

		// Create the Cg program
		if (CG_PROFILE_UNKNOWN != cgProfile)
		{
			mCgProgram = cgCreateProgram(&cgContext, CG_SOURCE, sourceCode, cgProfile, "main", nullptr);
			if (nullptr == mCgProgram)
			{
				// Error!
				RENDERER_OUTPUT_DEBUG_STRING("OpenGL Cg error: Failed to create the Cg program\n")
			}
		}
	}

	TessellationControlShaderCg::~TessellationControlShaderCg()
	{
		// Destroy the Cg program
		if (nullptr != mCgProgram)
		{
			cgDestroyProgram(mCgProgram);
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char *TessellationControlShaderCg::getShaderLanguageName() const
	{
		return ShaderLanguageCg::NAME;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
