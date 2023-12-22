#pragma once

#include "../Scene.h"
#include "Utils/Math.h"

namespace Shapes
{
	class Triangle
	{
	public:
		Triangle() = default;

		void Init(::Triangle triangle);

		void AddToScene(Scene* scene);

		void SetPointA(glm::vec3 p);

		void SetPointB(glm::vec3 p);

		void SetPointC(glm::vec3 p);

		void SetMaterialIndex(int index);
		void SetBsdfIndex(int index);

		void ComputeNormal();
		void FlipNormal();
		void FlipSequence();

		::Triangle GetData()
		{
			return m_tri;
		}
	private:
		::Triangle m_tri{};
	};
}
