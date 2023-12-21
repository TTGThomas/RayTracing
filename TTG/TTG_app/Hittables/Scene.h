#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "../Materials/Bsdf.h"

#include "Materials.h"

#include "HittableBase.h"

#include "HitTriangle.h"
#include "HitSphere.h"
#include "HitPlane.h"

//struct Sphere : public ShapeBase
//{
//	glm::vec3 Position = glm::vec3(0.0f);
//	float radius = 1.0f;
//};
//
//struct Plane : public ShapeBase
//{
//	glm::vec3 Position = glm::vec3(0.0f);
//	glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
//};
//
//struct Triangle : public ShapeBase
//{
//	glm::vec3 PosA = glm::vec3(0.0f);
//	glm::vec3 PosB = glm::vec3(0.0f);
//	glm::vec3 PosC = glm::vec3(0.0f);
//
//	glm::vec3 na = glm::vec3(0.0f);
//	glm::vec3 nb = glm::vec3(0.0f);
//	glm::vec3 nc = glm::vec3(0.0f);
//};

struct Scene
{
	std::vector<Hittable> objects;
	std::vector<Material> mat;
	std::vector<std::unique_ptr<Bsdf>> bsdf;
};
