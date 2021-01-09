#pragma once

#include "ray_math.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Texture
{
	virtual v3f value(float u, float v, v3f& p) = 0;
};

struct Solid_Color : public Texture
{
	v3f color_value;

	Solid_Color(v3f c) : color_value(c) {}

	virtual v3f value(float u, float v, v3f& p) override
	{
		return color_value;
	}
};

struct Checker_Texture : public Texture
{
	shared_ptr<Solid_Color> empty;
	shared_ptr<Solid_Color> fill;

	Checker_Texture(v3f c1, v3f c2) : empty(make_shared<Solid_Color>(c1)), fill(make_shared<Solid_Color>(c2)) {}
	Checker_Texture(shared_ptr<Solid_Color> c1, shared_ptr<Solid_Color> c2) : empty(c1), fill(c2) {}

	virtual v3f value(float u, float v, v3f& p) override
	{
		float sines = sin(10.0f*p.x)*sin(10.0f*p.y)*sin(10.0f*p.z);
		if (sines < 0)
			return empty->value(u, v, p);
		else
			return fill->value(u, v, p);
	}
};

struct Image_Texture : public Texture 
{
	Image_Texture() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

	Image_Texture(const char* filename) 
	{
		auto components_per_pixel = bytes_per_pixel;

		data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);

		if (!data) 
		{
			std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
			width = height = 0;
		}

		bytes_per_scanline = bytes_per_pixel * width;
	}

	~Image_Texture() 
	{
		delete data;
	}

	virtual v3f value(float u, float v, v3f& p) override 
	{
		// If we have no texture data, then return solid cyan as a debugging aid.
		if (data == nullptr)
			return v3f{ .0f, 1.0f, 1.0f };

		// Clamp input texture coordinates to [0,1] x [1,0]
		u = clamp(u, .0f, 1.0f);
		v = 1.0f - clamp(v, .0f, 1.0f);  // Flip V to image coordinates

		auto i = static_cast<int>(u * width);
		auto j = static_cast<int>(v * height);

		// Clamp integer mapping, since actual coordinates should be less than 1.0
		if (i >= width)  i = width - 1;
		if (j >= height) j = height - 1;

		const float color_scale = 1.0f / 255.0f;
		auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

		return v3f{ color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2] };
	}

private:
	unsigned char *data;
	int width, height;
	int bytes_per_scanline;
	int bytes_per_pixel = 3;
};
