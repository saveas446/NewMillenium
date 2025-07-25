#include "../doomdef.h"
#include "../command.h"
#include "../i_video.h"
#include "i_video.h"
#include <SDL.h>
#include "../screen.h"
#include "../z_zone.h"
#include "../i_system.h"

rendermode_t rendermode = render_soft;

boolean highcolor = false;

boolean allow_fullscreen = false;

consvar_t cv_vidwait = {"vid_wait", "On", NULL, CAT_VIDEO, CV_SAVE, CV_OnOff, NULL, 0, 0, NULL};

SDL_Window* SDL_window = NULL;

SDL_Renderer* SDL_renderer;
SDL_Surface* surface;
SDL_Surface* window_surface;
SDL_Surface* icon_surface;

vmode_t window_modes[10] = {
		// Fallback mode, 320x200 is gross
		{
			NULL,
			"320x200W", //faB: W to make sure it's the windowed mode
			320, 200,   //(200.0/320.0)*(320.0/240.0),
			320, 1,     // rowbytes, bytes per pixel
			1, 2,       // windowed (TRUE), numpages
			NULL,
			NULL,
			0          // misc
		},
		// Non-fallback copy of 320x200W, if you WANT to use 320x200W for some reason
		{
			NULL,
			"320x200W", //faB: W to make sure it's the windowed mode
			320, 200,   //(200.0/320.0)*(320.0/240.0),
			320, 1,     // rowbytes, bytes per pixel
			1, 2,       // windowed (TRUE), numpages
			NULL,
			NULL,
			0          // misc
		},
		{
			NULL,
			"320x240W", //faB: W to make sure it's the windowed mode
			320, 240,   //(200.0/320.0)*(320.0/240.0),
			320, 1,     // rowbytes, bytes per pixel
			1, 2,       // windowed (TRUE), numpages
			NULL,
			NULL,
			0          // misc
		},
		{
			NULL,
			"640x400W", //faB: W to make sure it's the windowed mode
			640, 400,   //(200.0/320.0)*(320.0/240.0),
			640, 1,     // rowbytes, bytes per pixel
			1, 2,       // windowed (TRUE), numpages
			NULL,
			NULL,
			0          // misc
		},
		{
			NULL,
			"640x480W", //faB: W to make sure it's the windowed mode
			640, 480,   //(200.0/320.0)*(320.0/240.0),
			640, 1,     // rowbytes, bytes per pixel
			1, 2,       // windowed (TRUE), numpages
			NULL,
			NULL,
			0          // misc
		},
		{
			NULL,
			"800x600W", //faB: W to make sure it's the windowed mode
			800, 600,   //(200.0/320.0)*(320.0/240.0),
			800, 1,     // rowbytes, bytes per pixel
			1, 2,       // windowed (TRUE), numpages
			NULL,
			NULL,
			0          // misc
		},
		{
			NULL,
			"1024x768W", //faB: W to make sure it's the windowed mode
			1024, 768,   //(200.0/320.0)*(320.0/240.0),
			1024, 1,     // rowbytes, bytes per pixel
			1, 2,       // windowed (TRUE), numpages
			NULL,
			NULL,
			0          // misc
		},
		{
			NULL,
			"1280x720W", //faB: W to make sure it's the windowed mode
			1280, 720,   //(200.0/320.0)*(320.0/240.0),
			1280, 1,     // rowbytes, bytes per pixel
			1, 2,       // windowed (TRUE), numpages
			NULL,
			NULL,
			0          // misc
		},
		{
			NULL,
			"1280x800W", //faB: W to make sure it's the windowed mode
			1280, 800,   //(200.0/320.0)*(320.0/240.0),
			1280, 1,     // rowbytes, bytes per pixel
			1, 2,       // windowed (TRUE), numpages
			NULL,
			NULL,
			0          // misc
		},
};

static void SetSDLIcon(SDL_Window* window)
{

	// Somebody please find a better way to do this later, I hate #including .c files
#include "Srb2win.c"

	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	int shift = (my_icon.bytes_per_pixel == 3) ? 8 : 0;
	rmask = 0xff000000 >> shift;
	gmask = 0x00ff0000 >> shift;
	bmask = 0x0000ff00 >> shift;
	amask = 0x000000ff >> shift;
#else // little endian, like x86
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = (gimp_image.bytes_per_pixel == 3) ? 0 : 0xff000000;
#endif

	icon_surface = SDL_CreateRGBSurfaceFrom((void*)gimp_image.pixel_data,
		gimp_image.width, gimp_image.height, gimp_image.bytes_per_pixel * 8,
		gimp_image.bytes_per_pixel * gimp_image.width, rmask, gmask, bmask, amask);

	SDL_SetWindowIcon(SDL_window, icon_surface);

	SDL_FreeSurface(icon_surface);

}

void I_ShutdownGraphics(void){
	CONS_Printf("I_ShutdownGraphics...\n");
	SDL_DestroyRenderer(SDL_renderer);
	SDL_DestroyWindow(SDL_window);
	SDL_FreeSurface(surface);
	free(vid.buffer);
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	graphics_started = false;
}

SDL_Color palettebuf[256];

// Translate Doom palette into SDL palette
void I_SetPalette(byte *palette)
{
	RGB_t* rgbpalette;
	rgbpalette = palette;

	// 256 colors * 3 color channels
	for (int i = 0; i < 256; i++) {

		palettebuf[i].r = rgbpalette[i].r;
		palettebuf[i].g = rgbpalette[i].g;
		palettebuf[i].b = rgbpalette[i].b;
		palettebuf[i].a = 255;

		SDL_SetPaletteColors(surface->format->palette, palettebuf, 0, 256);
	}
}

int VID_NumModes(void)
{
	// TODO: Find a way to get length of windowed_modes and return that
	return 10;
}

int VID_GetModeForSize(int w, int h)
{
	return 3;
}

// TODO: Some of the stuff done when we change modes might not be needed. See how much we can keep between mode switches (For example, we might just be able to change the window's dimensions instead of destroying and remaking it)
int VID_SetMode(int modenum)
{
	if (SDL_window != NULL)
		SDL_DestroyWindow(SDL_window);

	if (SDL_renderer != NULL)
		SDL_DestroyRenderer(SDL_renderer);

	if (surface != NULL)
		SDL_FreeSurface(surface);

	vid.modenum = modenum;
	vid.width = window_modes[modenum].width;
	vid.height = window_modes[modenum].height;
	vid.bpp = window_modes[modenum].bytesperpixel;
	vid.rowbytes = window_modes[modenum].rowbytes;
	vid.dupx = vid.width / 320;
	vid.dupy = vid.height / 200;
	vid.recalc = 1;

	// Init window (hardcoded to 640x400 for now) in the center of the screen
	SDL_window = SDL_CreateWindow("NewMillennium (SDL2 backend)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, vid.width, vid.height, 0);
	SetSDLIcon(SDL_window);

	if (!SDL_window)
		I_Error("I_StartupGraphics(): Could not create window!");

	SDL_renderer = SDL_CreateRenderer(SDL_window, -1, SDL_RENDERER_ACCELERATED);
	if (!SDL_renderer)
		I_Error("I_StartupGraphics(): Could not create renderer!");

	surface = SDL_CreateRGBSurfaceWithFormat(0, vid.width, vid.height, 8, SDL_PIXELFORMAT_INDEX8);

	window_surface = SDL_GetWindowSurface(SDL_window);

	// allocate buffer
	if (vid.buffer)
		free(vid.buffer);

	vid.buffer = malloc((vid.width * vid.height * vid.bpp * NUMSCREENS) + (vid.width * ST_HEIGHT * vid.bpp));

	return 0;
}

void I_StartupGraphics(void) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		I_Error("Could not initialize SDL2: %s\n", SDL_GetError());
	
	VID_SetMode(3);

	graphics_started = true;
}

const char *VID_GetModeName(int modenum)
{
	return window_modes[modenum].name;
}

void I_UpdateNoBlit(void){}

void I_FinishUpdate(void)
{
	byte* pixels = surface->pixels;
	// Copy vid.buffer to our surface
	for (int i = 0; i < vid.width * vid.height; i++) {
		pixels[i] = vid.buffer[i];
	}

	SDL_BlitSurface(surface, NULL, window_surface, NULL);
	SDL_UpdateWindowSurface(SDL_window);
}

void I_WaitVBL(int count)
{
	count = 0;
}

void I_ReadScreen(byte *scr)
{
	SDL_memcpy(scr, vid.buffer, vid.width * vid.height * vid.bpp);
}

// For some reason, I get a message about this not existing when building on Linux
#if _WIN32
void VID_BlitLinearScreen(byte* srcptr, byte* destptr, int width, int height, int srcrowbytes, int destrowbytes){
	if (srcrowbytes == destrowbytes)
	{
		SDL_memcpy(destptr, srcptr, srcrowbytes * height);
	}
	else
	{
		while (height--)
		{
			memcpy(destptr, srcptr, width);
			destptr += destrowbytes;
			srcptr += srcrowbytes;
		}
	}
}
#endif

void I_BeginRead(void){}

void I_EndRead(void){}

