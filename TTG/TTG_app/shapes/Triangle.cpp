#include "Triangle.h"

void Shapes::Triangle::init(::Triangle triangle)
{
	m_tri = triangle;
}

void Shapes::Triangle::addToScene(Scene* scene)
{
	scene->triangles.emplace_back(m_tri);
}

void Shapes::Triangle::setPointA(glm::vec3 p)
{
	m_tri.PosA = p;
}

void Shapes::Triangle::setPointA(float x, float y, float z)
{
	setPointA(glm::vec3(x, y, z));
}

void Shapes::Triangle::setPointB(glm::vec3 p)
{
	m_tri.PosB = p;
}

void Shapes::Triangle::setPointB(float x, float y, float z)
{
	setPointB(glm::vec3(x, y, z));
}

void Shapes::Triangle::setPointC(glm::vec3 p)
{
	m_tri.PosC = p;
}

void Shapes::Triangle::setPointC(float x, float y, float z)
{
	setPointC(glm::vec3(x, y, z));
}

void Shapes::Triangle::setMaterialIndex(int index)
{
	m_tri.matIndex = index;
}

void Shapes::Triangle::setBsdfIndex(int index)
{
	m_tri.bsdfIndex = index;
}

void Shapes::Triangle::computeNormal()
{
	glm::vec3 normal = -TTG::Math::normalize(TTG::Math::cross(m_tri.PosB - m_tri.PosA, m_tri.PosC - m_tri.PosA));
	normal.y = -normal.y;
	m_tri.na = normal;
	m_tri.nb = normal;
	m_tri.nc = normal;
}

void Shapes::Triangle::flipNormal()
{
	glm::vec3 normal = -m_tri.na;
	m_tri.na = normal;
	m_tri.nb = normal;
	m_tri.nc = normal;
}

void Shapes::Triangle::flipSequence()
{
	glm::vec3 temp = m_tri.PosB;
	m_tri.PosB = m_tri.PosC;
	m_tri.PosC = temp;
}
