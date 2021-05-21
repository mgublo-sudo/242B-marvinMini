#include "main.h"
#include "okapi/api.hpp"
#include <fstream>
using namespace okapi;

// FILE FOR SD CARD
// FILE * usd_file_write = fopen("/usd/example.txt", "w");

Controller controller;
ControllerButton indexUpBtn (ControllerDigital::L1);
ControllerButton extakeBtn (ControllerDigital::L2);
ControllerButton intakeBtn (ControllerDigital::R1);
ControllerButton test (ControllerDigital::R2);
ControllerButton slowExtakeBtn (ControllerDigital::A);
Motor lowerRollers (15);
Motor upperRoller (-1);
Motor frontLeftDrive (-17);
Motor backLeftDrive (-20);
Motor frontRightDrive (19);
Motor backRightDrive (18);
Motor leftIntake (-11);
Motor rightIntake (14);
double color = 0;

void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		color = 0;
		pros::lcd::set_text(2, "RED");
	} else {
		color = 1;
		pros::lcd::set_text(2, "BLUE");
	}
}

std::shared_ptr<ChassisController> myStraightChassis =
ChassisControllerBuilder()
	.withMotors(
		{frontLeftDrive, backLeftDrive},
		{frontRightDrive, backRightDrive}
		)
	// Green gearset, 4 in wheel diam, 11.5 in wheel track
	.withDimensions({AbstractMotor::gearset::green, (36.0 / 60.0)}, {{3.25_in, 9.75_in}, imev5GreenTPR})		.build();

std::shared_ptr<AsyncMotionProfileController> straightProfileController = 
AsyncMotionProfileControllerBuilder()
	.withLimits({
	4.0, // Maximum linear velocity of the Chassis in m/s
	3.0, // Maximum linear acceleration of the Chassis in m/s/s
	4.0 // Maximum linear jerk of the Chassis in m/s/s/s
	})
	.withOutput(myStraightChassis)
	.buildMotionProfileController();

// 3.2ft is pretty close to 360_deg
std::shared_ptr<ChassisController> myTurnChassis =
ChassisControllerBuilder()
	.withMotors(
		{frontLeftDrive, backLeftDrive},
		{-18, -19}
		)
	// Green gearset, 4 in wheel diam, 11.5 in wheel track
	.withDimensions({AbstractMotor::gearset::green, (36.0 / 60.0)}, {{3.25_in, 9.75_in}, imev5GreenTPR})
	.build();

std::shared_ptr<AsyncMotionProfileController> turnProfileController = 
AsyncMotionProfileControllerBuilder()
	.withLimits({
	3.0, // Maximum linear velocity of the Chassis in m/s
	2.0, // Maximum linear acceleration of the Chassis in m/s/s
	6.25 // Maximum linear jerk of the Chassis in m/s/s/s
	})
	.withOutput(myTurnChassis)
	.buildMotionProfileController();

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

void intakesOutSlow() {
	leftIntake.moveVelocity(-200);
	rightIntake.moveVelocity(-200);
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
void ballExtakeSlow() {
	if (slowExtakeBtn.isPressed()) {
		intakesOutSlow();
	}
}

void ballFunctions() {
	while(true) {
		if (!(intakeBtn.isPressed() | extakeBtn.isPressed() | indexUpBtn.isPressed() | slowExtakeBtn.isPressed())) {
			intakesOff();
			indexerOff();
		} else {
			ballIntake();
			ballExtake();
			indexUp();
			ballExtakeSlow();
		}

		pros::delay(20);
	}
}

void initialize() {
	pros::lcd::initialize();
	pros::lcd::register_btn1_cb(on_center_button);
	pros::lcd::set_text(3, "\"pretty good for a 242 bot\"");
	pros::lcd::set_text(4, "-myles 8114A");

	lowerRollers.setGearing(AbstractMotor::gearset::blue);
	upperRoller.setGearing(AbstractMotor::gearset::blue);
	leftIntake.setGearing(AbstractMotor::gearset::green);
	rightIntake.setGearing(AbstractMotor::gearset::green);
	frontRightDrive.setBrakeMode(AbstractMotor::brakeMode::hold);
  	frontLeftDrive.setBrakeMode(AbstractMotor::brakeMode::hold);
  	backRightDrive.setBrakeMode(AbstractMotor::brakeMode::hold);
  	backLeftDrive.setBrakeMode(AbstractMotor::brakeMode::hold);
}

void disabled() {}

void competition_initialize() {}

void homeRowAuto() {
	intakesOut();
	pros::delay(1500);
	intakesOff();
	pros::delay(3000);
	intakesIn();
	indexerUp();
	pros::delay(700);
	intakesOff();
	pros::delay(700);
	indexerOff();
	pros::delay(7000);
	intakesIn();
	indexerUp();
	pros::delay(1500);
	indexerOff();
	intakesOff();
}

void autonomous() {

	pros::Task homeRowAutoTask(homeRowAuto);

	if (color == 0) { //red

		straightProfileController->generatePath(
			{{0_ft, 0_ft, 0_deg}, {2.9_ft, 0_ft, 0_deg}}, "S1");

		homeRowAutoTask.resume();

		straightProfileController->setTarget("S1");
		straightProfileController->waitUntilSettled();

		turnProfileController->generatePath(
			{{0_ft, 0_ft, 0_deg}, {0.57_ft, 0_ft, 0_deg}}, "T1");
		turnProfileController->setTarget("T1");
		turnProfileController->waitUntilSettled();

		straightProfileController->generatePath(
			{{0_ft, 0_ft, 0_deg}, {1.55_ft, 0_ft, 0_deg}}, "S2");
		straightProfileController->setTarget("S2");
		straightProfileController->waitUntilSettled();

		straightProfileController->generatePath(
			{{0_ft, 0_ft, 0_deg}, {-3_ft, 0_ft, 0_deg}}, "B");
		straightProfileController->setTarget("B", true);
		straightProfileController->waitUntilSettled();

		turnProfileController->generatePath(
			{{0_ft, 0_ft, 0_deg}, {1.20_ft, 0_ft, 0_deg}}, "T2");
		turnProfileController->setTarget("T2");
		turnProfileController->waitUntilSettled();

		straightProfileController->generatePath(
			{{0_ft, 0_ft, 0_deg}, {6.5_ft, 0_ft, 0_deg}}, "C");
		straightProfileController->setTarget("C");
		straightProfileController->waitUntilSettled();

		turnProfileController->generatePath(
			{{0_ft, 0_ft, 0_deg}, {0.3_ft, 0_ft, 0_deg}}, "T3");
		turnProfileController->setTarget("T3", true);
		turnProfileController->waitUntilSettled();

		straightProfileController->generatePath(
			{{0_ft, 0_ft, 0_deg}, {1.25_ft, 0_ft, 0_deg}}, "D");
		straightProfileController->setTarget("D");
		straightProfileController->waitUntilSettled();

	} else if (color == 1) { //blue

		straightProfileController->generatePath(
			{{0_ft, 0_ft, 0_deg}, {2.8_ft, 0_ft, 0_deg}}, "S1");

		homeRowAutoTask.resume();

		straightProfileController->setTarget("S1");
		straightProfileController->waitUntilSettled();

		turnProfileController->generatePath(
			{{0_ft, 0_ft, 0_deg}, {0.57_ft, 0_ft, 0_deg}}, "T1");
		turnProfileController->setTarget("T1");
		turnProfileController->waitUntilSettled();

		straightProfileController->generatePath(
			{{0_ft, 0_ft, 0_deg}, {1.55_ft, 0_ft, 0_deg}}, "S2");
		straightProfileController->setTarget("S2");
		straightProfileController->waitUntilSettled();

		straightProfileController->generatePath(
			{{0_ft, 0_ft, 0_deg}, {-3_ft, 0_ft, 0_deg}}, "B");
		straightProfileController->setTarget("B", true);
		straightProfileController->waitUntilSettled();

		turnProfileController->generatePath(
			{{0_ft, 0_ft, 0_deg}, {1.25_ft, 0_ft, 0_deg}}, "T2");
		turnProfileController->setTarget("T2");
		turnProfileController->waitUntilSettled();

		straightProfileController->generatePath(
			{{0_ft, 0_ft, 0_deg}, {6.0_ft, 0_ft, 0_deg}}, "C");
		straightProfileController->setTarget("C");
		straightProfileController->waitUntilSettled();

		turnProfileController->generatePath(
			{{0_ft, 0_ft, 0_deg}, {0.45_ft, 0_ft, 0_deg}}, "T3");
		turnProfileController->setTarget("T3", true);
		turnProfileController->waitUntilSettled();

		straightProfileController->generatePath(
			{{0_ft, 0_ft, 0_deg}, {2.0_ft, 0_ft, 0_deg}}, "D");
		straightProfileController->setTarget("D");
		straightProfileController->waitUntilSettled();

	}
}

void opcontrol() {
	// pros::delay(500);
	// fclose(usd_file_write);
	//homeRowAutoTask.suspend();
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
