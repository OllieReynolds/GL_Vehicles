#pragma once

#include <cmath>

namespace maths {
	class vec2 {
	public:
		vec2() : n{0.f, 0.f} {}
		vec2(const float v) : n{v, v} {}
		vec2(const float x, const float y) : n{x, y} {}

		vec2& operator  = (const vec2& v) { x  = v.x; y  = v.y; return *this; }
		vec2& operator += (const vec2& v) { x += v.x; y += v.y; return *this; }
		vec2& operator -= (const vec2& v) { x -= v.x; y -= v.y; return *this; }
		vec2& operator *= (const float v) { x *= v; y *= v; return *this; }
		vec2& operator /= (const float v) { x /= v; y /= v; return *this; }

		inline       float& operator [] (int i)       { return n[i]; }
		inline const float& operator [] (int i) const { return n[i]; }

		friend int operator == (const vec2& a, const vec2& b) { return a.x == b.x && a.y == b.y; }
		friend int operator != (const vec2& a, const vec2& b) { return !(a == b); }

		friend vec2 operator + (const vec2& a, const vec2& b) { return { a.x + b.x, a.y + b.y }; }
		friend vec2 operator - (const vec2& a, const vec2& b) { return { a.x - b.x, a.y - b.y }; }
		friend vec2 operator * (const vec2& a, const float v) { return { a.x * v, a.y * v }; }
		friend vec2 operator / (const vec2& a, const float v) { return { a.x / v, a.y / v }; }

		static float magnitude(const vec2& v) { 
			return sqrt(magnitude_squared(v)); 
		}

		static float magnitude_squared(const vec2& v) { 
			return v.x * v.x + v.y * v.y; 
		}

		static vec2 normalise(const vec2& v) { 
			return (v == vec2{0.f}) ? v : v / magnitude(v); 
		}

		union {
			struct {
				float x;
				float y;
			};
			float n[2];
		};
	};

	
	class vec3 {
	public:
		vec3() : n{0.f, 0.f, 0.f} {}
		vec3(const float v) : n{v, v, v} {}
		vec3(const float x, const float y, const float z) : n{x, y, z} {}
		
		vec3& operator  = (const vec3& v) { x  = v.x; y  = v.y; z  = v.z; return *this; }
		vec3& operator += (const vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
		vec3& operator -= (const vec3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
		vec3& operator *= (const float v) { x *= v; y *= v; z *= v; return *this; }
		vec3& operator /= (const float v) { x /= v; y /= v; z /= v; return *this; }

		inline       float& operator [] (int i)       { return n[i]; }
		inline const float& operator [] (int i) const { return n[i]; }

		friend int operator == (const vec3& a, const vec3& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
		friend int operator != (const vec3& a, const vec3& b) { return !(a == b); }

		friend vec3 operator + (const vec3& a, const vec3& b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
		friend vec3 operator - (const vec3& a, const vec3& b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
		friend vec3 operator * (const vec3& a, const float v) { return { a.x * v, a.y * v, a.z * v }; }
		friend vec3 operator / (const vec3& a, const float v) { return { a.x / v, a.y / v, a.z / v }; }

		static float magnitude(const vec3& v) { 
			return sqrt(magnitude_squared(v)); 
		}
		
		static float magnitude_squared(const vec3& v) { 
			return v.x * v.x + v.y * v.y + v.z * v.z; 
		}
		
		static vec3 normalise(const vec3& v) { 
			return (v == vec3{0.f}) ? v : v / magnitude(v); 
		}

		union {
			struct {
				float x;
				float y;
				float z;
			};
			float n[3];
		};
	};

	
	class vec4 {
	public:
		vec4() : n{0.f, 0.f, 0.f, 0.f} {}
		vec4(const float v) : n{v, v, v, v} {}
		vec4(const float x, const float y, const float z, const float w) : n{x, y, z, w} {}

		vec4& operator  = (const vec4& v) { x  = v.x; y  = v.y; z  = v.z; w  = v.w; return *this; }
		vec4& operator += (const vec4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
		vec4& operator -= (const vec4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
		vec4& operator *= (const float v) { x *= v; y *= v; z *= v; w *= v; return *this; }
		vec4& operator /= (const float v) { x /= v; y /= v; z /= v; w /= v; return *this; }

		inline       float& operator [] (int i)       { return n[i]; }
		inline const float& operator [] (int i) const { return n[i]; }

		friend int operator == (const vec4& a, const vec4& b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }
		friend int operator != (const vec4& a, const vec4& b) { return !(a == b); }

		friend vec4 operator + (const vec4& a, const vec4& b) { return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
		friend vec4 operator - (const vec4& a, const vec4& b) { return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }
		friend vec4 operator * (const vec4& a, const float v) { return { a.x * v, a.y * v, a.z * v, a.w * v }; }
		friend vec4 operator / (const vec4& a, const float v) { return { a.x / v, a.y / v, a.z / v, a.w / v }; }

		static float magnitude(const vec4& v) { 
			return sqrt(magnitude_squared(v)); 
		}

		static float magnitude_squared(const vec4& v) { 
			return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w; 
		}

		static vec4 normalise(const vec4& v) { 
			return (v == vec4{0.f}) ? v : v / magnitude(v); 
		}

		union {
			struct {
				float x;
				float y;
				float z;
				float w;
			};
			float n[4];
		};
	};


	class mat4 {
	public:
		mat4() : n{{1, 0, 0, 0},{0, 1, 0, 0},{0, 0, 1, 0},{0, 0, 0, 1}} {}
		mat4(const vec4& a, const vec4& b, const vec4& c, const vec4& d) : n{a, b, c, d} {}

		mat4& operator = (const mat4& v) { x = v.x; y = v.y; z = v.z; w = v.w; return *this; }

		inline       vec4& operator [] (int i)       { return n[i]; }
		inline const vec4& operator [] (int i) const { return n[i]; }

		static void scale(const vec3& v, mat4& m) { 
			m.x.x = v.x; 
			m.y.y = v.y; 
			m.z.z = v.z; 
		
		}
		static void translate(const vec3& v, mat4& m) { 
			m.z.x = v.x; 
			m.z.y = v.y; 
			m.z.z = v.z; 
		}

		union {
			struct {
				vec4 x;
				vec4 y;
				vec4 z;
				vec4 w;
			};
			vec4 n[4];
		};
	};

	static mat4 orthographic_matrix(const vec2& resolution, float nZ, float fZ) {
		mat4 m{};
		m.x.x =  2.f / resolution.x;
		m.y.y =  2.f / resolution.y;
		m.z.z = -2.f / (fZ - nZ);

		m.w.x = -resolution.x / resolution.x;
		m.w.y = -resolution.y / resolution.y;
		m.w.z = -(fZ + nZ) / (fZ - nZ);
		return m;
	}
}