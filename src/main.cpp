#include "main.h"
#include "okapi/api.hpp"
using namespace okapi;

Controller controller;
ControllerButton indexUp (ControllerDigital::L1);
ControllerButton indexDown (ControllerDigital::L2);
ControllerButton intakesIn (ControllerDigital::R1);
ControllerButton intakesOut (ControllerDigital::R2);
Motor rollers1 (9);
Motor rollers2 (-10);
Motor frontLeftDrive (-2);
Motor backLeftDrive (-1);
Motor frontRightDrive (4);
Motor backRightDrive (3);
Motor leftIntake (-19);
Motor rightIntake (20);

void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(2, "marvin mini - woo hoo!");

	rollers1.setGearing(AbstractMotor::gearset::blue);
	rollers2.setGearing(AbstractMotor::gearset::blue);
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
	std::shared_ptr<ChassisController> myChassis =
	ChassisControllerBuilder()
		.withMotors(
			{frontLeftDrive, backLeftDrive},
			{frontRightDrive, backRightDrive}
			)
		// Green gearset, 4 in wheel diam, 11.5 in wheel track
		.withDimensions({AbstractMotor::gearset::green, (36.0 / 60.0)}, {{3.25_in, 9.75_in}, imev5GreenTPR})
		.build();

	std::shared_ptr<AsyncMotionProfileController> profileController = 
	AsyncMotionProfileControllerBuilder()
		.withLimits({
		1.0, // Maximum linear velocity of the Chassis in m/s
		2.0, // Maximum linear acceleration of the Chassis in m/s/s
		10.0 // Maximum linear jerk of the Chassis in m/s/s/s
		})
		.withOutput(myChassis)
		.buildMotionProfileController();

}

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
									controller.getAnalog(ControllerAnalog::rightX));

		if (intakesIn.isPressed()) {
			leftIntake.moveVelocity(200);
			rightIntake.moveVelocity(200);
		} else {
			leftIntake.moveVelocity(0);
			rightIntake.moveVelocity(0);
		}
		if (indexUp.isPressed()) {
			rollers1.moveVelocity(600);
			rollers2.moveVelocity(600);
		} else {
			rollers1.moveVelocity(0);
			rollers2.moveVelocity(0);
		}
		if (indexDown.isPressed()) {
			rollers1.moveVelocity(-600);
			rollers2.moveVelocity(-600);
			leftIntake.moveVelocity(-200);
			rightIntake.moveVelocity(-200);
		}
		// else {
		// 	rollers1.moveVelocity(0);
		// 	rollers2.moveVelocity(0);
		// 	leftIntake.moveVelocity(0);
		// 	rightIntake.moveVelocity(0);
		// }

		pros::delay(20);
	}
}
