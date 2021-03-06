#include "..\include\simulation.h"

int Simulation::instance_id = 0;

Simulation::Simulation() {
	index_state = 1;
	generation = 0;

	mouse_pressed = false;
	is_updating = false;
	is_drawing = true;
	draw_sensors = true;
	draw_sensor_outlines = true;

	ui = UI(camera);
	text_renderer = Text_Renderer(static_cast<int>(camera.resolution.x / 60.f), "data/OpenSans-Regular.ttf");


	// Constructor of Physics makes a ton of objects, but add_vehicle is doing this as well. Works if physics initialised before, but is wrong.
	{
		// Init Physics
		physics = new Physics(transforms_vehicles.size(), transforms_vehicles, attributes_vehicles);

		// Init Vehicles
		for (int i = 0; i < 10; i++) {
			bool is_predator = i % 2 == 0;
			add_vehicle(is_predator);
		}
	}

	attributes_wheels = vector<Wheel_Attributes>(4) = { { 315.f, 0.f },{ 225.f, 0.f },{ 45.f, 180.f },{ 135.f, 180.f } };

	// Init Walls
	transforms_walls = vector<Transform>(5);
	transforms_walls[0] = { vec3{    0.f, 200.f,  400.f }, vec3{ 40.f }, vec3{  270.f, 0.f,   0.f } };
	transforms_walls[1] = { vec3{    0.f, 200.f, -400.f }, vec3{ 40.f }, vec3{ -270.f, 0.f,   0.f } };
	transforms_walls[2] = { vec3{  400.f, 200.f,    0.f }, vec3{ 40.f }, vec3{    0.f, 0.f,  90.f } };
	transforms_walls[3] = { vec3{ -400.f, 200.f,    0.f }, vec3{ 40.f }, vec3{    0.f, 0.f, -90.f } };
	transforms_walls[4] = { vec3{    0.f,   0.f,    0.f }, vec3{ 40.f }, vec3{    0.f, 0.f,   0.f } };

	// Init Boundaries 
	transforms_boundaries = vector<Transform>(4);
	transforms_boundaries[0] = { vec3{ physics->wall_1->body->GetPosition().x, 10.f, physics->wall_1->body->GetPosition().y }, vec3{ 4.f, 780.f, 0.f }, vec3{ 90.f, box2d_to_simulation_angle(physics->wall_1->body->GetAngle()), 0.f } };
	transforms_boundaries[1] = { vec3{ physics->wall_2->body->GetPosition().x, 10.f, physics->wall_2->body->GetPosition().y }, vec3{ 4.f, 780.f, 0.f }, vec3{ 90.f, box2d_to_simulation_angle(physics->wall_2->body->GetAngle()), 0.f } };
	transforms_boundaries[2] = { vec3{ physics->wall_3->body->GetPosition().x, 10.f, physics->wall_3->body->GetPosition().y }, vec3{ 4.f, 780.f, 0.f }, vec3{  0.f, box2d_to_simulation_angle(physics->wall_3->body->GetAngle()), 90.f } };
	transforms_boundaries[3] = { vec3{ physics->wall_4->body->GetPosition().x, 10.f, physics->wall_4->body->GetPosition().y }, vec3{ 4.f, 780.f, 0.f }, vec3{  0.f, box2d_to_simulation_angle(physics->wall_4->body->GetAngle()), 90.f } };

	// Init Lights
	lights = map<int, Light>();
	for (map<int, Transform>::iterator it = transforms_vehicles.begin(); it != transforms_vehicles.end(); ++it) {
		lights.insert(pair<int, Light>(it->first, { { 0.f, 30.f, 0.f }, attributes_vehicles[it->first].colour.XYZ(), 1.f }));
	}
}

void Simulation::init() {
	wheel_texture.init("data/wheel_texture.png");
	floor_texture.init("data/floor.png");

	wheel_model.init("data/wheel.obj");
	grid_model.init("data/grid.obj");

	cube_renderer.init();
	line_renderer.init();
	quad_renderer.init();
	text_renderer.init(camera.resolution);
	circle_renderer.init();
	model_renderer.init();
	tri_renderer.init();

	inactivity_timer.init(transforms_vehicles);
}

void Simulation::update() {

	// Check collision events and remove/add any eligible vehicles
	{
		vector<int> remove_indices;
		for (pair<VehicleData*, VehicleData*> e : vehicle_collision_events) {
			if ((e.first->is_predator && !e.second->is_predator) || (!e.first->is_predator && e.second->is_predator)) {
				int index = -1;
				if (e.first->is_predator) {
					attributes_vehicles[e.first->instance_id].energy = 100.f;
					remove_indices.push_back( e.second->instance_id);
				}
				else {
					attributes_vehicles[e.second->instance_id].energy = 100.f;
					remove_indices.push_back(e.first->instance_id);
				}
			}
		}

		vehicle_collision_events.clear();

		for (uint8 i = 0; i < remove_indices.size(); i++) {
			bool is_predator = true;
			if (attributes_vehicles[remove_indices[i]].is_predator)
				is_predator = false;

			remove_vehicle(remove_indices[i]);
			add_vehicle(is_predator);
		}
	}

	for (map<int, Transform>::iterator it = transforms_vehicles.begin(); it != transforms_vehicles.end(); ++it) {
		lights[it->first].position = it->second.position;
		lights[it->first].intensity = attributes_vehicles[it->first].energy * 0.01f;
	}

	if (is_updating) {
		// Update Physics
		physics->update();

		// Vehicles Transforms
		old_transforms_vehicles = transforms_vehicles;
		update_simulation_transforms_from_physics();
		update_sensors_from_simulation_transforms();

		check_detected_vehicles();
		//check_detected_walls();
		predator_prey();

		vector<int> remove_ids;
		for (map<int, Vehicle_Attributes>::iterator it = attributes_vehicles.begin(); it != attributes_vehicles.end(); ++it) {
			Vehicle_Attributes& tmp = it->second;

			if (!almost_equal(transforms_vehicles[it->first].position.XZ(), old_transforms_vehicles[it->first].position.XZ(), 1.f)) {
				tmp.energy -= 0.15f;
			}

			tmp.energy -= (tmp.is_predator) ? 0.1f : 0.05f;

			if (tmp.energy < 0.f) {
				remove_ids.push_back(it->first);
			}
		}

		for (uint8 i = 0; i < remove_ids.size(); i++) {
			bool is_predator = true;
			if (attributes_vehicles[remove_ids[i]].is_predator)
				is_predator = false;
			remove_vehicle(remove_ids[i]);
			add_vehicle(is_predator);
		}
		

		if (transforms_vehicles.size() >= 2) {
			inactivity_timer.update(transforms_vehicles);
			if (inactivity_timer.remaining_milliseconds < 0.f) {
				reset();
				is_updating = true;
			}
		}
	}

	ui.update(cursor_position, mouse_pressed);
	camera.update(transforms_vehicles);

	mouse_pressed = false;
}

void Simulation::draw() {
	if (is_drawing) {
		glEnable(GL_DEPTH_TEST);
		{
			// Walls & Floor
			model_renderer.draw_multiple_3D_textured(transforms_walls.size(), grid_model, camera, transforms_walls, floor_texture, lights);

			// Boundaries
			vec4 c = { 0.2f, 0.3f, 0.2f, 1.f };
			quad_renderer.draw_multiple_3D_coloured(camera, transforms_boundaries, c);

			if (!transforms_vehicles.empty()) {
				// Vehicles
				cube_renderer.draw_multiple(camera, transforms_vehicles, attributes_vehicles, lights);

				// Wheels
				model_renderer.draw_multiple_3D_textured(transforms_wheels.size(), wheel_model, camera, transforms_wheels, wheel_texture, lights);
			}
		}

	
		glEnable(GL_BLEND);
		{
			if (!transforms_vehicles.empty()) {
				// Vehicle Sensors
				for (map<int, Vehicle_Sensors>::iterator it = vehicle_sensors.begin(); it != vehicle_sensors.end(); ++it) {

					float alpha = ((attributes_vehicles[it->first].energy * 0.1f) * 0.01f);

					Vehicle_Sensors& tmp = it->second;

					if (draw_sensors) {
						tri_renderer.draw_3D_coloured(camera, tmp.la, tmp.lb, tmp.lc, vec4{ attributes_vehicles[it->first].colour.XYZ(), alpha });
						tri_renderer.draw_3D_coloured(camera, tmp.ra, tmp.rb, tmp.rc, vec4{ attributes_vehicles[it->first].colour.XYZ(), alpha });
					}

					float l_alpha = alpha * 5.f;
					if (draw_sensor_outlines) {
						line_renderer.draw_lineloop(camera, { tmp.la, tmp.lb, tmp.lc, }, vec4{ attributes_vehicles[it->first].colour.XYZ(), l_alpha });
						line_renderer.draw_lineloop(camera, { tmp.ra, tmp.rb, tmp.rc, }, vec4{ attributes_vehicles[it->first].colour.XYZ(), l_alpha });
					}
				}
			}
		}

		glDisable(GL_DEPTH_TEST);
		{
			// UI
			if (ui.index_active_button != -1)
				quad_renderer.draw_2D(camera, ui.attributes_ui[ui.index_active_button].position, ui.attributes_ui[ui.index_active_button].size * 1.1f, utils::colour::yellow);
			for (size_t i = 0; i < ui.attributes_ui.size(); i++) {
				Button_Attributes& tmp = ui.attributes_ui[i];
				quad_renderer.draw_2D(camera, tmp.position, tmp.size, tmp.colour);
				text_renderer.draw(tmp.label, tmp.position + vec2{ 0.f, -10.f }, true, colour::white);
			}

			int num_predators = 0;
			int num_prey = 0;
			for (map<int, Vehicle_Attributes>::iterator it = attributes_vehicles.begin(); it != attributes_vehicles.end(); ++it) {
				attributes_vehicles[it->first].is_predator ? num_predators++ : num_prey++;
			}

			// Side Menu
			{
				float text_x = camera.resolution.x * 0.04685212298f;
				float text_y = camera.resolution.y * 0.85f;
				float text_y_offset = 30.f;

				// Draw details about the predator/prey scenario
				quad_renderer.draw_2D(camera, { camera.resolution.x * 0.125f, camera.resolution.y / 2.f }, { camera.resolution.x * 0.2f, camera.resolution.y * 0.8f }, { 0.f, 0.f, 0.f, 0.7f });
				string gen =           "Generation:      " + to_string(generation);
				string population =	   "Population:      " + to_string(transforms_vehicles.size());
				string predator_prey = "Predator/Prey: " + to_string(num_predators) + "/" + to_string(num_prey);
				text_renderer.draw("SIM INFO",		{ text_x, text_y - (text_y_offset * 0.f) }, false, utils::colour::yellow);
				text_renderer.draw(gen,				{ text_x, text_y - (text_y_offset * 1.f) }, false, utils::colour::white);
				text_renderer.draw(population,		{ text_x, text_y - (text_y_offset * 2.f) }, false, utils::colour::white);
				text_renderer.draw(predator_prey,	{ text_x, text_y - (text_y_offset * 3.f) }, false, utils::colour::white);

				// Draw energy levels to UI
				int y_offset_multiplier = 5;
				text_renderer.draw("ENERGY LEVELS", { text_x, text_y - (text_y_offset * y_offset_multiplier++) }, false, utils::colour::yellow);
				if (attributes_vehicles.empty()) {
					text_renderer.draw("No vehicles running", { text_x, text_y - (text_y_offset * y_offset_multiplier++) }, false, utils::colour::grey);
				} else {
					for (map<int, Vehicle_Attributes>::iterator it = attributes_vehicles.begin(); it != attributes_vehicles.end(); ++it) {
						if (y_offset_multiplier > 14) {
							text_renderer.draw("<more...>", { text_x, text_y - (text_y_offset * y_offset_multiplier++) }, false, utils::colour::white);
							break;
						}
						else {
							Vehicle_Attributes& tmp = it->second;
							string str_i = friendly_float(tmp.id, 3);
							string str_energy = friendly_float(tmp.energy, 4);
							string display = "Vehicle " + str_i + ": " + str_energy;
							vec4 colour = tmp.colour;
							colour += vec4(0.25f);
							text_renderer.draw(display, { text_x, text_y - (text_y_offset * y_offset_multiplier++) }, false, colour);
						}
					}
				}

				// Draw progress of inactivity tracker
				y_offset_multiplier++;
				string str_timer = (friendly_float(inactivity_timer.remaining_milliseconds, 4));
				text_renderer.draw("INACTIVITY TRACKER", { text_x, text_y - (text_y_offset * y_offset_multiplier++) }, false, utils::colour::yellow);
				text_renderer.draw("Remaining: " + str_timer, { text_x, text_y - (text_y_offset * y_offset_multiplier++) }, false, utils::colour::white);
			}
		}

		glDisable(GL_BLEND);
	}
}

void Simulation::destroy() {
	cube_renderer.destroy();
	line_renderer.destroy();
	quad_renderer.destroy();
	text_renderer.destroy();
	circle_renderer.destroy();
	model_renderer.destroy();
	tri_renderer.destroy();

	wheel_texture.destroy();
	floor_texture.destroy();

	wheel_model.destroy();
	grid_model.destroy();

	physics->destroy();
	delete physics;
}

// Should have another version for random selection
void Simulation::add_vehicle(bool is_predator) {
	vec2 rand_pos = { utils::gen_random(-320.f, 320.f),  utils::gen_random(-320.f, 320.f) };

	Transform t = {
		vec3{ rand_pos.x, 4.f, rand_pos.y },
		vec3{ 20.f, 2.f, 16.f },
		vec3{ 0.f, utils::gen_random(0.f, 360.f), 0.f }
	};


	int key = instance_id++;

	Vehicle_Attributes av = {
		utils::gen_random(0.2f, 0.5f),
		utils::gen_random(2.5f, 3.5f),
		(is_predator) ? utils::colour::red : utils::colour::blue,
		is_predator,
		100.f,
		key
	};

	transforms_vehicles.insert(pair<int, Transform>(key, t));
	attributes_vehicles.insert(pair<int, Vehicle_Attributes>(key, av));
	lights.insert(pair<int, Light>(key, { { 0.f, 30.f, 0.f }, av.colour.XYZ(), 1.f }));
	transforms_wheels.insert(pair<int, vector<Transform>>(key, { {}, {}, {}, {} }));

	float SENSOR_ANGLE = utils::gen_random(40.f, 120.f);
	float SENSOR_OFFSET = utils::gen_random(0.f, 40.f);
	float SENSOR_RANGE = utils::gen_random(200.f, 500.f);
	{
		float y = t.position.y - 6.f + ((vehicle_sensors.size() + 1) * 0.8f);

		float _a = t.rotation.y - SENSOR_OFFSET;
		vec2 a_left = polar_to_cartesian(to_radians(_a - SENSOR_ANGLE / 2.f)) * SENSOR_RANGE;
		vec2 a_right = polar_to_cartesian(to_radians(_a + SENSOR_ANGLE / 2.f)) * SENSOR_RANGE;
		vec3 la = t.position + vec3{ a_left.x, y, a_left.y };
		vec3 lb = t.position + vec3{ 0.f, y, 0.f };
		vec3 lc = t.position + vec3{ a_right.x, y, a_right.y };

		y = t.position.y - 6.f + ((vehicle_sensors.size() + 2) * 0.8f);

		float _b = t.rotation.y + SENSOR_OFFSET;
		vec2 b_left = polar_to_cartesian(to_radians(_b - SENSOR_ANGLE / 2.f)) * SENSOR_RANGE;
		vec2 b_right = polar_to_cartesian(to_radians(_b + SENSOR_ANGLE / 2.f)) * SENSOR_RANGE;
		vec3 ra = t.position + vec3{ b_left.x, y, b_left.y };
		vec3 rb = t.position + vec3{ 0.f, y, 0.f };
		vec3 rc = t.position + vec3{ b_right.x, y, b_right.y };

		vehicle_sensors.insert(pair<int, Vehicle_Sensors>(key, { la, lb, lc, ra, rb, rc, SENSOR_ANGLE, SENSOR_OFFSET, SENSOR_RANGE, {} }));
	}

	physics->add_vehicle(key, t, is_predator);
}

void Simulation::remove_vehicle() {
	if (!transforms_vehicles.empty()) {
		transforms_vehicles.erase(--transforms_vehicles.end());
		attributes_vehicles.erase(--attributes_vehicles.end());
		transforms_wheels.erase(--transforms_wheels.end());
		vehicle_sensors.erase(--vehicle_sensors.end());
		lights.erase(--lights.end());
	
		physics->remove_vehicle();

		if (transforms_vehicles.empty()) {
			camera.follow_vehicle = false;
			is_updating = false;
		}
	}
}

void Simulation::remove_vehicle(int instance_id) {
	transforms_vehicles.erase(instance_id);
	attributes_vehicles.erase(instance_id);
	transforms_wheels.erase(instance_id);
	vehicle_sensors.erase(instance_id);
	lights.erase(instance_id);
	
	physics->remove_vehicle(instance_id);

	if (transforms_vehicles.empty())
		camera.follow_vehicle = false;
}

void Simulation::reset() {
	generation++;
	instance_id = 0;
	inactivity_timer.reset();
	size_t n = transforms_vehicles.size();

	bool camera_follow = camera.follow_vehicle;

	for (size_t i = 0; i < n; i++)
		remove_vehicle();
	for (size_t i = 0; i < n; i++) {
		bool is_predator = (i % 2 == 0);
		add_vehicle(is_predator);
	}

	camera.follow_vehicle = camera_follow;
}

void Simulation::check_detected_walls() {
	for (map<int, Transform>::iterator it = transforms_vehicles.begin(); it != transforms_vehicles.end(); ++it) {
		
		int instance_id = it->first;

		// Move wall related data to its own source file
		static const vec2 p0 = { -400.f, -400.f };
		static const vec2 p1 = { -400.f,  400.f };
		static const vec2 p2 = { 400.f,  400.f };
		static const vec2 p3 = { 400.f, -400.f };

		vec2 a = vehicle_sensors[instance_id].la.XZ();
		vec2 b = vehicle_sensors[instance_id].lb.XZ();
		vec2 c = vehicle_sensors[instance_id].lc.XZ();

		if (   utils::shared::Intersecting(p0, p1, a, b, c) 
			|| utils::shared::Intersecting(p1, p2, a, b, c) 
			|| utils::shared::Intersecting(p2, p3, a, b, c) 
			|| utils::shared::Intersecting(p3, p0, a, b, c)) 
		{
			vehicle_sensors[instance_id].detection_events.push_back({0.f, true, false, false, false, true});
		}


		a = vehicle_sensors[instance_id].ra.XZ();
		b = vehicle_sensors[instance_id].rb.XZ();
		c = vehicle_sensors[instance_id].rc.XZ();

		if (   utils::shared::Intersecting(p0, p1, a, b, c)
			|| utils::shared::Intersecting(p1, p2, a, b, c)
			|| utils::shared::Intersecting(p2, p3, a, b, c)
			|| utils::shared::Intersecting(p3, p0, a, b, c))
		{
			vehicle_sensors[instance_id].detection_events.push_back({ 0.f, false, true, false, false, true });
		}
	}
}

void Simulation::update_simulation_transforms_from_physics() {
	for (map<int, Transform>::iterator it = transforms_vehicles.begin(); it != transforms_vehicles.end(); ++it) {
		int instance_key = it->first;
		it->second.rotation.y = physics->get_vehicle_rotation(instance_key) + 90.f;
		it->second.position = vec3{ physics->get_vehicle_position(instance_key).x, 4.f, physics->get_vehicle_position(instance_key).y };

		for (int i = 0; i < 4; i++) {
			transforms_wheels[instance_key][i] = attributes_wheels[i].gen_transform_from_vehicle(physics->vehicles[instance_key].body->GetLinearVelocity(), it->second, 8.f);
		}
	}
}

void Simulation::update_sensors_from_simulation_transforms() {
	int sensor_num = 0;
	for (map<int, Transform>::iterator it = transforms_vehicles.begin(); it != transforms_vehicles.end(); ++it) {
		int instance_key = it->first;
		Transform& tmp = it->second;

		float y = tmp.position.y - 6.f + (sensor_num++ * .8f);
		float a = tmp.rotation.y -  vehicle_sensors[instance_key].offset;
		vec2 a_left = polar_to_cartesian(to_radians(a - vehicle_sensors[instance_key].angle / 2.F)) * vehicle_sensors[instance_key].range;
		vec2 a_right = polar_to_cartesian(to_radians(a + vehicle_sensors[instance_key].angle / 2.F)) * vehicle_sensors[instance_key].range;
		vehicle_sensors[instance_key].la = tmp.position + vec3{ a_left.x, y, a_left.y };
		vehicle_sensors[instance_key].lb = tmp.position + vec3{ 0.f, y, 0.f };
		vehicle_sensors[instance_key].lc = tmp.position + vec3{ a_right.x, y, a_right.y };

		y = tmp.position.y - 6.f + (sensor_num++ * .8f);
		float b = tmp.rotation.y + vehicle_sensors[instance_key].offset;
		vec2 b_left = polar_to_cartesian(to_radians(b - vehicle_sensors[instance_key].angle / 2.F)) * vehicle_sensors[instance_key].range;
		vec2 b_right = polar_to_cartesian(to_radians(b + vehicle_sensors[instance_key].angle / 2.F)) * vehicle_sensors[instance_key].range;
		vehicle_sensors[instance_key].ra = tmp.position + vec3{ b_left.x, y, b_left.y };
		vehicle_sensors[instance_key].rb = tmp.position + vec3{ 0.f, y, 0.f };
		vehicle_sensors[instance_key].rc = tmp.position + vec3{ b_right.x, y, b_right.y };
	}
}

void Simulation::check_detected_vehicles() {
	const static float HITBOX_SIZE = 10.f;


	for (map<int, Transform>::iterator i = transforms_vehicles.begin(); i != transforms_vehicles.end(); ++i) {
		for (map<int, Transform>::iterator j = transforms_vehicles.begin(); j != transforms_vehicles.end(); ++j) {
			int id_i = i->first;
			int id_j = j->first;
			
			if (id_i != id_j) {

				vec2 p = j->second.position.XZ();
				vec2 p1 = p + vec2{ -HITBOX_SIZE, -HITBOX_SIZE };
				vec2 p2 = p + vec2{  HITBOX_SIZE, -HITBOX_SIZE };
				vec2 p3 = p + vec2{ -HITBOX_SIZE,  HITBOX_SIZE };
				vec2 p4 = p + vec2{  HITBOX_SIZE,  HITBOX_SIZE };

				bool ldetected = false;
				bool rdetected = false;

				vec2 a = vehicle_sensors[id_i].la.XZ();
				vec2 b = vehicle_sensors[id_i].lb.XZ();
				vec2 c = vehicle_sensors[id_i].lc.XZ();
				if (point_triangle_intersect(p1, a, b, c) || point_triangle_intersect(p2, a, b, c) ||
					point_triangle_intersect(p3, a, b, c) || point_triangle_intersect(p4, a, b, c))
				{

					if ((attributes_vehicles[id_i].is_predator && !attributes_vehicles[id_j].is_predator) || (!attributes_vehicles[id_i].is_predator && attributes_vehicles[id_j].is_predator))
						ldetected = true;
				}

				a = vehicle_sensors[id_i].ra.XZ();
				b = vehicle_sensors[id_i].rb.XZ();
				c = vehicle_sensors[id_i].rc.XZ();
				if (point_triangle_intersect(p1, a, b, c) || point_triangle_intersect(p2, a, b, c) ||
					point_triangle_intersect(p3, a, b, c) || point_triangle_intersect(p4, a, b, c))
				{
					if ((attributes_vehicles[id_i].is_predator && !attributes_vehicles[id_j].is_predator) || (!attributes_vehicles[id_i].is_predator && attributes_vehicles[id_j].is_predator))
						rdetected = true;
				}

				if (ldetected || rdetected) {
					float dist = distance(i->second.position, j->second.position);
					vehicle_sensors[id_i].detection_events.push_back({dist, ldetected, rdetected, attributes_vehicles[id_j].is_predator, !attributes_vehicles[id_j].is_predator, false });
				}
			}
		}
	}
}

void Simulation::predator_prey() {
	for (map<int, Transform>::iterator it = transforms_vehicles.begin(); it != transforms_vehicles.end(); ++it) {

		int instance_id = it->first;
		Vehicle& tmp_vehicle = physics->vehicles[instance_id];
		Vehicle_Sensors& tmp_sensor = vehicle_sensors[instance_id];

		if (!tmp_sensor.detection_events.empty()) {

			int index_of_event_with_closest_distance = 0;
			float closest_distance = FLT_MAX;
			for (uint8 j = 0; j < tmp_sensor.detection_events.size(); j++) {
				if (tmp_sensor.detection_events[j].distance < closest_distance) {
					closest_distance = tmp_sensor.detection_events[j].distance;
					index_of_event_with_closest_distance = j;
				}
			}
			Detection_Event e = tmp_sensor.detection_events[index_of_event_with_closest_distance];

			if (attributes_vehicles[instance_id].is_predator) {
				if (e.detected_prey) {
					if (e.ldetected && e.rdetected)  {
						tmp_vehicle.desired_speed = 100;
					}
					else if (e.ldetected) {
						tmp_vehicle.desired_speed = 100;
						tmp_vehicle.desired_angle = -70;
					}
					else if (e.rdetected) {
						tmp_vehicle.desired_speed = 100;
						tmp_vehicle.desired_angle = 70;
					}
				}
				else {
					tmp_vehicle.desired_speed = 0;
					tmp_vehicle.desired_angle = 0;
				}
			}
			else {
				if (e.detected_predator) {
					if (e.ldetected && e.rdetected) {
						tmp_vehicle.desired_speed = -100;
					}
					else if (e.ldetected) {
						tmp_vehicle.desired_speed = -100;
						tmp_vehicle.desired_angle = 70;
					}
					else if (e.rdetected) {
						tmp_vehicle.desired_speed = -100;
						tmp_vehicle.desired_angle = -70;
					}
				}
				else {
					tmp_vehicle.desired_speed = 0;
					tmp_vehicle.desired_angle = 0;
				}
			}

			tmp_sensor.detection_events.clear();
		}
		else {
			tmp_vehicle.desired_speed = 0;
			tmp_vehicle.desired_angle = 0;
		}
	}
}