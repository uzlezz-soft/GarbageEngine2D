#pragma once

#include "Core/Base.h"
#include "Math/Vector2.h"

class GARBAGE_API Primitive2D
{
public:

	Vector2 Position;

	Primitive2D() : Position(0, 0) {}
	Primitive2D(float x, float y) : Position(x, y) {}
	Primitive2D(Vector2 position) : Position(position) {}

	virtual bool IsPointInside(const Vector2& point) const noexcept = 0;

};