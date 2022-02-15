#pragma once

#include "simulation/game.h"
#include "rlbot/bot.h"

class CrystalBot : public rlbot::Bot {
    Game game;
public:
    CrystalBot(int _index, int _team, std::string _name);

    RLBotBM::ControllerInput GetOutput(RLBotBM::GameState& state) override;
};
