#include "Renderer.h"

#include <execution>
#include <thread>

#include <vector>

glm::vec<4, unsigned char> Renderer::convertToChar(glm::vec4 color)
{
	unsigned char r = color.r * 255.0f;
	unsigned char g = color.g * 255.0f;
	unsigned char b = color.b * 255.0f;
	unsigned char a = color.a * 255.0f;
	return { r, g, b, a};
}


bool Renderer::onResize()
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

void Renderer::render(TTG::Config& config)
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
            if (m_activateCamera->getMoved() || getFrameIndex() == 1)
            {
                m_activateCamera->reCalculateRayDirections(config, x, y);
            }

            //if (slowRandom)
            //{
            //    preCalculateRandom = TTG::Random::inUnitSphere();
            //}
            //else
            //{
            //    unsigned int seed = (y * m_width + x) * m_frameIndex;
            //    preCalculateRandom = TTG::Random::inUnitSphere(seed);
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

HitPayload Renderer::traceRay(Ray ray)
{
	HitPayload hits[3];
	hits[0] = traceSpheres(ray);
	hits[1] = tracePlanes(ray);
	hits[2] = traceTriangles(ray);
	int index = 0;
	for (int i = 0; i < 3; i++)
		if (hits[i].hasHit)
			if (hits[i].hitDistance < hits[index].hitDistance || !hits[index].hasHit)
				index = i;
	return hits[index];
}

HitPayload Renderer::traceSpheres(Ray ray)
{
	int index = -1;
	float hitDistance = std::numeric_limits<float>::max();
	for (int i = 0; i < m_activateScene->spheres.size(); i++)
	{
		Sphere& sphere = m_activateScene->spheres[i];
		if (!sphere.visible)
			continue;

		glm::vec3 sphereOrigin;
		sphereOrigin = sphere.Position;
		sphereOrigin.y = 0.0f - sphereOrigin.y;

		float radius = sphere.radius;

		glm::vec3 origin = ray.Origin - sphereOrigin;

		// origin formula = (x)^2 + (y)^2 = r^2
        
        // a = ray origin
        // b = ray direction
        // r = radius
        // t = hit distance
		// (bx^2 + by^2 + bz^2)t^2 + (axbx + ayby + azbz)2t + (ax^2 + ay^2 + az^2 - r^2) = 0;

		//float a = glm::dot(ray.Direction, ray.Direction);
		float a = 1.0f;
		float b = TTG::Math::dot(origin, ray.Direction);// b / 2
		float c = TTG::Math::dot(origin, origin) - radius * radius;

		// Quad = b^2 - 4ac

		float discriminant = (b * b) - (a * c);
		// completes the quadratic equation
        float closestT = (-b - TTG::Math::sqrt(discriminant)) / (a);

		if (closestT < hitDistance && closestT > 0.0f)
		{
			hitDistance = closestT;
			index = i;
		}
        else
        {
            closestT = (-b + TTG::Math::sqrt(discriminant)) / (a);
            
            if (closestT < hitDistance && closestT > 0.0f)
            {
                hitDistance = closestT;
                index = i;
            }
        }
	}

	if (index < 0)
		return miss();

	return closestHit(ray, hitDistance, index, HitType::SPHERE);
}

HitPayload Renderer::tracePlanes(Ray ray)
{
	int index = -1;
	float hitDistance = std::numeric_limits<float>::max();
	for (int i = 0; i < m_activateScene->planes.size(); i++)
	{
		Plane& plane = m_activateScene->planes[i];
		if (!plane.visible)
			continue;

		float denomanator = TTG::Math::dot(plane.normal, ray.Direction);
		if (denomanator == 0)
			continue;

		float numerator = TTG::Math::dot(plane.normal, plane.Position * glm::vec3(1.0f, -1.0f, 1.0f)) - glm::dot(plane.normal, ray.Origin);

		float closestT = numerator / denomanator;
		if (closestT < hitDistance && closestT > 0.0f)
		{
			hitDistance = closestT;
			index = i;
		}
	}

	if (index < 0)
		return miss();

	return closestHit(ray, hitDistance, index, HitType::PLANE);
}

HitPayload Renderer::traceTriangles(Ray ray)
{
	int index = -1;
	float hitDistance = FLT_MAX;
	float retU = 0;
	float retV = 0;
	float retW = 0;
	for (int i = 0; i < m_activateScene->triangles.size(); i++)
	{
		Triangle& tri = m_activateScene->triangles[i];
		if (!tri.visible)
			continue;

		glm::vec3 a = tri.PosA;
		a.y = 0.0f - a.y;

		glm::vec3 b = tri.PosB;
		b.y = 0.0f - b.y;

		glm::vec3 c = tri.PosC;
		c.y = 0.0f - c.y;

		glm::vec3 edgeAB = b - a;
		glm::vec3 edgeAC = c - a;
		glm::vec3 normalVector = TTG::Math::cross(edgeAB, edgeAC);
		glm::vec3 ao = ray.Origin - a;
		glm::vec3 dao = TTG::Math::cross(ao, ray.Direction);

		float determinant = -TTG::Math::dot(ray.Direction, normalVector);
		float invDet = 1 / determinant;

		float dst = TTG::Math::dot(ao, normalVector) * invDet;
		float u = TTG::Math::dot(edgeAC, dao) * invDet;
		float v = -TTG::Math::dot(edgeAB, dao) * invDet;
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
		return miss();

	Triangle& tri = m_activateScene->triangles[index];

	HitPayload payload;
	payload.hasHit = true;
	payload.hitPos = ray.Origin + (ray.Direction * hitDistance);
	payload.hitNormal = TTG::Math::normalize(tri.na * retW + tri.nb * retU + tri.nc * retV);
	payload.hitDistance = hitDistance;
	payload.hitType = HitType::TRIANGLE;
	payload.hitIndex = index;
	return payload;
}

HitPayload Renderer::closestHit(Ray ray, float t, int index, HitType hitType)
{
	HitPayload payload;
	payload.hitType = hitType;
	payload.hitDistance = t;
	payload.hasHit = true;
	payload.hitIndex = index;

	switch (hitType)
	{
	case HitType::SPHERE:
        {
            glm::vec3 sphereOrigin = m_activateScene->spheres[index].Position;
            sphereOrigin.y = 0.0f - sphereOrigin.y;
            glm::vec3 origin = ray.Origin - sphereOrigin;
            
            glm::vec3 hitPoint = origin + (ray.Direction * t);
            glm::vec3 normal = TTG::Math::normalize(hitPoint);
            hitPoint += sphereOrigin;
            
            payload.hitNormal = normal;
            payload.hitPos = hitPoint;
        }
		break;
	case HitType::PLANE:
        {
            payload.hitNormal = -m_activateScene->planes[index].normal;
            payload.hitPos = ray.Origin + (ray.Direction * t);
        }
		break;
	default:
        {
        }
		break;
	}
	return payload;
}

HitPayload Renderer::miss()
{
	HitPayload payload;
	payload.hasHit = false;
	payload.hitDistance = -FLT_MAX;
	payload.hitIndex = -1;
	return payload;
}

glm::vec4 Renderer::perPixel(int x, int y)
{
	//return glm::vec4((float)x / (float)m_width, (float)y / (float)m_height, 0.0f, 1.0f);

	Ray ray;
	ray.Origin = m_activateCamera->position;
	ray.Direction = m_activateCamera->rayDirections[y * m_width + x];

    if (useDistBlur)
    {
        glm::vec3 point = ray.Origin + ray.Direction * distBlur;
        ray.Origin += TTG::Random::inUnitSphere() * 2.0f;
        ray.Direction = point - ray.Origin;
        ray.Direction = TTG::Math::normalize(ray.Direction);
    }

	//unsigned int seed = (y * m_width + x) * m_frameIndex;

	glm::vec3 light(0.0f);
	glm::vec3 contribution(1.0f);

	const int bounces = 8;
	for (int i = 0; i < bounces; i++)
	{
		//seed += i;
		HitPayload hit = traceRay(ray);
		if (!hit.hasHit)
		{
			glm::vec3 skyColor = { 0.6f, 0.7f, 0.9f };
			if (skyLight)
				light += skyColor * contribution;
			break;
		}
		Material mat;
		if (hit.hitType == HitType::SPHERE)
			mat = m_activateScene->mat[m_activateScene->spheres[hit.hitIndex].matIndex];
		else if (hit.hitType == HitType::PLANE)
			mat = m_activateScene->mat[m_activateScene->planes[hit.hitIndex].matIndex];
		else if (hit.hitType == HitType::TRIANGLE)
			mat = m_activateScene->mat[m_activateScene->triangles[hit.hitIndex].matIndex];
		
		glm::vec3 color = glm::vec3(0.0f);
		//return mat.albedo;
		color = mat.albedo;

		glm::vec3 emission = mat.getEmission();
		if (emission.x + emission.y + emission.z > 0.0f)
		{
			light += emission * contribution;
			break;
		}
		contribution *= color;

        int bsdfIndex = -1;

		if (hit.hitType == HitType::SPHERE)
        {
            bsdfIndex = m_activateScene->spheres[hit.hitIndex].bsdfIndex;
        }
		else if (hit.hitType == HitType::PLANE)
        {
            bsdfIndex = m_activateScene->planes[hit.hitIndex].bsdfIndex;
        }
		else if (hit.hitType == HitType::TRIANGLE)
        {
            bsdfIndex = m_activateScene->triangles[hit.hitIndex].bsdfIndex;
        }

		BsdfPayload t;
		t.mat = mat;
		t.payload = hit;
		t.preCalculate = false;

		m_activateScene->bsdf[bsdfIndex]->processHit(t, &ray);
	}
	return glm::vec4(light, 1.0f);
}
