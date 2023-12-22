#include "Triangle.h"
#include "Sphere.h"

void Shapes::Sphere::Init(::Sphere sphere)
{
	m_sphere = sphere;
}

void Shapes::Sphere::AddToScene(Scene* scene)
{
	scene->objects.emplace_back(std::make_unique<::Sphere>(m_sphere));
}

void Shapes::Sphere::SetPosition(glm::vec3 pos)
{
	m_sphere.m_position = pos;
}

void Shapes::Sphere::SetRadius(float radius)
{
	m_sphere.m_radius = radius;
}

void Shapes::Sphere::SetMaterialIndex(int index)
{
	m_sphere.m_matIndex = index;
}

void Shapes::Sphere::SetBsdfIndex(int index)
{
	m_sphere.m_bsdfIndex = index;
}
