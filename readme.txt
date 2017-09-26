StepperControl:
	
	Stepper Motor Controller for 28BYJ-48.

	Features 

		* Different drive modes: 

			FullStep
			HalfStep
			WaveDrive

		* Direction Control
		* Speed Control
		* Actions, sequence of specified movements with start and end delay and callback functionality
	
	For a more generic stepper motor library please check the Arduino Stepper.h lib.
	It handles stepper motors with 2/4/5 pins!

	Example:

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



	Action Example:

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