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
#include "OpenGLRenderer/FragmentShaderCg.h"
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
	FragmentShaderCg::FragmentShaderCg(OpenGLRenderer &openGLRenderer, _CGcontext &cgContext, const char *sourceCode, const char *profile) :
		FragmentShader(openGLRenderer),
		mCgProgram(nullptr)
	{
		// Get the profile from a user given string
		CGprofile cgProfile = (nullptr != profile) ? cgGetProfile(profile) : cgGetProfile("glslf"); // We're using a GLSL profile as default so ATI users have reasonable shader support when using Cg

		// On unknown or invalid profile, choose a fallback profile
		if (CG_PROFILE_UNKNOWN == cgProfile || (!cgGetProfileProperty(cgProfile, CG_IS_FRAGMENT_PROFILE) && CG_PROFILE_GLSLF != cgProfile))
		{
			cgProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
			if (CG_PROFILE_UNKNOWN == cgProfile && cgGLIsProfileSupported(CG_PROFILE_GLSLF))
			{
				cgProfile = CG_PROFILE_GLSLF;
			}
		}

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

	FragmentShaderCg::~FragmentShaderCg()
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
	const char *FragmentShaderCg::getShaderLanguageName() const
	{
		return ShaderLanguageCg::NAME;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
