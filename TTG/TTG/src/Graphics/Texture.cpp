#include "Texture.h"

namespace TTG
{
	void Texture::LoadTextureFromFile(std::string filePath)
	{
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif // STB_IMAGE_IMPLEMENTATION
        if (m_id != -1)
            glDeleteTextures(1, &m_id);
		glGenTextures(1, &m_id);

		int nChannels;
		unsigned char* data = stbi_load(filePath.c_str(), &m_width, &m_height, &nChannels, 0);

		GLenum  colorMode = GL_RGB;
		switch (nChannels)
		{
		case 1:
			colorMode = GL_RED;
			break;
		case 4:
			colorMode = GL_RGBA;
			break;
		}

		if (data != NULL)
		{
			glBindTexture(GL_TEXTURE_2D, m_id);
			glTexImage2D(GL_TEXTURE_2D, 0, colorMode, m_width, m_height, 0, colorMode, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}

		stbi_image_free(data);
	}

	void Texture::LoadTextureFromData(unsigned char* data, int nChannels, int width, int height)
	{
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif // STB_IMAGE_IMPLEMENTATION

		m_width = width;
		m_height = height;
        
        if (m_id != -1)
            glDeleteTextures(1, &m_id);

		glGenTextures(1, &m_id);

		GLenum colorMode = GL_RGB;
		switch (nChannels)
		{
		case 1:
			colorMode = GL_RED;
			break;
		case 4:
			colorMode = GL_RGBA;
			break;
		}

		if (data != NULL)
		{
			glBindTexture(GL_TEXTURE_2D, m_id);
			glTexImage2D(GL_TEXTURE_2D, 0, colorMode, m_width, m_height, 0, colorMode, GL_UNSIGNED_BYTE, data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
	}
}
