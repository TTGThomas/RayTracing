#include "Camera.h"

namespace TTG
{
	ImVec2 Camera::oldMousePos = {0.0f, 0.0f};
	bool Camera::isFirst = true;

	Camera::Camera()
	{
		updateMatrix();
	}

	void Camera::updateMatrix()
	{
		if (rotation.x > maxRotationX)
			rotation.x = maxRotationX;
		else if (rotation.x < -maxRotationX)
			rotation.x = -maxRotationX;

		if (rotation.y > 360.0f)
			rotation.y -= 360.0f;

		if (rotation.y < -360.0f)
			rotation.y += 360.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
		direction.y = sin(glm::radians(rotation.x));
		direction.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
		cameraFront = Math::normalize(direction);

		cameraRight = Math::normalize(Math::cross(cameraFront, worldUp));
		cameraUp = Math::normalize(Math::cross(cameraRight, cameraFront));

		front = Math::normalize(glm::vec3(direction.x, 0.0f, direction.z));
		right = Math::normalize(Math::cross(front, worldUp));
	}

	glm::mat4 Camera::getView()
	{
		return glm::lookAt(position, position + cameraFront, cameraUp);
	}

	glm::mat4 Camera::getInverseView()
	{
		return glm::inverse(getView());
	}

	glm::mat4 Camera::getProjection(const Config& config) const
	{
		return glm::perspective(zoom, config.screenWidth / config.screenHeight, 0.1f, 100.0f);
	}

	glm::mat4 Camera::getInverseProjection(const Config& config) const
	{
		return glm::inverse(getProjection(config));
	}

	void Camera::addPosition(const DIRECTION& dir, const Config& config)
	{
		float add = speed * config.delta;
		switch (dir)
		{
		case DIRECTION::FORWARD:
			position += cameraFront * add;
			break;

		case DIRECTION::BACKWARD:
			position -= cameraFront * add;
			break;

		case DIRECTION::LEFT:
			position -= cameraRight * add;
			break;

		case DIRECTION::RIGHT:
			position += cameraRight * add;
			break;

		case DIRECTION::UP:
			position -= worldUp * add;
			break;

		case DIRECTION::DOWN:
			position += worldUp * add;
			break;

		default:
			break;
		}
	}

	bool Camera::update(Config& config)
	{
		if (rayDirections.size() == 0)
		{
			updateMatrix();
			reCalculateRayDirections(config);
			return true;
		}
		if (!ImGui::IsMouseDown(ImGuiMouseButton_Right))
		{
			isFirst = true;
			return false;
		}

		moved = false;

		if (isFirst)
		{
			oldMousePos = ImGui::GetMousePos();
			isFirst = false;
		}

		glm::vec2 mousePos = {
			ImGui::GetMousePos().x - oldMousePos.x,
			ImGui::GetMousePos().y - oldMousePos.y
		};

		oldMousePos = ImGui::GetMousePos();

		rotation.x += mousePos.y * 0.5f;
		rotation.y += mousePos.x * 0.5f;

		if (mousePos != glm::vec2(0.0f))
			moved = true;

		if (ImGui::IsKeyDown(ImGuiKey_W))
		{
			moved = true;
			addPosition(DIRECTION::FORWARD, config);
		}
		if (ImGui::IsKeyDown(ImGuiKey_S))
		{
			moved = true;
			addPosition(DIRECTION::BACKWARD, config);
		}
		if (ImGui::IsKeyDown(ImGuiKey_A))
		{
			moved = true;
			addPosition(DIRECTION::LEFT, config);
		}
		if (ImGui::IsKeyDown(ImGuiKey_D))
		{
			moved = true;
			addPosition(DIRECTION::RIGHT, config);
		}
		if (ImGui::IsKeyDown(ImGuiKey_E))
		{
			moved = true;
			addPosition(DIRECTION::UP, config);
		}
		if (ImGui::IsKeyDown(ImGuiKey_Q))
		{
			moved = true;
			addPosition(DIRECTION::DOWN, config);
		}

		updateMatrix();
		return moved;
	}

	void Camera::reCalculateRayDirections(const Config& config)
	{
		rayDirections.resize((size_t)config.screenWidth * (size_t)config.screenHeight);

		for (int y = 0; y < config.screenHeight; y++)
		{
			for (int x = 0; x < config.screenWidth; x++)
			{
				glm::vec2 coord = { (float)x / config.screenWidth, (float)y / config.screenHeight };
				coord = coord * 2.0f - 1.0f;

				glm::vec4 target = getInverseProjection(config) * glm::vec4(coord.x, coord.y, 1, 1);
				glm::vec3 rayDirection =
					glm::vec3(getInverseView() * glm::vec4(Math::normalize(glm::vec3(target) / target.w),
						0)); // world space
				rayDirections[(size_t)y * (size_t)config.screenWidth + (size_t)x] = rayDirection;
			}
		}
	}
	void Camera::reCalculateRayDirections(const Config& config, int x, int y)
	{
		glm::vec2 coord = { (float)x / config.screenWidth, (float)y / config.screenHeight };
		coord = coord * 2.0f - 1.0f;

		glm::vec4 target = getInverseProjection(config) * glm::vec4(coord.x, coord.y, 1, 1);
		glm::vec3 rayDirection =
			glm::vec3(getInverseView() * glm::vec4(Math::normalize(glm::vec3(target) / target.w), 0)); // world space
		rayDirections[(size_t)y * (size_t)config.screenWidth + (size_t)x] = rayDirection;
	}
}
