#include "orbuculum.h"

#include <string>
#include <iostream>
#include <linear_algebra/vec.h>
#include <simulation/game.h>
#include <mechanics/drive.h>
#include <simulation/field.h>
#include <utils/bridge.h>
#include <rlutilities.h>
#include <utils/rlurenderer.h>

#include "rlbot/bot.h"
#include "rlbot/color.h"
#include "rlbot/interface.h"
#include "rlbot/statesetting.h"

#include "actionSequence.h"
#include "simulate.h"



Orbuculum::Orbuculum(int _index, int _team, std::string _name) : Bot(_index, _team, _name) {
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

ActionSequence seq;
ActionSequenceExecutor seqEx;
ActionSequenceExecutor simSeqEx;
RLBotBM::ControllerInput Orbuculum::GetOutput(RLBotBM::GameState& state) {
    readState(game, state);
	int dt = state.tick - lastTick;
	lastTick = state.tick;

	if (reset > 250) {
		reset = 0;
		seq.clear();
		seq.push_back({ 53, { .throttle = 1, .boost = 1 } });
		seq.push_back({ 40, { .throttle = 1, .steer = -1, .boost = 1 } });
		seq.push_back({ 10, { .throttle = 1, .boost = 1 } });
		seq.push_back({ 30, { .throttle = 1, .jump = 1, .boost = 1 } });

		seqEx.reset();

		stateSet = true;
	}

	if (stateSet && game.cars[index].velocity[0] > 800.f) {
		if (game.ball.position[0] != 1000)
			return { .throttle = 1.f };

		std::cout << "set: " << game.cars[index].position[0] << ' ' << game.cars[index].position[1] << "   " << game.ball.position[0] << ' ' << game.ball.position[1] << std::endl;

		simulate(simSeqEx, game.cars[index], game.ball, seq, getLength(seq) - 1, [&](Car& car, Ball& ball, int tick) {
			return std::abs(ball.velocity[0]) >= .1f;
		}, [&](Car& car, Ball& ball, int tick, bool timeout) {
			std::cout << "sim: " << car.position[0] << ' ' << car.position[1] << "   " << ball.position[0] << ' ' << ball.position[1] << ' ' << tick << std::endl;
		});
		seqStartTick = state.tick;
		stateSet = false;
	}
		
	if (!stateSet && seqEx.step(seq, dt)) {
		if (reset == 0)
			std::cout << "exe: " << game.cars[index].position[0] << ' ' << game.cars[index].position[1] << "   " << game.ball.position[0] << ' ' << game.ball.position[1] << ' ' << (state.tick - seqStartTick) << std::endl;

		reset += dt;
		if (reset >= 250 && reset - dt < 250) {
			rlbot::GameState gameState = rlbot::GameState();
			gameState.ballState.physicsState.location = { 1000, 0, 100 };
			gameState.ballState.physicsState.velocity = { 0, 0, 0 };
			gameState.ballState.physicsState.angularVelocity = { 0, 0, 0 };

			gameState.carStates[index] = rlbot::CarState();
			gameState.carStates[index]->physicsState.location = { 0, 2500, 25.53 };
			gameState.carStates[index]->physicsState.velocity = { 0, 0, 0 };
			gameState.carStates[index]->physicsState.angularVelocity = { 0, 0, 0 };
			gameState.carStates[index]->physicsState.rotation = { 0, -1, 0 };
			gameState.carStates[index]->boostAmount = 100;
			rlbot::Interface::SetGameState(gameState);
		}

		return { .throttle = 1.f };
	}



    return seqEx.getInput(seq);
}
