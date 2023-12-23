#pragma once

#include "../Scene.h"
#include "Utils/Math.h"

#include <glm/glm.hpp>

namespace Shapes
{
	class Plane
	{
	public:
		void Init(const ::Plane& plane);

		void AddToScene(Scene* scene);

		void SetPosition(const glm::vec3& pos);
		void SetNormal(const glm::vec3& normal);

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
