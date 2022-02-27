#pragma once

#include "simulation/game.h"
#include "rlbot/bot.h"
#include "utils/ringBuffer.hpp"

class CrystalBot : public rlbot::Bot {
    Game game;
public:
    CrystalBot(int _index, int _team, std::string _name);

	RingBuffer<RLBotBM::ControllerInput, 20> lastControls;

    RLBotBM::ControllerInput tick(RLBotBM::GameState& state);
    RLBotBM::ControllerInput GetOutput(RLBotBM::GameState& state) override;
};
