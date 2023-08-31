#include "pch.h"
#include "ImageReader.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void ReadImage(const std::string filename, std::vector<uint8_t>& image, int& width, int& height)
{
	int channels;
	unsigned char* img = stbi_load(filename.c_str(), &width, &height, &channels, 0);
	// assert(channels == 4);
	std::cout << filename << " " << width << " " << height << " " << channels << std::endl;
	image.resize(width * height * 4);

	if (channels == 1)
	{
		for (size_t i = 0; i < width * height; i++)
		{
			uint8_t g = img[i * channels + 0];
			for (size_t c = 0; c < 4; c++)
			{
				image[4 * i + c] = g;
			}
		}
	}
	else if (channels == 2)
	{
		for (size_t i = 0; i < width * height; i++)
		{
			for (size_t c = 0; c < 2; c++)
			{
				image[4 * i + c] = img[i * channels + c];
			}
			image[4 * i + 2] = 255;
			image[4 * i + 3] = 255;
		}
	}
	else if (channels == 3)
	{
		for (size_t i = 0; i < width * height; i++)
		{
			for (size_t c = 0; c < 3; c++)
			{
				image[4 * i + c] = img[i * channels + c];
			}
			image[4 * i + 3] = 255;
		}
	}
	else if (channels == 4)
	{
		for (size_t i = 0; i < width * height; i++)
		{
			for (size_t c = 0; c < 4; c++)
			{
				image[4 * i + c] = img[i * channels + c];
			}
		}
	}
	else
	{
		std::cout << "Cannot read " << channels << " channels" << std::endl;
	}
}