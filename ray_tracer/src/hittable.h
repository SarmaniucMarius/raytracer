#pragma once

struct Material;

struct Hit_Record
{
	v3f p;
	v3f n;
	bool from_outside;
	shared_ptr<Material> mat;
	float t;
	v3f color;

	float u;
	float v;
};

struct Hittable
{
	virtual bool hit(Ray r, float t_min, float t_max, Hit_Record& rec) = 0;
};

struct World
{
	v3f background;
	vector<shared_ptr<Hittable>> objects;

	void add_object(shared_ptr<Hittable> o) { objects.push_back(o); }
};

struct Plane : public Hittable
{
	v3f n;
	float d;
	shared_ptr<Material> mat;

	Plane(v3f n, float d, shared_ptr<Material> m) : n(n), d(d), mat(m) {}

	bool hit(Ray r, float t_min, float t_max, Hit_Record& rec) override
	{
		float denom = dot(n, r.direction);
		if (denom != 0)
		{
			float t = (-d - dot(n, r.origin)) / denom;
			if ((t > t_min) && (t < t_max))
			{
				rec.p = r.point_at(t);
				rec.n = n;
				rec.from_outside = true;
				rec.t = t;
				rec.mat = mat;
				return true;
			}
		}

		return false;
	}
};

struct Sphere : public Hittable
{
	v3f origin;
	float radius;
	shared_ptr<Material> mat;

	Sphere(v3f o, float r, shared_ptr<Material> m) : origin(o), radius(r), mat(m) {}

	bool hit(Ray r, float t_min, float t_max, Hit_Record& rec) override
	{
		v3f relative_sphere_origin = r.origin - origin;
		float a = dot(r.direction, r.direction);
		float b = 2 * dot(relative_sphere_origin, r.direction);
		float c = dot(relative_sphere_origin, relative_sphere_origin) - radius * radius;

		float determinant = b * b - 4 * a*c;
		if (determinant > 0)
		{
			float t1 = (-b - (float)sqrt(determinant)) / (2.0f*a);
			if ((t1 > t_min) && (t1 < t_max))
			{
				rec.p = r.point_at(t1);
				rec.n = (rec.p - origin) / radius;
				rec.from_outside = true;
				if (dot(r.direction, rec.n) > 0)
				{
					// handles the case when a ray hits the sphere from inside
					rec.n = -rec.n;
					rec.from_outside = false;
				}
				rec.t = t1;
				rec.mat = mat;
				get_sphere_uv(rec.n, rec.u, rec.v);
				return true;
			}

			float t2 = (-b + (float)sqrt(determinant)) / (2.0f*a);
			if ((t2 > t_min) && (t2 < t_max))
			{
				rec.p = r.point_at(t2);
				rec.n = (rec.p - origin) / radius;
				rec.from_outside = true;
				if (dot(r.direction, rec.n) > 0)
				{
					// handles the case when a ray hits the sphere from inside
					rec.n = -rec.n;
					rec.from_outside = false;
				}
				rec.t = t2;
				rec.mat = mat;
				get_sphere_uv(rec.n, rec.u, rec.v);
				return true;
			}
		}

		return false;
	}


private:
	void get_sphere_uv(v3f& p, float& u, float& v)
	{
		double theta = acos(-p.y);
		double phi = atan2(-p.z, p.x) + PI;

		u = float(phi) / (2.0f * PI);
		v = float(theta) / PI;
	}
};

struct Triangle : public Hittable
{
	v3f a, b, c; // this must be in clockwise direction
	shared_ptr<Material> mat;

	Triangle(v3f v0, v3f v1, v3f v2, shared_ptr<Material> m) : a(v0), b(v1), c(v2), mat(m) {}

	bool hit(Ray r, float t_min, float t_max, Hit_Record& rec) override
	{
		v3f triangle_normal = normalize(cross(b - a, c - a));

		// determine if ray intersects triangle's plane
		float denom = dot(triangle_normal, r.direction);
		if (denom != 0)
		{
			float d = dot(triangle_normal, a);
			float t = (d - dot(triangle_normal, r.origin)) / denom;
			if (t >= t_min && t <= t_max)
			{
				// determine if the point that intersects the plane is in the triangle
				v3f q = r.point_at(t);
				if (dot(cross(b - a, q - a), triangle_normal) >= 0 && 
					dot(cross(q - a, c - a), triangle_normal) >= 0 &&
					dot(cross(c - b, q - b), triangle_normal) >= 0)
				{
					// point is within the triangle
					rec.p = q;
					rec.n = triangle_normal;
					rec.from_outside = true;
					rec.t = t;
					rec.mat = mat;

					return true;
				}
			}
		}

		return false;
	}
};
