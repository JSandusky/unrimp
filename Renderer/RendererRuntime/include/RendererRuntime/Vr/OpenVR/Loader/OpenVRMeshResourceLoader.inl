/*********************************************************\
 * Copyright (c) 2012-2018 The Unrimp Team
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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline vr::RenderModel_t* OpenVRMeshResourceLoader::getVrRenderModel() const
	{
		return mVrRenderModel;
	}

	inline OpenVRMeshResourceLoader::BufferData& OpenVRMeshResourceLoader::getTangentsData()
	{
		return mTangentsData;
	}

	inline OpenVRMeshResourceLoader::BufferData& OpenVRMeshResourceLoader::getBinormalsData()
	{
		return mBinormalsData;
	}


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	inline ResourceLoaderTypeId OpenVRMeshResourceLoader::getResourceLoaderTypeId() const
	{
		return TYPE_ID;
	}

	inline bool OpenVRMeshResourceLoader::hasDeserialization() const
	{
		return false;
	}

	inline void OpenVRMeshResourceLoader::onDeserialization(IFile&)
	{
		// We're using the OpenVR API instead of reading from a file
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	inline OpenVRMeshResourceLoader::OpenVRMeshResourceLoader(IResourceManager& resourceManager, IRendererRuntime& rendererRuntime) :
		IMeshResourceLoader(resourceManager, rendererRuntime),
		mVrRenderModel(nullptr),
		mVertexArray(nullptr)
	{
		// Nothing here
	}

	inline OpenVRMeshResourceLoader::~OpenVRMeshResourceLoader()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
