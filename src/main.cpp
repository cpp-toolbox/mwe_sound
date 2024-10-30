#include "sound_system/sound_system.hpp"
#include "fixed_frequency_loop/fixed_frequency_loop.hpp"
#include <iostream>

std::map<SoundType, double> sound_frequencies = {
    {SoundType::SOUND_1, 0.5}, {SoundType::SOUND_2, 0.25}, {SoundType::SOUND_3, 2.0}};

std::map<SoundType, double> time_since_last_play = {
    {SoundType::SOUND_1, 0.0}, {SoundType::SOUND_2, 0.0}, {SoundType::SOUND_3, 0.0}};

std::map<SoundType, glm::vec3> sound_positions = {{SoundType::SOUND_1, glm::vec3(0.0f, 0.0f, 0.0f)},
                                                  {SoundType::SOUND_2, glm::vec3(0.0f, 0.0f, 0.0f)},
                                                  {SoundType::SOUND_3, glm::vec3(0.0f, 0.0f, 0.0f)}};

void update_position(SoundType type, glm::vec3 &position, double elapsed_time) {
    double speed = 0.5;
    double scale = 5;
    switch (type) {
    case SoundType::SOUND_1:
        position.x = std::sin(elapsed_time * speed) * scale; // Move left-right
        break;
    case SoundType::SOUND_2:
        position.y = std::sin(elapsed_time * speed) * scale; // Move up-down
        break;
    case SoundType::SOUND_3:
        position.z = std::sin(elapsed_time * speed) * scale; // Move forward-back
        break;
    }
}

double total_runtime = 0;

void play_sounds(double delta_t, SoundSystem &sound_system) {

    for (auto &sound_time : time_since_last_play) {
        SoundType type = sound_time.first;
        double &elapsed_time = sound_time.second;
        double frequency = sound_frequencies[type];

        elapsed_time += delta_t;
        total_runtime += delta_t;

        update_position(type, sound_positions[type], total_runtime);

        if (elapsed_time >= frequency) {
            elapsed_time = 0.0;
            sound_system.queue_sound(type, sound_positions[type]);
        }
    }

    sound_system.play_all_sounds();
}

int main() {
    std::unordered_map<SoundType, std::string> sound_type_to_file = {
        {SoundType::SOUND_1, "assets/sounds/BD2575.WAV"},
        {SoundType::SOUND_2, "assets/sounds/CH.WAV"},
        {SoundType::SOUND_3, "assets/sounds/MC50.WAV"},
    };

    SoundSystem sound_system(100, sound_type_to_file);
    FixedFrequencyLoop ffl;

    auto play_sound_closure = [&](double dt) { play_sounds(dt, sound_system); };
    auto term = []() { return false; };
    ffl.start(60, play_sound_closure, term);

    return 0;
}
