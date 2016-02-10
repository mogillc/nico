//
//  Arm6DOF.cpp
//
//  Created by Matt Bunting on 12/28/15.
//
//

#include "Arm6DOF.h"

#include <sstream>

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
		using namespace Math;
	using namespace Robot;

//#ifdef PENGL_EXISTS
			using namespace ::Simulation;

			void Arm6DOF::setScene(Scene* scene) {
				if (scene != NULL) {
					std::stringstream filename;
					std::string modelsLocation = Scene::getResourceDirectory();
					modelsLocation.append("/models/arm");
					Node* segment = nodeLocation;  //->child(0);

					for (int i = 0; i < 7; i++) {
      segment = segment->child(0);
      filename.str("");
      filename << "segment" << i << ".STL" << std::ends;
						scene->attachMeshToNode(segment, scene->addMesh(filename.str(), modelsLocation));
					}
				}
			}
//#endif

			void Arm6DOF::build(Node* root) {
				Bot::buildNodeStructure(root);
				static int ID = 0;
				std::stringstream tempName, filename;
				//#ifdef OPENGL_EXISTS
				//		std::string modelsLocation = Scene::getResourceDirectory();
				//		modelsLocation.append("/models/arm");
				//#endif

				linkConfigForArmMB();

				tempName << "arm6DOF_" << ID << std::ends;
				nodeLocation = root->addNode(tempName.str());
				nodeLocation->setScale(2.0 / MM_PER_METER);

				Vector fulcrum(3);
				Node* segment = nodeLocation;  //->addNode(tempName.str());
				for (int i = 0; i < 7; i++) {
					tempName.str("");
					tempName << "arm6DOF_" << ID << "_segment_" << i << std::ends;
					// filename.str("");
					// filename << "segment" << i << ".STL" << std::ends;
					segment = segment->addNode(tempName.str());
					//#ifdef OPENGL_EXISTS
					//			if (scene != NULL) {
					//				segment->addMeshID( scene->addMesh(filename.str(),
					//modelsLocation));
					//			}
					//#endif
					segment->setLocation(fulcrum);

					fulcrum(0) = linkConfig(0, i);
					fulcrum(1) = linkConfig(1, i);
					fulcrum(2) = linkConfig(2, i);
				}

				for (int i = 0; i < linkConfig.numColumns(); i++) {
					Vector link(3);
					link(0) = linkConfig(0, i);
					link(1) = linkConfig(1, i);
					link(2) = linkConfig(2, i);
					ikSolver->setLink(i, link);
				}
				// ikSolver->setConfiguration( linkConfig );
				//#ifdef OPENGL_EXISTS
				//		setScene(scene);
				//#endif

				ID++;
			}

			int Arm6DOF::setAngles(const Vector* angles) {
				if (angles != NULL) {
					if (angles->size() == 6) {
      //		Node *child = nodeLocation->child(0)->child(0);
      //		child->setOrientation(result(0), zAxis);
      //		child = child->child(0);
      //		child->setOrientation(result(1), yAxis);
      //		child = child->child(0);
      //		child->setOrientation(result(2), yAxis);
      //		child = child->child(0);
      //		child->setOrientation(result(3), xAxis);
      //		child = child->child(0);
      //		child->setOrientation(result(4), zAxis);
      //		child = child->child(0);
      //		child->setOrientation(result(5), yAxis);
      // std::cerr << "angles->value(0, 0) = " << angles->value(0, 0) <<
      // std::endl;
      Node* child = nodeLocation->child(0)->child(0);
      child->setOrientation(angles->valueAsConst(0, 0), Vector::zAxis);
      child = child->child(0);
      child->setOrientation(angles->valueAsConst(1, 0), Vector::yAxis);
      child = child->child(0);
      child->setOrientation(angles->valueAsConst(2, 0), Vector::yAxis);
      child = child->child(0);
      child->setOrientation(angles->valueAsConst(3, 0), Vector::xAxis);
      child = child->child(0);
      child->setOrientation(angles->valueAsConst(4, 0), Vector::zAxis);
      child = child->child(0);
      child->setOrientation(angles->valueAsConst(5, 0), Vector::yAxis);

      return 0;
					}
					std::cerr << " - - - - - Angles size is not 6...... angles->size() = "
					<< angles->size() << std::endl;
				}
				std::cerr << " - - - - - Angles is null..." << std::endl;

				return -1;
			}

			void Arm6DOF::linkConfigForArmMB() {
				linkConfig.setSize(3, 7);

				// Joint 1 config, give the location:
				linkConfig(0, 0) = 0;
				linkConfig(1, 0) = 0;
				// linkConfig[0](2,0) = RX_64_HINGE_WIDTH/2.0 + EX_106_HORN_TO_IDLER/2.0 +
				// EX_106_HINGE_RADIUS_BUNTING;
				linkConfig(2, 0) =
				RX_64_ANGLE_BRACKET_BUNTING +
				RX_64_HORN_TO_IDLER / 2.0;  // + EX_106_HINGE_RADIUS_BUNTING;

				// Joint 2 config,
				linkConfig(2, 1) = EX_106_HINGE_RADIUS_BUNTING;

				// Joint 3 config,
				linkConfig(0, 2) = EX_106_DOUBLE_MATE;  // 93.03;

				// Joint 4 config,
				linkConfig(0, 3) = RX_28_HORN_TO_IDLER / 2.0;  // 65.43;//68.5;//36;
				linkConfig(2, 3) = EX_106_HINGE_RADIUS_BUNTING + RX_28_SIDE_OFFSET +
				RX_28_SIDE_HOLE_OFFSET;  // 50.7;//55.1;//41.92;

				// Joint 5 config,
				linkConfig(0, 4) = RX_28_COLUMN_BUNTING + RX_28_SIDE_OFFSET +
				RX_28_LONG_HOLE_OFFSET;  // 68.06;

				// Joint 6 config,
				linkConfig(0, 5) = RX_28_HINGE_OFFSET + RX_28_SIDE_OFFSET +
				RX_28_LONG_HOLE_OFFSET;  // 66.80;

				// Joint 7 config,
				linkConfig(0, 6) = RX_28_HINGE_OFFSET;  // + END_EFFECTOR_CC;//74.26;
			}

			Arm6DOF::Arm6DOF() {
				ikSolver = InverseKinematics::create(InverseKinematics::ZYYXZY);
			};

			Arm6DOF::~Arm6DOF() { delete ikSolver; }

			const Vector& Arm6DOF::computeAngles(const Vector& location) {
				int error = ikSolver->compute(location);
				switch (error) {
					case KINEMATICS_NO_ERROR:
      break;
					case KINEMATICS_OVER_MAX_ERROR:
      std::cout << "Bad 6DOF solution for " << name
						<< ": KINEMATICS_OVER_MAX_ERROR" << std::endl;
      break;
					case KINEMATICS_BAD_RESULT:
      std::cout << "Bad 6DOF solution for " << name << ": KINEMATICS_BAD_RESULT"
						<< std::endl;
      break;

					default:
      std::cout << "Bad 6DOF solution for " << name << ", error = " << error
						<< std::endl;

      break;
				}

				Vector result = ikSolver->getMotorAngles();

				//
				//		Node *child = nodeLocation->child(0)->child(0);
				//		child->setOrientation(result(0), zAxis);
				//		child = child->child(0);
				//		child->setOrientation(result(1), yAxis);
				//		child = child->child(0);
				//		child->setOrientation(result(2), yAxis);
				//		child = child->child(0);
				//		child->setOrientation(result(3), xAxis);
				//		child = child->child(0);
				//		child->setOrientation(result(4), zAxis);
				//		child = child->child(0);
				//		child->setOrientation(result(5), yAxis);

				this->setAngles(&result);

				return ikSolver->getMotorAngles();
			}

			void Arm6DOF::setBase(const Vector& location, const Quaternion& orientation) {
				nodeLocation->child(0)->setLocation(location);
				nodeLocation->child(0)->setOrientation(orientation);
				ikSolver->setBase(location, orientation);
			}

	
#ifdef _cplusplus
}
#endif
