#include "Math/Circle.h"
#include "Math/Rect.h"

bool Circle::IsPointInside(const Vector2& point) const noexcept
{
	return point.X > Position.X - Radius
		&& point.Y > Position.Y - Radius
		&& point.X < Position.X + Radius
		&& point.Y < Position.Y + Radius;
}

bool Circle::IsIntersectsWith(const Rect& other) const noexcept
{
	return other.IsIntersectsWith(*this);
}

bool Circle::IsIntersectsWith(const Circle& other) const noexcept
{
	return Position.Distance(other.Position) < Radius + other.Radius;
}
