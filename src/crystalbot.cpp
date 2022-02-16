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



CrystalBot::CrystalBot(int _index, int _team, std::string _name) : Bot(_index, _team, _name) {
    // Initialize RLU with the path to the assets folder, relative to the executable.
    rlu::initialize("assets/");

    // Tell RLU which field mesh and ball parameters to use
    Game::set_mode("soccar");

    // Read field info. Only needs to be called once
    readFieldInfo(game, GetFieldInfo());

    // State setting example
    rlbot::GameState gameState = rlbot::GameState();
    gameState.ballState.physicsState.location = {0, 0, 1000};
    gameState.ballState.physicsState.velocity = {0, 0, 100};
    rlbot::Interface::SetGameState(gameState);
}


int reset = 0;
int lastTick = 0;
int seqStartTick = 0;
bool stateSet = false;

constexpr int EXTRA_TICKS = 1;

ActionSequence seq;
ActionSequenceExecutor seqEx;
ActionSequenceExecutor simSeqEx;
RLBotBM::ControllerInput CrystalBot::GetOutput(RLBotBM::GameState& state) {
    readState(game, state);
	auto& stateSetObj = rlbot::bmInterface->getStateSetObj();
	int dt = state.tick - lastTick;
	lastTick = state.tick;

	if (reset > EXTRA_TICKS) {
		reset = 0;
		seq.clear();
		seq.push_back({ 53, { .throttle = 1, .boost = 1 } });
		seq.push_back({ 4, { .throttle = 1, .steer = -1, .boost = 1 } });
		seq.push_back({ 10, { .throttle = 1, .boost = 1 } });
		seq.push_back({ 20, { .throttle = 1, .steer = (rand() % 3) - 1.f }});
		// seq.push_back({ 30, { .throttle = 1, .jump = 1, .boost = 1 } });

		seqEx.reset();

		stateSet = true;
	}

	if (stateSet && game.cars[index].velocity[0] > 800.f) {
		if (game.ball.position[0] != 1000)
			return { .throttle = 1.f };

		// std::cout << "set: " << game.cars[index].position[0] << ' ' << game.cars[index].position[1] << "   " << game.ball.position[0] << ' ' << game.ball.position[1] << std::endl;

		// simulate(simSeqEx, game.cars[index], game.ball, seq, getLength(seq) - 1, [&](Car& car, Ball& ball, int tick) {
		// 	return std::abs(ball.velocity[0]) >= .1f;
		// }, [&](Car& car, Ball& ball, int tick, bool timeout) {
		// 	std::cout << "sim: " << car.position[0] << ' ' << car.position[1] << "   " << ball.position[0] << ' ' << ball.position[1] << ' ' << tick << std::endl;
		// });
		seqStartTick = state.tick;
		stateSet = false;
	}
		
	if (!stateSet && seqEx.step(seq, dt)) {
		if (reset == 0)
			std::cout << "exe: " << game.cars[index].position[0] << ' ' << game.cars[index].position[1] << "   " << game.ball.position[0] << ' ' << game.ball.position[1] << ' ' << (state.tick - seqStartTick) << std::endl;

		reset += dt;
		if (reset >= EXTRA_TICKS && reset - dt < EXTRA_TICKS) {
			
			stateSetObj.balls[0].position = { 1000, 0, 100 };
			stateSetObj.balls[0].velocity = { 0, 0, 0 };
			stateSetObj.balls[0].angularVelocity = { 0, 0, 0 };

			stateSetObj.cars[index].position = { 0, 2500, 25.53 };
			stateSetObj.cars[index].velocity = { 0, 0, 0 };
			stateSetObj.cars[index].angularVelocity = { 0, 0, 0 };
			auto quat = quatFromRPY({ 0, 0, -1 });
			stateSetObj.cars[index].orientation = reinterpret_cast<RLBotBM::Shared::StateSetQuat&>(quat);
			for (auto& wheel : stateSetObj.cars[index].wheels)
				wheel.spinSpeed = -80;
			stateSetObj.cars[index].boost = 100;

			stateSetObj.setAny = true;
			std::cout << "do stateset" << std::endl;
		}

		return { .throttle = 1.f };
	}



    return seqEx.getInput(seq);
}
