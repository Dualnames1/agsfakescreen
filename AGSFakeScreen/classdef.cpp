#include "plugin/agsplugin.h"

class BMFontRenderer : public IAGSFontRenderer {
  public:
    IAGSEngine* engine;


void Replacer(int i)
{
//	FontRenderer= new IAGSFontRenderer;
	engine->ReplaceFontRenderer(i,this);
}
bool LoadFromDisk(int fontNumber, int fontSize) 
{
	return false;
}
void FreeMemory(int fontNumber)
{
}

void RenderText(const char* text, int fontNumber, BITMAP* destination, int x, int y, int color)
{
	BITMAP* src=destination;
	int src_width;
	int src_height;
	int src_depth;
	engine->GetBitmapDimensions (src, &src_width, &src_height, &src_depth);
	unsigned int** pixel_src = (unsigned int**)engine->GetRawBitmapSurface(src);

	int xx=0;
	while (xx < src_width)
	{
		int yy=0;
		while (yy < src_height)
		{
			pixel_src[yy][xx]=0;//SetColor(255,0,0,255);
			yy++;
		}
		xx++;
	}
	
	engine->ReleaseBitmapSurface(src);
}

void AdjustYCoordinateForFont(int *ycoord, int fontNumber){}
void EnsureTextValidForFont(char *text, int fontNumber){}
int GetTextHeight(const char *text, int fontNumber)
{
	return 20;
}
int GetTextWidth(const char *text, int fontNumber)
{
	return 20;
}
bool SupportsExtendedCharacters(int fontNumber){return false;}


};
