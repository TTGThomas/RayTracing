#include "Renderer.h"

#include <execution>
#include <thread>

#include <vector>

glm::vec<4, unsigned char> Renderer::ConvertToChar(glm::vec4 color)
{
	unsigned char r = color.r * 255.0f;
	unsigned char g = color.g * 255.0f;
	unsigned char b = color.b * 255.0f;
	unsigned char a = color.a * 255.0f;
	return { r, g, b, a};
}

bool Renderer::OnResize()
{
	if (m_imageData == nullptr || m_width != ImGui::GetWindowWidth() || m_height != ImGui::GetWindowHeight() - 60.0f || m_frameIndex == 0)
	{
        m_frameIndex = 0;
		m_width = ImGui::GetWindowWidth();
		m_height = ImGui::GetWindowHeight() - 60.0f;

		delete[] m_imageData;
		delete[] m_accumulateData;

		unsigned int size = (m_width * m_height) * 4;
		m_imageData = new unsigned char[size];
		m_accumulateData = new glm::vec4[m_width * m_height];

		m_widthIter.resize(m_width);
		m_heightIter.resize(m_height);
        
        for (int i = 0; i < m_width * m_height; i++)
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
#define mt 0
#if mt
	std::for_each(std::execution::par, m_heightIter.begin(), m_heightIter.end(), 
		[this, config](int y)
		{
			for (int x = 0; x < m_width; x++)
			{
				if (m_activateCamera->getMoved() || getFrameIndex() == 1)
				{
					m_activateCamera->reCalculateRayDirections(config, x, y);
				}

				//if (slowRandom)
				//{
				//	preCalculateRandom = TTG::Random::inUnitSphere();
				//}
				//else
				//{
				//	unsigned int seed = (y * m_width + x) * m_frameIndex;
				//	preCalculateRandom = TTG::Random::inUnitSphere(seed);
				//}

				glm::vec4 pixelColor = perPixel(x, y);
				if (m_accumulate)
				{
					m_accumulateData[y * m_width + x] += pixelColor;
					glm::vec4 accumulateColor = m_accumulateData[y * m_width + x] / (float)m_frameIndex;
					pixelColor = accumulateColor;
				}
				pixelColor = glm::clamp(pixelColor, glm::vec4(0.0f), glm::vec4(1.0f));
				glm::vec<4, unsigned char> inColor = convertToChar(pixelColor);
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

HitPayload Renderer::TraceRay(Ray ray)
{
	HitPayload hits[3];
	hits[0] = TraceSpheres(ray);
	hits[1] = TracePlanes(ray);
	hits[2] = TraceTriangles(ray);
	int index = 0;
	for (int i = 0; i < 3; i++)
		if (hits[i].m_hasHit)
			if (hits[i].m_hitDistance < hits[index].m_hitDistance || !hits[index].m_hasHit)
				index = i;
	return hits[index];
}

HitPayload Renderer::TraceSpheres(Ray ray)
{
	int index = -1;
	float hitDistance = std::numeric_limits<float>::max();
	for (int i = 0; i < m_activateScene->objects.size(); i++)
	{
		Sphere& sphere = m_activateScene->objects[i];
		if (!sphere.m_visible)
			continue;

		glm::vec3 sphereOrigin;
		sphereOrigin = sphere.m_position;
		sphereOrigin.y = 0.0f - sphereOrigin.y;

		float radius = sphere.m_radius;

		glm::vec3 origin = ray.m_origin - sphereOrigin;

		// origin formula = (x)^2 + (y)^2 = r^2
        
        // a = ray origin
        // b = ray direction
        // r = radius
        // t = hit distance
		// (bx^2 + by^2 + bz^2)t^2 + (axbx + ayby + azbz)2t + (ax^2 + ay^2 + az^2 - r^2) = 0;

		//float a = glm::dot(ray.Direction, ray.Direction);
		float a = 1.0f;
		float b = TTG::Math::Dot(origin, ray.m_direction);// b / 2
		float c = TTG::Math::Dot(origin, origin) - radius * radius;

		// Quad = b^2 - 4ac

		float discriminant = (b * b) - (a * c);
		// completes the quadratic equation
        float closestT = (-b - TTG::Math::Sqrt(discriminant)) / (a);

		if (closestT < hitDistance && closestT > 0.0f)
		{
			hitDistance = closestT;
			index = i;
		}
        else
        {
            closestT = (-b + TTG::Math::Sqrt(discriminant)) / (a);
            
            if (closestT < hitDistance && closestT > 0.0f)
            {
                hitDistance = closestT;
                index = i;
            }
        }
	}

	if (index < 0)
		return Miss();

	return ClosestHit(ray, hitDistance, index, HitType::SPHERE);
}

HitPayload Renderer::TracePlanes(Ray ray)
{
	int index = -1;
	float hitDistance = std::numeric_limits<float>::max();
	for (int i = 0; i < m_activateScene->objects.size(); i++)
	{
		Plane& plane = m_activateScene->objects[i];
		if (!plane.m_visible)
			continue;

		float denomanator = TTG::Math::Dot(plane.m_normal, ray.m_direction);
		if (denomanator == 0)
			continue;

		float numerator = TTG::Math::Dot(plane.m_normal, plane.m_position * glm::vec3(1.0f, -1.0f, 1.0f)) - TTG::Math::Dot(plane.m_normal, ray.m_origin);

		float closestT = numerator / denomanator;
		if (closestT < hitDistance && closestT > 0.0f)
		{
			hitDistance = closestT;
			index = i;
		}
	}

	if (index < 0)
		return Miss();

	return ClosestHit(ray, hitDistance, index, HitType::PLANE);
}

HitPayload Renderer::TraceTriangles(Ray ray)
{
	int index = -1;
	float hitDistance = FLT_MAX;
	float retU = 0;
	float retV = 0;
	float retW = 0;
	for (int i = 0; i < m_activateScene->objects.size(); i++)
	{
		Triangle& tri = m_activateScene->objects[i];
		if (!tri.m_visible)
			continue;

		glm::vec3 a = tri.m_posA;
		a.y = 0.0f - a.y;

		glm::vec3 b = tri.m_posB;
		b.y = 0.0f - b.y;

		glm::vec3 c = tri.m_posC;
		c.y = 0.0f - c.y;

		glm::vec3 edgeAB = b - a;
		glm::vec3 edgeAC = c - a;
		glm::vec3 normalVector = TTG::Math::Cross(edgeAB, edgeAC);
		glm::vec3 ao = ray.m_origin- a;
		glm::vec3 dao = TTG::Math::Cross(ao, ray.m_direction);

		float determinant = -TTG::Math::Dot(ray.m_direction, normalVector);
		float invDet = 1 / determinant;

		float dst = TTG::Math::Dot(ao, normalVector) * invDet;
		float u = TTG::Math::Dot(edgeAC, dao) * invDet;
		float v = -TTG::Math::Dot(edgeAB, dao) * invDet;
		float w = 1 - u - v;

		bool hasHit = determinant >= 0.0001f && dst >= 0 && u >= 0 && v >= 0 && w >= 0;

		if (!hasHit)
			continue;

		if (dst < hitDistance)
		{
			hitDistance = dst;
			retU = u;
			retV = v;
			retW = w;
			index = i;
		}
	}

	if (index == -1)
		return Miss();

	Triangle& tri = m_activateScene->objects[index];

	HitPayload payload;
	payload.m_hasHit = true;
	payload.m_hitPos = ray.m_origin + (ray.m_direction * hitDistance);
	payload.m_hitNormal = TTG::Math::Normalize(tri.m_na * retW + tri.m_nb * retU + tri.m_nc * retV);
	payload.m_hitDistance = hitDistance;
	payload.m_hitType = HitType::TRIANGLE;
	payload.m_hitIndex = index;
	return payload;
}

HitPayload Renderer::ClosestHit(Ray ray, float t, int index, HitType hitType)
{
	HitPayload payload;
	payload.m_hitType = hitType;
	payload.m_hitDistance = t;
	payload.m_hasHit = true;
	payload.m_hitIndex = index;

	switch (hitType)
	{
	case HitType::SPHERE:
        {
            glm::vec3 sphereOrigin = m_activateScene->objects[index].m_position;
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
            payload.m_hitNormal = -m_activateScene->objects[index].m_normal;
            payload.m_hitPos = ray.m_origin + (ray.m_direction * t);
        }
		break;
	default:
        {
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
	//return glm::vec4((float)x / (float)m_width, (float)y / (float)m_height, 0.0f, 1.0f);

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
		Material mat;
		if (hit.m_hitType == HitType::SPHERE)
			mat = m_activateScene->mat[m_activateScene->objects[hit.m_hitIndex].m_matIndex];
		else if (hit.m_hitType == HitType::PLANE)
			mat = m_activateScene->mat[m_activateScene->objects[hit.m_hitIndex].m_matIndex];
		else if (hit.m_hitType == HitType::TRIANGLE)
			mat = m_activateScene->mat[m_activateScene->objects[hit.m_hitIndex].m_matIndex];
		
		glm::vec3 color = glm::vec3(0.0f);
		//return mat.albedo;
		color = mat.m_albedo;

		glm::vec3 emission = mat.GetEmission();
		if (emission.x + emission.y + emission.z > 0.0f)
		{
			light += emission * contribution;
			break;
		}
		contribution *= color;

        int bsdfIndex = -1;

		if (hit.m_hitType == HitType::SPHERE)
        {
            bsdfIndex = m_activateScene->objects[hit.m_hitIndex].m_bsdfIndex;
        }
		else if (hit.m_hitType == HitType::PLANE)
        {
            bsdfIndex = m_activateScene->objects[hit.m_hitIndex].m_bsdfIndex;
        }
		else if (hit.m_hitType == HitType::TRIANGLE)
        {
            bsdfIndex = m_activateScene->objects[hit.m_hitIndex].m_bsdfIndex;
        }

		BsdfPayload t;
		t.m_mat = mat;
		t.m_payload = hit;

		m_activateScene->bsdf[bsdfIndex]->ProcessHit(t, &ray);
	}
	return glm::vec4(light, 1.0f);
}
