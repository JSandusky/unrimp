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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/CompositorPassFactory.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/Clear/CompositorResourcePassClear.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/Clear/CompositorInstancePassClear.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/VrHiddenAreaMesh/CompositorResourcePassVrHiddenAreaMesh.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/VrHiddenAreaMesh/CompositorInstancePassVrHiddenAreaMesh.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/Quad/CompositorResourcePassQuad.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/Quad/CompositorInstancePassQuad.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/Copy/CompositorResourcePassCopy.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/Copy/CompositorInstancePassCopy.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/ShadowMap/CompositorResourcePassShadowMap.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/ShadowMap/CompositorInstancePassShadowMap.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/DebugGui/CompositorResourcePassDebugGui.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/DebugGui/CompositorInstancePassDebugGui.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/ResolveMultisample/CompositorResourcePassResolveMultisample.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/ResolveMultisample/CompositorInstancePassResolveMultisample.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::ICompositorPassFactory methods ]
	//[-------------------------------------------------------]
	ICompositorResourcePass* CompositorPassFactory::createCompositorResourcePass(const CompositorTarget& compositorTarget, CompositorPassTypeId compositorPassTypeId) const
	{
		ICompositorResourcePass* compositorResourcePass = nullptr;

		// Define helper macros
		#define IF_VALUE(resource)			 if (compositorPassTypeId == resource::TYPE_ID) compositorResourcePass = new resource(compositorTarget);
		#define ELSE_IF_VALUE(resource) else if (compositorPassTypeId == resource::TYPE_ID) compositorResourcePass = new resource(compositorTarget);

		// Evaluate the compositor pass type
		IF_VALUE(CompositorResourcePassClear)
		ELSE_IF_VALUE(CompositorResourcePassVrHiddenAreaMesh)
		ELSE_IF_VALUE(CompositorResourcePassScene)
		ELSE_IF_VALUE(CompositorResourcePassShadowMap)
		ELSE_IF_VALUE(CompositorResourcePassResolveMultisample)
		ELSE_IF_VALUE(CompositorResourcePassCopy)
		ELSE_IF_VALUE(CompositorResourcePassQuad)
		ELSE_IF_VALUE(CompositorResourcePassDebugGui)

		// Undefine helper macros
		#undef IF_VALUE
		#undef ELSE_IF_VALUE

		// Done
		return compositorResourcePass;
	}

	ICompositorInstancePass* CompositorPassFactory::createCompositorInstancePass(const ICompositorResourcePass& compositorResourcePass, const CompositorNodeInstance& compositorNodeInstance) const
	{
		ICompositorInstancePass* compositorInstancePass = nullptr;

		// Define helper macros
		#define IF_VALUE(resource, instance)		   if (compositorPassTypeId == resource::TYPE_ID) compositorInstancePass = new instance(static_cast<const resource&>(compositorResourcePass), compositorNodeInstance);
		#define ELSE_IF_VALUE(resource, instance) else if (compositorPassTypeId == resource::TYPE_ID) compositorInstancePass = new instance(static_cast<const resource&>(compositorResourcePass), compositorNodeInstance);

		// Evaluate the compositor pass type
		const CompositorPassTypeId compositorPassTypeId = compositorResourcePass.getTypeId();
		IF_VALUE(CompositorResourcePassClear,					CompositorInstancePassClear)
		ELSE_IF_VALUE(CompositorResourcePassVrHiddenAreaMesh,	CompositorInstancePassVrHiddenAreaMesh)
		ELSE_IF_VALUE(CompositorResourcePassScene,				CompositorInstancePassScene)
		ELSE_IF_VALUE(CompositorResourcePassShadowMap,			CompositorInstancePassShadowMap)
		ELSE_IF_VALUE(CompositorResourcePassResolveMultisample,	CompositorInstancePassResolveMultisample)
		ELSE_IF_VALUE(CompositorResourcePassCopy,				CompositorInstancePassCopy)
		ELSE_IF_VALUE(CompositorResourcePassQuad,				CompositorInstancePassQuad)
		ELSE_IF_VALUE(CompositorResourcePassDebugGui,			CompositorInstancePassDebugGui)

		// Undefine helper macros
		#undef IF_VALUE
		#undef ELSE_IF_VALUE

		// Done
		return compositorInstancePass;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
