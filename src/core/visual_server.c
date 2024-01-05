#include "visual_server.h"

#include "../game_objects/textures/filepath_table.h"

static SDL_Renderer* Canvas;

typedef struct {
    unsigned short ref_count;
    SDL_Texture* texture;
} TextureResource;

static TextureResource textureDB[MAX_TEXTURE_COUNT];

BOOL InitVisualServer(SDL_Window* HWind) {
    if(!(Canvas = SDL_CreateRenderer(HWind, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)))
        return FALSE;
    SDL_SetRenderDrawBlendMode(Canvas, SDL_BLENDMODE_BLEND);
    memset(textureDB, 0, MAX_TEXTURE_COUNT*sizeof(TextureResource));
    return TRUE;
}

void TerminateVisualServer() {
    SDL_DestroyRenderer(Canvas);
    TextureResource* end = textureDB + MAX_TEXTURE_COUNT;
    for(TextureResource* itr = textureDB; itr != end; ++itr)
        if(itr->ref_count) 
            SDL_DestroyTexture(itr->texture);
}

void DrawCanvas(Map* map) {
    SDL_SetRenderTarget(Canvas, NULL);
    SDL_RenderClear(Canvas);

    SDL_Rect rect = {
        map->window_offset.x,
        map->window_offset.y,
        WINDOW_WIDTH - map->window_offset.w,
        WINDOW_HEIGHT - map->window_offset.h
    };
    
    Z_Index* end = map->z_index + map->z_index_count;
    for(Z_Index* itr = map->z_index; itr != end; ++itr)
        if(itr->entity.val[0] != NULL)
            SDL_RenderCopy(Canvas, itr->cache_layer, NULL, &rect);
    SDL_RenderPresent(Canvas);
}

void ClearCanvas() {
    SDL_SetRenderTarget(Canvas, NULL);
    SDL_RenderClear(Canvas);
    SDL_RenderPresent(Canvas);
}

SDL_Texture* CreateCacheLayer(const unsigned short width, const unsigned short height) {
    SDL_Texture* temp = SDL_CreateTexture(Canvas, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    SDL_SetTextureBlendMode(temp, SDL_BLENDMODE_BLEND);
    return temp;
}

void SetCacheLayer(SDL_Texture* layer) {
    SDL_SetRenderTarget(Canvas, layer);
    SDL_SetRenderDrawColor(Canvas, 0, 0, 0, 0);
    SDL_RenderClear(Canvas);
}

void UpdateCacheLayer(Entity* entity) {
    SDL_Rect src_rect = get_src_rect(entity);
    SDL_FRect dest_rect = get_dest_rect(entity);
    SDL_RenderCopyExF(
        Canvas, 
        textureDB[entity->texture_ref].texture, 
        &src_rect, 
        &dest_rect,
        entity->angle,
        &(entity->extent.offset),
        entity->flip
    );
    entity->redraw = FALSE;
}

SDL_Texture* GetTexture(TextureEnums code) {
    return textureDB[code].texture;
}

void LoadTextureResource(TextureEnums code) {
    textureDB[code].ref_count++;
    if(textureDB[code].ref_count != 1)
        return;
    char absolutePath[256];
    snprintf(absolutePath, sizeof(absolutePath), "%s%s", SDL_GetBasePath(), GetAssetFilePath(code));
    textureDB[code].texture = IMG_LoadTexture(Canvas, absolutePath);
}

void FreeTextureResource(TextureEnums code) {
    textureDB[code].ref_count--;
    if(!(textureDB[code].ref_count))
        SDL_DestroyTexture(textureDB[code].texture);
}

BOOL FileSystemAssetsScan() {
    BOOL result = TRUE;
    FILE* temp;
    for(int i = 0; i < MAX_TEXTURE_COUNT; ++i) {
        char absolutePath[256];
        snprintf(absolutePath, sizeof(absolutePath), "%s%s", SDL_GetBasePath(), GetAssetFilePath(i));
        if(!(temp = fopen(absolutePath, "rb"))) {
            result = FALSE;
            fprintf(stderr, "Required Asset file: \"%s\" is missing...\n", absolutePath);
            continue;
        }
        fclose(temp);
    }
    return result;
}