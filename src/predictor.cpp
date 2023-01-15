#include "predictor.h"

#include "RLBotBM.h"

#include <iostream>
#include <thread>


void Predictor::main() {
	std::cout << "thread hello!" << std::endl;
	Orbuculum orb;
	std::cout << "thread initialized!" << std::endl;

	RLBotBM::GameState state;
	int ticks = 0;
	int lastsecondTicks = 0;
	auto lastsecond = std::chrono::high_resolution_clock::now();

	orb.setBotInput({ .throttle = 1 }, 0);
	while (true) {
		orb.waitNextTick(state);
		std::cout << "t";
		ticks++;
		if (std::chrono::high_resolution_clock::now() - lastsecond > std::chrono::seconds(1)) {
			std::cout << "ticks this second: " << ticks - lastsecondTicks << std::endl;
			lastsecondTicks = ticks;
			lastsecond = std::chrono::high_resolution_clock::now();
		}
	}
}