#include "proxima.h"
#include <chrono>
#include <iostream>

using namespace std::chrono;

inline void show_fps() {
    static time_point<high_resolution_clock> last_time;
    auto new_time = high_resolution_clock::now();
    auto dur = duration_cast<milliseconds>(new_time - last_time);
    last_time = new_time;
    std::cout << "\rFPS: " << 1000 / dur.count() << std::flush;
}

