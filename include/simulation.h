#pragma once

#include <map>

#include <glew.h>
#include <glfw3.h>

#include "camera.h"
#include "inactivity_timer.h"
#include "maths.h"
#include "model.h"
#include "physics.h"
#include "renderer.h"
#include "types.h"
#include "ui.h"

using namespace maths;
using namespace utils;

class Simulation {
public:
	Simulation();

	void init();
	void update();
	void draw();
	void destroy();

	void update_simulation_transforms_from_physics();
	void update_sensors_from_simulation_transforms();
	void check_detected_walls();
	void check_detected_vehicles();
	void predator_prey();

	void add_vehicle(bool is_predator);
	void remove_vehicle();
	void remove_vehicle(int index);
	void reset();

	Cube_Renderer cube_renderer;
	Line_Renderer line_renderer;
	Quad_Renderer quad_renderer;
	Text_Renderer text_renderer;
	Model_Renderer model_renderer;
	Circle_Renderer circle_renderer;
	Triangle_Renderer tri_renderer;

	Model wheel_model;
	Model grid_model;

	Texture wheel_texture;
	Texture floor_texture;

	UI ui;
	Camera camera;
	Physics* physics;

	Inactivity_Timer inactivity_timer;

	bool mouse_pressed;
	bool is_updating;
	bool is_drawing;

	bool draw_sensors;
	bool draw_sensor_outlines;

	int index_state;
	int generation;

	vec2 cursor_position;

	// Environment Properties
	vector<Transform>			transforms_walls;
	vector<Transform>			transforms_boundaries;
	vector<Wheel_Attributes>	attributes_wheels;
	
	static int instance_id;

	// Vehicle Properties
	map<int, Vehicle_Attributes>	attributes_vehicles;
	map<int, Light>					lights;
	map<int, Vehicle_Sensors>		vehicle_sensors;
	map<int, Transform>				transforms_vehicles;
	map<int, Transform>				old_transforms_vehicles;
	map<int, vector<Transform>>		transforms_wheels;
};