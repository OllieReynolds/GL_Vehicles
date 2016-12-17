#pragma once

#include "global_data.h"
#include "maths.h"
#include "shader.h"
#include "utils.h"

using namespace maths;
using namespace utils;

class DrawQuad {
public:
	DrawQuad(const vec4& colour = utils::colour::black) : colour(colour) {}

	void init_quad(const mat4& projection_matrix);
	void draw_quad(const mat4& view_matrix, const mat4& projection_matrix, const vec2& position, const vec2& size, const vec4& colour);
	void destroy_quad();

	vec4 colour;

private:
	GLuint vao;
	GLuint vbo;
	Shader shader;
};