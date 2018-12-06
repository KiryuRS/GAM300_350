#include "stdafx.h"
#include <queue>
#include "Animation.h"

Animation::Animation(const sAnimation& anim)
: name{ anim.name },
	duration{ anim.duration },
	ticks_per_sec{ anim.ticks_per_sec },
	channels{ std::map<std::string, Channel>() }
{
	for (auto& item : anim.channels)
	{
		channels[item.name].name = item.name;
		for (unsigned i = 0; i < item.pk_timestamp.size(); ++i)
		{
			channels[item.name].positionKeys.push_back(VectorKey(item.pk_timestamp[i], item.pk_value[i]));
		}
		for (unsigned i = 0; i < item.rk_timestamp.size(); ++i)
		{
			channels[item.name].rotationKeys.push_back(QuatKey(item.rk_timestamp[i], item.rk_value[i]));
		}
		for (unsigned i = 0; i < item.sk_timestamp.size(); ++i)
		{
			channels[item.name].scalingKeys.push_back(VectorKey(item.sk_timestamp[i], item.sk_value[i]));
		}
	}
}

Animation::~Animation()
{
}


Node::~Node()
{
}
