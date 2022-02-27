#pragma once

#include <simulation/game.h>
#include <rlbot/packets.h>
#include "RLBotBM.h"

vec3 vec3ToRLU(const RLBotBM::Vec3& v);

mat3 quatToRLU(const RLBotBM::Quat& q);

RLBotBM::Quat quatFromRPY(std::array<float, 3> rpy);

Input inputToRLU(const RLBotBM::ControllerInput& i);

vec3 flatVectorToVec3(const rlbot::flat::Vector3 *v);

mat3 flatRotatorToMat3(const rlbot::flat::Rotator *r);

void readFieldInfo(Game &game, const rlbot::FieldInfo &fieldInfo);

void readState(Game &game, const RLBotBM::GameState& state);

rlbot::Controller inputToController(const Input & input);