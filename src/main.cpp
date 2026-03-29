#include <SDL3/SDL.h>
#include <algorithm>
#include <string>
#include <vector>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int BAR_PADDING = 1;

const int MIN_BARS = 10;
const int MAX_BARS = 200;
const int BAR_STEP = 10;

const int MIN_DELAY = 1;
const int MAX_DELAY = 100;
const int DELAY_STEP = 5;

enum class State { IDLE, SORTING, DONE };

struct BubbleSort {
  int i = 0;
  int j = 0;
};

void initArray(std::vector<int> &arr) {
  for (int i = 0; i < (int)arr.size(); i++)
    arr[i] = i + 1;
  for (int i = (int)arr.size() - 1; i > 0; i--) {
    int j = SDL_rand(i + 1);
    std::swap(arr[i], arr[j]);
  }
}

bool stepBubbleSort(std::vector<int> &arr, BubbleSort &bs) {
  int n = arr.size();
  if (arr[bs.j] > arr[bs.j + 1])
    std::swap(arr[bs.j], arr[bs.j + 1]);
  bs.j++;
  if (bs.j >= n - 1 - bs.i) {
    bs.i++;
    bs.j = 0;
  }
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
      SDL_SetRenderDrawColor(renderer, 0, 220, 100, 255);
    } else if (state == State::SORTING && (i == bs.j || i == bs.j + 1)) {
      SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
    } else if (state == State::SORTING && i >= (int)arr.size() - bs.i) {
      SDL_SetRenderDrawColor(renderer, 0, 220, 100, 255);
    } else {
      SDL_SetRenderDrawColor(renderer, 100, 200, 255, 255);
    }

    SDL_RenderFillRect(renderer, &rect);
  }
}

// Updates the window title to reflect current settings and state
void updateTitle(SDL_Window *window, int numBars, int stepDelay, State state) {
  std::string status;
  switch (state) {
  case State::IDLE:
    status = "SPACE to sort";
    break;
  case State::SORTING:
    status = "Sorting...";
    break;
  case State::DONE:
    status = "Done! R to reset";
    break;
  }

  std::string title = "Sorting Visualizer  |  Bars: " + std::to_string(numBars) +
                      " (UP/DOWN)  |  Speed: " + std::to_string(stepDelay) +
                      "ms (LEFT/RIGHT)  |  " + status;
  SDL_SetWindowTitle(window, title.c_str());
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

  int numBars = 50;
  int stepDelay = 15;

  std::vector<int> arr(numBars);
  initArray(arr);

  State state = State::IDLE;
  BubbleSort bs;
  Uint64 lastStepTime = 0;

  updateTitle(window, numBars, stepDelay, state);

  bool running = true;
  SDL_Event event;

  while (running) {
    bool titleDirty = false;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        running = false;

      if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.key) {
        case SDLK_ESCAPE:
          running = false;
          break;

        case SDLK_SPACE:
          if (state == State::IDLE) {
            state = State::SORTING;
            bs = BubbleSort{};
            lastStepTime = SDL_GetTicks();
            titleDirty = true;
          }
          break;

        case SDLK_R:
          initArray(arr);
          state = State::IDLE;
          bs = BubbleSort{};
          titleDirty = true;
          break;

        case SDLK_UP:
          // Increase bar count (only when idle so we can reinitialize cleanly)
          if (state == State::IDLE && numBars + BAR_STEP <= MAX_BARS) {
            numBars += BAR_STEP;
            arr.resize(numBars);
            initArray(arr);
            titleDirty = true;
          }
          break;

        case SDLK_DOWN:
          if (state == State::IDLE && numBars - BAR_STEP >= MIN_BARS) {
            numBars -= BAR_STEP;
            arr.resize(numBars);
            initArray(arr);
            titleDirty = true;
          }
          break;

        case SDLK_RIGHT:
          // Faster = lower delay
          if (stepDelay - DELAY_STEP >= MIN_DELAY) {
            stepDelay -= DELAY_STEP;
            titleDirty = true;
          }
          break;

        case SDLK_LEFT:
          // Slower = higher delay
          if (stepDelay + DELAY_STEP <= MAX_DELAY) {
            stepDelay += DELAY_STEP;
            titleDirty = true;
          }
          break;
        }
      }
    }

    if (state == State::SORTING) {
      Uint64 now = SDL_GetTicks();
      if (now - lastStepTime >= (Uint64)stepDelay) {
        bool done = stepBubbleSort(arr, bs);
        if (done) {
          state = State::DONE;
          titleDirty = true;
        }
        lastStepTime = now;
      }
    }

    if (titleDirty)
      updateTitle(window, numBars, stepDelay, state);

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
