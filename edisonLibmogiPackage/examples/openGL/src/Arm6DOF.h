/*
 *  Arm6DOF.h
 *
 *
 *  Created by Matt Bunting on 12/28/15.
 *  Copyright 2015 Mogi LLC. All rights reserved.
 *
 */

#ifndef ARM6DOF_FOR_ARMS
#define ARM6DOF_FOR_ARMS

#include <mogi/math/kinematics.h>
#include <mogi/robot/bot.h>
#include <mogi/simulation/scene.h>

class Arm6DOF : public Mogi::Robot::Bot {
	Mogi::Math::InverseKinematics* ikSolver;
	Mogi::Math::Matrix linkConfig;
	void linkConfigForArmMB();

public:
	Arm6DOF();

	~Arm6DOF();

	const Mogi::Math::Vector& computeAngles(const Mogi::Math::Vector& location);

	void setBase(const Mogi::Math::Vector& location,
				 const Mogi::Math::Quaternion& orientation);

	void build(Mogi::Math::Node* root);

	void setScene(Mogi::Simulation::Scene* scene);

	int setAngles(const Mogi::Math::Vector* angles = NULL);
};



#endif
