#include "host.h"

Host::Host() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("CHIP8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
}

Host::~Host() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Host::update(void const* buffer, int pitch) {
    
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}



/*
1 2 3 4 --> 1 2 3 C
Q W E R --> 4 5 6 D
A S D F --> 7 8 9 E
Z X C V --> A 0 B F
*/
bool Host::pollKeys(uint8_t* keys) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return true;
        } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            bool pressed = event.type == SDL_KEYDOWN;
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE: return true;  ; break;
                case SDLK_1: keys[0x1] = pressed; break;
                case SDLK_2: keys[0x2] = pressed; break;
                case SDLK_3: keys[0x3] = pressed; break;
                case SDLK_4: keys[0xC] = pressed; break;
                case SDLK_q: keys[0x4] = pressed; break;
                case SDLK_w: keys[0x5] = pressed; break;
                case SDLK_e: keys[0x6] = pressed; break;
                case SDLK_r: keys[0xD] = pressed; break;
                case SDLK_a: keys[0x7] = pressed; break;
                case SDLK_s: keys[0x8] = pressed; break;
                case SDLK_d: keys[0x9] = pressed; break;
                case SDLK_f: keys[0xE] = pressed; break;
                case SDLK_z: keys[0xA] = pressed; break;
                case SDLK_x: keys[0x0] = pressed; break;
                case SDLK_c: keys[0xB] = pressed; break;
                case SDLK_v: keys[0xF] = pressed; break;
            }
        }
    }
    return false;
}