#pragma once

#include <stb/stb_image.h>
#include "ImGuiOpenGL.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

namespace TTG
{
	class Texture
	{
	public:
		void LoadTextureFromFile(const std::string& filePath);
		void LoadTextureFromData(unsigned char* data, const int& nChannels, const int& width, const int& height);

		unsigned int GetID() { return m_id; };
		ImTextureID GetDescriptorSet() { return (void*)(intptr_t)m_id; };
		float GetWidth() { return (float)m_width; };
		float GetHeight() { return (float)m_height; };
	private:
		unsigned int m_id = -1;
		int m_width;
		int m_height;
	};
}
