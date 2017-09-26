
/**
 * Library for 28byj-48 Stepper Motor
 * 
 * @author Erhan Yilgör
*/

#ifndef StepperControl_h
#define StepperControl_h

#include "Arduino.h"


 class StepperControl 
{
  public:

    enum StepDirection
    {
      Forward,
      Backward
    };

    enum StepType
    {
      WaveDrive,
      FullStep,
      HalfStep  
    };

    StepperControl(const uint32_t rotationStepCount, const uint8_t pin1, const uint8_t pin2, const uint8_t pin3, const uint8_t pin4);
    
    void SetRotationSpeed(const double rpm);
    void SetRotationSteps(const uint32_t rotationSteps);
    void SetDirection(const StepDirection direction);
    void SetStepType(const StepType type);
    void Step(const uint32_t steps);
    
    inline StepType GetCurrentStepType()
    {
      return _stepType;
    }

    inline StepDirection GetInvertedDirection()
    {
      return _direction == Forward ? Backward : Forward;
    }

    inline StepDirection GetDirection()
    {
      return _direction;
    }

    inline uint32_t GetStepsFromDegrees(const double degrees)
    {
      const double rotation = degrees / 360.0;
      return rotation * (_rotationSteps / (_stepType == WaveDrive ? 2L : 1L));
    }

  private:

    void Think();
    void ThinkWaveDrive();
    void ThinkFullStep();
    void ThinkHalfStep();
  
    uint8_t _pins[4];
    int  _steps;
    StepDirection _direction;
    StepType _stepType;
    uint32_t _rotationSteps;
    double _rpm;
    unsigned long _stepDelay;
    unsigned long _lastStepTime;
};

#endif