#include <iostream>
#include <string>

#include <fcntl.h>
#include <math.h>
#include <unistd.h>

#include <boost/format.hpp>
#include <SDL2/sdl.h>

static const int         WIDTH     = 32 * 6;
static const int         HEIGHT    = 64 * 3;
static const int         WHEEL_DIA = 16;
static const float       PI        = 3.14159265358979f;
static const int         TIMEOUT   = 3000;
static const std::string SERIAL    = "/dev/cu.usbmodem12341";

struct Stats {
  float mph, dist, time;
  unsigned long last_rev;
};

void draw_text(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, std::string text) {
  SDL_Rect source = { 0, 0, 0, 64 };
  SDL_Rect dest   = { x, y, 0, 64 };

  for (std::string::iterator i = text.begin(); i != text.end(); ++i) {
    if ((*i) >= '0' && (*i) <= '9') {
      source.x = ((*i) - '/') * 32;
      source.w = dest.w = 32;
    } else if ((*i) == ' ') {
      source.x = 0;
      source.w = dest.w = 32;
    } else if ((*i) == ':') {
      source.x = 352;
      source.w = dest.w = 16;
    } else if ((*i) == '.') {
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

  long sec = lroundf(stats->time);

  draw_text(renderer, texture, 0, 0, boost::str(boost::format("%4.1f") % stats->mph));
  draw_text(renderer, texture, 0, 64, boost::str(boost::format("%5.2f") % stats->dist));
  draw_text(renderer, texture, 0, 128, boost::str(boost::format("%u:%02u:%02u") % (sec / 3600) % ((sec / 60) % 60) % (sec % 60)));

  draw_rect(renderer, texture, 132, 0, 384, 0, 64, 64);
  draw_rect(renderer, texture, 160, 64, 448, 0, 32, 64);

  SDL_RenderPresent(renderer);
}

void handle_rev(int rpm, Stats* stats) {
  unsigned long current = SDL_GetTicks();
  long elapsed = current - stats->last_rev;

  if (elapsed < TIMEOUT) {
    stats->mph   = rpm * WHEEL_DIA * PI * 60 / 5280 / 12;
    stats->dist += WHEEL_DIA * PI / 5280 / 12;
  } else {
    stats->mph = 0;
  }

  stats->last_rev = current;
}

int main() {
  SDL_Window* window = SDL_CreateWindow("Bike Stats", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

  SDL_Surface* surface = SDL_LoadBMP("sprites.bmp");
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_Event event;
  bool running = true;

  int serial = open(SERIAL.c_str(), O_RDONLY | O_NONBLOCK);
  if (serial == -1) {
    std::cerr << "Error opening serial device";
    return -1;
  }

  long last_tick = 0;

  Stats stats = { 0.0f, 0.0f, 0, 0 };
  char buffer[256];

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) running = false;
    }

    ssize_t bytes = read(serial, buffer, 256);

    if (bytes > 0) {
      int rpm = atoi(buffer);
      fprintf(stderr, "Got rpm %u\n", rpm);
      handle_rev(rpm, &stats);
    } else if (bytes == 0) {
      std::cerr << "EOF on serial device" << std::endl;
      break;
    } else if (errno != EAGAIN) {
      std::cerr << "Error reading from serial device: " << errno << std::endl;
      break;
    }

    long current = SDL_GetTicks();
    long elapsed_tick = current - last_tick;
    long elapsed_rev = current - stats.last_rev;

    if (elapsed_rev > TIMEOUT) stats.mph = 0.0f;
    if (stats.mph > 0) stats.time += elapsed_tick / 1000.0f;

    last_tick = current;

    draw(renderer, texture, &stats);
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  return 0;
}
