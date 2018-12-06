#pragma once
#include "GuiWindow.h"

class CollisionMatrix : public GuiWindow
{
	std::array<bool, 36> collisionMatrix;
public:
	static constexpr char windowName[] = "Collision_Matrix";
	CollisionMatrix(Editor& edit) : GuiWindow(edit, windowName),
		collisionMatrix{false,false, false, false, false, false, false, 
		false, false, false, false, false, false, false, false, false, false, 
		false, false, false, false, false, false, false, false, false, false, 
		false, false, false, false, false, false, false, false, false}
	{
		for (unsigned i = 0; i < collisionMatrix.size(); ++i)
		{
			std::stringstream stream;
			stream << "checkbox" << i;
			AddSerializable(stream.str(), collisionMatrix[i]);
		}
	}
	void Display(Editor&) override;
};