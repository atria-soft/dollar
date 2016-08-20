/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#include <dollar/Rectangle.h>
#include <dollar/debug.h>

#define MAX_FLOAT std::numeric_limits<float>::max()

dollar::Rectangle::Rectangle(const vec2& _pos, const vec2& _size):
  m_pos(_pos),
  m_size(_size) {
	
}
dollar::Rectangle::Rectangle(const std::vector<vec2>& _points) {
	vec2 minPos(MAX_FLOAT,MAX_FLOAT);
	vec2 maxPos(-MAX_FLOAT,-MAX_FLOAT);
	for (auto &it : _points) {
		minPos.setMin(it);
		maxPos.setMax(it);
	}
	m_pos = minPos;
	m_size = maxPos-minPos;
}
dollar::Rectangle::Rectangle(const std::vector<std::vector<vec2>>& _points) {
	vec2 minPos(MAX_FLOAT,MAX_FLOAT);
	vec2 maxPos(-MAX_FLOAT,-MAX_FLOAT);
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


