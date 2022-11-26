#pragma once

#include "Core/SceneComponent.h"
#include "SoundListener.generated.h"

GCLASS();
class SoundListener : public SceneComponent
{
	GENERATED_BODY()

public:

	GPROPERTY();
	bool Enabled = true;

};