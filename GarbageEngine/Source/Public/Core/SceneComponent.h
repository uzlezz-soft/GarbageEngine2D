#pragma once

#include "Core/Base.h"
#include "Core/Registry.h"
#include "Math/Vector3.h"
#include "SceneComponent.generated.h"

GCLASS();
class GARBAGE_API SceneComponent : public ObjectBase
{
	GENERATED_BODY()

public:

	GPROPERTY();
	Vector3 Location;
	GPROPERTY();
	Vector3 Rotation;
	GPROPERTY();
	Vector3 Scale;

};

GCLASS();
class GARBAGE_API Entity : public SceneComponent
{
	GENERATED_BODY()

public:

	GPROPERTY();
	std::u8string Name;
	GPROPERTY();
	SceneComponent* Parent;

	virtual void Start() {}

	virtual void Tick(float deltaTime) {}

};