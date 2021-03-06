#pragma once

#include "Object.hpp"
#include <map>

namespace FishEngine
{
	std::vector<Object*> CloneObjects(std::vector<Object*> objects, std::map<Object*, Object*>& memo);

	// memo is a mapping from original object to cloned
	Object* CloneObject(Object* obj, std::map<Object*, Object*>& memo);

	inline Object* CloneObject(Object* obj)
	{
		std::map<Object*, Object*> memo;
		return CloneObject(obj, memo);
	}
}