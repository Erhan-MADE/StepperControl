#include "StepperControl.h"

const uint32_t kMaximumSteps(8);

const unsigned long kSecondToMicroSecond = 1000000L;
// #define DEBUG_STEPS

StepperControl::StepperControl(const uint32_t rotationStepCount, const uint8_t pin1, const uint8_t pin2, const uint8_t pin3, const uint8_t pin4) :
_rotationSteps(rotationStepCount),
_stepperActions(nullptr),
_currentActionIndex(0),
_direction(Forward),
_stepType(HalfStep),
_lastStepTime(0),
_actionCount(0),
_stepDelay(0),
_steps(0),
_rpm(0.0)
{
    _pins[0] = pin1;
    _pins[1] = pin2;
    _pins[2] = pin3;
    _pins[3] = pin4;
    
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
    pinMode(pin3, OUTPUT);
    pinMode(pin4, OUTPUT);
}

StepperControl::~StepperControl()
{
    if(_stepperActions != nullptr)
    {
        delete[] _stepperActions;
        _actionCount = 0;
    }
}

void StepperControl::CopyActionValuesFromTo(const StepperAction& source, StepperAction& destination)
{
    destination.Type = source.Type;
    destination.Direction = source.Direction;
    destination.Steps = source.Steps;
    destination.StartDelay = source.StartDelay;
    destination.EndDelay = source.EndDelay;
    destination.Rpm = source.Rpm;
    destination.DidEndCallback = source.DidEndCallback;
}

void StepperControl::AddStepperAction(const StepperAction& action)
{
    if(_actionCount == 255)
        return;
    
    StepperAction* actions = new StepperAction[_actionCount+1];
    
    for(int i = 0; i < _actionCount; i++)
    {
        StepperControl::CopyActionValuesFromTo(_stepperActions[i], actions[i]);
    }
    
    StepperControl::CopyActionValuesFromTo(action, actions[_actionCount]);
    
    _actionCount++;
    
    if(_stepperActions != nullptr)
        delete[] _stepperActions;
    
    _stepperActions = actions;
}

void StepperControl::SetRotationSpeed(const double rpm)
{
    _rpm = rpm;
    _stepDelay = (unsigned long) ((60L * kSecondToMicroSecond) / (unsigned long) _rotationSteps) * (1.0 / _rpm);
}

void StepperControl::SetFullRotationStepCount(const uint32_t rotationSteps)
{
    _rotationSteps = rotationSteps;
}

void StepperControl::SetDirection(const StepDirection direction)
{
    _direction = direction;
}

void StepperControl::SetStepType(const StepType type)
{
    _stepType = type;
}

void StepperControl::Step(const uint32_t steps)
{
    if(steps == 1)
    {
        this->Think();
        return;
    }
    
    uint32_t stepCount = 0;
    
    while(stepCount < steps)
    {
        const unsigned long time = micros();
        const unsigned long stepDelay = _stepDelay * (_stepType == WaveDrive ? 2L : 1L);
        
        if(time - _lastStepTime >= stepDelay)
        {
            this->Think();
            stepCount++;
            _lastStepTime = time;
        }
    }
}

void StepperControl::StartAction()
{
    if(_actionCount == 0)
        return;
    
    this->ThinkAction();
}

void StepperControl::RemoveAllActions()
{
    if(_actionCount == 0)
        return;
    
    delete[] _stepperActions;
    _stepperActions = nullptr;
    _currentActionIndex = 0;
    _actionCount = 0;
}

void StepperControl::ThinkAction()
{
    while(_currentActionIndex < _actionCount)
    {
        this->SetRotationSpeed(_stepperActions[_currentActionIndex].Rpm);
        this->SetStepType(_stepperActions[_currentActionIndex].Type);
        this->SetDirection(_stepperActions[_currentActionIndex].Direction);
        
        delay(_stepperActions[_currentActionIndex].StartDelay);
        
        this->Step(_stepperActions[_currentActionIndex].Steps);
        
        delay(_stepperActions[_currentActionIndex].EndDelay);
        
        if(_stepperActions[_currentActionIndex].DidEndCallback != nullptr)
            _stepperActions[_currentActionIndex].DidEndCallback();
        
        _currentActionIndex++;
    }
    _currentActionIndex = 0;
}

void StepperControl::Think()
{
    switch(_stepType)
    {
        case WaveDrive:
            this->ThinkWaveDrive();
            break;
        case FullStep:
            this->ThinkFullStep();
            break;
        case HalfStep:
            this->ThinkHalfStep();
            break;
    }
    
    if(_direction == Forward)
        _steps = ++_steps >= kMaximumSteps ? 0 : _steps;
    else if(_direction == Backward)
        _steps = --_steps < 0 ? kMaximumSteps-1 : _steps;
    
#if defined (DEBUG_STEPS)
    Serial.println(_steps);
#endif
}

void StepperControl::ThinkWaveDrive()
{
    switch(_steps)
    {
        case 0 :
            digitalWrite(_pins[0], HIGH);
            digitalWrite(_pins[1], LOW);
            digitalWrite(_pins[2], LOW);
            digitalWrite(_pins[3], LOW);
            break;
        case 1 :
            digitalWrite(_pins[0], LOW);
            digitalWrite(_pins[1], HIGH);
            digitalWrite(_pins[2], LOW);
            digitalWrite(_pins[3], LOW);
            break;
        case 2 :
            digitalWrite(_pins[0], LOW);
            digitalWrite(_pins[1], LOW);
            digitalWrite(_pins[2], HIGH);
            digitalWrite(_pins[3], LOW);
            break;
        case 3 :
            digitalWrite(_pins[0], LOW);
            digitalWrite(_pins[1], LOW);
            digitalWrite(_pins[2], LOW);
            digitalWrite(_pins[3], HIGH);
            break;
        case 4 :
            digitalWrite(_pins[0], HIGH);
            digitalWrite(_pins[1], LOW);
            digitalWrite(_pins[2], LOW);
            digitalWrite(_pins[3], LOW);
            break;
        case 5 :
            digitalWrite(_pins[0], LOW);
            digitalWrite(_pins[1], HIGH);
            digitalWrite(_pins[2], LOW);
            digitalWrite(_pins[3], LOW);
            break;
        case 6 :
            digitalWrite(_pins[0], LOW);
            digitalWrite(_pins[1], LOW);
            digitalWrite(_pins[2], HIGH);
            digitalWrite(_pins[3], LOW);
            break;
        case 7 :
            digitalWrite(_pins[0], LOW);
            digitalWrite(_pins[1], LOW);
            digitalWrite(_pins[2], HIGH);
            digitalWrite(_pins[3], HIGH);
            break;
    }
}

void StepperControl::ThinkFullStep()
{
    switch(_steps)
    {
        case 0 :
            digitalWrite(_pins[0], HIGH);
            digitalWrite(_pins[1], LOW);
            digitalWrite(_pins[2], LOW);
            digitalWrite(_pins[3], HIGH);
            break;
        case 1 :
            digitalWrite(_pins[0], HIGH);
            digitalWrite(_pins[1], LOW);
            digitalWrite(_pins[2], LOW);
            digitalWrite(_pins[3], HIGH);
            break;
        case 2 :
            digitalWrite(_pins[0], HIGH);
            digitalWrite(_pins[1], HIGH);
            digitalWrite(_pins[2], LOW);
            digitalWrite(_pins[3], LOW);
            break;
        case 3 :
            digitalWrite(_pins[0], HIGH);
            digitalWrite(_pins[1], HIGH);
            digitalWrite(_pins[2], LOW);
            digitalWrite(_pins[3], LOW);
            break;
        case 4 :
            digitalWrite(_pins[0], LOW);
            digitalWrite(_pins[1], HIGH);
            digitalWrite(_pins[2], HIGH);
            digitalWrite(_pins[3], LOW);
            break;
        case 5 :
            digitalWrite(_pins[0], LOW);
            digitalWrite(_pins[1], HIGH);
            digitalWrite(_pins[2], HIGH);
            digitalWrite(_pins[3], LOW);
            break;
        case 6 :
            digitalWrite(_pins[0], LOW);
            digitalWrite(_pins[1], LOW);
            digitalWrite(_pins[2], HIGH);
            digitalWrite(_pins[3], HIGH);
            break;
        case 7 :
            digitalWrite(_pins[0], LOW);
            digitalWrite(_pins[1], LOW);
            digitalWrite(_pins[2], HIGH);
            digitalWrite(_pins[3], HIGH);
            break;
    }
}

void StepperControl::ThinkHalfStep()
{
    switch(_steps)
    {
        case 0 :
            digitalWrite(_pins[0], HIGH);
            digitalWrite(_pins[1], LOW);
            digitalWrite(_pins[2], LOW);
            digitalWrite(_pins[3], LOW);
            break;
        case 1 :
            digitalWrite(_pins[0], HIGH);
            digitalWrite(_pins[1], HIGH);
            digitalWrite(_pins[2], LOW);
            digitalWrite(_pins[3], LOW);
            break;
        case 2 :
            digitalWrite(_pins[0], LOW);
            digitalWrite(_pins[1], HIGH);
            digitalWrite(_pins[2], LOW);
            digitalWrite(_pins[3], LOW);
            break; 
        case 3 :
            digitalWrite(_pins[0], LOW);
            digitalWrite(_pins[1], HIGH);
            digitalWrite(_pins[2], HIGH);
            digitalWrite(_pins[3], LOW);
            break; 
        case 4 :
            digitalWrite(_pins[0], LOW);
            digitalWrite(_pins[1], LOW);
            digitalWrite(_pins[2], HIGH);
            digitalWrite(_pins[3], LOW);
            break; 
        case 5 :
            digitalWrite(_pins[0], LOW);
            digitalWrite(_pins[1], LOW);
            digitalWrite(_pins[2], HIGH);
            digitalWrite(_pins[3], HIGH);
            break; 
        case 6 :
            digitalWrite(_pins[0], LOW);
            digitalWrite(_pins[1], LOW);
            digitalWrite(_pins[2], LOW);
            digitalWrite(_pins[3], HIGH);
            break; 
        case 7 :
            digitalWrite(_pins[0], HIGH);
            digitalWrite(_pins[1], LOW);
            digitalWrite(_pins[2], LOW);
            digitalWrite(_pins[3], HIGH);
            break; 
    }  
}

