#include <SDL3/SDL.h>
#include <algorithm>
#include <vector>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int NUM_BARS = 50;
const int BAR_PADDING = 1; // gap between bars in pixels

// Fills the array with values 1..size and shuffles them randomly
void initArray(std::vector<int> &arr) {
  for (int i = 0; i < (int)arr.size(); i++)
    arr[i] = i + 1;
  // SDL_rand gives a random int in [0, n)
  // Fisher-Yates shuffle: walk backwards, swap each element with a random one
  // before it
  for (int i = (int)arr.size() - 1; i > 0; i--) {
    int j = SDL_rand(i + 1);
    std::swap(arr[i], arr[j]);
  }
}

// Draws all bars onto the renderer
void renderBars(SDL_Renderer *renderer, const std::vector<int> &arr) {
  int n = arr.size();

  // Each bar gets an equal slice of the window width
  float barWidth = (float)WINDOW_WIDTH / n;

  for (int i = 0; i < n; i++) {
    // Bar height is proportional to its value
    // arr[i] ranges from 1..n, map to 1..WINDOW_HEIGHT
    float barHeight = ((float)arr[i] / n) * WINDOW_HEIGHT;

    SDL_FRect rect;
    rect.x = i * barWidth + BAR_PADDING;
    rect.y = WINDOW_HEIGHT - barHeight; // SDL y=0 is top, so flip
    rect.w = barWidth - BAR_PADDING;
    rect.h = barHeight;

    SDL_SetRenderDrawColor(renderer, 100, 200, 255, 255); // light blue
    SDL_RenderFillRect(renderer, &rect);
  }
}

int main(int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL_Init failed: %s", SDL_GetError());
    return 1;
  }

  SDL_Window *window =
      SDL_CreateWindow("Sorting Visualizer", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  if (!window) {
    SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
  if (!renderer) {
    SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  std::vector<int> arr(NUM_BARS);
  initArray(arr);

  bool running = true;
  SDL_Event event;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        running = false;
      if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
        running = false;
    }

    // Clear to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    renderBars(renderer, arr);

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
