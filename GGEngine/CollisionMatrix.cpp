#include "stdafx.h"


void CollisionMatrix::Display(Editor&)
{
	ImGui::Text("   0   1   2   3   4   5   6   7");
	unsigned long num = 0;
	for (unsigned i = 8; i > 0; --i)
	{
		std::stringstream str;
		str << i - 1;
		ImGui::Text(str.str().c_str());
		ImGui::SameLine();
		for (unsigned j = i - 1; j > 0; --j)
		{
			ImGui::PushID(num);
			ImGui::Checkbox("", &collisionMatrix[num++]);
			ImGui::SameLine();
			ImGui::PopID();
		}
		ImGui::PushID(num);
		ImGui::Checkbox("", &collisionMatrix[num++]);
		ImGui::PopID();
	}
}