#include "Renderer.h"

#include <execution>
#include <thread>

#include <vector>

glm::vec<4, unsigned char> Renderer::ConvertToChar(const glm::vec4& color)
{
	unsigned char r = (unsigned char)(color.r * 255.0f);
	unsigned char g = (unsigned char)(color.g * 255.0f);
	unsigned char b = (unsigned char)(color.b * 255.0f);
	unsigned char a = (unsigned char)(color.a * 255.0f);
	return { r, g, b, a};
}

bool Renderer::OnResize()
{
	if (m_imageData == nullptr || m_width != ImGui::GetWindowWidth() || m_height != ImGui::GetWindowHeight() - 60.0f || m_frameIndex == 0)
	{
        m_frameIndex = 0;
		m_width = (unsigned int)ImGui::GetWindowWidth();
		m_height = (unsigned int)(ImGui::GetWindowHeight() - 60.0f);

		delete[] m_imageData;
		delete[] m_accumulateData;

		unsigned int size = (m_width * m_height) * 4;
		m_imageData = new unsigned char[size];
		m_accumulateData = new glm::vec4[m_width * m_height];

		m_widthIter.resize(m_width);
		m_heightIter.resize(m_height);
        
        for (unsigned int i = 0; i < m_width * m_height; i++)
            m_accumulateData[i] = glm::vec4(0.0f);

		for (int i = 0; i < m_widthIter.size(); i++)
			m_widthIter[i] = i;

		for (int i = 0; i < m_heightIter.size(); i++)
			m_heightIter[i] = i;

		return true;
	}

	return false;
}

void Renderer::Render(TTG::Config& config)
{
	m_frameIndex++;
#define mt 1
#if mt
	std::for_each(std::execution::par, m_heightIter.begin(), m_heightIter.end(), 
		[this, config](unsigned int y)
		{
			for (unsigned int x = 0; x < m_width; x++)
			{
				if (m_activateCamera->GetMoved() || GetFrameIndex() == 1)
				{
					m_activateCamera->ReCalculateRayDirections(config, x, y);
				}

				glm::vec4 pixelColor = PerPixel(x, y);
				if (m_accumulate)
				{
					m_accumulateData[y * m_width + x] += pixelColor;
					glm::vec4 accumulateColor = m_accumulateData[y * m_width + x] / (float)m_frameIndex;
					pixelColor = accumulateColor;
				}
				pixelColor = glm::clamp(pixelColor, glm::vec4(0.0f), glm::vec4(1.0f));
				glm::vec<4, unsigned char> inColor = ConvertToChar(pixelColor);
				// map to image data

				m_imageData[(y * m_width + x) * 4 + 0] = inColor.r;
				m_imageData[(y * m_width + x) * 4 + 1] = inColor.g;
				m_imageData[(y * m_width + x) * 4 + 2] = inColor.b;
				m_imageData[(y * m_width + x) * 4 + 3] = inColor.a;
			}
		});
#else
    auto xTick = [this, config](int y) {
        for (unsigned int x = 0; x < m_width; x++)
        {
            if (m_activateCamera->GetMoved() || GetFrameIndex() == 1)
            {
                m_activateCamera->ReCalculateRayDirections(config, x, y);
            }
            
            glm::vec4 pixelColor = PerPixel(x, y);
            if (m_accumulate)
            {
                m_accumulateData[y * m_width + x] += pixelColor;
                glm::vec4 accumulateColor = m_accumulateData[y * m_width + x] / (float)m_frameIndex;
                pixelColor = accumulateColor;
            }
            pixelColor = glm::clamp(pixelColor, glm::vec4(0.0f), glm::vec4(1.0f));
            glm::vec<4, unsigned char> inColor = ConvertToChar(pixelColor);
            // map to image data

            m_imageData[(y * m_width + x) * 4 + 0] = inColor.r;
            m_imageData[(y * m_width + x) * 4 + 1] = inColor.g;
            m_imageData[(y * m_width + x) * 4 + 2] = inColor.b;
            m_imageData[(y * m_width + x) * 4 + 3] = inColor.a;
        }
    };
    std::vector<std::thread> threads;
	for (unsigned int y = 0; y < m_height; y++)
	{
        threads.push_back(std::thread(xTick, y));
	}
    for (unsigned int y = 0; y < m_height; y++)
    {
        threads[y].join();
    }
#endif
}

HitPayload Renderer::TraceRay(const Ray& ray)
{
	float minDist = FLT_MAX;
	int index = -1;
	for (int i = 0; i < m_activateScene->objects.size(); i++)
	{
		Hittable* object = m_activateScene->objects[i].get();
		float dist = object->Hit(ray);
		if (dist < minDist && dist > 0.0f)
		{
			index = i;
			minDist = dist;
		}
	}
	if (index == -1)
		return Miss();
	return ClosestHit(ray, minDist, index);
}

HitPayload Renderer::ClosestHit(const Ray& ray, float t, int index)
{
	HitType hitType = m_activateScene->objects[index]->m_type;

	HitPayload payload;
	payload.m_hitType = hitType;
	payload.m_hitDistance = t;
	payload.m_hasHit = true;
	payload.m_hitIndex = index;

	switch (hitType)
	{
	case HitType::SPHERE:
	{
		::Sphere* sphere = reinterpret_cast<::Sphere*>(m_activateScene->objects[index].get());
		//::Sphere* sphere = dynamic_cast<::Sphere*>(m_activateScene->objects[index].get());

		glm::vec3 sphereOrigin = sphere->m_position;
		sphereOrigin.y = 0.0f - sphereOrigin.y;
		glm::vec3 origin = ray.m_origin - sphereOrigin;

		glm::vec3 hitPoint = origin + (ray.m_direction * t);
		glm::vec3 normal = TTG::Math::Normalize(hitPoint);
		hitPoint += sphereOrigin;

		payload.m_hitNormal = normal;
		payload.m_hitPos = hitPoint;
	}
		break;
	case HitType::PLANE:
	{
		::Plane* plane = reinterpret_cast<::Plane*>(m_activateScene->objects[index].get());
		//::Plane* plane = dynamic_cast<::Plane*>(m_activateScene->objects[index].get());

		payload.m_hitNormal = -plane->m_normal;
		payload.m_hitPos = ray.m_origin + (ray.m_direction * t);
	}
		break;
	case HitType::TRIANGLE:
	{
		::Triangle* tri = reinterpret_cast<::Triangle*>(m_activateScene->objects[index].get());
		//::Triangle* tri = dynamic_cast<::Triangle*>(m_activateScene->objects[index].get());

		payload.m_hitPos = ray.m_origin + (ray.m_direction * t);
		payload.m_hitNormal = TTG::Math::Normalize(tri->m_na * tri->m_w + tri->m_nb * tri->m_u + tri->m_nc * tri->m_v);
	}
		break;
	}
	return payload;
}

HitPayload Renderer::Miss()
{
	HitPayload payload;
	payload.m_hasHit = false;
	payload.m_hitDistance = -FLT_MAX;
	payload.m_hitIndex = -1;
	return payload;
}

glm::vec4 Renderer::PerPixel(int x, int y)
{
	Ray ray;
	ray.m_origin = m_activateCamera->m_position;
	ray.m_direction = m_activateCamera->m_rayDirections[y * m_width + x];

    if (m_useDistBlur)
    {
        glm::vec3 point = ray.m_origin + ray.m_direction * m_distBlur;
        ray.m_origin += TTG::Random::InUnitSphere() * 2.0f;
        ray.m_direction = point - ray.m_origin;
        ray.m_direction = TTG::Math::Normalize(ray.m_direction);
    }

	//unsigned int seed = (y * m_width + x) * m_frameIndex;

	glm::vec3 light(0.0f);
	glm::vec3 contribution(1.0f);

	const int bounces = 8;
	for (int i = 0; i < bounces; i++)
	{
		//seed += i;
		HitPayload hit = TraceRay(ray);
		if (!hit.m_hasHit)
		{
			glm::vec3 skyColor = { 0.6f, 0.7f, 0.9f };
			if (m_skyLight)
				light += skyColor * contribution;
			break;
		}
		Hittable* object = m_activateScene->objects[hit.m_hitIndex].get();
		Material& mat = m_activateScene->mat[object->m_matIndex];

#define ALBEDOSHADER 0
#if ALBEDOSHADER
		return mat.m_albedo;
#endif

		glm::vec3 emission = mat.GetEmission();
		if (emission.x + emission.y + emission.z > 0.0f)
		{
			light += emission * contribution;
			break;
		}
		
		glm::vec3 color = mat.m_albedo;
		contribution *= color;

		BsdfPayload t;
		t.m_mat = mat;
		t.m_payload = hit;
		int bsdfIndex = object->m_bsdfIndex;

		m_activateScene->bsdf[bsdfIndex]->ProcessHit(t, &ray);
	}
	return glm::vec4(light, 1.0f);
}
