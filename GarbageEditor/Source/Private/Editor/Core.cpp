#include "Editor/Core.h"
#include "GarbageEditorReflection.h"

namespace GarbageEditor
{

	void Init()
	{
		GarbageEngine2D::Init();
		GarbageEditorReflection::Register();
	}

}