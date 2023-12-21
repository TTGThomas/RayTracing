#pragma once

#include "../Hittables/Scene.h"
#include "Utils/Math.h"

namespace Shapes
{
	class Triangle
	{
	public:
		void Init(::Triangle triangle);

		void AddToScene(Scene* scene);

		void SetPointA(glm::vec3 p);
		void SetPointA(float x, float y, float z);

		void SetPointB(glm::vec3 p);
		void SetPointB(float x, float y, float z);

		void SetPointC(glm::vec3 p);
		void SetPointC(float x, float y, float z);

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
		::Triangle m_tri;
	};
}
