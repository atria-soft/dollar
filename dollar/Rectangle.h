/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <math.h>
#include <string>
#include <list>
#include <vector>
#include <etk/math/Vector2D.h>

namespace dollar {
	/**
	 * @brief Simple rectangle area (AABB mode)
	 */
	class Rectangle {
		protected:
			vec2 m_pos; //!< Position of the start of the rectangle (min position)
			vec2 m_size; //!< Size of the rectangle
		public:
			/**
			 * @brief Create a rectangle as a bounding box
			 * @param[in] _points List of point that is contained in this area
			 */
			Rectangle(const std::vector<vec2>& _points);
			/**
			 * @brief Create a rectangle as a bounding box
			 * @param[in] _points List of point that is contained in this area
			 */
			Rectangle(const std::vector<std::vector<vec2>>& _points);
			/**
			 * @brief Create a rectangle with values
			 * @param[in] _pos Start position
			 * @param[in] _size Size of the rectangle
			 */
			Rectangle(const vec2& _pos, const vec2& _size);
		public:
			/**
			 * @brief Get start position of the rectangle
			 * @return Start position.
			 */
			const vec2& getPos() const;
			/**
			 * @brief Get the size of the rectangle
			 * @return Size of the rectangle
			 */
			const vec2& getSize() const;
	};
}


