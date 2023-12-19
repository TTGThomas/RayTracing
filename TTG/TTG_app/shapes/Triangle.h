#pragma once

#include "../Scene.h"
#include "Utils/Math.h"

namespace Shapes
{
	class Triangle
	{
	public:
		void init(::Triangle triangle);

		void addToScene(Scene* scene);

		void setPointA(glm::vec3 p);
		void setPointA(float x, float y, float z);

		void setPointB(glm::vec3 p);
		void setPointB(float x, float y, float z);

		void setPointC(glm::vec3 p);
		void setPointC(float x, float y, float z);

		void setMaterialIndex(int index);
		void setBsdfIndex(int index);

		void computeNormal();
		void flipNormal();
		void flipSequence();

		::Triangle getData()
		{
			return m_tri;
		}
	private:
		::Triangle m_tri;
	};
}