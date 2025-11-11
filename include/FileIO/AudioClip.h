#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>

#include <vector>
#include <string>
#include <iostream>
#include <random>

class AudioClip {
public:
    explicit AudioClip(const char* path, unsigned int maxStreams = 8)
            : MAX_STREAMS(maxStreams)
    {
        if (!Load(path)) {
            std::cerr << "Failed to load AudioClip: " << path << "\n";
            return;
        }
        InitStreams();
    }

    void Cleanup(){
        if (audio_buf) SDL_free(audio_buf);

        try {
            for (auto *s: streams) {
                if (s) SDL_DestroyAudioStream(s);
            }
        } catch (...){
            std::cout << "Failed to clean up audio streams!\n";
        }
    }

    ~AudioClip() = default;

    // Play once; re-queues the sample each time. Ratio affects pitch+speed.
    bool Play(float freqRatio = 1.0f, float gain = 1.0f) {
        if (streams.empty()) return false;

        SDL_AudioStream* s = streams[m_curStreamIdx];
        if (!s) return false;


        // Clear any leftover data to ensure a clean start
        SDL_FlushAudioStream(s);

        // Set ratio before pushing so resampling applies to the whole clip
        if (!SDL_SetAudioStreamFrequencyRatio(s, freqRatio)) {
            return false;
        }

        // Re-queue audio data for this playback
        if (!SDL_PutAudioStreamData(s, audio_buf, audio_len)) {
            return false;
        }

        SDL_SetAudioStreamGain(s, gain);
        // Make sure device is running; OpenAudioDeviceStream creates a paused device
        SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(s));

        // Round-robin next stream to allow overlapping SFX
        m_curStreamIdx = (m_curStreamIdx + 1) % streams.size();
        return true;
    }

    // Plays with a small random pitch variation for natural variety
    bool PlayWithJitter(float center = 1.0f, float range = 0.03f, float gain = 1.0f) {
        static thread_local std::mt19937 rng{ std::random_device{}() };
        std::uniform_real_distribution<float> dist(center - range, center + range);
        return Play(dist(rng), gain);
    }

private:
    const unsigned int MAX_STREAMS;
    unsigned int m_curStreamIdx = 0;

    SDL_AudioSpec spec{};
    Uint8*  audio_buf = nullptr;
    Uint32  audio_len = 0;
    std::vector<SDL_AudioStream*> streams;

    bool Load(const char* path) {
        if (!SDL_LoadWAV(path, &spec, &audio_buf, &audio_len)) {
            return false;
        }
        return true;
    }

    void InitStreams() {
        streams.reserve(MAX_STREAMS);
        for (unsigned i = 0; i < MAX_STREAMS; ++i) {
            SDL_AudioStream* s = SDL_OpenAudioDeviceStream(
                    SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                    &spec,
                    nullptr, nullptr
            );
            if (!s) {
                std::cerr << "Failed to open audio stream " << i << ": " << SDL_GetError() << "\n";
                continue;
            }

            // Start with device resumed so queued data plays immediately when put
            SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(s));
            streams.push_back(s);
        }
    }
};
