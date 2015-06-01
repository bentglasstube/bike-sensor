#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>

#include <SDL2/SDL.h>

static const int   WIDTH        = 32 * 6;
static const int   HEIGHT       = 64 * 3;
static const int   WHEEL_DIA    = 16;
static const int   TIMEOUT      = 3000;
static const float PI           = 3.14159265358979f;
static const float DIST_PER_REV = WHEEL_DIA * PI / 5280 / 12;
static const float VELO_FALLOFF = 0.0005f;

// TODO pass at runtime
static const char* SERIAL = "/dev/cu.usbmodem12341";

typedef struct {
  float dist, time, velo;
  unsigned long last_rev, next_rev;
} Stats;

void draw_text(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, char* text) {
  SDL_Rect source = { 0, 0, 0, 64 };
  SDL_Rect dest   = { x, y, 0, 64 };

  for (int i = 0; text[i] != 0; ++i) {
    if (text[i] >= '0' && text[i] <= '9') {
      source.x = (text[i] - '/') * 32;
      source.w = dest.w = 32;
    } else if (text[i] == ' ') {
      source.x = 0;
      source.w = dest.w = 32;
    } else if (text[i] == ':') {
      source.x = 352;
      source.w = dest.w = 16;
    } else if (text[i] == '.') {
      source.x = 368;
      source.w = dest.w = 16;
    }

    SDL_RenderCopy(renderer, texture, &source, &dest);
    dest.x += source.w;
  }
}

void draw_rect(SDL_Renderer* renderer, SDL_Texture* texture, int dx, int dy, int sx, int sy, int w, int h) {
  SDL_Rect source = {sx, sy, w, h};
  SDL_Rect dest = {dx, dy, w, h};
  SDL_RenderCopy(renderer, texture, &source, &dest);
}

void draw(SDL_Renderer* renderer, SDL_Texture* texture, Stats* stats) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  unsigned int sec = roundf(stats->time);
  char buffer[20] = "";

  sprintf(buffer, "%4.1f", stats->velo);
  draw_text(renderer, texture, 0, 0, buffer);

  sprintf(buffer, "%5.2f", stats->dist);
  draw_text(renderer, texture, 0, 64, buffer);

  sprintf(buffer, "%u:%02u:%02u", sec / 3600, (sec / 60) % 60, sec % 60);
  draw_text(renderer, texture, 0, 128, buffer);

  draw_rect(renderer, texture, 132, 0, 384, 0, 64, 64);
  draw_rect(renderer, texture, 160, 64, 448, 0, 32, 64);

  SDL_RenderPresent(renderer);
}

void handle_rev(int rpm, Stats* stats) {
  unsigned long current = SDL_GetTicks();
  long elapsed = current - stats->last_rev;

  if (elapsed < TIMEOUT) {
    stats->dist += DIST_PER_REV;
    stats->velo = rpm * 60 * DIST_PER_REV;

    fprintf(stderr, "Revolution!  v:%.2f\n", stats->velo);

    if (stats->velo > 0.0f) {
      float next = DIST_PER_REV / stats->velo;
      fprintf(stderr, "Next rev in %.3f seconds\n", next * 3600);
      stats->next_rev = current + lroundf(next * 3600000);
    } else {
      stats->next_rev = 0;
    }

  } else {
    stats->velo = 0;
    fprintf(stderr, "Timeout!\n");
  }

  stats->last_rev = current;
}

int main() {
  SDL_Window* window = SDL_CreateWindow("Bike Stats", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

  SDL_Surface* surface = SDL_LoadBMP("sprites.bmp");
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_Event event;
  int running = 1;

  int serial = open(SERIAL, O_RDONLY | O_NONBLOCK);
  if (serial == -1) {
    fprintf(stderr, "Error opening serial device %s: %u\n", SERIAL, errno);
    return -1;
  }

  long last_tick = 0;

  Stats stats = { 0.0f, 0.0f, 0.0f, 0, 0 };
  char buffer[256];

  while (running == 1) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) running = 0;
    }

    ssize_t bytes = read(serial, buffer, 256);

    if (bytes > 0) {
      int rpm = atoi(buffer);
      fprintf(stderr, "Got RPM %u\n", rpm);
      handle_rev(rpm, &stats);
    } else if (bytes == 0) {
      // ignore this and continue
      // fprintf(stderr, "EOF on serial device\n");
      // break;
    } else if (errno != EAGAIN) {
      fprintf(stderr, "Error reading from serial device: %u\n", errno);
      break;
    }

    unsigned long current = SDL_GetTicks();
    unsigned long elapsed = current - last_tick;

    if (stats.velo > 0.0f) stats.time += elapsed / 1000.0f;

    last_tick = current;

    if (current > stats.next_rev) {
      stats.velo -= VELO_FALLOFF;
      if (stats.velo < 0.0f) stats.velo = 0.0f;
    }

    draw(renderer, texture, &stats);
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  return 0;
}
