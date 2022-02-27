#include "utils/renderBall.h"
#include "utils/bridge.h"

#include <array>
#include <iostream>
#include <utility>
#include <ranges>


constexpr float M_PI = 3.14159265358979323846;

void renderCircle(RLURenderer& renderer, const vec3& c, float radius, const vec3& axis, const size_t numPoints, rlbot::Color color) {
	std::vector<vec3> points{ numPoints + 1 };
	float angle = 0;
	const vec3 orth1 = normalize(vec3{ 0, -axis[2], axis[1] });
	const vec3 orth2 = normalize(cross(axis, orth1));
	for (int i = 0; i < numPoints; i++) {
		angle += 2 * M_PI / numPoints;
		points[i] = c + radius * (cosf(angle) * orth1 + sinf(angle) * orth2);
	}
	points.back() = points.front();
	renderer.DrawPolyLine3D(color, points);
}


void renderBall(RLURenderer& renderer, const RLBotBM::Ball& ball, rlbot::Color color) {
	auto sphereCenter = vec3ToRLU(ball.position) + vec3{300, 0, 0};
	constexpr int numCircles = 3;
	constexpr float cornerSpace = .1f;
	// constexpr int numCircles = 5;
	// constexpr float cornerSpace = .145f;

	const auto ballRotator = quatToRLU(ball.orientation);
	std::array axes { vec3{ 1, 0, 0 }, vec3{ 0, 1, 0 }, vec3{ 0, 0, 1 } };
	for (auto& axis : axes)
		axis = dot(ballRotator, axis) * ball.radius;

	constexpr int sideCircles = numCircles / 2;
	constexpr bool middleCircle = numCircles % 2;
	constexpr int numArcPoints = sideCircles + middleCircle;
	constexpr float stepPerSideCircle = (1.f - cornerSpace) / (sideCircles + 1);

	typedef std::array<vec3, numArcPoints> ArcPoints;
	struct AxisArcPoint {
		vec3 sin;
		ArcPoints cosCos;
		ArcPoints cosSin;
	};
	typedef std::array<AxisArcPoint, axes.size()> AxesM;
	std::array<AxesM, numArcPoints> allAxesM;

	for (int i = 0; i < axes.size(); i++)
		for (int j = 0; j < numArcPoints; j++) {
			float sin = (j + ((float)!middleCircle) / 2) * stepPerSideCircle;
			allAxesM[j][i].sin = sin * axes[i];
			float cos = sqrtf(1 - sin * sin);
			for (int k = 0; k < numArcPoints; k++) {
				// sin[k] has to be equal to cosSin[...][k]
				float cosSin = (k + ((float)!middleCircle) / 2) * stepPerSideCircle;
				allAxesM[j][i].cosSin[k] = cosSin * axes[i];
				float sin = cosSin / cos;
				allAxesM[j][i].cosCos[k] = sqrtf(1 - sin * sin) * cos * axes[i];
			}
		}

	std::vector<vec3> points{ numCircles * 4 * numCircles * 3 + 1 }; // todo: some extra points for even numbers
	auto it = points.begin();

	auto cInv = [&](vec3 v, bool inv) {
		return inv ? -v : v;
	};
	auto arcPoint = [&](const vec3& center, const vec3& cosComp, const vec3& sinComp, bool fc = false, bool fCos = false, bool fSin = false) {
		*it++ = sphereCenter + cInv(center, fc) + cInv(cosComp, fCos) + cInv(sinComp, fSin);
	};
	auto arc45start = [&](int c, int f, int t, const AxesM& axes, int startAt, bool fc = false, bool ff = false, bool ft = false) {
		for (int k = startAt; k < numArcPoints; k++)
			arcPoint(axes[c].sin, axes[f].cosCos[k], axes[t].cosSin[k], fc, ff, ft);
	};
	auto arc45 = [&](int c, int f, int t, const AxesM& axes, bool fc = false, bool ff = false, bool ft = false) {
		arc45start(c, f, t, axes, 0, fc, ff, ft);
	};
	auto arc45end = [&](int c, int f, int t, const AxesM& axes, int endAt, bool fc = false, bool ff = false, bool ft = false) {
		for (int k = 0; k < endAt; k++)
			arcPoint(axes[c].sin, axes[f].cosCos[k], axes[t].cosSin[k], fc, ff, ft);
	};
	auto arc45r = [&](int c, int f, int t, const AxesM& axes, bool fc = false, bool ff = false, bool ft = false) {
		for (auto k = numArcPoints; k --> middleCircle; )
			arcPoint(axes[c].sin, axes[t].cosCos[k], axes[f].cosSin[k], fc, ft, ff);
	};
	auto arc90 = [&](int c, int f, int t, const AxesM& axes, bool fc = false, bool ff = false, bool ft = false) {
		arc45(c, f, t, axes, fc, ff, ft);
		arc45r(c, f, t, axes, fc, ff, ft);
	};
	auto arc180 = [&](int c, int f, int t, const AxesM& axes, bool fc = false, bool ff = false, bool ft = false) {
		arc90(c, f, t, axes, fc, ff, ft);
		arc90(c, t, f, axes, fc, ft, !ff);
	};
	auto arc270 = [&](int c, int f, int t, const AxesM& axes, bool fc = false, bool ff = false, bool ft = false) {
		arc180(c, f, t, axes, fc, ff, ft);
		arc90(c, f, t, axes, fc, !ff, !ft);
	}; 
	auto arc360OffsetStart = [&](int c, int f, int t, const AxesM& axes, int startAt, bool fc = false, bool ff = false, bool ft = false) {
		arc45start(c, f, t, axes, startAt, fc, ff, ft);
		arc45r(c, f, t, axes, fc, ff, ft);
		arc270(c, t, f, axes, fc, ft, !ff);
		arc45end(c, f, t, axes, startAt, fc, ff, ft);
	};
	auto arc360 = [&](int c, int f, int t, const AxesM& axes, bool fc = false, bool ff = false, bool ft = false) {
		arc360OffsetStart(c, f, t, axes, 0, fc, ff, ft);
	};


	for (int i = 0; i < numArcPoints; i++) {
		const auto& axesM = allAxesM[i];
		
		auto& firstPoint = *it;
		for (int dir = 0; dir < 2; dir++) {
			for (int j = 0; j < numArcPoints; j++) {
				if ((dir == -1 || !middleCircle || i) && i == j) {
					arc45start(1, 0, 2, axesM, j, dir, dir, false);
					arc45r(1, 0, 2, axesM, dir, dir, false);
					arc90(1, 2, 0, axesM, dir, false, !dir);

					arc45end(1, 0, 2, axesM, j, dir, !dir, true);
					if (dir == 1)
						arc360OffsetStart(2, 0, 1, axesM, j, true, false, true);
					arc45start(1, 0, 2, axesM, j, dir, !dir, true);
					arc45r(1, 0, 2, axesM, dir, !dir, true);
					arc90(1, 2, 0, axesM, dir, true, dir);

					arc45end(1, 0, 2, axesM, j, dir, dir, false);
				}
					
				arcPoint(axesM[2].sin, axesM[0].cosCos[j], axesM[1].cosSin[j], false, dir, dir);
			}
			for (int j = numArcPoints; j --> middleCircle;) {
				if (i == j)
					arc360OffsetStart(0, 1, 2, axesM, j, dir, dir, false);
				arcPoint(axesM[2].sin, axesM[1].cosCos[j], axesM[0].cosSin[j], false, dir, dir);
			}
			if (middleCircle && !i && dir == 0)
				arc360(0, 1, 2, axesM, false, false, false);
			arc90(2, 1, 0, axesM, false, dir, !dir);
		}
		
		*it++ = firstPoint;

		// arc90(2, 0, 1, axesM);
		// arc360(0, 1, 2, axesM);
		// arc270(2, 1, 0, axesM, false, false, true);
		// arc360(1, 0, 2, axesM);
	}

	if (middleCircle) {
		arc45r(1, 0, 2, allAxesM[0]);
		arc270(1, 2, 0, allAxesM[0], false, false, true);
	}

	// std::cout << it - points.begin() << std::endl;;
	// points.resize(it - points.begin());
	points.back() = points.front();

	renderer.DrawPolyLine3D(color, points);

}