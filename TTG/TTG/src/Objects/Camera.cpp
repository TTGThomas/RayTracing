#include "Camera.h"

namespace TTG
{
	ImVec2 Camera::oldMousePos = {0.0f, 0.0f};
	bool Camera::isFirst = true;

	Camera::Camera()
	{
		UpdateMatrix();
	}

	void Camera::UpdateMatrix()
	{
		if (m_rotation.x > m_maxRotationX)
			m_rotation.x = m_maxRotationX;
		else if (m_rotation.x < -m_maxRotationX)
			m_rotation.x = -m_maxRotationX;

		if (m_rotation.y > 360.0f)
			m_rotation.y -= 360.0f;

		if (m_rotation.y < -360.0f)
			m_rotation.y += 360.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(m_rotation.y)) * cos(glm::radians(m_rotation.x));
		direction.y = sin(glm::radians(m_rotation.x));
		direction.z = sin(glm::radians(m_rotation.y)) * cos(glm::radians(m_rotation.x));
		m_cameraFront = Math::Normalize(direction);

		m_cameraRight = Math::Normalize(Math::Cross(m_cameraFront, m_worldUp));
		m_cameraUp = Math::Normalize(Math::Cross(m_cameraRight, m_cameraFront));

		m_front = Math::Normalize(glm::vec3(direction.x, 0.0f, direction.z));
		m_right = Math::Normalize(Math::Cross(m_front, m_worldUp));
	}

	glm::mat4 Camera::GetView()
	{
		return glm::lookAt(m_position, m_position + m_cameraFront, m_cameraUp);
	}

	glm::mat4 Camera::GetInverseView()
	{
		return glm::inverse(GetView());
	}

	glm::mat4 Camera::GetProjection(const Config& config) const
	{
		return glm::perspective(m_zoom, config.screenWidth / config.screenHeight, 0.1f, 100.0f);
	}

	glm::mat4 Camera::GetInverseProjection(const Config& config) const
	{
		return glm::inverse(GetProjection(config));
	}

	void Camera::AddPosition(const DIRECTION& dir, const Config& config)
	{
		float add = m_speed * config.delta;
		switch (dir)
		{
		case DIRECTION::FORWARD:
			m_position += m_cameraFront * add;
			break;

		case DIRECTION::BACKWARD:
			m_position -= m_cameraFront * add;
			break;

		case DIRECTION::LEFT:
			m_position -= m_cameraRight * add;
			break;

		case DIRECTION::RIGHT:
			m_position += m_cameraRight * add;
			break;

		case DIRECTION::UP:
			m_position += m_worldUp * add;
			break;

		case DIRECTION::DOWN:
			m_position -= m_worldUp * add;
			break;

		default:
			break;
		}
	}

	bool Camera::Update(Config& config)
	{
		if (m_rayDirections.size() == 0)
		{
			UpdateMatrix();
			ReCalculateRayDirections(config);
			return true;
		}
		if (!ImGui::IsMouseDown(ImGuiMouseButton_Right))
		{
			isFirst = true;
			return false;
		}

		m_moved = false;

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

		m_rotation.x += mousePos.y * -0.5f;// invert Y
		m_rotation.y += mousePos.x * 0.5f;

		if (mousePos != glm::vec2(0.0f))
			m_moved = true;

		if (ImGui::IsKeyDown(ImGuiKey_W))
		{
			m_moved = true;
			AddPosition(DIRECTION::FORWARD, config);
		}
		if (ImGui::IsKeyDown(ImGuiKey_S))
		{
			m_moved = true;
			AddPosition(DIRECTION::BACKWARD, config);
		}
		if (ImGui::IsKeyDown(ImGuiKey_A))
		{
			m_moved = true;
			AddPosition(DIRECTION::LEFT, config);
		}
		if (ImGui::IsKeyDown(ImGuiKey_D))
		{
			m_moved = true;
			AddPosition(DIRECTION::RIGHT, config);
		}
		if (ImGui::IsKeyDown(ImGuiKey_E))
		{
			m_moved = true;
			AddPosition(DIRECTION::UP, config);
		}
		if (ImGui::IsKeyDown(ImGuiKey_Q))
		{
			m_moved = true;
			AddPosition(DIRECTION::DOWN, config);
		}

		UpdateMatrix();
		return m_moved;
	}

	void Camera::ResizeBuffer(const Config& config)
	{
		m_rayDirections.resize((size_t)config.screenWidth * (size_t)config.screenHeight);
	}

	void Camera::ReCalculateRayDirections(const Config& config)
	{
		m_rayDirections.resize((size_t)config.screenWidth * (size_t)config.screenHeight);

		for (int y = 0; y < config.screenHeight; y++)
		{
			for (int x = 0; x < config.screenWidth; x++)
			{
				glm::vec2 coord = { (float)x / config.screenWidth, (float)y / config.screenHeight };
				coord = coord * 2.0f - 1.0f;

				glm::vec4 target = GetInverseProjection(config) * glm::vec4(coord.x, coord.y, 1, 1);
				glm::vec3 rayDirection =
					glm::vec3(GetInverseView() * glm::vec4(Math::Normalize(glm::vec3(target) / target.w),
						0)); // world space
				m_rayDirections[(size_t)y * (size_t)config.screenWidth + (size_t)x] = rayDirection;
			}
		}
	}
	void Camera::ReCalculateRayDirections(const Config& config, const int& x, const int& y)
	{
		glm::vec2 coord = { (float)x / config.screenWidth, (float)y / config.screenHeight };
		coord = coord * 2.0f - 1.0f;

		glm::vec4 target = GetInverseProjection(config) * glm::vec4(coord.x, coord.y, 1, 1);
		glm::vec3 rayDirection =
			glm::vec3(GetInverseView() * glm::vec4(Math::Normalize(glm::vec3(target) / target.w), 0)); // world space
		m_rayDirections[(size_t)y * (size_t)config.screenWidth + (size_t)x] = rayDirection;
	}
}
