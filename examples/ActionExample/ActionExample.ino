#include <StepperControl.h>

StepperControl control(StepperControl::DefaultStepCount, 8,9,10,11);

void Callback()
{
  Serial.println("i am a callback");
}
void Callback2()
{
  Serial.println("i, too am a callback");
}

void setup()
{
  Serial.begin(9600);
  control.SetDirection(StepperControl::Backward);
  control.SetRotationSpeed(2);
  control.SetStepType(StepperControl::FullStep);

  StepperControl::StepperAction action;
  action.Direction = StepperControl::Forward;
  action.Type = StepperControl::WaveDrive;
  action.Rpm = 1.5;
  action.StartDelay = 1000;
  action.Steps = 1400;
  action.EndDelay = 500;
  action.DidEndCallback = &Callback;

  control.AddStepperAction(action);


  StepperControl::StepperAction action2;
  action2.Direction = StepperControl::Backward;
  action2.Type = StepperControl::FullStep;
  action2.Rpm = 2.5;
  action2.StartDelay = 2000;
  action2.Steps = 1800;
  action2.EndDelay = 560;
  action2.DidEndCallback = &Callback2;

  control.AddStepperAction(action2);
}

void loop()
{
  control.StartAction();
  delay(1000);
}
