#include "..\include\renderer.h"

void Circle_Renderer::init() {
	shader_2D = {
		"shaders/uniform_MP.v.glsl",
		"shaders/uniform_colour_circle.f.glsl"
	};

	shader_3D = {
		"shaders/uniform_MVP.v.glsl",
		"shaders/uniform_colour_circle.f.glsl"
	};

	shader_3D_shadow = {
		"shaders/uniform_MVP.v.glsl",
		"shaders/SPOT_SHADOW.f.glsl"
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data::mesh::quad_points_textured), &data::mesh::quad_points_textured, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void Circle_Renderer::draw_2D(const Camera& camera, const vec2& position, const vec2& size, const vec4& colour, bool filled) {
	shader_2D.use();
	glBindVertexArray(vao);

	shader_2D.set_uniform("uniform_colour", colour);
	shader_2D.set_uniform("projection", camera.matrix_projection_ortho);
	shader_2D.set_uniform("model", utils::gen_model_matrix(size, position));
	shader_2D.set_uniform("draw_filled", filled);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	shader_2D.release();
}

void Circle_Renderer::draw_3D(const Camera& camera, const Transform& transform, const vec4& colour, bool filled) {
	shader_3D.use();
	glBindVertexArray(vao);

	shader_3D.set_uniform("uniform_colour", colour);
	shader_3D.set_uniform("projection", camera.matrix_projection_persp);
	shader_3D.set_uniform("view", camera.matrix_view);
	shader_3D.set_uniform("model", utils::gen_model_matrix(transform));
	shader_3D.set_uniform("draw_filled", filled);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	shader_3D.release();
}

void Circle_Renderer::draw_3D_shadow(const Camera& camera, const Transform& transform) {
	shader_3D_shadow.use();
	glBindVertexArray(vao);

	shader_3D_shadow.set_uniform("projection", camera.matrix_projection_persp);
	shader_3D_shadow.set_uniform("view", camera.matrix_view);
	shader_3D_shadow.set_uniform("model", utils::gen_model_matrix(transform));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	shader_3D_shadow.release();
}

void Circle_Renderer::destroy() {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	shader_2D.destroy();
	shader_3D.destroy();
	shader_3D_shadow.destroy();
}

void Quad_Renderer::init() {
	shader_2D = {
		"shaders/uniform_MP.v.glsl",
		"shaders/uniform_colour.f.glsl"
	};

	shader_3D = {
		"shaders/uniform_MVP.v.glsl",
		"shaders/texture.f.glsl"
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data::mesh::quad_points_textured), &data::mesh::quad_points_textured, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void Quad_Renderer::draw_2D(const Camera& camera, const vec2& position, const vec2& size, const vec4& colour) {
	shader_2D.use();
	glBindVertexArray(vao);

	shader_2D.set_uniform("uniform_colour", colour);
	shader_2D.set_uniform("projection", camera.matrix_projection_ortho);
	shader_2D.set_uniform("model", utils::gen_model_matrix(size, position));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	shader_2D.release();
}

void Quad_Renderer::draw_3D(const Camera& camera, const Transform& transform, const vec4& colour) {
	shader_3D.use();
	glBindVertexArray(vao);

	shader_3D.set_uniform("uniform_colour", colour);
	shader_3D.set_uniform("projection", camera.matrix_projection_persp);
	shader_3D.set_uniform("view", camera.matrix_view);
	shader_3D.set_uniform("model", utils::gen_model_matrix(transform));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	shader_3D.release();
}

void Quad_Renderer::draw_2D_textured(const Camera& camera, const vec2& position, const vec2& size, Texture& tex) {
	shader_2D.use();
	glBindVertexArray(vao);

	tex.use();

	shader_2D.set_uniform("projection", camera.matrix_projection_ortho);
	shader_2D.set_uniform("model", utils::gen_model_matrix(size, position));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	shader_2D.release();
}

void Quad_Renderer::draw_3D_textured(const Camera& camera, const Transform& transform, Texture& tex) {
	shader_3D.use();
	glBindVertexArray(vao);

	tex.use();

	shader_3D.set_uniform("projection", camera.matrix_projection_persp);
	shader_3D.set_uniform("view", camera.matrix_view);
	shader_3D.set_uniform("model", utils::gen_model_matrix(transform));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	shader_3D.release();
}

void Quad_Renderer::destroy() {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	shader_2D.destroy();
	shader_3D.destroy();
}

void Cube_Renderer::init() {
	shader = {
		"shaders/MVP_NORMALS.v.glsl",
		"shaders/DIFFUSE.f.glsl",
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(utils::data::mesh::cube_vertices_normals), &utils::data::mesh::cube_vertices_normals, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
}

void Cube_Renderer::draw(const Camera& camera, const vec3& position, const vec3& size, float rotation, const vec4& colour) {
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	shader.use();

	shader.set_uniform("model", utils::gen_model_matrix(size, position, rotation));
	shader.set_uniform("view", camera.matrix_view);
	shader.set_uniform("projection", camera.matrix_projection_persp);
	shader.set_uniform("uniform_colour", colour);
	shader.set_uniform("light_position", vec3{ 0.f, 30.f, 0.f });
	glDrawArrays(GL_TRIANGLES, 0, 36);

	shader.release();
	glBindVertexArray(0);
}

void Cube_Renderer::draw_multiple(int n, const Camera& camera, const std::vector<Transform>& transform_list, const vec4& colour) {
	shader.use();

	shader.set_uniform("view", camera.matrix_view);
	shader.set_uniform("projection", camera.matrix_projection_persp);
	shader.set_uniform("uniform_colour", colour);
	shader.set_uniform("light_position", vec3{ 0.f, 30.f, 0.f });

	for (int i = 0; i < n; i++) {
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		shader.set_uniform("model", utils::gen_model_matrix(transform_list[i].size, transform_list[i].position, transform_list[i].rotation));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	shader.release();
}

void Cube_Renderer::destroy() {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	shader.destroy();
}

void Line_Renderer::init() {
	shader = {
		"shaders/uniform_MVP.v.glsl",
		"shaders/uniform_colour.f.glsl"
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 2, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
}

void Line_Renderer::draw(const Camera& camera, const vec3& world_space_a, const vec3& world_space_b, const vec4& colour) {
	shader.use();
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	vec3 values[2] = { world_space_a, world_space_b };
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * 2, &values);

	shader.set_uniform("view", camera.matrix_view);
	shader.set_uniform("projection", camera.matrix_projection_persp);
	shader.set_uniform("model", mat4());
	shader.set_uniform("uniform_colour", colour);

	glDrawArrays(GL_LINES, 0, 2);
}

void Line_Renderer::destroy() {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	shader.destroy();
}

void Model_Renderer::init() {
	shader_coloured = {
		"shaders/MVP_NORMALS.v.glsl",
		"shaders/DIFFUSE.f.glsl",
	};

	shader_textured = {
		"shaders/MVP_NORMALS_UVS.v.glsl",
		"shaders/DIFFUSE_TEXTURE.f.glsl",
	};
}

void Model_Renderer::draw_multiple_3D_textured(int n, Model& model, const Camera& camera, const std::vector<Transform>& transform_list, Texture& texture) {
	shader_textured.use();
	shader_textured.set_uniform("view", camera.matrix_view);
	shader_textured.set_uniform("projection", camera.matrix_projection_persp);
	shader_textured.set_uniform("light_position", vec3{ 0.f, 30.f, 0.f });

	texture.use();

	for (int j = 0; j < n; j++) {
		shader_textured.set_uniform("model", gen_model_matrix(transform_list[j].size, transform_list[j].position, transform_list[j].rotation));

		for (int i = 0; i < model.meshes.size(); i++) {
			glBindVertexArray(model.meshes[i].vao);
			glDrawArrays(GL_TRIANGLES, 0, model.meshes[i].vertices.size());
			glBindVertexArray(0);
		}
	}

	shader_textured.release();
}

void Model_Renderer::draw_3D_textured(Model& model, const Camera& camera, const Transform& transform, Texture& texture) {
	shader_textured.use();
	shader_textured.set_uniform("view", camera.matrix_view);
	shader_textured.set_uniform("projection", camera.matrix_projection_persp);
	shader_textured.set_uniform("light_position", vec3{ 0.f, 30.f, 0.f });
	shader_textured.set_uniform("model", gen_model_matrix(transform.size, transform.position, transform.rotation));

	texture.use();

	for (int i = 0; i < model.meshes.size(); i++) {
		glBindVertexArray(model.meshes[i].vao);
		glDrawArrays(GL_TRIANGLES, 0, model.meshes[i].vertices.size());
		glBindVertexArray(0);
	}

	shader_textured.release();
}

void Model_Renderer::draw_3D_coloured(Model& model, const Camera& camera, const Transform& transform, const vec4& colour) {

	for (int i = 0; i < model.meshes.size(); i++) {
		glBindVertexArray(model.meshes[i].vao);
		shader_coloured.use();

		shader_coloured.set_uniform("model", gen_model_matrix(transform.size, transform.position, transform.rotation));
		shader_coloured.set_uniform("view", camera.matrix_view);
		shader_coloured.set_uniform("projection", camera.matrix_projection_persp);
		shader_coloured.set_uniform("uniform_colour", colour);
		shader_coloured.set_uniform("light_position", vec3{ 0.f, 30.f, 0.f });
		glDrawArrays(GL_TRIANGLES, 0, model.meshes[i].vertices.size());

		shader_coloured.release();
		glBindVertexArray(0);
	}
}

void Model_Renderer::destroy() {
	shader_coloured.destroy();
	shader_textured.destroy();
}

void Text_Renderer::init(const vec2& screen_resolution) {
	{
		FT_Library ft_lib;
		FT_Face ff;

		FT_Init_FreeType(&ft_lib);
		FT_New_Face(ft_lib, font.c_str(), 0, &ff);
		FT_Set_Pixel_Sizes(ff, 0, pixel_size);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		for (GLubyte c = 0; c < 128; ++c) {
			FT_Load_Char(ff, c, FT_LOAD_RENDER);

			GLuint tex;
			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ff->glyph->bitmap.width, ff->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, ff->glyph->bitmap.buffer);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glyph_map.insert(std::pair<GLchar, Glyph>(
				c,
				{
					tex,
					ff->glyph->advance.x,
					vec2(ff->glyph->bitmap.width, ff->glyph->bitmap.rows),
					vec2(ff->glyph->bitmap_left, ff->glyph->bitmap_top)
				}
			));
		}

		FT_Done_Face(ff);
		FT_Done_FreeType(ft_lib);
	}

	{ // GL Data
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	}

	{ // Shaders
		shader = {
			"shaders/text.v.glsl",
			"shaders/text.f.glsl"
		};

		shader.set_uniform("colour", vec4(1.f, 1.f, 1.f, 1.f));
		shader.set_uniform("projection", orthographic_matrix(screen_resolution, -1.f, 1.f, mat4()));
	}
}

void Text_Renderer::draw(const std::string& msg, const vec2& position, bool centered, const vec4& colour) {
	shader.use();
	shader.set_uniform("colour", colour);

	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);

	float x = position.x;

	if (centered) {
		float x_ = x;
		for (GLchar c : msg)
			x_ += (glyph_map[c].next_glyph_offset >> 6);

		x -= ((x_ - x) * .5f);
	}

	for (GLchar c : msg) {
		Glyph g = glyph_map[c];

		float xPos = x + g.bearing_offset.x;
		float yPos = position.y - (g.glyph_size.y - g.bearing_offset.y);

		x += (g.next_glyph_offset >> 6);

		float vertices[6][4] = {
			{ xPos,                  yPos + g.glyph_size.y, 0.f, 0.f },
			{ xPos,                  yPos,                  0.f, 1.f },
			{ xPos + g.glyph_size.x, yPos,                  1.f, 1.f },
			{ xPos,                  yPos + g.glyph_size.y, 0.f, 0.f },
			{ xPos + g.glyph_size.x, yPos,                  1.f, 1.f },
			{ xPos + g.glyph_size.x, yPos + g.glyph_size.y, 1.f, 0.f }
		};

		glBindTexture(GL_TEXTURE_2D, g.data);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Text_Renderer::destroy() {
	shader.destroy();
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}