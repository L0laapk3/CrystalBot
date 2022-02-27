#include "actionSequence.h"





int getLength(const ActionSequence& seq) {
	int length = 0;
	for (const auto& action : seq) {
		length += action.duration;
	}
	return length;
}


void ActionSequenceExecutor::reset(ActionSequence::const_iterator begin) {
	currentStep = begin;
	actionTicks = 0;
}

bool ActionSequenceExecutor::step(ActionSequence::const_iterator end, int ticks) {
	actionTicks += ticks;
	
	while (true) {
		if (finished(end))
			return true;

		if (actionTicks < currentStep->duration)
			return false;
		
		actionTicks -= currentStep->duration;
		currentStep++;
	}
}

bool ActionSequenceExecutor::finished(ActionSequence::const_iterator end) {
	return currentStep >= end;
}

const RLBotBM::ControllerInput& ActionSequenceExecutor::getInput() {
	return currentStep->input;
}