#ifndef __TGALOADER_H__
#define __TGALOADER_H__


namespace Renderer
{
	class ITexture2D;
	class IRenderer;
}
extern Renderer::ITexture2D *loadTGATexture(Renderer::IRenderer &renderer, const char *filename);


#endif // __TGALOADER_H__
