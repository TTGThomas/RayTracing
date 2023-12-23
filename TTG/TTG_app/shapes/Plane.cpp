#include "Plane.h"

void Shapes::Plane::Init(const ::Plane& plane)
{
	m_plane = plane;
}

void Shapes::Plane::AddToScene(Scene* scene)
{
	scene->objects.emplace_back(std::make_unique<::Plane>(m_plane));
}

void Shapes::Plane::SetPosition(const glm::vec3& pos)
{
	m_plane.m_position = pos;
}

void Shapes::Plane::SetNormal(const glm::vec3& normal)
{
	m_plane.m_normal = normal;
}

void Shapes::Plane::SetMaterialIndex(int index)
{
	m_plane.m_matIndex = index;
}

void Shapes::Plane::SetBsdfIndex(int index)
{
	m_plane.m_bsdfIndex = index;
}
