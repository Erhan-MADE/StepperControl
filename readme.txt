StepperControl:
	
	Stepper Motor Controller for 28BYJ-48.

	Features 

		* Different drive modes: 

			FullStep
			HalfStep
			WaveDrive

		* Direction Control
		* Speed Control
	
	For a more generic stepper motor library please check the Arduino Stepper.h lib.
	It handles stepper motors with 2/4/5 pins!

	Example:

		#include <StepperControl.h>
		
		StepperControl control(4096, 8,9,10,11);

		void start()
		{
		 	control.SetDirection(StepperControl::Backward);
		  	control.SetRotationSpeed(2);
		  	control.SetStepType(StepperControl::FullStep);
	  	}

	  	void loop()
	  	{
	  		// NOTE: initial Step count is always based on FullStep. WaveDrive will
	  		// need only half of the steps for a full rotation you can just query 
	  		// the controller to get right amount of steps for the specified rotation
	 		const uint32_t steps = control.GetStepsFromDegrees(360);
	  		control.Step(steps);

	  		control.SetDirection(control.GetInvertedDirection());
	  		delay(1000);
	  	}