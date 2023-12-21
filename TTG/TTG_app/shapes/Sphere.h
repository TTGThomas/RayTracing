#pragma once

#include "../Hittables/Scene.h"
#include "Utils/Math.h"

namespace Shapes
{
	class Sphere
	{
	public:
		void Init(::Sphere sphere);

		void AddToScene(Scene* scene);

		void SetMaterialIndex(int index);
		void SetBsdfIndex(int index);

		::Sphere GetData()
		{
			return m_sphere;
		}
	private:
		::Sphere m_sphere;
	};
}
