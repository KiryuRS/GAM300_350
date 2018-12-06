#pragma once
#include "StackWalker.h"
#include <Windows.h>

class StackWalkerWrapper final : public StackWalker
{
	std::string cs_buffer;

protected:
	virtual void OnOutput(LPCSTR szText)
	{
		cs_buffer += szText;
		StackWalker::OnOutput(szText);
	}

public:
	std::string GetSWBuffer() const { return cs_buffer; }
	void ClearSWBuffer() { cs_buffer.clear(); }
};