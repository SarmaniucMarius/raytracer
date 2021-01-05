#pragma once

struct Camera
{
	v3f look_from;
	v3f u, v, w;
	v3f horizontal;
	v3f vertical;
	v3f lower_left;
	float lens_radius;

	Camera(float aspect_ratio, float y_fov_degrees, v3f look_from, v3f look_at, v3f up, float aperture)
	{
		const float viewport_height = tan(degrees_to_radians(y_fov_degrees / 2.0f)) * 2.0f;
		const float viewport_width = viewport_height * aspect_ratio;

		this->look_from = look_from;
		w = normalize(look_from - look_at);
		u = normalize(cross(V3f(.0f, 1.0f, .0f), w));
		v = cross(w, u);

		const float dist_to_focus = length(look_from - look_at);
		
		horizontal = dist_to_focus * u*viewport_width;
		vertical = dist_to_focus * v*viewport_height;
		lower_left = look_from - horizontal * .5f - vertical * .5f - dist_to_focus * w;

		lens_radius = aperture / 2.0f;

		// horizontal = u*viewport_width;
		// vertical = v*viewport_height;
		// lower_left = look_from - horizontal*.5f - vertical*.5f - w;
	}

	Ray get_ray(float film_x, float film_y)
	{
		v3f rd = lens_radius * random_in_unit_disk();
		v3f offset = u * rd.x + v * rd.y;
		Ray r = Ray(look_from + offset, lower_left + film_x * horizontal + film_y * vertical - look_from - offset);
		// Ray r = Ray(look_from, lower_left + film_x*horizontal + film_y*vertical - look_from);

		return r;
	}
};