#pragma once

#include <fstream>
#include <string>

#include <assert.h>

namespace TTG
{
	namespace Utils
	{
		inline std::string Read(const char* filePath)
		{
			std::ifstream file;
			file.open(filePath);

			if (!file.is_open())
			{
				// cannot open the file specified
				assert(0);// 0 asserts and 1 doesnt assert
			}

			std::string ret;
			std::string line;
			while (std::getline(file, line))
			{
				ret.append(line);
				ret.append("\n");
			}
			return ret;
		}
	}
}