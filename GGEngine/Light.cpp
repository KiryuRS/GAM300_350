#include "stdafx.h"
#include "Light.h"

bool Light_container::HasSpace() const
{
  return mCount < MAX_LIGHTS - 1;
}

bool Light_container::Empty() const
{
  return mCount == 0;
}

unsigned Light_container::find(Light * l) const
{
  for (unsigned i = 0; i < mCount; ++i)
    if (mValue[i] == l)
      return i;

  return static_cast<unsigned>(-1);
}
