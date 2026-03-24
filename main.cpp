#include "chip8.h"
#include "host.h"
#include <iostream>
#include <chrono>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << "<delay> <rom>" << std::endl;
        return 1;
    }
    int delay = std::stoi(argv[1]);
    const char* rom = argv[2];

    Host host;
    Chip8 chip8;
    if (!chip8.load(rom)) {
        std::cerr << "Failed to load ROM: " << rom << std::endl;
        return 1;
    }

    auto last = std::chrono::high_resolution_clock::now();
    bool exit = false;
    while (!exit) {
        exit = host.pollKeys(chip8.keys);

        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count();
        if (elapsed >= delay) {
            chip8.cycle();
            host.update(chip8.display, sizeof(chip8.display[0]) * 64);
            last = now;
        }
    }
    return 0;
}