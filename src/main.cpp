#include "main.h"
#include "okapi/api.hpp"
using namespace okapi;

Controller controller;
ControllerButton indexUp (ControllerDigital::R1);
ControllerButton indexDown (ControllerDigital::R2);
Motor rollers1 (-3);
Motor rollers2 (-4);
Motor frontLeftDrive (2);
Motor backLeftDrive (1);
Motor frontRightDrive (-8);
Motor backRightDrive (-9);

void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(2, "marvin mini - woo hoo!");

	rollers1.setGearing(AbstractMotor::gearset::blue);
	rollers2.setGearing(AbstractMotor::gearset::blue);
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

		pros::delay(20);
	}
}
