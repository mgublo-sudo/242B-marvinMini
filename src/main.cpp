#include "main.h"
#include "okapi/api.hpp"
using namespace okapi;

Controller controller;
ControllerButton indexUp (ControllerDigital::R1);
ControllerButton indexDown (ControllerDigital::R2);
Motor rollers1 (-3);
Motor rollers2 (-4);
Motor frontLeftDrive (2);
Motor backLeftDrive (-1);
Motor frontRightDrive (-8);
Motor backRightDrive (9);

void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(2, "marvin mini - woo hoo!");

	rollers1.setGearing(AbstractMotor::gearset::blue);
	rollers2.setGearing(AbstractMotor::gearset::blue);
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
    .withDimensions({AbstractMotor::gearset::green, (36.0 / 60.0)}, {{3.25_in, 9.5_in}, imev5GreenTPR})
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

  profileController->generatePath(
    {{0_ft, 0_ft, 0_deg}, {3_ft, 3_ft, 0_deg}}, "A");

  profileController->setTarget("A");
  profileController->waitUntilSettled();

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
