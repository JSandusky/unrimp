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
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{
	class VulkanContext;
}


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
	*    Supported Vulkan graphic card extensions
	*
	*  @remarks
	*    You must check if the extension is supported by the current hardware before
	*    you use it. If the extension isn't available you should offer an alternative
	*    technique aka fallback.
	*
	*  @see
	*    - Vulkan extension registry at TODO(co) Address for more information about
	*      the different extensions
	*/
	class Extensions
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] vulkanContext
		*    Owner Vulkan context
		*/
		explicit Extensions(VulkanContext& vulkanContext);

		/**
		*  @brief
		*    Destructor
		*
		*  @note
		*    - Not virtual by intent
		*/
		~Extensions();

		/**
		*  @brief
		*    Returns whether the extensions are initialized or not
		*
		*  @return
		*    "true" if the extension are initialized, else "false"
		*/
		bool isInitialized() const;

		///////////////////////////////////////////////////////////
		// Returns whether an extension is supported or not
		///////////////////////////////////////////////////////////
		// TODO(co) Implement me


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Checks whether an extension is supported by the given hardware or not
		*
		*  @param[in] extension
		*    ASCII name of the extension, if a null pointer, nothing happens
		*
		*  @return
		*    "true" if the extensions is supported, else "false"
		*/
		bool isSupported(const char* extension) const;

		/**
		*  @brief
		*    Checks whether an extension is supported by the given hardware or not
		*
		*  @param[in] extension
		*    ASCII name of the extension, if a null pointer, nothing happens
		*
		*  @return
		*    "true" if the extensions is supported, else "false"
		*/
		bool checkExtension(const char* extension) const;

		/**
		*  @brief
		*    Resets the extensions
		*/
		void resetExtensions();

		/**
		*  @brief
		*    Initialize the supported extensions
		*
		*  @param[in] useExtensions
		*    Use extensions?
		*
		*  @return
		*    "true" if all went fine, else "false"
		*
		*  @note
		*    - Platform dependent implementation
		*/
		bool initialize(bool useExtensions = true);

		/**
		*  @brief
		*    Initialize the supported universal extensions
		*
		*  @return
		*    "true" if all went fine, else "false"
		*
		*  @note
		*    - Platform independent implementation
		*/
		bool initializeUniversal();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		VulkanContext* mVulkanContext;	///< Owner Vulkan context, always valid!
		bool		   mInitialized;	///< Are the extensions initialized?

		// Supported extensions
		// TODO(co) Implement me


	};


	//[-------------------------------------------------------]
	//[ Define helper macro                                   ]
	//[-------------------------------------------------------]
	#ifdef EXTENSIONS_DEFINE
		#define FNDEF_EX(funcName, funcTypedef) funcTypedef funcName = nullptr
	#else
		#define FNDEF_EX(funcName, funcTypedef) extern funcTypedef funcName
	#endif


	// TODO(co) Implement me


	//[-------------------------------------------------------]
	//[ Undefine helper macro                                 ]
	//[-------------------------------------------------------]
	#undef FNDEF_EX


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer
