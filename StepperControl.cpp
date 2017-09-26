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
  }
}

void StepperControl::AddStepperAction(const StepperAction& action)
{
  _actionCount++;
 
  StepperAction* actions = new StepperAction[_actionCount];
  StepperAction copyAction = const_cast<StepperAction&>(action);

  // FIXME: late night programming, make this less redundant
  if(_stepperActions == nullptr)
  {
    _stepperActions = actions;
    _stepperActions[0].Type = action.Type;
    _stepperActions[0].Direction = action.Direction;
    _stepperActions[0].Steps = action.Steps;
    _stepperActions[0].StartDelay = action.StartDelay;
    _stepperActions[0].EndDelay = action.EndDelay;
    _stepperActions[0].Rpm = action.Rpm;
    _stepperActions[0].DidEndCallback = action.DidEndCallback;
    return;
  }

  for(int i = 0; i < (_actionCount-1); i++)
  {
    actions[i].Type = _stepperActions[i].Type;
    actions[i].Direction = _stepperActions[i].Direction;
    actions[i].Steps = _stepperActions[i].Steps;
    actions[i].StartDelay = _stepperActions[i].StartDelay;
    actions[i].EndDelay = _stepperActions[i].EndDelay;
    actions[i].Rpm = _stepperActions[i].Rpm;
    actions[i].DidEndCallback = _stepperActions[i].DidEndCallback;
  }

  actions[_actionCount-1].Type = action.Type;
  actions[_actionCount-1].Direction = action.Direction;
  actions[_actionCount-1].Steps = action.Steps;
  actions[_actionCount-1].StartDelay = action.StartDelay;
  actions[_actionCount-1].EndDelay = action.EndDelay;
  actions[_actionCount-1].Rpm = action.Rpm;
  actions[_actionCount-1].DidEndCallback = action.DidEndCallback;

  delete[] _stepperActions;
  _stepperActions = actions;
}

void StepperControl::SetRotationSpeed(const double rpm)
{
  _rpm = rpm;
  _stepDelay = (unsigned long) ((60L * kSecondToMicroSecond) / (unsigned long) _rotationSteps) * (1.0 / _rpm);
}

void StepperControl::SetRotationSteps(const uint32_t rotationSteps)
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
  _currentActionIndex = 0;
  _actionCount = 0;
  delete[] _stepperActions;
  _stepperActions = nullptr;
}

void StepperControl::ThinkAction()
{
  while(_currentActionIndex < _actionCount)
  {
    this->SetRotationSpeed(_stepperActions[_currentActionIndex].Rpm);
    this->SetStepType(_stepperActions[_currentActionIndex].Type);
    this->SetDirection(_stepperActions[_currentActionIndex].Direction);

    delay(_stepperActions[_currentActionIndex].StartDelay);

    const uint32_t stepCount = _stepperActions[_currentActionIndex].Steps;
    this->Step(stepCount);

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

