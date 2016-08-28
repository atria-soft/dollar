/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <dollar/tools.h>
#include <dollar/debug.h>
#include <math.h>
#include <algorithm>


float dollar::pathLength(std::vector<vec2> _points) {
	float distance = 0;
	for (size_t iii = 1; iii < _points.size(); ++iii) {
		distance += (_points[iii] - _points[iii-1]).length();
	}
	return distance;
}

vec2 dollar::getBaryCenter(const std::vector<vec2>& _points) {
	vec2 center(0,0);
	for (auto &it : _points) {
		center += it;
	}
	center /= float(_points.size());
	return center;
}

// TODO: Change this with the use of a generic matrix 2D...
std::vector<vec2> dollar::rotateBy(const std::vector<vec2>& _points, float _rotation) {
	std::vector<vec2> out;
	vec2 center = getBaryCenter(_points);
	float cosine = std::cos(_rotation);
	float sine   = std::sin(_rotation);
	for (auto &it : _points) {
		float qx = (it.x() - center.x()) * cosine - (it.y() - center.y()) * sine   + center.x();
		float qy = (it.x() - center.x()) * sine   + (it.y() - center.y()) * cosine + center.y();
		out.push_back(vec2(qx, qy));
	}
	return out;
}

std::vector<vec2> dollar::rotateToZero(const std::vector<vec2>& _points) {
	vec2 center = getBaryCenter(_points);
	float rotation = std::atan2(center.y() - _points[0].y(), center.x() - _points[0].x());
	return rotateBy(_points, -rotation);
}

// TODO : Rework this to have a correct scale with keeping aspect ration ... or not ...
std::vector<vec2> dollar::scaleToOne(const std::vector<vec2>& _points, bool _keepAspectRation) {
	dollar::Rectangle box(_points);
	std::vector<vec2> out;
	vec2 scale(1.0f/box.getSize().x(), 1.0f/box.getSize().y());
	vec2 offset(0,0);
	if (_keepAspectRation == true) {
		float val = 1;
		offset = box.getSize() * val;
		if (box.getSize().x() > box.getSize().y()) {
			val = 1.0f/box.getSize().x();
			offset = vec2(0.0f, (1.0f-offset.y())*0.5f);
		} else {
			val = 1.0f/box.getSize().y();
			offset = vec2((1.0f-offset.x())*0.5f, 0.0f);
		}
		scale = vec2(val,val);
	}
	for (auto &it : _points) {
		vec2 tmp = it - box.getPos();
		tmp *= scale;
		//tmp += offset;
		out.push_back(tmp);
	}
	return out;
}

std::vector<std::vector<vec2>> dollar::scaleToOne(const std::vector<std::vector<vec2>>& _points, bool _keepAspectRation) {
	dollar::Rectangle box(_points);
	std::vector<std::vector<vec2>> out;
	vec2 scale(1.0f/box.getSize().x(), 1.0f/box.getSize().y());
	vec2 offset(0,0);
	if (_keepAspectRation == true) {
		float val = 1;
		offset = box.getSize() * val;
		if (box.getSize().x() > box.getSize().y()) {
			val = 1.0f/box.getSize().x();
			offset = vec2(0.0f, (1.0f-offset.y())*0.5f);
		} else {
			val = 1.0f/box.getSize().y();
			offset = vec2((1.0f-offset.x())*0.5f, 0.0f);
		}
		scale = vec2(val,val);
	}
	for (auto &it : _points) {
		std::vector<vec2> stroke;
		for (auto &itPoint : it) {
			vec2 tmp = itPoint - box.getPos();
			tmp *= scale;
			//tmp += offset;
			stroke.push_back(tmp);
		}
		out.push_back(stroke);
	}
	return out;
}

std::vector<vec2> dollar::translateBariCenterToZero(std::vector<vec2> _points) {
	std::vector<vec2> out;
	vec2 center = getBaryCenter(_points);
	for (auto &it :_points) {
		out.push_back(it - center);
	}
	return out;
}

static std::vector<vec2> discretize(std::vector<vec2> _points, float _interval) {
	std::vector<vec2> out;
	if (_points.size() == 0) {
		return out;
	}
	// same first point ==> no change
	out.push_back(_points.front());
	float distance = 0.0f;
	// For all other point we have to resample elements
	for (size_t iii=1; iii<_points.size(); ++iii) {
		vec2 currentPoint = _points[iii];
		vec2 previousPoint = _points[iii-1];
		float tmpDist = (currentPoint-previousPoint).length();
		if ((distance + tmpDist) >= _interval) {
			vec2 point = previousPoint + (currentPoint - previousPoint) * ((_interval - distance) / tmpDist);
			out.push_back(point);
			_points.insert(_points.begin() + iii, point);
			distance = 0.0;
		} else {
			distance += tmpDist;
		}
	}
	return out;
}

std::vector<vec2> dollar::resample(std::vector<vec2> _points, int32_t _nbPoints) {
	std::vector<vec2> out;
	if (_points.size() == 0) {
		return out;
	}
	// calculate the interval between every points ...
	float interval = pathLength(_points) / (_nbPoints - 1);
	out = discretize(_points, interval);
	// somtimes we fall a rounding-error short of adding the last point, so add it if so
	if (int64_t(out.size()) == (_nbPoints - 1)) {
		out.push_back(_points.back());
	}
	return out;
}

std::vector<std::vector<vec2>> dollar::makeReferenceStrokes(const std::vector<std::vector<vec2>>& _strokes) {
	std::vector<std::vector<vec2>> out;
	// create the ordr of all possibilities of writing the strokes ... (ABC, ACB, BAC, BCA ...)
	std::vector<size_t> order;
	for(size_t iii=0; iii<_strokes.size(); ++iii) {
		order.push_back(iii);
	}
	// For all orders (every permutation of the path):
	do {
		// now we have an other problem: the user can write in multiple way the path
		size_t nbPermutation = std::pow(2, order.size());
		// we use the bit like a flag to know the order of the draw
		for (size_t permut=0; permut<nbPermutation; ++permut) {
			std::vector<vec2> stroke;
			for (size_t iii=0; iii<order.size(); ++iii) {
				std::vector<vec2> pts = _strokes[order[iii]];
				// check to permut the value order
				if (((permut>>iii) & 0x01) == 1) {
					reverse(pts.begin(),pts.end());
				}
				// Add point in next of the path...
				for (auto &it : pts) {
					stroke.push_back(it);
				}
			}
			// Add new generated stroke
			out.push_back(stroke);
		}
	} while (next_permutation(order.begin(), order.end()));
	return out;
}

std::vector<vec2> dollar::combineStrokes(const std::vector<std::vector<vec2>>& _strokes) {
	std::vector<vec2> out;
	for (auto &it : _strokes) {
		for (auto &pointIt : it) {
			out.push_back(pointIt);
		}
	}
	return out;
}

vec2 dollar::getStartVector(const std::vector<vec2>& _points, float _index) {
	DOLLAR_ASSERT(_index > 0, "index must be != of 0");
	if (_points.size() <= _index) {
		return vec2(1.0, 0.0);
	}
	vec2 vect = _points[_index] - _points[0];
	float len = vect.length();
	return vect / len;
}

std::vector<vec2> dollar::normalyse(const std::vector<vec2>& _points) {
	float sum = 0.0;
	std::vector<vec2> out = _points;
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


std::vector<vec2> dollar::normalizePath(std::vector<vec2> _points, size_t _nbSample, bool _ignoreRotation, bool _keepAspectRatio) {
	_points = dollar::resample(_points, _nbSample);
	if (_ignoreRotation == true) {
		_points = rotateToZero(_points);
	}
	_points = scaleToOne(_points, _keepAspectRatio);
	return translateBariCenterToZero(_points);
}


std::vector<vec2> dollar::normalizePathToPoints(std::vector<std::vector<vec2>> _points, float _distance, bool _keepAspectRatio) {
	// Scale point to (0.0,0.0) position and (1.0,1.0) size
	_points = dollar::scaleToOne(_points, _keepAspectRatio);
	std::vector<vec2> out;
	for (auto &it : _points) {
		if (it.size() == 0) {
			continue;
		}
		if (it.size() == 1) {
			out.push_back(it[0]);
			continue;
		}
		std::vector<vec2> tmp = discretize(it, _distance);
		for (auto &pointIt : tmp) {
			out.push_back(pointIt);
		}
		if (tmp[tmp.size()-1] != it[it.size()-1]) {
			out.push_back(it[it.size()-1]);
		}
	}
	return out;
	// NOTE: this is a bad idea ... return translateBariCenterToZero(out);
}