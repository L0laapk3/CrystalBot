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
#include "rlbot/bmInterface.h"

#include "utils/bridge.h"
#include "actionSequence.h"
#include "simulate.h"

#include "utils/renderBall.h"

#include "predictor.h"



CrystalBot::CrystalBot(int _index, int _team, std::string _name) :
	Bot(_index, _team, _name),
	predictorThread([](){
		Predictor predictor;
		predictor.main();
	}) {
    // Initialize RLU with the path to the assets folder, relative to the executable.
    rlu::initialize("assets/");

    // Tell RLU which field mesh and ball parameters to use
    Game::set_mode("soccar");

    // Read field info. Only needs to be called once
    readFieldInfo(game, GetFieldInfo());


}


int reset = 0;
int lastTick = 0;



std::vector<float> different_values{};

bool hasCapturedTick2 = false;

bool statevar = true;

// int index = 0;

ActionSequence seq;
ActionSequenceExecutor seqEx;
ActionSequenceExecutor simSeqEx;
bool seqInitialized = false;

float lastThrottle = 0;
RLBotBM::ControllerInput CrystalBot::tick(RLBotBM::GameState& state) {

	using namespace std::chrono_literals;
	// std::this_thread::sleep_for(50ms);
		
	return RLBotBM::ControllerInput{ .throttle = 1 };

	// auto& stateSetObj = rlbot::bmInterface->getStateSetObj();
	// int dt = state.tick - lastTick;
	// lastTick = state.tick;

	// if (dt != 1)
	// 	std::cout << "skipped ticks in rlbot-bm kekw? dt=" << dt << " tick=" << state.tick << std::endl;

	// if (seqInitialized)
	// 	seqInitialized = !seqEx.step(seq.end(), dt);
	
    // if (!seqInitialized) {
	// 	float steer = (float)rand() / RAND_MAX * 2 - 1;
	// 	int time = 300 * rand() / RAND_MAX;
	// 	std::cout << "sequence time: " << time << " steer: " << steer << std::endl;

	// 	seq.clear();
	// 	seq.push_back({ 2,    { }}); // defines number of statesetting ticks
	// 	seq.push_back({ 250,  { .throttle = 1, .steer = -.1, .boost = 1 }});
	// 	seq.push_back({ 1,    { .throttle = 1, .jump = 1 }});
	// 	seq.push_back({ 10,   { .throttle = 1, .pitch = steer, .yaw = steer, .roll = steer, .jump = 1 }});	
	// 	seq.push_back({ time, { .throttle = 1, .pitch = steer, .yaw = steer, .roll = steer, .boost = 1 }});
	// 	seq.push_back({ 1,    { .throttle = 1, .steer = steer  }});	

	// 	seqEx.reset(seq.begin());
	// 	seqInitialized = true;

	// 	// index = rand() > RAND_MAX / 2 ? 0 : 1;
	// }

	// if (seqEx.currentStep == seq.begin()) {
	// 	auto quat = quatFromRPY({ 0, 0, -1 });
	// 	if (statevar) {
	// 		stateSetObj.balls[0].position = { 1000, 0, 100 };
	// 		stateSetObj.balls[0].velocity = { 0, 0, 0 };
	// 		stateSetObj.balls[0].angularVelocity = { 100, 0, 0 };
	// 		stateSetObj.balls[0].orientation = reinterpret_cast<RLBotBM::StateSetQuat&>(quat);
	// 		stateSetObj.cars[index].boost = 100;

	// 		std::cout << "setting car & ball" << std::endl;
			
	// 		stateSetObj.cars[index].position = { 0, 2500, 25.53 };
	// 		stateSetObj.cars[index].velocity = { 0, 0, 0 };
	// 		stateSetObj.cars[index].angularVelocity = { 0, 0, 0 };
	// 		stateSetObj.cars[index].orientation = reinterpret_cast<RLBotBM::StateSetQuat&>(quat);
	// 	} else {
	// 		if (!hasCapturedTick2) {
				
	// 		}
	// 		stateSetObj.cars[index].position = { 0, 2500, 25.53 };
	// 		stateSetObj.cars[index].velocity = { 0, 0, 0 };
	// 		stateSetObj.cars[index].angularVelocity = { 0, 0, 0 };
	// 		stateSetObj.cars[index].orientation = reinterpret_cast<RLBotBM::StateSetQuat&>(quat);

	// 		std::cout << "setting car" << index << std::endl;
	// 	}

	// 	stateSetObj.setAny = true;

	// 	statevar = false;
	// }

	// if (seqEx.currentStep == seq.begin() + 3 && !statevar) {
	// 	if (std::find(different_values.begin(), different_values.end(), game.ball.position[1]) == different_values.end())
	// 		different_values.push_back(game.ball.position[1]);
	// 	std::cout << std::setprecision(20) << "ball end position: " << game.ball.position << " # unique outcomes: " << different_values.size() << std::endl;
	// 	statevar = true;
	// }

    // return seqEx.getInput();
}


RLBotBM::ControllerInput CrystalBot::GetOutput(RLBotBM::GameState& state) {
    readState(game, state);

	RLURenderer renderer(std::to_string(index)); 
	renderBall(renderer, state.balls[0], rlbot::Color::cyan);

	return tick(state);
}
