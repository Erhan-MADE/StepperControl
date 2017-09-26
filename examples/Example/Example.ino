#include <StepperControl.h>

StepperControl control(StepperControl::DefaultStepCount, 8,9,10,11);

void setup()
{
  control.SetDirection(StepperControl::Backward);
  control.SetRotationSpeed(2);
  control.SetStepType(StepperControl::FullStep);
}

void loop()
{
  // NOTE: initial Step count is always based on FullStep. WaveDrive will
  // need only half of the steps for a full rotation, you can just query
  // the controller to get the right amount of steps for the specified rotation
  const uint32_t steps = control.GetStepsFromDegrees(360);
  control.Step(steps);

  control.SetDirection(control.GetInvertedDirection());
  delay(1000);
}
