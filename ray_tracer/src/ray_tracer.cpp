#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cassert>
#include <thread>
#include <atomic>

using namespace std;

#include "ray_tracer.h"
#include "hittable.h"
#include "texture.h"
#include "material.h"
#include "camera.h"

#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"

struct Job
{
	Image* image;
	World* world;
	Camera* camera;
	int x_min;
	int x_max;
	int y_min;
	int y_max;
};

struct Job_Queue
{
	Job *jobs;
	int jobs_count;

	int samples_per_pixel;
	int ray_depth;

	atomic<int> next_job;
	atomic<uint64_t> total_bounces;
	atomic<uint64_t> finished_jobs;
};

v3f ray_cast(World& world, v3f background, Ray r, int depth)
{
	if (depth <= 0)
	{
		return v3f{ .0f, .0f, .0f };
	}

	bool hit = false;
	float t_max = infinity;
	Hit_Record rec = {};
	for (auto object : world.objects)
	{
		Hit_Record temp_rec = {};
		if (object->hit(r, 0.0001f, t_max, temp_rec))
		{
			rec = temp_rec;
			t_max = temp_rec.t;
			hit = true;
		}
	}

	if (hit)
	{
		v3f attenuation = {};
		v3f emitted = rec.mat->emitted(rec.u, rec.v, rec.p);
		if (!rec.mat->scatter(rec, r, attenuation))
		{
			return emitted;
		}
		else
		{
			return emitted + attenuation * ray_cast(world, background, r, depth - 1);
		}
	}
	else
	{
		return background;
	}
}

bool render_tile(Job_Queue& queue)
{
	printf("\rRaycasting %.2f%%", 100.0f * (float(queue.finished_jobs.load()) / float(queue.jobs_count)));
	fflush(stdout);

	int job_index = queue.next_job.fetch_add(1);
	if (job_index >= queue.jobs_count)
	{
		return false;
	}

	Job& job = *(queue.jobs + job_index);
	Image& image = *job.image;
	World& world = *job.world;
	Camera& camera = *job.camera;
	int x_min = job.x_min;
	int x_max = job.x_max;
	int y_min = job.y_min;
	int y_max = job.y_max;

	int depth = queue.ray_depth;
	int samples_per_pixel = queue.samples_per_pixel;
	for (int y = y_min; y < y_max; y++)
	{
		uint32_t* buf = image.get_image_ptr(x_min, y);

		for (int x = x_min; x < x_max; x++)
		{
			v3f color = {};
			for (int sample = 0; sample < samples_per_pixel; sample++)
			{
				float film_x = (float(x) + random_float()) / float(image.width);
				float film_y = (float(y) + random_float()) / float(image.height);

				Ray r = camera.get_ray(film_x, film_y);

				color += ray_cast(world, world.background, r, depth);

				queue.total_bounces++;
			}

			color = color / float(samples_per_pixel);
			color = correct_gamma(color);
			*buf = unpack_rgba(color);
			buf++;
		}
	}

	queue.finished_jobs++;
	
	return true;
}

void do_work(Job_Queue& queue)
{
	while(render_tile(queue)) {}
}

bool aabb(Ray& r, v3f p0, v3f p1)
{
	float t0x = MIN((p0.x - r.origin.x) / r.direction.x, (p1.x - r.origin.x) / r.direction.x);
	float t1x = MAX((p0.x - r.origin.x) / r.direction.x, (p1.x - r.origin.x) / r.direction.x);

	float t0y = MIN((p0.y - r.origin.y) / r.direction.y, (p1.y - r.origin.y) / r.direction.y);
	float t1y = MAX((p0.y - r.origin.y) / r.direction.y, (p1.y - r.origin.y) / r.direction.y);

	float t_min = MAX(t0x, t0y);
	float t_max = MIN(t1x, t1y);

	if (t_min >= t_max) { return false; }

	float t0z = MIN((p0.z - r.origin.z) / r.direction.z, (p1.z - r.origin.z) / r.direction.z);
	float t1z = MIN((p0.z - r.origin.z) / r.direction.z, (p1.z - r.origin.z) / r.direction.z);

	t_min = MIN(t0z, t_min);
	t_max = MAX(t1z, t_max);

	if (t_min >= t_max) { return false; }

	return true;
}

enum World_Types
{
	DEFAULT_WORLD,
	LIGHTED_WORLD,
	MONKEY_WORLD,
};

World generate_world(World_Types type = DEFAULT_WORLD)
{
	World world = {};

	switch (type)
	{
		case DEFAULT_WORLD:
		{
			auto earth_texture = make_shared<Image_Texture>("resources/earthmap.jpg");
			auto earth_surface = make_shared<Lambertian>(earth_texture);
			auto checker_texture = make_shared<Checker_Texture>(v3f{ .2f, .3f, .1f }, v3f{ .9f, .9f, .9f });
			auto material_ground = make_shared<Lambertian>(checker_texture);
			auto material_big_metal = make_shared<Metal>(V3f(0.2f, 0.25f, 0.7f), .0f);
			auto material_big_dielectric = make_shared<Dielectric>(1.7f);
			auto material_from_behind = make_shared<Lambertian>(V3f(1.0f, .0f, .0f));

			auto big_sphere_dielectric = make_shared<Sphere>(V3f(2.5f, 2.0f, -3.0f), 2.0f, material_big_dielectric);
			auto behind1 = make_shared<Sphere>(V3f(3.0f, .5f, -12.0f), .5f, material_from_behind);
			auto behind2 = make_shared<Sphere>(V3f(7.0f, .5f, -20.0f), .5f, material_from_behind);
			auto behind3 = make_shared<Sphere>(V3f(9.0f, .5f, -13.0f), .5f, material_from_behind);
			auto big_sphere_metal = make_shared<Sphere>(V3f(-3.5f, 2.0f, -3.0f), 2.0f, material_big_metal);
			auto center = make_shared<Sphere>(V3f(.0f, .5f, -1.0f), .5f, earth_surface);
			auto left = make_shared<Sphere>(V3f(-2.0f, .5f, .0f), .5f, make_shared<Metal>(V3f(.8f, .6f, .2f), .7f));
			auto right = make_shared<Sphere>(V3f(2.0f, .5f, 0.7f), .5f, make_shared<Metal>(V3f(.8f, .6f, .2f), .3f));
			auto ground = make_shared<Sphere>(V3f(.0f, -1000.0f, .0f), 1000.0f, material_ground);

			world.add_object(center);
			world.add_object(left);
			world.add_object(right);
			world.add_object(ground);
			world.add_object(big_sphere_dielectric);
			world.add_object(behind1);
			world.add_object(behind2);
			world.add_object(behind3);
			world.add_object(big_sphere_metal);

			world.background = v3f{.7f, .8f, 1.0f};

		} break;

		case LIGHTED_WORLD:
		{
			auto light = make_shared<Diffuse_Light>(v3f{ 1.0f, 1.0f, 1.0f });
			auto material_center = make_shared<Lambertian>(V3f(.7f, .3f, .3f));
			auto checker_texture = make_shared<Checker_Texture>(v3f{ .2f, .3f, .1f }, v3f{ .9f, .9f, .9f });
			auto material_ground = make_shared<Lambertian>(checker_texture);

			auto sphere_light = make_shared<Sphere>(v3f{ .0f, 3.0f, .0f }, 1.5f, light);
			auto ground = make_shared<Sphere>(v3f{ .0f, -1000.0f, .0f }, 1000.0f, material_ground);
			auto center_sphere = make_shared<Sphere>(v3f{.0f, .5f, .0f}, .5f, material_center);

			world.add_object(sphere_light);
			world.add_object(ground);
			world.add_object(center_sphere);
			
			world.background = v3f{ .0f, .0f, .0f };

		} break;

		case MONKEY_WORLD:
		{
			auto checker_texture = make_shared<Checker_Texture>(v3f{ .2f, .3f, .1f }, v3f{ .9f, .9f, .9f });
			auto material_ground = make_shared<Lambertian>(checker_texture);
			auto ground = make_shared<Sphere>(v3f{ .0f, -1000.0f, .0f }, 1000.0f, material_ground);

			world.add_object(ground);

			auto redish = make_shared<Lambertian>(V3f(.7f, .3f, .3f));
			fastObjMesh* mesh = fast_obj_read("resources/suzanne.obj");

			v3f monkey_offset = { .0f, 1.0f, .0f };

			for (unsigned int i = 0; i < mesh->face_count; i++)
			{
				unsigned int v0_index = mesh->indices[i * 3].p;
				unsigned int v1_index = mesh->indices[i * 3 + 1].p;
				unsigned int v2_index = mesh->indices[i * 3 + 2].p;

				v3f a = v3f{ mesh->positions[v0_index * 3], mesh->positions[v0_index * 3 + 1], mesh->positions[v0_index * 3 + 2] } +monkey_offset;
				v3f b = v3f{ mesh->positions[v1_index * 3], mesh->positions[v1_index * 3 + 1], mesh->positions[v1_index * 3 + 2] } +monkey_offset;
				v3f c = v3f{ mesh->positions[v2_index * 3], mesh->positions[v2_index * 3 + 1], mesh->positions[v2_index * 3 + 2] } +monkey_offset;
				auto triangle = make_shared<Triangle>(a, b, c, redish);
				world.add_object(triangle);
			}

			world.background = v3f{ .7f, .8f, 1.0f };
		} break;

		default:
			printf("Couldn't generate such type of world!\n");
			break;
	}

	return world;
}

int main()
{
	// Image
	const float aspect_ratio = 16.0f / 9.0f;
	Image image = {};
	image.width = 1024;
	image.height = int(float(image.width) / aspect_ratio);
	image.pixels = (uint32_t*)malloc(image.width * image.height * sizeof(uint32_t));

	// World generation
	v3f look_from = v3f{ .0f, 1.0f, 5.0f };
	v3f look_at = v3f{ .0f, .0f, .0f };
	v3f vup = v3f{ .0f, 1.0f, .0f };
	float dist_to_focus = 10.0f;
	float aperture = .2f;
	Camera camera = Camera{ look_from, look_at, vup, aspect_ratio, 60.0f, aperture, dist_to_focus };
	World world = generate_world(MONKEY_WORLD);
	
	// tile division
	int core_count = 4; // this should be an OS query
	int tile_width = image.width / 8;
	int tile_height = tile_width;
	
	int tile_count_x = (image.width + tile_width - 1) / tile_width;
	int tile_count_y = (image.height + tile_height - 1) / tile_height;
	const int total_tiles = tile_count_x * tile_count_y;

	Job_Queue queue = {};
	queue.ray_depth = 8;
	queue.samples_per_pixel = 32;
	queue.jobs = new Job[total_tiles];

	for (int tile_y = 0; tile_y < tile_count_y; tile_y++)
	{
		int y_min = tile_y*tile_height;
		int y_max = y_min + tile_height;
		if (y_max > image.height)
		{
			y_max = image.height;
		}
		for (int tile_x = 0; tile_x < tile_count_x; tile_x++)
		{
			int x_min = tile_x*tile_width;
			int x_max = x_min + tile_width;
			if (x_max > image.width)
			{
				x_max = image.width;
			}
			
			Job& job = *(queue.jobs + queue.jobs_count++);
			assert(queue.jobs_count <= total_tiles);

			job.image = &image;
			job.world = &world;
			job.camera = &camera;
			job.x_min = x_min;
			job.x_max = x_max;
			job.y_min = y_min;
			job.y_max = y_max;
		}
	}
	assert(queue.jobs_count == total_tiles);

	printf("Using %d cores, total tiles: %d, %dx%d (%dk/tile)\n", core_count, total_tiles, tile_count_x, tile_count_y, tile_width*tile_height * 4 / 1024);
	printf("Image quality: %dx%d pixels, %d samples per pixel, %d ray depth\n", image.width, image.height, queue.samples_per_pixel, queue.ray_depth);

	vector<thread> threads;
	for (int core_index = 1; core_index < core_count; core_index++)
	{
		threads.push_back(thread{do_work, ref(queue)});
	}

	// raycasting
	clock_t start = clock();
	while (queue.finished_jobs.load() < queue.jobs_count)
	{
		render_tile(queue);
	}
	clock_t end = clock();

	for (thread& t : threads)
	{
		t.join();
	}
	
	printf("\nRaycasting Done!\n");

	printf("Writing to file!\n");
	write_ppm("image.ppm", image);
	printf("Done\n");

	uint64_t time_elapsed = end - start;
	printf("Total time: %lld ms\n", time_elapsed);
	printf("Total bounces: %lld\n", queue.total_bounces.load());
	printf("Time per bounce: %f ms\n", float(time_elapsed) / float(queue.total_bounces.load()));

	// NOTE: Don't close file handle because the OS will do that for us when the program exits.

	return 0;
}