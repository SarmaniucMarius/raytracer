#pragma once

#include "texture.h"

struct Material
{
	virtual bool scatter(Hit_Record& rec, Ray& r, v3f& attenuation) = 0;
	virtual v3f emitted(float u, float v, v3f p)
	{
		return v3f{ .0f, .0f, .0f };
	}
};

struct Diffuse_Light : public Material
{
	shared_ptr<Texture> emit;

	Diffuse_Light(shared_ptr<Texture> a) : emit(a) {}
	Diffuse_Light(v3f c) : emit(make_shared<Solid_Color>(c)) {}

	virtual bool scatter(Hit_Record& rec, Ray& r, v3f& attenuation) override
	{
		return false;
	}

	virtual v3f emitted(float u, float v, v3f p)
	{
		return emit->value(u, v, p);
	}
};

struct Lambertian : Material
{
	shared_ptr<Texture> albedo;

	Lambertian(v3f a) : albedo(make_shared<Solid_Color>(a)) {}
	Lambertian(shared_ptr<Texture> a) : albedo(a) {}

	bool scatter(Hit_Record& rec, Ray& r, v3f& attenuation) override
	{
		v3f scattered_dir = rec.n + random_in_unit_vector();
		r.direction = scattered_dir;
		r.origin = rec.p;
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}
};

struct Metal : Material
{
	v3f albedo;
	float fuzz;

	Metal(v3f a, float f) : albedo(a), fuzz(f) {}

	bool scatter(Hit_Record& rec, Ray& r, v3f& attenuation) override
	{
		v3f scattered_dir = reflect(r.direction, rec.n) + fuzz * random_in_unit_vector();
		r.direction = scattered_dir;
		r.origin = rec.p;
		attenuation = albedo;
		return true;
	}
};

struct Dielectric : Material
{
	float index_of_refraction;

	Dielectric(float ir) : index_of_refraction(ir) {}

	bool scatter(Hit_Record& rec, Ray& r, v3f& attenuation) override
	{
		attenuation = V3f(1.0f, 1.0f, 1.0f);
		float refraction_ratio = rec.from_outside ? 1.0f / index_of_refraction : index_of_refraction;

		v3f unit_direction = normalize(r.direction);
		float cos_theta = fmin(dot(-unit_direction, rec.n), 1.0f);
		float sin_theta = sqrt(1.0f - cos_theta * cos_theta);

		bool cannot_refract = refraction_ratio * sin_theta > 1.0f;
		v3f direction;
		// if(cannot_refract || (reflectance(cos_theta, refraction_ratio) > random_float()))
		if (cannot_refract)
		{
			direction = reflect(unit_direction, rec.n);
		}
		else
		{
			direction = refract(unit_direction, rec.n, refraction_ratio);
		}

		r.origin = rec.p;
		r.direction = direction;
		return true;
	}

private:
	v3f refract(v3f uv, v3f n, float etai_over_etat)
	{
		float cos_theta = fmin(dot(-uv, n), 1.0f);
		v3f r_out_perp = etai_over_etat * (uv + cos_theta * n);
		v3f r_out_parallel = -sqrt(fabs(1.0f - length_squared(r_out_perp))) * n;
		return r_out_perp + r_out_parallel;
	}

	float reflectance(float cosine, float ref_idx)
	{
		float r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0)*(float)pow((1 - cosine), 5);
	}
};