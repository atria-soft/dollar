/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <dollar/EngineP.hpp>
#include <dollar/debug.hpp>
#include <dollar/Rectangle.hpp>
#include <dollar/tools.hpp>
#include <etk/os/FSNode.hpp>
#include <cmath>
#include <algorithm>
#define MAX_FLOAT std::numeric_limits<float>::max()


dollar::EngineP::EngineP():
  m_scaleKeepRatio(false),
  m_numPointsInGesture(128) {
	DOLLAR_ASSERT(m_numPointsInGesture>16, "NB element in a path must be > 16 ...");
	
}


void dollar::EngineP::setNumberPointInGesture(size_t _value) {
	if (_value == m_numPointsInGesture) {
		return;
	}
	m_numPointsInGesture = _value;
	DOLLAR_ASSERT(m_numPointsInGesture>16, "NB element in a path must be > 16 ...");
	for (auto &it: m_gestures) {
		if (it == nullptr) {
			continue;
		}
		it->configure(m_numPointsInGesture);
	}
}

size_t dollar::EngineP::getNumberPointInGesture() {
	return m_numPointsInGesture;
}

void dollar::EngineP::setScaleKeepRatio(bool _value) {
	if (_value == m_scaleKeepRatio) {
		return;
	}
	m_scaleKeepRatio = _value;
	for (auto &it: m_gestures) {
		if (it == nullptr) {
			continue;
		}
		it->configure(m_numPointsInGesture);
	}
}

bool dollar::EngineP::getScaleKeepRatio() {
	return m_scaleKeepRatio;
}

static float cloudDistance(const std::vector<vec2>& _points1, const std::vector<vec2>& _points2, size_t _start) {
	std::vector<bool> matched;
	matched.resize(_points1.size(), false);
	float out = 0;
	size_t iii = _start;
	do {
		float min = MAX_FLOAT;
		size_t index = 0;
		for (size_t jjj=0; jjj<matched.size(); ++jjj) {
			if (matched[jjj] == true) {
				continue;
			}
			float distance = (_points1[iii] - _points2[jjj]).length();
			if (distance < min) {
				min = distance;
				index = jjj;
			}
		}
		matched[index] = true;
		float weight = 1.0 - ((iii - _start + _points1.size()) % _points1.size())/_points1.size();
		out = out + weight* min;
		iii = (iii + 1) % _points1.size();
	} while(iii != _start);
	return out;
}

//Greedy-Cloud-Match
static float calculateBestDistance(const std::vector<vec2>& _points, const std::vector<vec2>& _reference) {
	float out = MAX_FLOAT;
	float si = 0.5f;
	float step = pow(_points.size(), si-1);
	if (step < 1) {
		// DOLLAR_ERROR(" step is too small ... " << step);
		step = 1.0f;
	}
	for (size_t iii=0; iii<_points.size(); iii+=int32_t(step)) {
		float d1 = cloudDistance(_points, _reference, iii);
		float d2 = cloudDistance(_reference, _points, iii);
		out = std::min(out, std::min(d1,d2));
	}
	return out;	// Distance to the nearest point must be < 2.0 (maximum distance visible)
}


bool dollar::EngineP::loadGesture(const std::string& _filename) {
	ememory::SharedPtr<dollar::Gesture> ref = ememory::makeShared<dollar::GestureP>();
	DOLLAR_DEBUG("Load Gesture: " << _filename);
	if (ref->load(_filename) == true) {
		addGesture(ref);
		return true;
	}
	return false;
}

void dollar::EngineP::addGesture(ememory::SharedPtr<dollar::Gesture> _gesture) {
	ememory::SharedPtr<dollar::GestureP> gest = ememory::dynamicPointerCast<dollar::GestureP>(_gesture);
	if (gest != nullptr) {
		gest->configure(m_numPointsInGesture);
		m_gestures.push_back(gest);
	}
}

dollar::Results dollar::EngineP::recognize2(const std::vector<std::vector<vec2>>& _strokes) {
	std::vector<vec2> points = dollar::combineStrokes(_strokes);
	points = dollar::normalizePath(points, m_numPointsInGesture, false, m_scaleKeepRatio);
	// Keep maximum 5 results ...
	float bestDistance[m_nbResult];
	int32_t indexOfBestMatch[m_nbResult];
	for (size_t iii=0; iii<m_nbResult; ++iii) {
		bestDistance[iii] = MAX_FLOAT;
		indexOfBestMatch[iii] = -1;
	}
	// for each multistroke
	for (size_t iii=0; iii<m_gestures.size(); ++iii) {
		DOLLAR_DEBUG("[" << iii << "] '" << m_gestures[iii]->getName() << "'");
		ememory::SharedPtr<GestureP> gesture = m_gestures[iii];
		if (gesture->getPath().size() == 0) {
			DOLLAR_ERROR("Reference path with no Value");
			continue;
		}
		float distance = MAX_FLOAT;
		distance = calculateBestDistance(points, gesture->getPath());
		for (size_t kkk=0; kkk<m_nbResult; ++kkk) {
			if (distance < bestDistance[kkk]) {
				if (kkk == 0) {
					DOLLAR_DEBUG("[" << iii << "]                     d=" << distance << " < bd=" << bestDistance << " ");
				}
				if (indexOfBestMatch[kkk] != int64_t(iii)) {
					for (int32_t rrr=m_nbResult-1; rrr>int32_t(kkk); --rrr) {
						bestDistance[rrr] = bestDistance[rrr-1];
						indexOfBestMatch[rrr] = indexOfBestMatch[rrr-1];
					}
					indexOfBestMatch[kkk] = iii;
				}
				bestDistance[kkk] = distance;
				break;
			} else {
				if (kkk == 0) {
					DOLLAR_VERBOSE("[" << iii << "]                     d=" << distance << " < bd=" << bestDistance << " ");
				}
			}
		}
	}
	// Check if we have match ...
	if (-1 == indexOfBestMatch[0]) {
		DOLLAR_WARNING("Couldn't find a good match.");
		return Results();
	}
	Results res;
	for (size_t iii=0; iii<m_nbResult; ++iii) {
		if (-1 != indexOfBestMatch[iii]) {
			//float score = std::max((2.0 - bestDistance[iii])/2.0, 0.0);
			float score = bestDistance[iii];
			res.addValue(m_gestures[indexOfBestMatch[iii]]->getName(), score);
		}
	}
	return res;
}

