#include "stdafx.h"

InputManager::InputManager(Window & win)
	: window{ win }, default_input{ }
{
	CoreSystem::SetName("Input Manager");
	Push(default_input);
}

InputManager::~InputManager()
{
	while (!stack_inputs.empty())
	{
		stack_inputs.top() = nullptr;
		stack_inputs.pop();
	}
}

void InputManager::Push(Input & input)
{
	// Pushing a new input into the stack
	stack_inputs.emplace(nullptr);
	stack_inputs.top() = &input;
}

void InputManager::Pop(bool reset)
{
  UNREFERENCED_PARAMETER(reset);

	if (!stack_inputs.empty())
	{
		if (!stack_inputs.top()->mouse.display)
			ShowCursor(true);
		stack_inputs.top()->ResetKeys();
		stack_inputs.top() = nullptr;
		stack_inputs.pop();
	}
	else
	{
		CONSOLE_WARNING("There's nothing to pop in InputManager!");
		return;
	}
}

void InputManager::SetInput(Input & input)
{
	if (!stack_inputs.empty())
	{
		stack_inputs.top() = &input;
		return;
	}

	// Push a new one if the stack is empty
	Push(input);
}

Input & InputManager::GetCurrentInput()
{
	return *stack_inputs.top();
}

void InputManager::RemoveInput(bool reset)
{
  UNREFERENCED_PARAMETER(reset);

	if (!stack_inputs.empty())
	{
		if (!stack_inputs.top()->mouse.display)
			ActualShowCursor(true);
		stack_inputs.top()->ResetKeys();
		stack_inputs.top() = &default_input;
		return;
	}

	Push(default_input);
}

void InputManager::Update()
{
	if (!stack_inputs.empty())
	{
		stack_inputs.top()->Update(window);
	}
}

bool InputManager::IsCurrentInput(Input & input)
{
	if (stack_inputs.empty())
		return false;

	return stack_inputs.top() == &input;
}

void InputManager::ResetKeys()
{
	if (stack_inputs.empty())
		stack_inputs.top()->ResetKeys();
}
