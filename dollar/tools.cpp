/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <dollar/tools.hpp>
#include <dollar/debug.hpp>

#include <cmath>
#include <algorithm>


float dollar::pathLength(etk::Vector<vec2> _points) {
	float distance = 0;
	for (size_t iii = 1; iii < _points.size(); ++iii) {
		distance += (_points[iii] - _points[iii-1]).length();
	}
	return distance;
}

vec2 dollar::getBaryCenter(const etk::Vector<vec2>& _points) {
	vec2 center(0,0);
	for (auto &it : _points) {
		center += it;
	}
	center /= float(_points.size());
	return center;
}

// TODO: Change this with the use of a generic matrix 2D...
etk::Vector<vec2> dollar::rotateBy(const etk::Vector<vec2>& _points, float _rotation) {
	etk::Vector<vec2> out;
	vec2 center = getBaryCenter(_points);
	float cosine = std::cos(_rotation);
	float sine   = std::sin(_rotation);
	for (auto &it : _points) {
		float qx = (it.x() - center.x()) * cosine - (it.y() - center.y()) * sine   + center.x();
		float qy = (it.x() - center.x()) * sine   + (it.y() - center.y()) * cosine + center.y();
		out.pushBack(vec2(qx, qy));
	}
	return out;
}

etk::Vector<vec2> dollar::rotateToZero(const etk::Vector<vec2>& _points) {
	vec2 center = getBaryCenter(_points);
	float rotation = std::atan2(center.y() - _points[0].y(), center.x() - _points[0].x());
	return rotateBy(_points, -rotation);
}

float maxKeepAspectRatio = 5.5f;

// TODO : Rework this to have a correct scale with keeping aspect ration ... or not ...
etk::Vector<vec2> dollar::scaleToOne(const etk::Vector<vec2>& _points, bool _keepAspectRation) {
	dollar::Rectangle box(_points);
	etk::Vector<vec2> out;
	vec2 scale(1.0f/box.getSize().x(), 1.0f/box.getSize().y());
	vec2 offset(0,0);
	if (_keepAspectRation == true) {
		if (box.getSize().x() > box.getSize().y()) {
			//if (box.getSize().y()/box.getSize().x() < 0.75f)
			{
				vec2 scale(1.0f/box.getSize().x(), 1.0f/box.getSize().x());
				offset = vec2(0.0f, (1.0f-box.getSize().y()/box.getSize().x())*0.5f);
			}
		} else {
			//if (box.getSize().x()/box.getSize().y() < 0.75f)
			{
				vec2 scale(1.0f/box.getSize().y(), 1.0f/box.getSize().y());
				offset = vec2((1.0f-box.getSize().x()/box.getSize().y())*0.5f, 0.0f);
			}
		}
	}
	for (auto &it : _points) {
		vec2 tmp = it - box.getPos();
		tmp *= scale;
		tmp += offset;
		out.pushBack(tmp);
	}
	return out;
}

etk::Vector<etk::Vector<vec2>> dollar::scaleToOne(const std::vector<std::vector<vec2>>& _points, bool _keepAspectRation) {
	dollar::Rectangle box(_points);
	etk::Vector<etk::Vector<vec2>> out;
	vec2 scale(1.0f/box.getSize().x(), 1.0f/box.getSize().y());
	vec2 offset(0,0);
	if (_keepAspectRation == true) {
		if (box.getSize().x() > box.getSize().y()) {
			//if (box.getSize().y()/box.getSize().x() < 0.75f)
			{
				vec2 scale(1.0f/box.getSize().x(), 1.0f/box.getSize().x());
				offset = vec2(0.0f, (1.0f-box.getSize().y()/box.getSize().x())*0.5f);
			}
		} else {
			//if (box.getSize().x()/box.getSize().y() < 0.75f)
			{
				vec2 scale(1.0f/box.getSize().y(), 1.0f/box.getSize().y());
				offset = vec2((1.0f-box.getSize().x()/box.getSize().y())*0.5f, 0.0f);
			}
		}
	}
	for (auto &it : _points) {
		etk::Vector<vec2> stroke;
		for (auto &itPoint : it) {
			vec2 tmp = itPoint - box.getPos();
			tmp *= scale;
			tmp += offset;
			stroke.pushBack(tmp);
		}
		out.pushBack(stroke);
	}
	return out;
}

etk::Vector<vec2> dollar::translateBariCenterToZero(etk::Vector<vec2> _points) {
	etk::Vector<vec2> out;
	vec2 center = getBaryCenter(_points);
	for (auto &it :_points) {
		out.pushBack(it - center);
	}
	return out;
}

static etk::Vector<vec2> discretize(etk::Vector<vec2> _points, float _interval) {
	etk::Vector<vec2> out;
	if (_points.size() == 0) {
		return out;
	}
	// same first point ==> no change
	out.pushBack(_points.front());
	float distance = 0.0f;
	// For all other point we have to resample elements
	for (size_t iii=1; iii<_points.size(); ++iii) {
		vec2 currentPoint = _points[iii];
		vec2 previousPoint = _points[iii-1];
		float tmpDist = (currentPoint-previousPoint).length();
		if ((distance + tmpDist) >= _interval) {
			vec2 point = previousPoint + (currentPoint - previousPoint) * ((_interval - distance) / tmpDist);
			out.pushBack(point);
			_points.insert(_points.begin() + iii, point);
			distance = 0.0;
		} else {
			distance += tmpDist;
		}
	}
	return out;
}

etk::Vector<vec2> dollar::resample(etk::Vector<vec2> _points, int32_t _nbPoints) {
	etk::Vector<vec2> out;
	if (_points.size() == 0) {
		return out;
	}
	// calculate the interval between every points ...
	float interval = pathLength(_points) / (_nbPoints - 1);
	out = discretize(_points, interval);
	// somtimes we fall a rounding-error short of adding the last point, so add it if so
	if (int64_t(out.size()) == (_nbPoints - 1)) {
		out.pushBack(_points.back());
	}
	return out;
}

etk::Vector<etk::Vector<vec2>> dollar::makeReferenceStrokes(const std::vector<std::vector<vec2>>& _strokes) {
	etk::Vector<etk::Vector<vec2>> out;
	// create the ordr of all possibilities of writing the strokes ... (ABC, ACB, BAC, BCA ...)
	etk::Vector<size_t> order;
	for(size_t iii=0; iii<_strokes.size(); ++iii) {
		order.pushBack(iii);
	}
	// For all orders (every permutation of the path):
	do {
		// now we have an other problem: the user can write in multiple way the path
		size_t nbPermutation = std::pow(2, order.size());
		// we use the bit like a flag to know the order of the draw
		for (size_t permut=0; permut<nbPermutation; ++permut) {
			etk::Vector<vec2> stroke;
			for (size_t iii=0; iii<order.size(); ++iii) {
				etk::Vector<vec2> pts = _strokes[order[iii]];
				// check to permut the value order
				if (((permut>>iii) & 0x01) == 1) {
					reverse(pts.begin(),pts.end());
				}
				// Add point in next of the path...
				for (auto &it : pts) {
					stroke.pushBack(it);
				}
			}
			// Add new generated stroke
			out.pushBack(stroke);
		}
	} while (next_permutation(order.begin(), order.end()));
	return out;
}

etk::Vector<vec2> dollar::combineStrokes(const etk::Vector<std::vector<vec2>>& _strokes) {
	etk::Vector<vec2> out;
	for (auto &it : _strokes) {
		for (auto &pointIt : it) {
			out.pushBack(pointIt);
		}
	}
	return out;
}

vec2 dollar::getStartVector(const etk::Vector<vec2>& _points, float _index) {
	DOLLAR_ASSERT(_index > 0, "index must be != of 0");
	if (_points.size() <= _index) {
		return vec2(1.0, 0.0);
	}
	vec2 vect = _points[_index] - _points[0];
	float len = vect.length();
	return vect / len;
}

etk::Vector<vec2> dollar::normalyse(const etk::Vector<vec2>& _points) {
	float sum = 0.0;
	etk::Vector<vec2> out = _points;
	for (auto &it : _points) {
		sum += it.length2();
	}
	float magnitude = sqrt(sum);
	if (magnitude == 0.0f) {
		DOLLAR_ERROR("Magnetude is == 0");
		return out;
	}
	for (auto &it : out) {
		it /= magnitude;
	}
	return out;
}


etk::Vector<vec2> dollar::normalizePath(etk::Vector<vec2> _points, size_t _nbSample, bool _ignoreRotation, bool _keepAspectRatio) {
	_points = dollar::resample(_points, _nbSample);
	if (_ignoreRotation == true) {
		_points = rotateToZero(_points);
	}
	_points = scaleToOne(_points, _keepAspectRatio);
	return translateBariCenterToZero(_points);
}

float dollar::getAspectRatio(etk::Vector<etk::Vector<vec2>> _points) {
	dollar::Rectangle box(_points);
	if (box.getSize().x() < box.getSize().y()) {
		return 1.0f - box.getSize().x() / box.getSize().y();
	} else {
		return -(1.0f - box.getSize().y() / box.getSize().x());
	}
}

etk::Vector<vec2> dollar::normalizePathToPoints(etk::Vector<std::vector<vec2>> _points, float _distance, bool _keepAspectRatio) {
	// Scale point to (0.0,0.0) position and (1.0,1.0) size
	_points = dollar::scaleToOne(_points, _keepAspectRatio);
	etk::Vector<vec2> out;
	for (auto &it : _points) {
		if (it.size() == 0) {
			continue;
		}
		if (it.size() == 1) {
			out.pushBack(it[0]);
			continue;
		}
		etk::Vector<vec2> tmp = discretize(it, _distance);
		for (auto &pointIt : tmp) {
			out.pushBack(pointIt);
		}
		if (tmp[tmp.size()-1] != it[it.size()-1]) {
			out.pushBack(it[it.size()-1]);
		}
	}
	return out;
	// NOTE: this is a bad idea ... return translateBariCenterToZero(out);
}