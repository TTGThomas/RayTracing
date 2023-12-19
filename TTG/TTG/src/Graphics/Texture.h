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
		void loadTextureFromFile(std::string filePath);
		void loadTextureFromData(unsigned char* data, int nChannels, int width, int height);

		unsigned int getID() { return m_id; };
		ImTextureID getDescriptorSet() { return (void*)(intptr_t)m_id; };
		float getWidth() { return (float)m_width; };
		float getHeight() { return (float)m_height; };
	private:
		unsigned int m_id = -1;
		int m_width;
		int m_height;
	};
}
