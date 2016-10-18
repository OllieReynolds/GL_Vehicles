#include "..\include\text.h"

namespace simulation {

	void Text::init_text() {
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
			shader.set_uniform("projection", orthographic_matrix({1366.f, 768.f}, -1.f, 1.f, mat4()));
		}
	}


	void Text::draw_text(const std::string& msg, const vec2& position) {
		shader.use();
		shader.set_uniform("colour", colour);

		glBindVertexArray(vao);

		glActiveTexture(GL_TEXTURE0);

		float x = position[0];
		float y = position[1];

		std::string::const_iterator c;
		for (c = msg.begin(); c != msg.end(); ++c) {
			Glyph g = glyph_map[*c];

			float xPos = x + g.bearing_offset[0];
			float yPos = y - (g.glyph_size[1] - g.bearing_offset[1]);

			float w = g.glyph_size[0];
			float h = g.glyph_size[1];
			float vertices[6][4] = {
				{xPos,     yPos + h, 0.f, 0.f},
				{xPos,     yPos,     0.f, 1.f},
				{xPos + w, yPos,     1.f, 1.f},
				{xPos,     yPos + h, 0.f, 0.f},
				{xPos + w, yPos,     1.f, 1.f},
				{xPos + w, yPos + h, 1.f, 0.f}
			};

			glBindTexture(GL_TEXTURE_2D, g.data);

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			x += (g.next_glyph_offset >> 6);
		}

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Text::destroy_text() {
		shader.destroy();
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}
}