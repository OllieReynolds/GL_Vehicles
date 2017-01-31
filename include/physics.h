#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#include <Box2D\Box2D.h>

#include "maths.h"
#include "utils.h"

using namespace maths;
using namespace std;

enum {
	LEFT = 0x1,
	RIGHT = 0X2,
	UP = 0x4,
	DOWN = 0x8
};

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

class Tyre {
public:
	Tyre(b2World* world, float max_forward_speed, float max_backward_speed, float max_drive_force, float max_lateral_impulse);

	b2Vec2 get_forward_velocity();
	b2Vec2 get_lateral_velocity();

	void destroy();
	void update_friction();
	void update_drive(int control_state);

	float max_forward_speed;
	float max_backward_speed;
	float max_drive_force;
	float max_lateral_impulse;

	b2Body* body;
};

class Vehicle {
public:
	Vehicle();

	void destroy();
	void init(b2World* world, b2Vec2 position, float rotation, int control_state);
	void update();

	std::vector<Tyre*> tyres;
	b2RevoluteJoint *fl_joint, *fr_joint;
	b2Body* body;
	float new_angle;
	int control_state;
};

class Physics {
public:
	Physics(int num_vehicles, std::vector<utils::Transform>& transforms);

	void				update();
	void				destroy();
	b2AABB				get_vehicle_AABB(int index);
	vec2				get_vehicle_position(int index);
	float				get_vehicle_rotation(int index);
	std::vector<vec2>	get_vehicle_positions();
	std::vector<float>	get_vehicle_rotations();
	void				add_vehicle(const utils::Transform& transform);
	void				remove_vehicle();

	b2Vec2 gravity;
	b2World world;
	b2Body *wall_1, *wall_2, *wall_3, *wall_4;
	vector<Vehicle> vehicles;
	float time_step;
	int velocity_iterations;
	int position_iterations;
};