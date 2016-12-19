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
#include "RendererRuntime/Resource/ShaderBlueprint/Cache/Preprocessor/Preprocessor.h"
#include "RendererRuntime/Core/Platform/PlatformTypes.h"	// For "RENDERERRUNTIME_OUTPUT_DEBUG_PRINTF()"
#define MOJOSHADER_NO_VERSION_INCLUDE
#include "RendererRuntime/Resource/ShaderBlueprint/Cache/Preprocessor/mojoshader.h"

#include <cstring> // For strlen


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	void Preprocessor::preprocess(std::string& source, std::string& result)
	{
		// TODO(co) The usage of MojoShader just as preprocessor is overkill. Find a simpler but still efficient solution.

		// Preprocess
		const MOJOSHADER_preprocessData* preprocessData = MOJOSHADER_preprocess(nullptr, source.c_str(), static_cast<unsigned int>(source.length()), 0, 0, 0, 0, 0, 0, 0, 0, 0);

		// Evaluate the result
		if (preprocessData->error_count > 0)
		{
			for (int i = 0; i < preprocessData->error_count; ++i)
			{
				RENDERERRUNTIME_OUTPUT_DEBUG_PRINTF("Shader preprocessor %s:%d: Error: %s\n",
						preprocessData->errors[i].filename ? preprocessData->errors[i].filename : "???",
						preprocessData->errors[i].error_position,
						preprocessData->errors[i].error);
			}
		}
		else
		{
			result.assign(preprocessData->output, static_cast<size_t>(preprocessData->output_len));
		}
		MOJOSHADER_freePreprocessData(preprocessData);

		{ // TODO(co) The spaces all added over the place by the MojoShader c-preprocessor don't make things easier. Horrible hack, which is good enough for now (no time to implement a real solution).
			size_t pos = result.find("# version 410 core");
			if (pos != std::string::npos)
			{
				static const size_t numberOfCharacters = strlen("# version 410 core\n");
				result.replace(pos, numberOfCharacters, "#version  410 core\n");	// Performance: Don't change the length of the string
			}
		}
		{ // TODO(co) The MojoShader spaces make it really nasty, e.g. "sampleOffsets[15]" will become "sampleOffsets [ 15 ]" breaking GLSL shaders
			while (true)
			{
				const size_t pos = result.find(" [");
				if (pos == std::string::npos)
					break;
				result.replace(pos, 2, "[ ");	// Performance: Don't change the length of the string
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
