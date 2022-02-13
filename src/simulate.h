#pragma once


#include "RLBotBM.h"
#include "actionSequence.h"

#include "simulation/ball.h"
#include "simulation/car.h"

#include <functional>
#include <vector>


void simulate(ActionSequenceExecutor& seqEx, Car car, Ball ball, const ActionSequence& seq, int maxTicks, std::function<bool(Car& car, Ball& ball, int tick)> stopCond, std::function<void(Car& car, Ball& ball, int tick, bool timeout)> stopCb);