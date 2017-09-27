/**
 * Library for 28byj-48 Stepper Motor
 *
 * @author Erhan Yilgör
 */

#ifndef StepperControl_h
#define StepperControl_h

#include "Arduino.h"

typedef void (StepperCallback)();

class StepperControl
{
public:
    
    static const uint32_t DefaultStepCount = 4096;
    
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
    ~StepperControl();
    
    void SetRotationSpeed(const double rpm);
    void SetFullRotationStepCount(const uint32_t rotationSteps);
    void SetDirection(const StepDirection direction);
    void SetStepType(const StepType type);
    void Step(const uint32_t steps);
    void RemoveAllActions();
    void StartAction();
    
    inline StepType GetCurrentStepType()
    {
        return _stepType;
    }
    
    inline StepDirection GetInvertedDirection()
    {
        return _direction == Forward ? Backward : Forward;
    }
    
    inline StepDirection GetCurrentDirection()
    {
        return _direction;
    }
    
    inline uint32_t GetCurrentFullRotationStepCount()
    {
        return _rotationSteps;
    }
    
    inline double GetCurrentRotationSpeed()
    {
        return _rpm;
    }
    
    inline static uint32_t GetStepsFromDegreesForSettings(const double degrees, const StepType type, const uint32_t roationSteps = DefaultStepCount)
    {
        const double rotation = degrees / 360.0;
        return rotation * (roationSteps / (type == WaveDrive ? 2L : 1L));
    }
    
    inline uint32_t GetStepsFromDegrees(const double degrees)
    {
        return GetStepsFromDegreesForSettings(degrees, _stepType, _rotationSteps);
    }
    
    struct StepperAction
    {
    public:
        StepType Type;
        StepDirection Direction;
        uint32_t Steps;
        uint32_t StartDelay;
        uint32_t EndDelay;
        double Rpm;
        StepperCallback* DidEndCallback;
    };
    void AddStepperAction(const StepperAction& action);
    static void CopyActionValuesFromTo(const StepperAction& source, StepperAction& destination);
    
private:
    void Think();
    void ThinkAction();
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
    StepperAction* _stepperActions;
    uint8_t _currentActionIndex;
    uint8_t _actionCount;
};

#endif