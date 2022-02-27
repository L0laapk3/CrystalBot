#include "utils/renderBall.h"
#include "utils/bridge.h"

#include <array>
#include <iostream>
#include <utility>
#include <ranges>


constexpr float M_PI = 3.14159265358979323846;

void renderCircle(RLURenderer& renderer, const vec3& center, float radius, const vec3& axis, const size_t numPoints, rlbot::Color color) {
	std::vector<vec3> points{ numPoints + 1 };
	float angle = 0;
	const vec3 orth1 = normalize(vec3{ 0, -axis[2], axis[1] });
	const vec3 orth2 = normalize(cross(axis, orth1));
	for (int i = 0; i < numPoints; i++) {
		angle += 2 * M_PI / numPoints;
		points[i] = center + radius * (cosf(angle) * orth1 + sinf(angle) * orth2);
	}
	points.back() = points.front();
	renderer.DrawPolyLine3D(color, points);
}


void renderBall(RLURenderer& renderer, const RLBotBM::Ball& ball, rlbot::Color color) {
	auto sphereCenter = vec3ToRLU(ball.position) + vec3{300, 0, 0};
	constexpr int numCircles = 5;
	constexpr float cornerSpace = .125f;

	constexpr int sideCircles = numCircles / 2;
	constexpr bool middleCircle = numCircles % 2;
	constexpr float stepPerSideCircle = (1.f - cornerSpace) / (sideCircles + 1);
	std::array<std::pair<float, float>, sideCircles + middleCircle> sinCos;
	for (int i = 0; i < sinCos.size(); i++) {
		float sin = (i + !middleCircle) * stepPerSideCircle;
		sinCos[i] = { sin, sqrtf(1 - sin * sin) };
	}

	const auto ballRotator = quatToRLU(ball.orientation);
	std::array axes { vec3{ 1, 0, 0 }, vec3{ 0, 1, 0 }, vec3{ 0, 0, 1 } };
	for (auto& axis : axes)
		axis = dot(ballRotator, axis) * ball.radius;

	std::vector<vec3> points{ 1200 }; // todo: calculate
	auto it = points.begin();


	auto arc45start = [&](vec3 center, vec3 axisFrom, vec3 axisTo, int startAt) {
		for (int k = startAt; k < sinCos.size(); k++) {
			const auto& [sin, cos] = sinCos[k];
			*it++ = center + cos * axisFrom + sin * axisTo;
		}
	};
	auto arc45end = [&](vec3 center, vec3 axisFrom, vec3 axisTo, int endAt) {
		for (int k = 0; k < endAt; k++) {
			const auto& [sin, cos] = sinCos[k];
			*it++ = center + cos * axisFrom + sin * axisTo;
		}
	};
	auto arc45 = [&](vec3 center, vec3 axisFrom, vec3 axisTo) { arc45start(center, axisFrom, axisTo, 0); };
	auto arc45r = [&](vec3 center, vec3 axisFrom, vec3 axisTo) {
		for (auto rit = sinCos.crbegin(); rit != sinCos.crend() - middleCircle; ++rit) {
			auto& [sin, cos] = *rit;
			*it++ = center + sin * axisFrom + cos * axisTo;
		}
	};
	auto arc90 = [&](vec3 center, vec3 axisFrom, vec3 axisTo) {
		arc45(center, axisFrom, axisTo);
		arc45r(center, axisFrom, axisTo);
	};
	auto arc180 = [&](vec3 center, vec3 axisFrom, vec3 axisTo) {
		arc90(center, axisFrom, axisTo);
		arc90(center, axisTo, -axisFrom);
	};
	auto arc270 = [&](vec3 center, vec3 axisFrom, vec3 axisTo) {
		arc180(center, axisFrom, axisTo);
		arc90(center, -axisFrom, -axisTo);
	}; 
	// auto arc270 = [&](vec3 center, vec3 axisFrom, vec3 axisTo) {
	// 	arc90(center, axisFrom, -axisTo);
	// 	arc90(center, -axisTo, -axisFrom);
	// 	arc90(center, -axisFrom, axisTo);
	// };
	auto arc360OffsetStart = [&](vec3 center, vec3 axisFrom, vec3 axisTo, int startAt) {
		arc45start(center, axisFrom, axisTo, startAt);
		arc45r(center, axisFrom, axisTo);
		arc270(center, axisTo, -axisFrom);
		arc45end(center, axisFrom, axisTo, startAt);
	};
	auto arc360 = [&](vec3 center, vec3 axisFrom, vec3 axisTo) {
		arc360OffsetStart(center, axisFrom, axisTo, 0);
	};
	// auto arc360OffsetEnd = [&](vec3 center, vec3 axisFrom, vec3 axisTo, int endAt) {
		
	// 	for (int k = endAt; k --> 0;) {
	// 		const auto& [sin, cos] = sinCos[k];
	// 		*it++ = center + cos * axisFrom - sin * axisTo;
	// 	}
	// 	arc270(center, axisFrom, axisTo);
	// 	arc45(center, -axisTo, axisFrom);
	// 	for (int k = sinCos.size(); k --> endAt;) {
	// 		const auto& [sin, cos] = sinCos[k];
	// 		*it++ = center + cos * axisFrom - sin * axisTo;
	// 	}
	// };


	for (int i = 0; i < sinCos.size(); i++) {
		const auto& [sin1, cos1] = sinCos[i];
		const std::array sinAxes { sin1 * axes[0], sin1 * axes[1], sin1 * axes[2] };
		const std::array cosAxes { cos1 * axes[0], cos1 * axes[1], cos1 * axes[2] };
		

		for (int dir = 1; dir >= -1; dir -= 2) {
			for (int j = 0; j < sinCos.size(); j++) {
				const auto& [sin2, cos2] = sinCos[j];
				const auto sin2s = copysignf(sin2, dir), cos2s = copysignf(cos2, dir);
				if ((dir == -1 || !middleCircle || i) && i == j) {
					const auto circleCenter = sphereCenter + sin2s * axes[1];
					const auto circleFrom = cos2s * axes[0], circleTo = cos2 * axes[2];
					arc45start(circleCenter, circleFrom, circleTo, j);
					arc45r(circleCenter, circleFrom, circleTo);
					arc90(circleCenter, circleTo, -circleFrom);
					
					arc45end(circleCenter, -circleFrom, -circleTo, j);
					if (dir == 1)
						arc360OffsetStart(sphereCenter - sinAxes[2], -cosAxes[0], cosAxes[1], j);
					arc45start(circleCenter, -circleFrom, -circleTo, j);
					arc45r(circleCenter, -circleFrom, -circleTo);
					arc90(circleCenter, -circleTo, circleFrom);

					arc45end(circleCenter, circleFrom, circleTo, j);
				}
					
				*it++ = sphereCenter + sinAxes[2] + cos2s * cosAxes[0] + sin2s * cosAxes[1];
			}
			for (int j = sinCos.size(); j --> 1;) {
				const auto& [sin2, cos2] = sinCos[j];
				const auto sin2s = copysignf(sin2, dir), cos2s = copysignf(cos2, dir);
				if (i == j) {
					auto sIt = it;
					arc360OffsetStart(sphereCenter + sin2s * axes[0],  cos2s * axes[1], cos2 * axes[2], j);


					// sin2 * axes0
					// cos2 * cos3 * axes1
					// cos2 * sin3 * axes2
					// renderer.DrawLine3D(rlbot::Color::red, sphereCenter, *sIt);
				}
				*it++ = sphereCenter + sinAxes[2] + sin2s * cosAxes[0] + cos2s * cosAxes[1];
				// if (i == j)
					// cos1 * sin2 * axes0
					// cos1 * cos2 * axes1
					// sin1 * axes2
					// renderer.DrawLine3D(rlbot::Color::green, sphereCenter, *(it - 1));
			}
			if (dir == 1 && middleCircle && !i)
				arc360(sphereCenter, axes[1], axes[2]);
			arc90(sphereCenter + sinAxes[2], dir * cosAxes[1], dir * -cosAxes[0]);
		}
		
		*it++ = sphereCenter + sinAxes[2] + cosAxes[0];

		
		// arc90(sphereCenter + sinAxes[2], cosAxes[0], cosAxes[1]);
		// arc360(sphereCenter + sinAxes[0], cosAxes[1], cosAxes[2]);
		// arc270(sphereCenter + sinAxes[2], cosAxes[1], cosAxes[0]);
		// arc360(sphereCenter + sinAxes[1], cosAxes[0], cosAxes[2]);
	}


	// std::cout << it - points.begin() << std::endl;
	points.resize(it - points.begin() + 1);
	points.back() = points.front();
	renderer.DrawPolyLine3D(color, points);

}