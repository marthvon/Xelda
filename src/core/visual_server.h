#ifndef VISUALSERVER_H
#define VISUALSERVER_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "../globals.h"
#include "../components/entity.h"
#include "../components/map.h"
#include "../game_objects/textures/texture_table.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 640

BOOL InitVisualServer(SDL_Window* HWind);
void TerminateVisualServer();

void DrawCanvas(Map* map);
void ClearCanvas();
SDL_Texture* CreateCacheLayer(const unsigned short width, const unsigned short height);
void SetCacheLayer(SDL_Texture* layer);
void UpdateCacheLayer(Entity* entity);
SDL_Texture* GetTexture(TextureEnums code);

void LoadTextureResource(TextureEnums code);
void FreeTextureResource(TextureEnums code);

BOOL FileSystemAssetsScan();

#endif