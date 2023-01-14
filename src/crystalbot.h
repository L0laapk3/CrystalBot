#pragma once

#include "simulation/game.h"
#include "rlbot/bot.h"
#include "utils/ringBuffer.hpp"

#include "Orbuculum.h"

class CrystalBot : public rlbot::Bot {
    Game game;
public:
    CrystalBot(int index, int team, std::string name);

	Orbuculum orb;

	RingBuffer<RLBotBM::ControllerInput, 20> lastControls;

    RLBotBM::ControllerInput tick(RLBotBM::GameState& state);
    RLBotBM::ControllerInput GetOutput(RLBotBM::GameState& state) override;
};
