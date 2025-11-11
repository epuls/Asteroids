#pragma once
#include <SDL3/SDL.h>
#include <cstdio>


struct FrameTimer {
    Uint64 last = 0;
    double inv_freq = 1.0;
    double acc_time = 0.0;
    int    acc_frames = 0;

    void init() {
        last = SDL_GetPerformanceCounter();
        inv_freq = 1.0 / double(SDL_GetPerformanceFrequency());
    }

    // returns dt (seconds)
    double tick() {
        Uint64 now = SDL_GetPerformanceCounter();
        double dt = (now - last) * inv_freq;
        last = now;
        acc_time   += dt;
        acc_frames += 1;
        return dt;
    }

    // returns true when it emits a new sample
    bool sample_and_set_title(SDL_Window* w) {
        if (acc_time >= 1.0) {
            double fps = acc_frames / acc_time;
            double ms  = (acc_time / acc_frames) * 1000.0;
            char title[128];
            std::snprintf(title, sizeof(title), "Asteroids  |  %.1f fps  (%.2f ms)", fps, ms);
            SDL_SetWindowTitle(w, title);
            acc_time = 0.0;
            acc_frames = 0;
            return true;
        }
        return false;
    }
};