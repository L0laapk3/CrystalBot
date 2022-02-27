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
	
public:
	ActionSequence::const_iterator currentStep;
	int actionTicks;

	void reset(ActionSequence::const_iterator begin);
	bool step(ActionSequence::const_iterator end, int ticks);
	bool finished(ActionSequence::const_iterator end);
	const RLBotBM::ControllerInput& getInput();
};