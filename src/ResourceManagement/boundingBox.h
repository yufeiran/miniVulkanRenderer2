#pragma once
#include "Common/common.h"


// BoundingBox
struct BBox
{
	BBox() = default;
	BBox(glm::vec3 _min, glm::vec3 _max)
		: min(_min)
		, max(_max)
	{
	}

	BBox(const std::vector<glm::vec3>& corners)
	{
		for(auto& c : corners)
		{
			insert(c);
		}
	}

	void insert(const glm::vec3& v)
	{
		min = {std::min(min.x, v.x), std::min(min.y, v.y), std::min(min.z, v.z)};
		max = {std::max(max.x, v.x), std::max(max.y, v.y), std::max(max.z, v.z)};
	}

	void insert(const BBox& b)
	{
		insert(b.min);
		insert(b.max);
	}

	inline bool isEmpty() const 
	{
		return min == glm::vec3{std::numeric_limits<float>::max()}
				|| max == glm::vec3{std::numeric_limits<float>::lowest()};
	}

	inline uint32_t rank() const 
	{
		uint32_t result{0};
		result += min.x < max.x;
		result += min.y < max.y;
		result += min.z < max.z;
		return result;
	}

	inline bool              isPoint() const { return min == max; };
	inline bool              isLine() const { return rank() == 1u; }
	inline bool              isPlane() const { return rank() == 2u; }
	inline bool              isVolume() const { return rank() == 3u; }

	inline glm::vec3         getMin() { return min; }
	inline glm::vec3         getMax() { return max; }
	inline glm::vec3         extents() { return max - min; }
	inline glm::vec3         center() { return (min + max) * 0.5f; }
	inline float             radius() { return glm::length(max - min) * 0.5f; }

	BBox transform(glm::mat4 mat)
	{
		// 3d transformation + translation
		auto r = mat[3];
		const float epsilon = 1e-6f;
		assert(fabs(r.x) < epsilon && fabs(r.y) < epsilon && fabs(r.z) < epsilon && fabs(r.w - 1.0f) < epsilon);

		std::vector<glm::vec3> corners(8);
		corners[0] = glm::vec3(mat * glm::vec4(min.x, min.y, min.z, 1));
		corners[1] = glm::vec3(mat * glm::vec4(min.x, min.y, max.z, 1));
		corners[2] = glm::vec3(mat * glm::vec4(min.x, max.y, min.z, 1));
		corners[3] = glm::vec3(mat * glm::vec4(min.x, max.y, max.z, 1));
		corners[4] = glm::vec3(mat * glm::vec4(max.x, min.y, min.z, 1));
		corners[5] = glm::vec3(mat * glm::vec4(max.x, min.y, max.z, 1));
		corners[6] = glm::vec3(mat * glm::vec4(max.x, max.y, min.z, 1));
		corners[7] = glm::vec3(mat * glm::vec4(max.x, max.y, max.z, 1));

		BBox result(corners);
		return result;
	}

private:
	glm::vec3 min{std::numeric_limits<float>::max()};
	glm::vec3 max{-std::numeric_limits<float>::max()};
};