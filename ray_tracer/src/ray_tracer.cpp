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
#include "material.h"
#include "camera.h"

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

v3f ray_cast(World& world, Ray r, int depth)
{
	if (depth <= 0)
	{
		return V3f(.0f, .0f, .0f);
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
		if (rec.mat->scatter(rec, r, attenuation))
		{
			return attenuation * ray_cast(world, r, depth - 1);
		}
		else
		{
			return V3f(.0f, .0f, .0f);
		}
	}
	else
	{
		return V3f(.5f, .7f, 1.0f);
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

				color += ray_cast(world, r, depth);

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

int main()
{
	// Image
	const float aspect_ratio = 16.0f / 9.0f;
	Image image = {};
	image.width = 1024;
	image.height = int(float(image.width) / aspect_ratio);
	image.pixels = (uint32_t*)malloc(image.width * image.height * sizeof(uint32_t));

	// World
	World world = {};

	auto material_center = make_shared<Lambertian>(V3f(.7f, .3f, .3f));
	auto material_left = make_shared<Metal>(V3f(.8f, .6f, .2f), .7f);
	auto material_right = make_shared<Metal>(V3f(.8f, .6f, .2f), .3f);
	auto material_ground = make_shared<Lambertian>(V3f(.52f, .59f, .68f));
	auto material_big_metal = make_shared<Metal>(V3f(0.2f, 0.25f, 0.7f), .0f);
	auto material_big_dielectric = make_shared<Dielectric>(1.7f);
	auto material_from_behind = make_shared<Lambertian>(V3f(1.0f, .0f, .0f));

	auto big_sphere_dielectric = make_shared<Sphere>(V3f(2.5f, 2.0f, -3.0f), 2.0f, material_big_dielectric);
	auto foo = make_shared<Sphere>(V3f(3.0f, .5f, -12.0f), .5f, material_from_behind);
	auto bar = make_shared<Sphere>(V3f(7.0f, .5f, -20.0f), .5f, material_from_behind);
	auto baz = make_shared<Sphere>(V3f(9.0f, .5f, -13.0f), .5f, material_from_behind);
	auto big_sphere_metal = make_shared<Sphere>(V3f(-2.5f, 2.0f, -3.0f), 2.0f, material_big_metal);
	auto center = make_shared<Sphere>(V3f(.0f, .5f, -1.0f), .5f, material_center);
	auto left = make_shared<Sphere>(V3f(-2.0f, .5f, .0f), .5f, material_left);
	auto right = make_shared<Sphere>(V3f(1.0f, .5f, 0.7f), .5f, material_right);
	auto ground = make_shared<Plane>(V3f(.0f, 1.0f, .0f), .0f, material_ground);

	world.add_object(center);
	world.add_object(left);
	world.add_object(right);
	world.add_object(ground);
	world.add_object(big_sphere_dielectric);
	world.add_object(foo);
	world.add_object(bar);
	world.add_object(baz);
	world.add_object(big_sphere_metal);

	// Camera
	Camera camera = Camera(aspect_ratio, 90.0f, V3f(.0f, .5f, 1.5f), V3f(.0f, .5f, -1.0f), V3f(.0f, 1.0f, .0f), .2f);
	
	// tile division
	int core_count = 4; // this should be an OS query
	int tile_width = image.width / core_count;
	int tile_height = tile_width;
	
	int tile_count_x = (image.width + tile_width - 1) / tile_width;
	int tile_count_y = (image.height + tile_height - 1) / tile_height;
	const int total_tiles = tile_count_x * tile_count_y;

	Job_Queue queue = {};
	queue.ray_depth = 8;
	queue.samples_per_pixel = 128;
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