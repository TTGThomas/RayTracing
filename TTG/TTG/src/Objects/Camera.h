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

		void updateMatrix();

		glm::mat4 getView();
		glm::mat4 getInverseView();
		glm::mat4 getProjection(const Config& config) const;
		glm::mat4 getInverseProjection(const Config& config) const;

		void addPosition(const DIRECTION& dir, const Config& config);

		bool update(Config& config);

		void reCalculateRayDirections(const Config& config);
		void reCalculateRayDirections(const Config& config, int x, int y);

		bool getMoved() { return moved; }
	public:
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f, -90.0f, 0.0f);
		float zoom = 45.0f;
		double speed = 0.0;

		// the max that you can look up and down
		int maxRotationX = 89.0f;

		std::vector<glm::vec3> rayDirections;

	public:
		glm::vec3 cameraFront = glm::vec3(0.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f);
		glm::vec3 cameraRight = glm::vec3(0.0f);
		glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	public:
		glm::vec3 front = glm::vec3(0.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 right = glm::vec3(0.0f);
	private:
		static ImVec2 oldMousePos;
		static bool isFirst;
		bool moved = false;
	};
}