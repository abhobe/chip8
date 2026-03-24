#pragma once
#include <SDL2/SDL.h>

class Host {
public:
    Host();
    ~Host();
    void update(void const* buffer, int pitch);
    bool pollKeys(uint8_t* keys);

private:
	SDL_Window* window{};
	SDL_Renderer* renderer{};
	SDL_Texture* texture{};
};