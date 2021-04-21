#include "main.h"
#include "okapi/api.hpp"
using namespace okapi;

Controller controller;
ControllerButton indexUp (ControllerDigital::L1);
ControllerButton indexDown (ControllerDigital::L2);
ControllerButton intakesIn (ControllerDigital::R1);
ControllerButton intakesOut (ControllerDigital::R2);
Motor rollers1 (-9);
Motor rollers2 (-10);
Motor frontLeftDrive (-2);
Motor backLeftDrive (-1);
Motor frontRightDrive (3);
Motor backRightDrive (4);
Motor leftIntake (19);
Motor rightIntake (20);

void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(2, "marvin mini - woo hoo!");

	rollers1.setGearing(AbstractMotor::gearset::blue);
	rollers2.setGearing(AbstractMotor::gearset::blue);
	leftIntake.setGearing(AbstractMotor::gearset::green);
	rightIntake.setGearing(AbstractMotor::gearset::green);
}

void disabled() {}

void competition_initialize() {}

void autonomous() {}

void opcontrol() {
	std::shared_ptr<ChassisController> drive = ChassisControllerBuilder()
	 .withMotors(
		 {frontLeftDrive, backLeftDrive},
		 {frontRightDrive, backRightDrive}
		 )
	 .withDimensions(AbstractMotor::gearset::blue, {{4_in, 8_in}, imev5BlueTPR})
	 .build();

	while (true) {
		drive->getModel()->arcade(controller.getAnalog(ControllerAnalog::leftY),
															controller.getAnalog(ControllerAnalog::leftX));

		if (indexUp.isPressed()) {
			rollers1.moveVelocity(600);
			rollers2.moveVelocity(600);
		} else if (indexDown.isPressed()) {
			rollers1.moveVelocity(-600);
			rollers2.moveVelocity(-600);
		} else {
			rollers1.moveVelocity(0);
			rollers2.moveVelocity(0);
		}

		if (intakesIn.isPressed()) {
			leftIntake.moveVelocity(200);
			rightIntake.moveVelocity(200);
		} else if (intakesOut.isPressed()) {
			leftIntake.moveVelocity(-200);
			rightIntake.moveVelocity(-200);
		} else {
			leftIntake.moveVelocity(0);
			rightIntake.moveVelocity(0);
		}

		pros::delay(20);
	}
}
