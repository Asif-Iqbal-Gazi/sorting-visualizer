#include <SDL3/SDL.h>

int main(int argc, char* argv[]) {
    // Initialize SDL - we only need the video subsystem for now
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    // Create a window: title, width, height, flags
    SDL_Window* window = SDL_CreateWindow("Sorting Visualizer", 800, 600, 0);
    if (!window) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer tied to the window
    // The renderer is what actually draws things on screen
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Main loop - keeps the window open until the user closes it
    bool running = true;
    SDL_Event event;

    while (running) {
        // Poll for events (keyboard, mouse, window close, etc.)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            // ESC key also closes the window
            if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.key == SDLK_ESCAPE) {
                running = false;
            }
        }

        // Set draw color to black (R, G, B, A)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        // Clear the entire screen with the current draw color
        SDL_RenderClear(renderer);

        // Present the rendered frame to the screen (swap buffers)
        SDL_RenderPresent(renderer);
    }

    // Always clean up in reverse order of creation
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
