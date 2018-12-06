#pragma once
#include "Input.h"

class InputManager final : public CoreSystem
{
	std::stack<Input*> stack_inputs;
	Input default_input;
	Window& window;

public:
	InputManager(Window& win);
	~InputManager();
	void Push(Input& input);
	void Pop(bool reset = false);
	void SetInput(Input& input);
	Input& GetCurrentInput();
	void RemoveInput(bool reset = false);
	void Update();
	bool IsCurrentInput(Input& input);
	void ResetKeys();
};