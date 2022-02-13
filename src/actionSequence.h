#pragma once

#include "RLBotBM.h"

#include <vector>



struct Action {
	int duration;
	RLBotBM::ControllerInput input;
};

typedef std::vector<Action> ActionSequence;

int getLength(const ActionSequence& seq);

class ActionSequenceExecutor {
	int currentStep;
	int actionTicks;
public:
	void reset();
	bool step(const ActionSequence& seq, int ticks);
	RLBotBM::ControllerInput getInput(const ActionSequence& seq);
};