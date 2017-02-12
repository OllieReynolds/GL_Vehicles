#include "..\include\camera.h"

Camera::Camera() {
	follow_vehicle = false;
	field_of_view = 90.f;
	target_distance = 50.f;
	resolution = config::resolution;
	depth_range_ortho = vec2{ -1.f, 1.f };
	depth_range_persp = vec2{ 0.1f, 1500.f };
	position_current = vec3{ 0.f, 0.f, 0.f };
	position_start = vec3{ 0.f, 456.f, 0.f };
	position_target = vec3{ 0.f, 0.f, 0.f };
	orientation_up = vec3{ 0.f, 1.f, 0.f };
	aspect_ratio = resolution.x / resolution.y;
	matrix_view = shared::view_matrix(position_current, position_target, orientation_up);
	matrix_projection_ortho = orthographic_matrix(resolution, depth_range_ortho.x, depth_range_ortho.y, maths::mat4());
	matrix_projection_persp = shared::perspective_matrix(field_of_view, aspect_ratio, depth_range_persp.x, depth_range_persp.y);
}

void Camera::update(std::map<int, Transform>& transforms) {
	if (follow_vehicle) {
		vec2 direction = polar_to_cartesian(to_radians(transforms.begin()->second.rotation.y));
		direction *= target_distance;

		position_current = transforms.begin()->second.position;
		position_current.x -= direction.x;
		position_current.y += target_distance;
		position_current.z -= direction.y;
		position_target = position_current + vec3{ direction.x, -target_distance, direction.y };
		orientation_up = vec3(0.f, 1.f, 0.f);
	}
	else {
		position_current = position_start;
		position_target = vec3{ 0.f, 0.f, 0.f };
		orientation_up = vec3(0.f, 0.f, 1.f);
	}

	matrix_view = shared::view_matrix(position_current, position_target, orientation_up);
}