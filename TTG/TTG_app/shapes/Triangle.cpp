#include "Triangle.h"

void Shapes::Triangle::Init(const ::Triangle& triangle)
{
	m_tri = triangle;
}

void Shapes::Triangle::AddToScene(Scene* scene)
{
	scene->objects.emplace_back(std::make_unique<::Triangle>(m_tri));
}

void Shapes::Triangle::SetPointA(const glm::vec3& p)
{
	m_tri.m_posA = p;
}

void Shapes::Triangle::SetPointB(const glm::vec3& p)
{
	m_tri.m_posB = p;
}

void Shapes::Triangle::SetPointC(const glm::vec3& p)
{
	m_tri.m_posC = p;
}

void Shapes::Triangle::SetMaterialIndex(int index)
{
	m_tri.m_matIndex = index;
}

void Shapes::Triangle::SetBsdfIndex(int index)
{
	m_tri.m_bsdfIndex = index;
}

void Shapes::Triangle::ComputeNormal()
{
	glm::vec3 ab = m_tri.m_posB - m_tri.m_posA;
	glm::vec3 ac = m_tri.m_posC - m_tri.m_posA;
	glm::vec3 normal = -TTG::Math::Normalize(TTG::Math::Cross(ab, ac));
	//normal.y = -normal.y;
	m_tri.m_na = normal;
	m_tri.m_nb = normal;
	m_tri.m_nc = normal;
}

void Shapes::Triangle::FlipNormal()
{
	glm::vec3 normal = -m_tri.m_na;
	m_tri.m_na = normal;
	m_tri.m_nb = normal;
	m_tri.m_nc = normal;
}

void Shapes::Triangle::FlipSequence()
{
	glm::vec3 temp = m_tri.m_posB;
	m_tri.m_posB = m_tri.m_posC;
	m_tri.m_posC = temp;
}
