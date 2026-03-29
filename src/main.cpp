#include <SDL3/SDL.h>
#include <algorithm>
#include <vector>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int NUM_BARS = 50;
const int BAR_PADDING = 1;
const int STEP_DELAY_MS = 15; // milliseconds between sort steps

enum class State { IDLE, SORTING, DONE };

struct BubbleSort {
  int i = 0; // outer loop counter (how many passes done)
  int j = 0; // inner loop counter (current comparison index)
};

void initArray(std::vector<int> &arr) {
  for (int i = 0; i < (int)arr.size(); i++)
    arr[i] = i + 1;
  for (int i = (int)arr.size() - 1; i > 0; i--) {
    int j = SDL_rand(i + 1);
    std::swap(arr[i], arr[j]);
  }
}

// Advance bubble sort by one comparison step.
// Returns true if the sort is complete.
bool stepBubbleSort(std::vector<int> &arr, BubbleSort &bs) {
  int n = arr.size();

  // Compare adjacent pair at position j
  if (arr[bs.j] > arr[bs.j + 1])
    std::swap(arr[bs.j], arr[bs.j + 1]);

  bs.j++;

  // End of inner loop pass: the largest unsorted element has bubbled to its
  // final position
  if (bs.j >= n - 1 - bs.i) {
    bs.i++;
    bs.j = 0;
  }

  // Sort is done when we've completed n-1 passes
  return bs.i >= n - 1;
}

void renderBars(SDL_Renderer *renderer, const std::vector<int> &arr,
                const BubbleSort &bs, State state) {
  int n = arr.size();
  float barWidth = (float)WINDOW_WIDTH / n;

  for (int i = 0; i < n; i++) {
    float barHeight = ((float)arr[i] / n) * WINDOW_HEIGHT;

    SDL_FRect rect;
    rect.x = i * barWidth + BAR_PADDING;
    rect.y = WINDOW_HEIGHT - barHeight;
    rect.w = barWidth - BAR_PADDING;
    rect.h = barHeight;

    if (state == State::DONE) {
      // All bars green when sorted
      SDL_SetRenderDrawColor(renderer, 0, 220, 100, 255);
    } else if (state == State::SORTING && (i == bs.j || i == bs.j + 1)) {
      // Currently compared pair: orange
      SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
    } else if (state == State::SORTING && i >= (int)arr.size() - bs.i) {
      // Already sorted suffix: green
      SDL_SetRenderDrawColor(renderer, 0, 220, 100, 255);
    } else {
      // Default: light blue
      SDL_SetRenderDrawColor(renderer, 100, 200, 255, 255);
    }

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

  State state = State::IDLE;
  BubbleSort bs;
  Uint64 lastStepTime = 0;

  bool running = true;
  SDL_Event event;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        running = false;

      if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.key) {
        case SDLK_ESCAPE:
          running = false;
          break;

        case SDLK_SPACE:
          // Start sorting only if idle
          if (state == State::IDLE) {
            state = State::SORTING;
            bs = BubbleSort{};
            lastStepTime = SDL_GetTicks();
          }
          break;

        case SDLK_R:
          // Reset: reshuffle and go back to idle
          initArray(arr);
          state = State::IDLE;
          bs = BubbleSort{};
          break;
        }
      }
    }

    // Advance the sort one step per STEP_DELAY_MS
    if (state == State::SORTING) {
      Uint64 now = SDL_GetTicks();
      if (now - lastStepTime >= STEP_DELAY_MS) {
        bool done = stepBubbleSort(arr, bs);
        if (done)
          state = State::DONE;
        lastStepTime = now;
      }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    renderBars(renderer, arr, bs, state);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
