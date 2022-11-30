#pragma once

#include "Core/Base.h"
#include "Math/Primitive2D.h"

class GARBAGE_API Rect final : public Primitive2D
{
public:

	Vector2 Size;

	Rect() : Primitive2D(0, 0), Size(0, 0) {}
	Rect(Vector2 position, Vector2 size) : Primitive2D(position), Size(size) {}
	Rect(float x, float y, float sizeX, float sizeY) : Rect(Vector2(x, y), Vector2(sizeX, sizeY)) {}
	Rect(const Rect& other) : Rect(other.Position, other.Size) {}

	virtual bool IsPointInside(const Vector2& point) const noexcept;
	bool IsIntersectsWith(const Rect& other) const noexcept;
	bool IsIntersectsWith(const class Circle& other) const noexcept;

};