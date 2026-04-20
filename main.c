#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "raylib.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define TARGET_FPS 120

#define PARTICLE_MIN_RADIUS 10
#define PARTICLE_MAX_RADIUS 10
#define PARTICLE_MAX_SPEED 100

#define NUM_INIT_PARTICLES 100
#define NUM_ADD_PARTICLES 100

typedef struct Particle
{
  float r;
  Vector2 c;

  Vector2 speed;
  Vector2 vel;
} Particle;

Particle* particles;

void init_particle(Particle* p)
{
  p->r = GetRandomValue(PARTICLE_MIN_RADIUS, PARTICLE_MAX_RADIUS);
  p->c = (Vector2){
    GetRandomValue(p->r, SCREEN_WIDTH - p->r),
    GetRandomValue(p->r, SCREEN_HEIGHT - p->r)
  };
  p->speed = (Vector2){
    .x = GetRandomValue(1, PARTICLE_MAX_SPEED),
    .y = GetRandomValue(1, PARTICLE_MAX_SPEED)
  };
  p->vel = (Vector2){
    .x = p->speed.x * (GetRandomValue(1, 2) % 2 == 0 ? 1 : -1),
    .y = p->speed.y * (GetRandomValue(1, 2) % 2 == 0 ? 1 : -1)
  };
}

void init_particles(int size)
{
  for (int i = 0; i < size; i++)
    init_particle(&particles[i]);
}

void add_particles(int* size)
{
  int new_size = *size + NUM_ADD_PARTICLES;
  Particle* tmp = realloc(particles, new_size * sizeof(Particle));

  if (!tmp) return;

  particles = tmp;

  for (int i = *size; i < new_size; i++)
    init_particle(&particles[i]);

  *size = new_size;
}

void delete_particles(int* size)
{
  int new_size = *size - NUM_ADD_PARTICLES;
  Particle* tmp = realloc(particles, new_size * sizeof(Particle));

  if (!tmp) return;

  particles = tmp;

  for (int i = *size; i < new_size; i++)
    init_particle(&particles[i]);

  *size = new_size;
}

void update_pos(int size)
{
  for (int i = 0; i < size; i++)
  {
    particles[i].c.x += particles[i].vel.x * GetFrameTime();
    particles[i].c.y += particles[i].vel.y * GetFrameTime();
  }
}

void boundary_coll(int size)
{
  for (int i = 0; i < size; i++)
  {
    // left boundary
    if (particles[i].c.x - particles[i].r < 0)
    {
      particles[i].c.x = particles[i].r;
      particles[i].vel.x *= -1; // flip the direction
    }
    // right boundary
    if (particles[i].c.x + particles[i].r > SCREEN_WIDTH)
    {
      particles[i].c.x = SCREEN_WIDTH - particles[i].r;
      particles[i].vel.x *= -1; // flip the direction
    }
    // upper boundary
    if (particles[i].c.y - particles[i].r < 0)
    {
      particles[i].c.y = particles[i].r;
      particles[i].vel.y *= -1; // flip the direction
    }
    // bottom boundary
    if (particles[i].c.y + particles[i].r > SCREEN_HEIGHT)
    {
      particles[i].c.y = SCREEN_HEIGHT - particles[i].r;
      particles[i].vel.y *= -1; // flip the direction
    }
  }
}

void particles_coll(int size)
{
  for (int i = 0; i < size; i++)
  {
    for (int j = i + 1; j < size; j++)
    {
      // distance between 2 particles
      float d = sqrtf(pow(particles[i].c.x - particles[j].c.x, 2) + pow(particles[i].c.y - particles[j].c.y, 2));
      // overlay disctance
      float d_overlay = particles[i].r + particles[j].r - d;

      if (d == 0) d = 1;

      if (d < particles[i].r + particles[j].r) {
        // move the particles half the overlay disctance
        particles[i].c.x += ceil(0.5f * d_overlay/d * (particles[i].c.x - particles[j].c.x));
        particles[i].c.y += ceil(0.5f * d_overlay/d * (particles[i].c.y - particles[j].c.y));
        particles[j].c.x += ceil(0.5f * d_overlay/d * (particles[j].c.x - particles[i].c.x));
        particles[j].c.y += ceil(0.5f * d_overlay/d * (particles[j].c.y - particles[i].c.y));

        // swap the x velocities
        float tmp_x = particles[i].vel.x;
        particles[i].vel.x = particles[j].vel.x;
        particles[j].vel.x = tmp_x;

        // swap the y velocities
        float tmp_y = particles[i].vel.y;
        particles[i].vel.y = particles[j].vel.y;
        particles[j].vel.y = tmp_y;
      }
    }
  }
}

void draw_particles(int size)
{
  for (int i = 0; i < size; i++)
  {
    DrawCircleV(particles[i].c, particles[i].r, BLUE);
  }
}

int main()
{
  SetRandomSeed(time(NULL));

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Particle Simulation");

  int num_particles = NUM_INIT_PARTICLES;

  particles = (Particle*)malloc(num_particles*sizeof(Particle));
  init_particles(num_particles);

  SetTargetFPS(TARGET_FPS);
  // main loop
  while (!WindowShouldClose())
  {
    float k = (float)GetFPS()/TARGET_FPS;

    if (IsKeyPressed(KEY_UP))
      add_particles(&num_particles);
    if (IsKeyPressed(KEY_DOWN) && num_particles > NUM_INIT_PARTICLES)
      delete_particles(&num_particles);
    update_pos(num_particles);
    boundary_coll(num_particles);
    particles_coll(num_particles);


    // drawing section
    BeginDrawing();

      ClearBackground(RAYWHITE);
      draw_particles(num_particles);
      DrawText(TextFormat("%d", GetFPS()), SCREEN_WIDTH - 40, 10, 20, (Color){255*(1-k), 255*k, 0, 255});
      DrawText(TextFormat("Particles: %d", num_particles), 10, 10, 20, RED);

    EndDrawing();
  }
  free(particles);
  CloseWindow();
}
