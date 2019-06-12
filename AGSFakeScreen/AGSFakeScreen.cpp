////*

#ifdef WIN32
#define WINDOWS_VERSION
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma warning(disable : 4244)
#endif

#if !defined(BUILTIN_PLUGINS)
#define THIS_IS_THE_PLUGIN
#endif

#include <math.h>


#if defined(PSP_VERSION)
#include <pspsdk.h>
#include <pspmath.h>
#include <pspdisplay.h>
#define sin(x) vfpu_sinf(x)
#endif

#include "plugin/agsplugin.h"
#include "classdef.cpp"

#if defined(BUILTIN_PLUGINS)
namespace agsfakescreen {
#endif

#if defined(__GNUC__)
inline unsigned long _blender_alpha16_bgr(unsigned long y) __attribute__((always_inline));
inline void calc_x_n(unsigned long bla) __attribute__((always_inline));
#endif

const unsigned int Magic = 0xACAB0000;
const unsigned int Version = 1;
const unsigned int SaveMagic = Magic + Version;
const float PI = 3.14159265f;

int screen_width = 640;
int screen_height = 360;
int screen_color_depth = 32;

IAGSEngine* engine;

#include <stdio.h>
#include <string>
#include <math.h>

int Random(int value)
{
	return (rand() % (value+1));
}

float clamp(float x, float min, float max) {
    float value=x;
	if(value < min) value= min;
    if(value > max) value= max;
    return value;
}
int SetColorRGBA(int r,int g,int b,int a)
{
	r = int(clamp(float(r),0.0,255.0));
	g = int(clamp(float(g),0.0,255.0));
	b = int(clamp(float(b),0.0,255.0));
	a = int(clamp(float(a),0.0,255.0));
	return int ((r << 16) | (g << 8) | (b << 0) | (a << 24));
}
int getRcolor(int color) {
	return ((color >> 16) & 0xFF);
}
int getGcolor(int color) {
	return ((color >> 8) & 0xFF);
}
int getBcolor(int color) {
	return ((color >> 0) & 0xFF);
}
int getAcolor(int color)
{
	return ((color >> 24) & 0xFF);
}

void ReadWalkBehindIntoSprite(int sprite,int bgsprite,int walkbehindBaseline)
{
	BITMAP* src = engine->GetSpriteGraphic(sprite);	
	BITMAP* bgsrc = engine->GetSpriteGraphic(bgsprite);	
	int src_width=640;
	int src_height=360;
	int src_depth=32;
	engine->GetBitmapDimensions(src,&src_width,&src_height,&src_depth);
	BITMAP* wbh =engine->GetRoomMask(MASK_WALKBEHIND);
	
	unsigned int** sprite_pixels = (unsigned int**)engine->GetRawBitmapSurface(src);
	unsigned int** bgsprite_pixels = (unsigned int**)engine->GetRawBitmapSurface(bgsrc);
	unsigned char **walk_pixels = engine->GetRawBitmapSurface (wbh); //8bit
    
	
	engine->ReleaseBitmapSurface(wbh); 
	engine->ReleaseBitmapSurface(bgsrc); 

	

	 //WE GRAB ALL OF THEM INTO A BITMAP and thus we know where they are drawn
	 int x,y;
	 for (y = 0; y < src_height; y++)
	 {
		 for (x = 0; x < src_width; x++)
		 {
			 //READ COLOR
			
			 if (walk_pixels[y][x]>0)
			 {
				 int grabBaseline=engine->GetWalkbehindBaseline(walk_pixels[y][x]);
				 
				 if (grabBaseline==walkbehindBaseline)
				 {
					 sprite_pixels[y][x] = bgsprite_pixels[y][x];
				 }
			 }
		 }
	 }

	 engine->ReleaseBitmapSurface(src);
}


void DrawBoxBlur(int spriteD, int radius)
{
	int spriteD2=spriteD;
	BITMAP* src = engine->GetSpriteGraphic(spriteD);
	BITMAP* src2 = engine->GetSpriteGraphic(spriteD2);
	
	unsigned int** pixelb = (unsigned int**)engine->GetRawBitmapSurface(src);
	unsigned int** pixela = (unsigned int**)engine->GetRawBitmapSurface(src2);
	engine->ReleaseBitmapSurface(src2);
	int src_width=640;
	int src_height=360;
	int src_depth=32;

	engine->GetBitmapDimensions(src,&src_width,&src_height,&src_depth);
	

    int x,y;
	for (y = 0; y < src_height; y++)
	{
		for (x = 0; x < src_width; x++)
		{
			int totalRed=0;
			int totalGreen=0;
			int totalBlue=0;
			
			int vx=-(radius);
			int pixels_parsed=0;

			int setY=y;
			if (setY<0) setY=0;
			if (setY>src_height-1) setY=src_height-1;

			while (vx < (radius) +1)
			{
				int setX=x+vx;
				if (setX<0) setX=0;
				if (setX>src_width-1) setX=src_width-1;
				
				
				int color = pixela[setY][setX];
				
				totalRed+=getRcolor(color);
				totalGreen+=getGcolor(color);
				totalBlue+=getBcolor(color);
				
				pixels_parsed++;
				vx++;
			}

			int rN=totalRed/pixels_parsed;
			int gN=totalGreen/pixels_parsed;
			int bN=totalBlue/pixels_parsed;

			int r=int(clamp(rN,0.0,255.0));
			int g=int(clamp(gN,0.0,255.0));
			int b=int(clamp(bN,0.0,255.0));
			

			pixelb[y][x]= ((r << 16) | (g << 8) | (b << 0) | (255 << 24));



		}
	}
    

	engine->ReleaseBitmapSurface(src);
	src = engine->GetSpriteGraphic(spriteD);

	x=0;
	y=0;
	for (y = 0; y < src_height; y++)
	{
		for (x = 0; x < src_width; x++)
		{
			int totalRed=0;
			int totalGreen=0;
			int totalBlue=0;
			
			int pixels_parsed=0;
			int setX=x;
			if (setX<0) setX=0;
			if (setX>src_width-1) setX=src_width-1;
			
			int vy=-(radius);
			while (vy <(radius)+1)
			{
				int setY=y+vy;
				if (setY<0) setY=0;
				if (setY>src_height-1) setY=src_height-1;
				
				int color = pixela[setY][setX];
				
				totalRed+=getRcolor(color);
				totalGreen+=getGcolor(color);
				totalBlue+=getBcolor(color);
				
				pixels_parsed++;
				    
			    vy++;
			}

			int rN=totalRed/pixels_parsed;
			int gN=totalGreen/pixels_parsed;
			int bN=totalBlue/pixels_parsed;

			int r=int(clamp(rN,0.0,255.0));
			int g=int(clamp(gN,0.0,255.0));
			int b=int(clamp(bN,0.0,255.0));
			

			pixelb[y][x]= ((r << 16) | (g << 8) | (b << 0) | (255 << 24));



		}
	}

	engine->ReleaseBitmapSurface(src);

}

struct ScreenItem
{
  int objIndex;
  int chrIndex;
  int walkIndex;
  int baseLine;
  int ignoreWB;
};

ScreenItem ItemToDraw[128];
int Walkbehind[20];

int CalculateThings()
{
  int numThingsToDraw = 0;
  int i = 0;
  int RoomObjectCount=engine->GetNumObjects ();
  int GameCharacterCount=engine->GetNumCharacters();
  int MaxWalkBehinds=15;
  
  int pid= engine->GetPlayerCharacter();
  AGSCharacter *pchar = engine->GetCharacter(pid);
  int playerRoom=pchar->room;

 
  
  
  while (i < RoomObjectCount)
  {
	  AGSObject *objec=engine->GetObject(i);
	  
    int baseLine = objec->baseline;
    if (baseLine <= 0)
    {
      baseLine = objec->y;
    }
	if (objec->on && objec->transparent<100)
    {
      ItemToDraw[numThingsToDraw].objIndex = i;
      ItemToDraw[numThingsToDraw].chrIndex = -1;
      ItemToDraw[numThingsToDraw].walkIndex=-1;
      ItemToDraw[numThingsToDraw].baseLine = baseLine;
	  ItemToDraw[numThingsToDraw].ignoreWB = (objec->flags & OBJF_NOWALKBEHINDS);
      if (ItemToDraw[numThingsToDraw].ignoreWB)
      {
        ItemToDraw[numThingsToDraw].baseLine +=1000;
      }
      numThingsToDraw++;      
    }
    i++;
  }
  i = 0;
  
  while (i < GameCharacterCount)
  {
	AGSCharacter *getchar=engine->GetCharacter(i);

	if (getchar->room == playerRoom)
    {
	  int baseLine = getchar->baseline;
      if (baseLine <= 0)
      {
		  baseLine = getchar->y;
      }
	  if (getchar->transparency<100)
      {        
        ItemToDraw[numThingsToDraw].objIndex = -1;
        ItemToDraw[numThingsToDraw].chrIndex = i;
        ItemToDraw[numThingsToDraw].walkIndex=-1;
        ItemToDraw[numThingsToDraw].baseLine = baseLine;
        ItemToDraw[numThingsToDraw].ignoreWB = (getchar->flags & CHF_NOWALKBEHINDS);
        if (ItemToDraw[numThingsToDraw].ignoreWB)
        {
          ItemToDraw[numThingsToDraw].baseLine +=1000;
        }
        numThingsToDraw++;        
      }
    }
    i++;
  }
  
  //PARSE THE WALKBEHINDS
  i=1;
  while (i <= MaxWalkBehinds)
  {
    ItemToDraw[numThingsToDraw].objIndex=-1;
    ItemToDraw[numThingsToDraw].chrIndex=-1;	
    ItemToDraw[numThingsToDraw].baseLine=engine->GetWalkbehindBaseline(i);//Walkbehind[i];
	Walkbehind[i]=ItemToDraw[numThingsToDraw].baseLine;
    ItemToDraw[numThingsToDraw].walkIndex=i;
    numThingsToDraw++;
    i++;
  }
  //PARSE THE WALKBEHINDS
  
  
  // now bubble sort
  i = 0;
  while (i < numThingsToDraw-1)
  {
    int j = i+1;
    while (j < numThingsToDraw)
    {
      
      bool cond = ItemToDraw[j].baseLine < ItemToDraw[i].baseLine;
      
      if (cond)
      {
        // swap
        int objIndex = ItemToDraw[j].objIndex;
        int chrIndex = ItemToDraw[j].chrIndex;
        int wlkIndex = ItemToDraw[j].walkIndex;
        int baseLine = ItemToDraw[j].baseLine;
        
        
        ItemToDraw[j].objIndex = ItemToDraw[i].objIndex;
        ItemToDraw[j].chrIndex = ItemToDraw[i].chrIndex;
        ItemToDraw[j].walkIndex = ItemToDraw[i].walkIndex;
        ItemToDraw[j].baseLine = ItemToDraw[i].baseLine;
        ItemToDraw[i].objIndex = objIndex;
        ItemToDraw[i].chrIndex = chrIndex;
        ItemToDraw[i].walkIndex = wlkIndex;
        ItemToDraw[i].baseLine = baseLine;     
        
      }
      j++;
    }
    i++;
  }

  return numThingsToDraw;

}

int Objindex(int i)
{
	return ItemToDraw[i].objIndex;
}

int Chrindex(int i)
{
	return ItemToDraw[i].chrIndex;
}

int Walkindex(int i)
{
	return ItemToDraw[i].walkIndex;
}

int Baseindex(int i)
{
	return ItemToDraw[i].baseLine;
}


void AGS_EngineInitGfx(const char *driverID, void *data)
{
}


void AGS_EngineStartup(IAGSEngine *lpEngine)
{
  engine = lpEngine;
  
  if (engine->version < 13) 
    engine->AbortGame("Engine interface is too old, need newer version of AGS.");
  
	//engine->GetWalkbehindBaseline();
	engine->RegisterScriptFunction("DrawBoxBlur",(void*)&DrawBoxBlur);
	engine->RegisterScriptFunction("ReadWalkBehindIntoSprite",(void*)&ReadWalkBehindIntoSprite);
	engine->RegisterScriptFunction("CalculateThings",(void*)&CalculateThings);
	engine->RegisterScriptFunction("Objindex",(void*)&Objindex);
	engine->RegisterScriptFunction("Chrindex",(void*)&Chrindex);
	engine->RegisterScriptFunction("Walkindex",(void*)&Walkindex);
	engine->RegisterScriptFunction("Baseindex",(void*)&Baseindex);
    


  engine->RequestEventHook(AGSE_PREGUIDRAW);
  engine->RequestEventHook(AGSE_PRESCREENDRAW);
  engine->RequestEventHook(AGSE_SAVEGAME);
  engine->RequestEventHook(AGSE_RESTOREGAME);
  engine->RequestEventHook(AGSE_POSTSCREENDRAW);

}



void AGS_EngineShutdown()
{
	
}

int AGS_EngineOnEvent(int event, int data)
{
  if (event == AGSE_PREGUIDRAW)
  {
  }  
  else if (event == AGSE_RESTOREGAME)
  {
	  //engine->FRead(&SFX[j].repeat,sizeof(int),data);
  }
  else if (event == AGSE_SAVEGAME)
  {
	 // engine->FWrite(&currentMusic, sizeof(int), data);	  
  }
  else if (event == AGSE_PRESCREENDRAW)
  {
    // Get screen size once here.
    engine->GetScreenDimensions(&screen_width, &screen_height, &screen_color_depth);	
	
  }
  else if (event ==AGSE_POSTSCREENDRAW)
  {
	  
  }
  return 0;
}

int AGS_EngineDebugHook(const char *scriptName, int lineNum, int reserved)
{
  return 0;
}



#if defined(WINDOWS_VERSION) && !defined(BUILTIN_PLUGINS)

// ********************************************
// ***********  Editor Interface  *************
// ********************************************
const char* scriptHeader =
"import void DrawBoxBlur(int spriteD, int radius);\r\n"
"import void ReadWalkBehindIntoSprite(int sprite,int bgsprite,int walkbehindBaseline);\r\n"
"import int CalculateThings();\r\n"
"import int Objindex(int i);\r\n"
"import int Chrindex(int i);\r\n"
"import int Walkindex(int i);\r\n"
"import int Baseindex(int i);\r\n"

   ;

IAGSEditor* editor;


LPCSTR AGS_GetPluginName(void)
{
  // Return the plugin description
  return "AGSFakeScreen";
}

int  AGS_EditorStartup(IAGSEditor* lpEditor)
{
  // User has checked the plugin to use it in their game

  // If it's an earlier version than what we need, abort.
  if (lpEditor->version < 1)
    return -1;

  editor = lpEditor;
  editor->RegisterScriptHeader(scriptHeader);

  // Return 0 to indicate success
  return 0;
}

void AGS_EditorShutdown()
{
  // User has un-checked the plugin from their game
  editor->UnregisterScriptHeader(scriptHeader);
}

void AGS_EditorProperties(HWND parent)
{
  // User has chosen to view the Properties of the plugin
  // We could load up an options dialog or something here instead
  MessageBoxA(parent, "AGS Fake Screen", "About", MB_OK | MB_ICONINFORMATION);
}

int AGS_EditorSaveGame(char* buffer, int bufsize)
{
  // We don't want to save any persistent data
  return 0;
}

void AGS_EditorLoadGame(char* buffer, int bufsize)
{
  // Nothing to load for this plugin
}

#endif


#if defined(BUILTIN_PLUGINS)
} // namespace agsfakescreen
#endif
