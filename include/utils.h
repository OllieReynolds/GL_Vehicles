#pragma once

#include <chrono>
#include <random>

#include "maths.h"

namespace utils {
	static float elapsed_time() {
		using namespace std::chrono;
		static time_point<steady_clock> start = steady_clock::now();
		return duration_cast<duration<float>>(steady_clock::now() - start).count();
	}

	static float gen_random(float min = 0.f, float max = 10.f) {
		static std::random_device rd;
		static std::mt19937 mt(rd());
		std::uniform_real_distribution<float> dist(min, max);
		return dist(mt);
	}
}