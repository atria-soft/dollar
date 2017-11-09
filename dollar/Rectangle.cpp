/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#include <dollar/Rectangle.hpp>
#include <dollar/debug.hpp>

#include <etk/types.hpp>

//#define MAX_FLOAT std::numeric_limits<float>::max()

dollar::Rectangle::Rectangle(const vec2& _pos, const vec2& _size):
  m_pos(_pos),
  m_size(_size) {
	
}
dollar::Rectangle::Rectangle(const etk::Vector<vec2>& _points) {
	vec2 minPos(FLT_MAX, FLT_MAX);
	vec2 maxPos(FLT_MIN, FLT_MIN);
	for (auto &it : _points) {
		minPos.setMin(it);
		maxPos.setMax(it);
	}
	m_pos = minPos;
	m_size = maxPos-minPos;
}
dollar::Rectangle::Rectangle(const etk::Vector<etk::Vector<vec2>>& _points) {
	vec2 minPos(FLT_MAX, FLT_MAX);
	vec2 maxPos(FLT_MIN, FLT_MIN);
	for (auto &it : _points) {
		for (auto &itPoint : it) {
			minPos.setMin(itPoint);
			maxPos.setMax(itPoint);
		}
	}
	m_pos = minPos;
	m_size = maxPos-minPos;
}

const vec2& dollar::Rectangle::getPos() const {
	return m_pos;
}

const vec2& dollar::Rectangle::getSize() const {
	return m_size;
}


