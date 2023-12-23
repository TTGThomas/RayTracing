#pragma once

#include "../Scene.h"
#include "Utils/Math.h"

#include <glm/glm.hpp>

namespace Shapes
{
	class Sphere
	{
	public:
		void Init(const ::Sphere& sphere);

		void AddToScene(Scene* scene);

		void SetPosition(const glm::vec3& pos);
		void SetRadius(float radius);

		void SetMaterialIndex(int index);
		void SetBsdfIndex(int index);

		::Sphere GetData()
		{
			return m_sphere;
		}
	private:
		::Sphere m_sphere{};
	};
}
