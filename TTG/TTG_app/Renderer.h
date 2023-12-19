#pragma once

#include <ImGuiOpenGL.h>
#include <Graphics/Texture.h>
#include <Utils/Random.h>
#include "Utils/Math.h"
#include "Objects/Camera.h"
#include <glm/glm.hpp>
#include <glm/gtx/fast_square_root.hpp>

#include "Ray.h"
#include "Scene.h"
#include "HitPayload.h"

#include <algorithm>
#include <execution>

class Renderer
{
public:
	Renderer(Scene* scene, TTG::Camera* camera)
		: m_activateScene(scene), m_activateCamera(camera) {}

	bool onResize();
	void render(TTG::Config& config);

	void resetFrameIndex() { m_frameIndex = 0; }

	HitPayload traceRay(Ray ray);
	HitPayload traceSpheres(Ray ray);
	HitPayload tracePlanes(Ray ray);
	HitPayload traceTriangles(Ray ray);
	HitPayload closestHit(Ray ray, float t, int index, HitType hitType);
	HitPayload miss();

	glm::vec4 perPixel(int x, int y);

	glm::vec<4, unsigned char> convertToChar(glm::vec4 color);

	TTG::Texture& getFinalImage() { return m_finalImage; }
	unsigned char* getImageData() { return m_imageData; }

	unsigned int getWidth() { return m_width; }
	unsigned int getHeight() { return m_height; }

	int getFrameIndex() { return m_frameIndex; }
	bool* getAccumulate() { return &m_accumulate; };
	bool* getSlowRandom() { return &slowRandom; }
    bool* getSkyLightSwitch() { return &skyLight; }
private:
	int m_frameIndex = 0;
	glm::vec3 preCalculateRandom = glm::vec3(0.0f);

	std::vector<int> m_widthIter;
	std::vector<int> m_heightIter;

	unsigned char* m_imageData = nullptr;
	glm::vec4* m_accumulateData = nullptr;
	TTG::Texture m_finalImage;
	unsigned int m_width = 0, m_height = 0;

	Scene* m_activateScene = nullptr;
	TTG::Camera* m_activateCamera = nullptr;

	bool m_accumulate = false;
	bool slowRandom = true;
	bool skyLight = false;
};

