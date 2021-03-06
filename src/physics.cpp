#include "..\include\physics.h"

set<pair<VehicleData*, VehicleData*>> vehicle_collision_events;

Boundary::Boundary(b2World* world, const b2Vec2& position, float angle) {
	polygon_shape.SetAsBox(4.f, 800.f);
	body_def.type = b2_staticBody;
	body_def.position = position;
	body_def.angle = to_radians(angle);
	body = world->CreateBody(&body_def);
	b2Fixture* f = body->CreateFixture(&polygon_shape, 10.f);
	b2Filter filter;
	filter.categoryBits = ENV;
	filter.maskBits = TYRE | VEHICLE;
	filter.groupIndex = 0;
	f->SetFilterData(filter);
}

void Boundary::destroy() {
	body->GetWorld()->DestroyBody(body);
}

Tyre::Tyre(b2World* world, float max_forward_speed, float max_backward_speed, float max_drive_force, float max_lateral_impulse) 
	: max_forward_speed(max_forward_speed), max_backward_speed(max_backward_speed), max_drive_force(max_drive_force), max_lateral_impulse(max_lateral_impulse) 
{
	b2BodyDef body_def;
	body_def.type = b2_dynamicBody;
	body = world->CreateBody(&body_def);

	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(0.4, 0.4);
	b2Fixture* f = body->CreateFixture(&polygonShape, 1.f);
	
	
	b2Filter filter;
	filter.categoryBits = TYRE;
	filter.maskBits = ENV;
	filter.groupIndex = 0;

	f->SetFilterData(filter);

	body->SetUserData(this);
}

void Tyre::destroy() {
	body->GetWorld()->DestroyBody(body);
}

b2Vec2 Tyre::get_forward_velocity() {
	b2Vec2 forward_normal = body->GetWorldVector(b2Vec2(0, 1));
	return b2Dot(forward_normal, body->GetLinearVelocity()) * forward_normal;
}

b2Vec2 Tyre::get_lateral_velocity() {
	b2Vec2 right_normal = body->GetWorldVector(b2Vec2(1, 0));
	return b2Dot(right_normal, body->GetLinearVelocity()) * right_normal;
}

void Tyre::update_friction() {
	b2Vec2 impulse = body->GetMass() * -get_lateral_velocity();
	if (impulse.Length() > max_lateral_impulse) {
		impulse *= max_lateral_impulse / impulse.Length();
	}
	body->ApplyLinearImpulse(impulse, body->GetWorldCenter(), true);
	body->ApplyAngularImpulse(.1f * body->GetInertia() * -body->GetAngularVelocity(), true);

	b2Vec2 forward_normal = get_forward_velocity();
	float forward_speed = forward_normal.Normalize();
	float drag_magnitude = -2 * forward_speed;
	body->ApplyForce(drag_magnitude * forward_normal, body->GetWorldCenter(), true);
}

void Tyre::update_drive(float desired_speed) {

	if (desired_speed > max_forward_speed)
		desired_speed = max_forward_speed;
	else if (desired_speed < max_backward_speed)
		desired_speed = max_backward_speed;

	b2Vec2 forward_normal = body->GetWorldVector(b2Vec2(0, 1));
	float current_speed = b2Dot(get_forward_velocity(), forward_normal);

	float force = 0;

	if (desired_speed > current_speed)
		force = max_drive_force;
	else if (desired_speed < current_speed)
		force = -max_drive_force;
	else
		return;

	body->ApplyForce(force * forward_normal, body->GetWorldCenter(), true);
}

Vehicle::Vehicle() {
	desired_angle = 0; // 70
	desired_speed = 0; // 100
}

void Vehicle::destroy() {
	for (int i = 0; i < tyres.size(); i++) {
		tyres[i]->destroy();
		delete tyres[i];
	}

	body->GetWorld()->DestroyBody(body);
	delete data;
}


void Vehicle::init(b2World* world, b2Vec2 position, float rotation, bool is_predator, int index) {
	this->is_predator = is_predator;

	b2BodyDef body_def;
	body_def.type = b2_dynamicBody;
	body_def.position.Set(position.x, position.y);
	body = world->CreateBody(&body_def);
	body->SetAngularDamping(2);

	b2PolygonShape polygon_shape;
	polygon_shape.SetAsBox(8.f, 10.f);
	b2Fixture* fixture = body->CreateFixture(&polygon_shape, 0.1f);

	data = new VehicleData;
	data->instance_id = index;
	data->is_predator = is_predator;
	fixture->SetUserData((VehicleData*)data);

	b2Filter filter;
	filter.categoryBits = VEHICLE;
	filter.maskBits = VEHICLE;
	filter.groupIndex = 0;
	fixture->SetFilterData(filter);

	b2RevoluteJointDef joint_def;
	joint_def.bodyA = body;
	joint_def.enableLimit = true;
	joint_def.lowerAngle = 0;
	joint_def.upperAngle = 0;
	joint_def.localAnchorB.SetZero();

	float max_forward_speed = utils::gen_random(50.f, 450.f);
	float max_backward_speed = -utils::gen_random(50.f, 450.f);
	float back_tyre_max_drive_force = utils::gen_random(100.f, 400.f);
	float front_tyre_max_drive_force = utils::gen_random(100.f, 400.f);
	float back_tyre_max_lateral_impulse = utils::gen_random(10.f, 60.f);
	float front_tyre_max_lateral_impulse = utils::gen_random(10.f, 60.f);

	// Back Left
	Tyre* tyre = new Tyre(world, max_forward_speed, max_backward_speed, back_tyre_max_drive_force, back_tyre_max_lateral_impulse);
	joint_def.bodyB = tyre->body;
	joint_def.localAnchorA.Set(-13.f, 0.75f);
	world->CreateJoint(&joint_def);
	tyres.push_back(tyre);

	// Back Right
	tyre = new Tyre(world, max_forward_speed, max_backward_speed, back_tyre_max_drive_force, back_tyre_max_lateral_impulse);
	joint_def.bodyB = tyre->body;
	joint_def.localAnchorA.Set(13.f, 0.75f);
	world->CreateJoint(&joint_def);
	tyres.push_back(tyre);

	// Front Left
	tyre = new Tyre(world, max_forward_speed, max_backward_speed, front_tyre_max_drive_force, front_tyre_max_lateral_impulse);
	joint_def.bodyB = tyre->body;
	joint_def.localAnchorA.Set(-13.f, 8.5f);
	fl_joint = (b2RevoluteJoint*)world->CreateJoint(&joint_def);
	tyres.push_back(tyre);

	// Front Right
	tyre = new Tyre(world, max_forward_speed, max_backward_speed, front_tyre_max_drive_force, front_tyre_max_lateral_impulse);
	joint_def.bodyB = tyre->body;
	joint_def.localAnchorA.Set(13.f, 8.5f);
	fr_joint = (b2RevoluteJoint*)world->CreateJoint(&joint_def);
	tyres.push_back(tyre);
}

void Vehicle::update() {
	for (int i = 0; i < tyres.size(); i++)
		tyres[i]->update_friction();

	for (int i = 0; i < tyres.size(); i++)
		tyres[i]->update_drive(desired_speed);


	float angle = desired_angle * DEGTORAD;
	float turn_speed_per_second = 160 * DEGTORAD;
	float turn_per_time_step = turn_speed_per_second / 60.f;
	float angle_to_turn = angle - fl_joint->GetJointAngle();
	angle_to_turn = b2Clamp(angle_to_turn, -turn_per_time_step, turn_per_time_step);
	new_angle = fl_joint->GetJointAngle() + angle_to_turn;
	fl_joint->SetLimits(new_angle, new_angle);
	fr_joint->SetLimits(new_angle, new_angle);
}

void ContactListener::BeginContact(b2Contact* contact) {

	b2Filter fA = contact->GetFixtureA()->GetFilterData();
	b2Filter fB = contact->GetFixtureB()->GetFilterData();

	if (fA.categoryBits == VEHICLE && fB.categoryBits == VEHICLE) {

		VehicleData* dA = (VehicleData*)contact->GetFixtureA()->GetUserData(); 
		VehicleData* dB = (VehicleData*)contact->GetFixtureB()->GetUserData();

		vehicle_collision_events.insert(pair<VehicleData*, VehicleData*>(dA, dB));
	}
		
}

Physics::Physics(int num_vehicles, std::map<int, Transform>& transforms, std::map<int, Vehicle_Attributes>& v_attribs)
	: gravity{ 0.f, 0.f }, world(gravity), velocity_iterations(12), position_iterations(12), time_step(1.f / 30.f) 
{
	vehicles = map<int, Vehicle>();
	for (int i = 0; i < num_vehicles; i++)
		vehicles[i].init(&world, { transforms[i].position.x, transforms[i].position.z }, transforms[i].rotation.y, v_attribs[i].is_predator, i);

	// MAke these members and access via simulation
	wall_1 = new Boundary{ &world, b2Vec2{ -390.f, 0.f }, 0.f };
	wall_2 = new Boundary{ &world, b2Vec2{  390.f, 0.f }, 0.f };
	wall_3 = new Boundary{ &world, b2Vec2{ 0.f, -390.f }, 90.f };
	wall_4 = new Boundary{ &world, b2Vec2{ 0.f,  390.f }, 90.f };
	
	world.SetContactListener(&vehicle_contact_listener);

	for (int i = 0; i < vehicles.size(); i++)
		vehicles[i].update();

	world.Step(time_step, velocity_iterations, position_iterations);
}

void Physics::update() {
	world.Step(time_step, velocity_iterations, position_iterations);

	for (map<int, Vehicle>::iterator it = vehicles.begin(); it != vehicles.end(); ++it)
		it->second.update();
}

vec2 Physics::get_vehicle_position(int index) {
	return{ vehicles[index].body->GetPosition().x, vehicles[index].body->GetPosition().y };
}

float Physics::get_vehicle_rotation(int index) {
	return{ vehicles[index].body->GetAngle() * (float)(180 / 3.141592f) };
}

void Physics::destroy() {
	for (map<int, Vehicle>::iterator it = vehicles.begin(); it != vehicles.end(); ++it)
		it->second.destroy();

	delete wall_1;
	delete wall_2;
	delete wall_3;
	delete wall_4;
}

void Physics::add_vehicle(int instance_id, const Transform& transform, bool is_predator) {
	b2Vec2 position = { transform.position.x, transform.position.z };
	Vehicle v;
	v.init(&world, position, transform.rotation.y, is_predator, instance_id);
	vehicles.insert(pair<int, Vehicle>(instance_id, v));
}

void Physics::remove_vehicle() {
	(--vehicles.end())->second.destroy();
	vehicles.erase(--vehicles.end());
}

void Physics::remove_vehicle(int instance_id) {
	vehicles[instance_id].destroy();
	vehicles.erase(instance_id);
}