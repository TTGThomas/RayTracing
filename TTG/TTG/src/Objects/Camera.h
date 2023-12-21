#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ImGuiOpenGL.h>
#include "Utils/Math.h"

#include <vector>

namespace TTG
{
	struct Config
	{
		float screenWidth;
		float screenHeight;

		float delta;
	};

	enum class DIRECTION
	{
		UNKNOWN, FORWARD, BACKWARD, RIGHT, LEFT, UP, DOWN
	};

	// 3D = projection * view * model * vec4(pos)
	// 2D = view * model * vec4(pos)
	class Camera
	{
	public:
		Camera();

		void UpdateMatrix();

		glm::mat4 GetView();
		glm::mat4 GetInverseView();
		glm::mat4 GetProjection(const Config& config) const;
		glm::mat4 GetInverseProjection(const Config& config) const;

		void AddPosition(const DIRECTION& dir, const Config& config);

		bool Update(Config& config);

		void ReCalculateRayDirections(const Config& config);
		void ReCalculateRayDirections(const Config& config, int x, int y);

		bool GetMoved() { return m_moved; }
	public:
		glm::vec3 m_position = glm::vec3(0.0f);
		glm::vec3 m_rotation = glm::vec3(0.0f, -90.0f, 0.0f);
		float m_zoom = 45.0f;
		double m_speed = 0.0;

		// the max that you can look up and down
		int m_maxRotationX = 89.0f;

		std::vector<glm::vec3> m_rayDirections;

	public:
		glm::vec3 m_cameraFront = glm::vec3(0.0f);
		glm::vec3 m_cameraUp = glm::vec3(0.0f);
		glm::vec3 m_cameraRight = glm::vec3(0.0f);
		glm::vec3 m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	public:
		glm::vec3 m_front = glm::vec3(0.0f);
		glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 m_right = glm::vec3(0.0f);
	private:
		static ImVec2 oldMousePos;
		static bool isFirst;
		bool m_moved = false;
	};
}