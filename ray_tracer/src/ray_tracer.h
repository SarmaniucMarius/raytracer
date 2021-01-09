#pragma once

// Utilities

#define MIN(a, b) (a) < (b) ? (a) : (b)
#define MAX(a, b) (a) > (b) ? (a) : (b)

inline float
random_float()
{
	return (float)rand() / ((float)RAND_MAX + 1.0f);
}

inline float
random_float(float min, float max)
{
	return min + (max - min)*random_float();
}

inline float 
clamp(float x, float min, float max) 
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

#include "ray_math.h"
#include <limits>

const float infinity = std::numeric_limits<float>::infinity();

inline v3f
correct_gamma(v3f color)
{
	v3f result = {};

	result.r = (float)sqrt(color.r);
	result.g = (float)sqrt(color.g);
	result.b = (float)sqrt(color.b);

	return result;
}

struct Image
{
	int width;
	int height;
	uint32_t* pixels;

	uint32_t* get_image_ptr(int x, int y)
	{
		uint32_t* ptr = pixels + x + y * width;
		return ptr;
	}
};

inline int unpack_rgba(v3f color)
{
	int r = int(color.r * 255.99f);
	int g = int(color.g * 255.99f);
	int b = int(color.b * 255.99f);
	int a = 0;

	return (r << 24) | (g << 16) | (b << 8) | (a << 0);
}

void write_ppm(const char* file_name, Image& image)
{
	FILE *f = fopen(file_name, "wb");
	fprintf(f, "P3\n%d %d\n255\n", image.width, image.height);

	// ppm expects pixels to be top to bottom but our image is rendered bottom to top
	for (int y = image.height - 1; y >= 0; y--)
	{
		uint32_t* image_buf = image.get_image_ptr(0, y);
		for (int x = 0; x < image.width; x++)
		{
			uint32_t col = *image_buf;

			uint8_t r = (col >> 24) & 0xFF;
			uint8_t g = (col >> 16) & 0xFF;
			uint8_t b = (col >> 8) & 0xFF;

			fprintf(f, "%d %d %d\n", r, g, b);

			image_buf++;
		}
	}
}

struct Ray
{
	v3f origin;
	v3f direction;

	Ray(v3f o, v3f d) : origin(o), direction(d) {}

	v3f point_at(float t) { return origin + t * direction; }
};
