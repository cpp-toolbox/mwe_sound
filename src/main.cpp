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

double total_runtime = 0.0;
double looping_sound_timer = 0.0;
bool is_looping_active = false;
unsigned int looping_sound_source_id = 0;

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

void play_sounds(double delta_t, SoundSystem &sound_system) {
    total_runtime += delta_t;
    looping_sound_timer += delta_t;

    // Regular sounds
    for (auto &sound_time : time_since_last_play) {
        SoundType type = sound_time.first;
        double &elapsed_time = sound_time.second;
        double frequency = sound_frequencies[type];

        elapsed_time += delta_t;

        update_position(type, sound_positions[type], total_runtime);

        if (elapsed_time >= frequency) {
            elapsed_time = 0.0;
            sound_system.queue_sound(type, sound_positions[type]);
        }
    }

    // Looping sound logic (5 seconds on, 1 second off)
    if (is_looping_active) {
        if (looping_sound_timer >= 5.0) {
            sound_system.stop_looping_sound(looping_sound_source_id);
            std::cout << "tried to stop it, sound source id is:" << looping_sound_source_id << std::endl;
            looping_sound_source_id = 0;
            is_looping_active = false;
            looping_sound_timer = 0.0;
        }
    } else {
        if (looping_sound_timer >= 1.0) {
            looping_sound_source_id = sound_system.queue_looping_sound(SoundType::SOUND_4, glm::vec3(0));
            std::cout << "starting it, sound source id is: " << looping_sound_source_id << std::endl;
            is_looping_active = true;
            looping_sound_timer = 0.0;
        }
    }

    sound_system.play_all_sounds();
}

int main() {
    std::unordered_map<SoundType, std::string> sound_type_to_file = {
        {SoundType::SOUND_1, "assets/sounds/BD2575.WAV"},
        {SoundType::SOUND_2, "assets/sounds/CH.WAV"},
        {SoundType::SOUND_3, "assets/sounds/MC50.WAV"},
        {SoundType::SOUND_4, "assets/sounds/wilhelm.wav"},
    };

    SoundSystem sound_system(100, sound_type_to_file);
    FixedFrequencyLoop ffl;

    auto play_sound_closure = [&](double dt) { play_sounds(dt, sound_system); };
    auto term = []() { return false; };
    ffl.start(60, play_sound_closure, term);

    return 0;
}
