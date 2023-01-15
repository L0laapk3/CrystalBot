// #include "simulate.h"

// #include "utils/bridge.h"



// void simulate(ActionSequenceExecutor& seqEx, Car car, Ball ball, const ActionSequence& seq, int maxTicks, std::function<bool(Car&, Ball&, int)> stopCond, std::function<void(Car&, Ball&, int, bool)> stopCb) {
// 	seqEx.reset(seq.begin());
// 	int tick = 0;
// 	bool timeout = true;
// 	for (; tick < maxTicks; tick++) {
// 		auto input = seqEx.step(seq.end(), 1) ? RLBotBM::ControllerInput{} : seqEx.getInput();
			
// 		ball.step(1.f / 120.f, car);
// 		car.step(inputToRLU(input), 1.f / 120.f);

// 		if (stopCond(car, ball, tick)) {
// 			timeout = false;
// 			break;
// 		}
// 	}
// 	stopCb(car, ball, tick, timeout);
// }