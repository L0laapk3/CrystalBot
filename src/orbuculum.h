#pragma once

#include "simulation/game.h"
#include "rlbot/bot.h"

class Orbuculum : public rlbot::Bot {
    Game game;
public:
    Orbuculum(int _index, int _team, std::string _name);

    RLBotBM::ControllerInput GetOutput(RLBotBM::GameState& state) override;
};
