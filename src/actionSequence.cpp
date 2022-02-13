#include "actionSequence.h"





int getLength(const ActionSequence& seq) {
	int length = 0;
	for (const auto& action : seq) {
		length += action.duration;
	}
	return length;
}


void ActionSequenceExecutor::reset() {
	currentStep = 0;
	actionTicks = 0;
}

bool ActionSequenceExecutor::step(const ActionSequence& seq, int ticks) {
	while (true) {
		if (currentStep >= seq.size())
			return true;
			
		actionTicks += ticks;
		if (actionTicks < seq[currentStep].duration)
			return false;
		
		actionTicks -= seq[currentStep].duration;
		currentStep++;
	}
}

RLBotBM::ControllerInput ActionSequenceExecutor::getInput(const ActionSequence& seq) {
	return seq[currentStep].input;
}