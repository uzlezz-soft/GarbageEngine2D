#pragma once

#include "Core/Base.h"
#include "Math/Primitive2D.h"

class GARBAGE_API Circle final : public Primitive2D
{
public:

	float Radius = 0.0f;

	Circle() : Primitive2D(), Radius(0.0f) {}
	Circle(Vector2 position, float radius) : Primitive2D(position), Radius(radius) {}
	Circle(float x, float y, float radius) : Circle(Vector2(x, y), radius) {}
	Circle(const Circle& other) : Circle(other.Position, other.Radius) {}

	virtual bool IsPointInside(const Vector2& point) const noexcept;
	bool IsIntersectsWith(const class Rect& other) const noexcept;
	bool IsIntersectsWith(const Circle& other) const noexcept;

};