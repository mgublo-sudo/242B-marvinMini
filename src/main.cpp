#include "main.h"
#include "okapi/api.hpp"
using namespace okapi;

Controller controller;
ControllerButton indexUpBtn (ControllerDigital::L1);
ControllerButton extakeBtn (ControllerDigital::L2);
ControllerButton intakeBtn (ControllerDigital::R1);
ControllerButton test (ControllerDigital::R2);
Motor lowerRollers (15);
Motor upperRoller (-1);
Motor frontLeftDrive (-17);
Motor backLeftDrive (-20);
Motor frontRightDrive (19);
Motor backRightDrive (18);
Motor leftIntake (-11);
Motor rightIntake (14);

//Intakes In Function
void intakesIn() {
	leftIntake.moveVelocity(600);
	rightIntake.moveVelocity(600);
}

//Intakes Out Function
void intakesOut() {
	leftIntake.moveVelocity(-600);
	rightIntake.moveVelocity(-600);
}

//Stop Intakes Function
void intakesOff() {
	leftIntake.moveVelocity(0);
	rightIntake.moveVelocity(0);
}

//Indexer Rollers Up Function
void indexerUp() {
	upperRoller.moveVelocity(600);
	lowerRollers.moveVelocity(600);
}

//Indexer Rollers Down Function
void indexerDown() {
	upperRoller.moveVelocity(-600);
	lowerRollers.moveVelocity(-600);
}

//Stop Indexer Rollers Function
void indexerOff() {
	upperRoller.moveVelocity(0);
	lowerRollers.moveVelocity(0);
}

//Lower Indexer Rollers Function
void lowerIndexer() {
	lowerRollers.moveVelocity(200);
}

//Stop Lower Indexer Rollers Function
void lowerIndexerOff() {
	lowerRollers.moveVelocity(0);
}

void ballIntake() {
	if (intakeBtn.isPressed()) {
		intakesIn();
		lowerIndexer();
	}
}
void ballExtake() {
	if (extakeBtn.isPressed()) {
		intakesOut();
		indexerDown();
	}
}
void indexUp() {
	if (indexUpBtn.isPressed()) {
		indexerUp();
	}
}


void ballFunctions() {
	while(true) {
		if (!(intakeBtn.isPressed() | extakeBtn.isPressed() | indexUpBtn.isPressed())) {
			intakesOff();
			indexerOff();
		} else {
			ballIntake();
			ballExtake();
			indexUp();
		}

		pros::delay(20);
	}
}

void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(2, "marvin mini - woo hoo!");

	lowerRollers.setGearing(AbstractMotor::gearset::blue);
	upperRoller.setGearing(AbstractMotor::gearset::blue);
	leftIntake.setGearing(AbstractMotor::gearset::green);
	rightIntake.setGearing(AbstractMotor::gearset::green);
	frontRightDrive.setBrakeMode(AbstractMotor::brakeMode::brake);
  	frontLeftDrive.setBrakeMode(AbstractMotor::brakeMode::brake);
  	backRightDrive.setBrakeMode(AbstractMotor::brakeMode::brake);
  	backLeftDrive.setBrakeMode(AbstractMotor::brakeMode::brake);
}

void disabled() {}

void competition_initialize() {}

void autonomous() {
	// std::shared_ptr<ChassisController> myChassis =
	// ChassisControllerBuilder()
	// 	.withMotors(
	// 		{frontLeftDrive, backLeftDrive},
	// 		{frontRightDrive, backRightDrive}
	// 		)
	// 	// Green gearset, 4 in wheel diam, 11.5 in wheel track
	// 	.withDimensions({AbstractMotor::gearset::green, (36.0 / 60.0)}, {{3.25_in, 9.75_in}, imev5GreenTPR})
	// 	.build();

	// std::shared_ptr<AsyncMotionProfileController> profileController = 
	// AsyncMotionProfileControllerBuilder()
	// 	.withLimits({
	// 	1.0, // Maximum linear velocity of the Chassis in m/s
	// 	2.0, // Maximum linear acceleration of the Chassis in m/s/s
	// 	10.0 // Maximum linear jerk of the Chassis in m/s/s/s
	// 	})
	// 	.withOutput(myChassis)
	// 	.buildMotionProfileController();
	// myChassis->setMaxVelocity(100);
	// profileController->generatePath(
	// 	{{0_ft, 0_ft, 0_deg}, {6_ft, 0_ft, 0_deg}}, "A");
	// profileController->generatePath(
	// 	{{0_ft, 6_ft, 0_deg}, {-1_ft, 6_ft, 0_deg}}, "B");
	// pros::delay(2000);
	// profileController->setTarget("A");
	// profileController->waitUntilSettled();
	// pros::delay(2000);
	// profileController->setTarget("B");
	// profileController->waitUntilSettled();

	std::shared_ptr<OdomChassisController> myOdomChassis =
	ChassisControllerBuilder()
		.withMotors(
			{frontLeftDrive, backLeftDrive},
			{frontRightDrive, backRightDrive}
			)
		.withGains(
        {0.001, 0, 0.0001}, // Distance controller gains
        {0.001, 0, 0.0001}, // Turn controller gains
        {0.001, 0, 0.0001}  // Angle controller gains (helps drive straight)
    	)
		// Green gearset, 4 in wheel diam, 11.5 in wheel track
		.withDimensions({AbstractMotor::gearset::green, (36.0 / 60.0)}, {{3.25_in, 9.75_in}, imev5GreenTPR})
		.withOdometry() // use the same scales as the chassis (above)
    	.buildOdometry(); // build an odometry chassis
	
	// set the state to zero
	myOdomChassis->setState({0_in, 0_in, 0_deg});
	// turn 45 degrees and drive approximately 1.4 ft
	myOdomChassis->driveToPoint({1_ft, 1_ft});
	// turn approximately 45 degrees to end up at 90 degrees
	myOdomChassis->turnToAngle(90_deg);
	// turn approximately -90 degrees to face {5_ft, 0_ft} which is to the north of the robot
	myOdomChassis->turnToPoint({5_ft, 0_ft});
}

void opcontrol() {
	std::shared_ptr<ChassisController> drive = ChassisControllerBuilder()
	 .withMotors(
		 {frontLeftDrive, backLeftDrive},
		 {frontRightDrive, backRightDrive}
		 )
	 .withDimensions(AbstractMotor::gearset::blue, {{4_in, 8_in}, imev5BlueTPR})
	 .build();

	pros::Task ballFunctionsTask(ballFunctions);
	ballFunctionsTask.resume();

	while (true) {
		drive->getModel()->arcade(controller.getAnalog(ControllerAnalog::leftY),
									controller.getAnalog(ControllerAnalog::rightX));

		pros::delay(20);
	}
}
