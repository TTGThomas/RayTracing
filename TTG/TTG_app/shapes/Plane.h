#pragma once

#include "../Scene.h"
#include "Utils/Math.h"

#include <glm/glm.hpp>

namespace Shapes
{
	class Plane
	{
	public:
		void Init(::Plane plane);

		void AddToScene(Scene* scene);

		void SetPosition(glm::vec3 pos);
		void SetNormal(glm::vec3 normal);

		void SetMaterialIndex(int index);
		void SetBsdfIndex(int index);

		::Plane GetData()
		{
			return m_plane;
		}
	private:
		::Plane m_plane{};
	};
}
