#include "Math/Rect.h"
#include "Math/Circle.h"

bool Rect::IsPointInside(const Vector2& point) const noexcept
{
    return point.X > Position.X && point.X < (Position.X + Size.X)
        && point.Y > Position.Y && point.Y < (Position.Y + Size.Y);
}

bool Rect::IsIntersectsWith(const Rect& other) const noexcept
{
    return Position.X < other.Position.X + other.Size.X
        && Position.X + Size.X > other.Position.X
        && Position.Y < other.Position.Y + other.Size.Y
        && Position.Y + Size.Y > other.Position.Y;
}

bool Rect::IsIntersectsWith(const Circle& other) const noexcept
{
    return other.Position.X + other.Radius > Position.X
        && other.Position.Y + other.Radius > Position.Y
        && other.Position.X - other.Radius < Position.X + Size.X
        && other.Position.Y - other.Radius < Position.Y + Size.Y;
}
