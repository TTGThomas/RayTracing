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

	bool OnResize();
	void Render(TTG::Config& config);

	void ResetFrameIndex() { m_frameIndex = 0; }

	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float t, int index);
	HitPayload Miss();

	glm::vec4 PerPixel(int x, int y);

	glm::vec<4, unsigned char> ConvertToChar(const glm::vec4& color);

	TTG::Texture& GetFinalImage() { return m_finalImage; }
	unsigned char* GetImageData() { return m_imageData; }

	unsigned int GetWidth() { return m_width; }
	unsigned int GetHeight() { return m_height; }

	int GetFrameIndex() { return m_frameIndex; }
	bool* GetAccumulate() { return &m_accumulate; };
	bool* GetSlowRandom() { return &m_slowRandom; }
    bool* GetSkyLightSwitch() { return &m_skyLight; }
    float* GetDistBlur() { return &m_distBlur; }
    bool* GetDistBlurSwitch() { return &m_useDistBlur; }
private:
	int m_frameIndex = 0;

	std::vector<int> m_widthIter;
	std::vector<int> m_heightIter;

	unsigned char* m_imageData = nullptr;
	glm::vec4* m_accumulateData = nullptr;
	TTG::Texture m_finalImage;
	unsigned int m_width = 0, m_height = 0;

	Scene* m_activateScene = nullptr;
	TTG::Camera* m_activateCamera = nullptr;

	bool m_accumulate = false;
	bool m_slowRandom = true;
	bool m_skyLight = false;
    
    float m_distBlur = 17.0f;
    bool m_useDistBlur = false;
};

