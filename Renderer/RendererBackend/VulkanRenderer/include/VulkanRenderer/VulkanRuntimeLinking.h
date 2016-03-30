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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#ifdef WIN32
	// Disable warnings in external headers, we can't fix them
	__pragma(warning(push))
		__pragma(warning(disable: 4668))	// warning C4668: '<x>' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
		// TODO(co) Implement me
	__pragma(warning(pop))
#else
	// TODO(co) Implement me
#endif


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Vulkan runtime linking
	*/
	class VulkanRuntimeLinking
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Default constructor
		*/
		VulkanRuntimeLinking();

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~VulkanRuntimeLinking();

		/**
		*  @brief
		*    Return whether or not Vulkan is available
		*
		*  @return
		*    "true" if Vulkan is available, else "false"
		*/
		bool isVulkanAvaiable();


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Load the shared libraries
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadSharedLibraries();

		/**
		*  @brief
		*    Load the Vulkan entry points
		*
		*  @return
		*    "true" if all went fine, else "false"
		*/
		bool loadVulkanEntryPoints();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		void *mVulkanSharedLibrary;		///< Vulkan shared library, can be a null pointer
		bool  mEntryPointsRegistered;	///< Entry points successfully registered?
		bool  mInitialized;				///< Already initialized?


	};


	//[-------------------------------------------------------]
	//[ Vulkan functions                                      ]
	//[-------------------------------------------------------]
	// TODO(co) Implement me
	#ifdef VULKAN_DEFINERUNTIMELINKING
		#define FNDEF_VK(retType, funcName, args) retType (GLAPIENTRY *funcPtr_##funcName) args
	#else
		#define FNDEF_VK(retType, funcName, args) extern retType (GLAPIENTRY *funcPtr_##funcName) args
	#endif
	// FNDEF_GL(GLubyte *,	glGetString,		(GLenum));


	//[-------------------------------------------------------]
	//[ Macros & definitions                                  ]
	//[-------------------------------------------------------]
	#ifndef FNPTR
		#define FNPTR(name) funcPtr_##name
	#endif

	// Redirect vk* function calls to funcPtr_vk*

	// Vulkan
	// TODO(co) Implement me
	// #define glGetString			FNPTR(glGetString)


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
