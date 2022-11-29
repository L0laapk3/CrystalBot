#include "crystalbot.h"

#include <string>
#include <iostream>
#include <linear_algebra/vec.h>
#include <simulation/game.h>
#include <mechanics/drive.h>
#include <simulation/field.h>
#include <rlutilities.h>
#include <utils/rlurenderer.h>

#include "rlbot/bot.h"
#include "rlbot/color.h"
#include "rlbot/interface.h"
#include "rlbot/statesetting.h"
#include "rlbot/bminterface.h"

#include "utils/bridge.h"
#include "actionSequence.h"
#include "simulate.h"

#include "utils/renderBall.h"



CrystalBot::CrystalBot(int _index, int _team, std::string _name) : Bot(_index, _team, _name) {
    // Initialize RLU with the path to the assets folder, relative to the executable.
    rlu::initialize("assets/");

    // Tell RLU which field mesh and ball parameters to use
    Game::set_mode("soccar");

    // Read field info. Only needs to be called once
    readFieldInfo(game, GetFieldInfo());
}


int reset = 0;
int lastTick = 0;
int seqStartTick = 0;
bool stateSet = false;

constexpr int EXTRA_TICKS = 1;

std::vector<float> different_values{};
int stepcnt = 0;

bool printed = false;

ActionSequence seq;
ActionSequenceExecutor seqEx;
ActionSequenceExecutor simSeqEx;
bool seqInitialized = false;
RLBotBM::ControllerInput CrystalBot::tick(RLBotBM::GameState& state) {
    if (!seqInitialized) {
    	seqEx.reset(seq.begin());
		seqInitialized = true;
	}

	auto& stateSetObj = rlbot::bmInterface->getStateSetObj();
	int dt = state.tick - lastTick;
	lastTick = state.tick;

	if (reset > EXTRA_TICKS) {
		reset = 0;
		stepcnt = (stepcnt + 1) % 3;
		float steer = (float)rand() / RAND_MAX * 2 - 1;

		int jumpTime = 100 * rand() / RAND_MAX;
		std::cout << "sequence jumpTime: " << jumpTime << " steer: " << steer << std::endl;

		seq.clear();
		seq.push_back({ 1, { }}); // statesetting mo
		seq.push_back({ 250, { .throttle = 1, .steer = -.1, .boost = 1 } });
		seq.push_back({ 1,  { .throttle = 1, .steer = steer }});
		seq.push_back({ 230,  { .throttle = 1, .yaw = steer, .pitch = steer, .roll = steer, .jump = 1 }});	
		seq.push_back({ jumpTime, { .throttle = 1, .jump = 1, .boost = 1 } });
		seq.push_back({ 1, { .throttle = 1, .steer = steer  }});	

		seqEx.reset(seq.begin());

		stateSet = true;
	}

	if (stateSet) {
		// if (game.ball.position[0] != 1000)
		// 	return { .throttle = 1.f };

		// std::cout << "set: " << game.cars[index].position[0] << ' ' << game.cars[index].position[1] << "   " << game.ball.position[0] << ' ' << game.ball.position[1] << std::endl;

		// simulate(simSeqEx, game.cars[index], game.ball, seq, getLength(seq) - 1, [&](Car& car, Ball& ball, int tick) {
		// 	return std::abs(ball.velocity[0]) >= .1f;
		// }, [&](Car& car, Ball& ball, int tick, bool timeout) {
		// 	std::cout << "sim: " << car.position[0] << ' ' << car.position[1] << "   " << ball.position[0] << ' ' << ball.position[1] << ' ' << tick << std::endl;
		// });
		seqStartTick = state.tick;
		stateSet = false;
	}

	if (!stateSet && seqEx.step(seq.end(), dt)) {

		reset += dt;
		if (reset >= EXTRA_TICKS && reset - dt < EXTRA_TICKS) {
			auto quat = quatFromRPY({ 0, 0, -1 });
			
			// stateSetObj.balls[0].position = { 1000, 0, 100 };
			// stateSetObj.balls[0].velocity = { 0, 0, 0 };
			// stateSetObj.balls[0].angularVelocity = { 100, 0, 0 };
			// stateSetObj.balls[0].orientation = reinterpret_cast<RLBotBM::StateSetQuat&>(quat);

			stateSetObj.cars[index].position = { 0, 2500, 25.53 };
			stateSetObj.cars[index].velocity = { 0, 0, 0 };
			stateSetObj.cars[index].angularVelocity = { 0, 0, 0 };
			stateSetObj.cars[index].orientation = reinterpret_cast<RLBotBM::StateSetQuat&>(quat);
			// for (auto& wheel : stateSetObj.cars[index].wheels)
			// 	wheel.spinSpeed = -80;
			stateSetObj.cars[index].boost = 100;

			stateSetObj.setAny = true;
			std::cout << "do stateset" << std::endl;
			printed = false;
		}

		return { .throttle = 1.f };
	}

	if (seqEx.currentStep == seq.end() - 1) {
		if (std::find(different_values.begin(), different_values.end(), game.ball.position[1]) == different_values.end())
			different_values.push_back(game.ball.position[1]);
		std::cout << std::setprecision(20) << "exe: " << game.cars[index].position[0] << ' ' << game.cars[index].position[1] << "   " << game.ball.position[0] << ' ' << game.ball.position[1] << ' ' << (state.tick - seqStartTick) << " #diff: " << different_values.size() << std::endl;
		printed = true;
	}

	if (seqEx.currentStep == seq.end() - 1) {
			stateSetObj.balls[0].position = { 1000, 0, 100 };
			stateSetObj.balls[0].velocity = { 0, 0, 0 };
			stateSetObj.balls[0].angularVelocity = { 100, 0, 0 };
			auto quat = quatFromRPY({ 0, 0, -1 });
			stateSetObj.balls[0].orientation = reinterpret_cast<RLBotBM::StateSetQuat&>(quat);

			stateSetObj.cars[index].position = { 0, 2500, 25.53 };
			stateSetObj.cars[index].velocity = { 0, 0, 0 };
			stateSetObj.cars[index].angularVelocity = { 0, 0, 0 };
			stateSetObj.cars[index].orientation = reinterpret_cast<RLBotBM::StateSetQuat&>(quat);

			stateSetObj.setAny = true;

		printed = false;
	}


    return seqEx.getInput();
}


RLBotBM::ControllerInput CrystalBot::GetOutput(RLBotBM::GameState& state) {
    readState(game, state);

	RLURenderer renderer(std::to_string(index)); 
	renderBall(renderer, state.balls[0], rlbot::Color::cyan);

	auto controls = tick(state);

	// if (lastControls.full())
	// 	lastControls.pop_front();
	// lastControls.push_back(controls);

	return controls;
}
