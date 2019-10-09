#ifndef STDAFX_H
#define STDAFX_H


#include <core/SkBitmap.h>
#include <core/SkCanvas.h>
#include <core/SkTypeface.h>
#include <string.h>

#ifdef _WIN32

#pragma warning(disable:4996)
#include <SDL.h>
#include <SDL_thread.h>
#else
#include <SDL2/SDL.h>
#endif

constexpr auto MAX_BUFFER_SIZE = 2048;

SDL_Window* window;
SDL_Surface* surface;
SDL_Renderer* renderer;
SDL_Texture* texture;
SkBitmap bitmap;
SDL_Rect rect;
int fps;
int width = 800;
int height = 480;
bool quit = false;
char buffer[MAX_BUFFER_SIZE];

struct RGBA {
	Uint32 rmask = 0x00ff0000;
	Uint32 gmask = 0x0000ff00;
	Uint32 bmask = 0x000000ff;
	Uint32 amask = 0xff000000;
}RGBA;


SkBitmap Draw(int w, int h) {
	SkBitmap bitmap;
	bitmap.setInfo(SkImageInfo::Make(w, h, kBGRA_8888_SkColorType, kOpaque_SkAlphaType));
	bitmap.allocPixels();
	SkCanvas canvas(bitmap);
	SkPaint paint;
	canvas.clear(SK_ColorBLACK);
	paint.setAntiAlias(true);
	paint.setARGB(200, 230, 230, 230);
	paint.setTextSize(60);
	canvas.drawString("Voxel Logic", 200, 300, paint);
	paint.setTextSize(20);
	canvas.drawText(buffer, sizeof((char*)buffer) - 1, 10, 20, paint);
	return bitmap;
}

SDL_Rect CreateRect(SDL_Surface* surface) {
	SDL_Rect src = { 0, 0, surface->w, surface->h };
	return src;
}

int Update(void* data) {
	while (!quit) {
		if (surface != NULL) SDL_FreeSurface(surface);
		if (texture != NULL) SDL_DestroyTexture(texture);
		bitmap = Draw(rect.w, rect.h);
		surface = SDL_CreateRGBSurfaceFrom(bitmap.getPixels(), rect.w, rect.h, 32, rect.w * 4, RGBA.rmask, RGBA.gmask,
			RGBA.bmask, RGBA.amask);
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_RenderCopy(renderer, texture, NULL, &rect);
		SDL_RenderPresent(renderer);
		SDL_Delay(16.6);
		fps++;
	}
	return 0;
}

int UpdateFPS(void* data) {
	while (!quit) {
		sprintf(buffer, "FPS: %d", fps);
		fps = 0;
		SDL_Delay(1000);
	}
	return 0;
}

void Init(const char* name, int width, int height)
{
	quit = false;
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_RenderClear(renderer);
	surface = SDL_GetWindowSurface(window);
	rect = CreateRect(surface);
}

void Destroy()
{
	if (surface != NULL) SDL_FreeSurface(surface);
	if (texture != NULL) SDL_DestroyTexture(texture);
	if (renderer != NULL) SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void VoxelUI()
{
	Init("Voxel Logic", 800, 600);
	SDL_Thread* thread = SDL_CreateThread(Update, "update", (void*)NULL);
	SDL_Thread* fpsThread = SDL_CreateThread(UpdateFPS, "fps", (void*)NULL);
	SDL_Event e;
	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT)
				quit = true;
		}
	}
	Destroy();
}

#endif // !STDAFX_H
