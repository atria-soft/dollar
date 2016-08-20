/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <dollar/Engine.h>
#include <dollar/debug.h>
#include <dollar/Rectangle.h>
#include <dollar/tools.h>
#include <etk/os/FSNode.h>
#include <math.h>
#include <algorithm>
#define MAX_FLOAT std::numeric_limits<float>::max()
// simple decree to radian convertion
#define DEG_2_RAD(ddd) (ddd*M_PI/180.0)

// Test is done on the square of 1.0f * 1.0f ==> the result depend on the diagonal size
#define DIAGONAL (1.414213562)
#define HALF_DIAGONAL (0.707106781)
// an other magic number ratio of starting vetor pos... ==> must be reworked ==> nbElementInVector/RATIO_START_VECTOR
#define RATIO_START_VECTOR (8)
// angle precision of the detecting the way of rotation
#define ANGLE_ROTATION (2.0f)
// Angle to start processing of a start vector comparaison
#define ANGLE_THRESHOLD_START_PROCESSING DEG_2_RAD(30.0)

// A magic number: 0.5 * (-1.0 + sqrt(5.0))
#define MAGIC_RATIO (0.618033989)



static float angleBetweenUnitVectors(const vec2& _vect1, const vec2& _vect2) {
	float n = _vect1.dot(_vect2);
	// TODO : No needs
	if (n < -1.0 || n > +1.0){
		n = round(n*100000.0f)/100000.0f;
	}
	return std::acos(n); // arc cosine of the vector dot product
}

static float pathDistance(const std::vector<vec2>& _path1, const std::vector<vec2>& _path2) {
	// assumes pts1.size == pts2.size
	float distance = 0.0;
	if (_path1.size() != _path2.size()) {
		DOLLAR_ERROR("Path have not the same size: " << _path1.size() << " != " << _path2.size());
		return MAX_FLOAT;
	}
	for (size_t iii=0; iii<_path1.size(); ++iii) {
		distance += (_path2[iii]-_path1[iii]).length();
	}
	DOLLAR_VERBOSE("distance: " << distance << " size= " << _path1.size());
	return (distance / _path1.size());
}

dollar::Engine::Engine() {
	m_numPointsInGesture = 128;
	DOLLAR_ASSERT(m_numPointsInGesture>16, "NB element in a path must be > 16 ...");
	setRotationInvariance(false);
}


void dollar::Engine::setNumberPointInGesture(size_t _value) {
	if (_value == m_numPointsInGesture) {
		return;
	}
	m_numPointsInGesture = _value;
	DOLLAR_ASSERT(m_numPointsInGesture>16, "NB element in a path must be > 16 ...");
	for (auto &it: m_gestures) {
		it.configure(m_numPointsInGesture/RATIO_START_VECTOR, m_numPointsInGesture, m_paramterIgnoreRotation);
	}
}

size_t dollar::Engine::getNumberPointInGesture() {
	return m_numPointsInGesture;
}

float dollar::Engine::distanceAtBestAngle(const std::vector<vec2>& _points, const std::vector<vec2>& _reference) {
	float startRange = -m_angleRange;
	float endRange = m_angleRange;
	float x1 = MAGIC_RATIO * startRange + (1.0 - MAGIC_RATIO) * endRange;
	float f1 = pathDistance(dollar::rotateBy(_points, x1), _reference);
	float x2 = (1.0 - MAGIC_RATIO) * startRange + MAGIC_RATIO * endRange;
	float f2 = pathDistance(dollar::rotateBy(_points, x2), _reference);
	DOLLAR_VERBOSE("init: startRange=" << startRange << " endRange=" << endRange << " MAGIC_RATIO=" << MAGIC_RATIO << " x1=" << x1 << " f1=" << f1 << " x2=" << x2 << " f2=" << f2);
	while (fabs(endRange - startRange) > ANGLE_ROTATION) {
		if (f1 < f2) {
			endRange = x2;
			x2 = x1;
			f2 = f1;
			x1 = MAGIC_RATIO * startRange + (1.0f - MAGIC_RATIO) * endRange;
			f1 = pathDistance(dollar::rotateBy(_points, x1), _reference);
		} else {
			startRange = x1;
			x1 = x2;
			f1 = f2;
			x2 = (1.0f - MAGIC_RATIO) * startRange + MAGIC_RATIO * endRange;
			f2 = pathDistance(dollar::rotateBy(_points, x2), _reference);
		}
	}
	return std::min(f1, f2);
}

float dollar::Engine::optimalCosineDistance(const std::vector<vec2>& _vect1, const std::vector<vec2>& _vect2) {
	if (_vect1.size() != _vect2.size()) {
		DOLLAR_ERROR("Vector have not the same size: " << _vect1.size() << " != " << _vect2.size());
		return M_PI;
	}
	double somDot = 0;
	double somCross = 0;
	for (size_t iii=0; iii<_vect1.size(); ++iii) {
		somDot += _vect1[iii].dot(_vect2[iii]);
		somCross += _vect1[iii].cross(_vect2[iii]);
	}
	if (somDot == 0.0f) {
		DOLLAR_ERROR("devide by 0");
		return M_PI;
	}
	float angle = std::atan(somCross / somDot);
	return std::acos(somDot * std::cos(angle) + somCross * std::sin(angle));
}



void dollar::Engine::setRotationInvariance(bool _ignoreRotation) {
	m_paramterIgnoreRotation = _ignoreRotation;
	if (m_paramterIgnoreRotation == true) {
		m_angleRange = 45.0;
	} else {
		m_angleRange = 15.0;
	}
}

bool dollar::Engine::loadPath(const std::string& _path) {
	DOLLAR_INFO("Load Path: " << _path);
	etk::FSNode path(_path);
	std::vector<std::string> files = path.folderGetSub(false, true, "*.json");
	for (auto &it : files) {
		loadGesture(it);
	}
	return true;
}

bool dollar::Engine::loadGesture(const std::string& _filename) {
	dollar::Gesture ref;
	DOLLAR_INFO("Load Gesture: " << _filename);
	if (ref.load(_filename) == true) {
		addGesture(std::move(ref));
		return true;
	}
	return false;
}

void dollar::Engine::addGesture(Gesture _gesture) {
	_gesture.configure(m_numPointsInGesture/RATIO_START_VECTOR, m_numPointsInGesture, m_paramterIgnoreRotation);
	m_gestures.push_back(std::move(_gesture));
}

dollar::Results dollar::Engine::recognize(const std::vector<vec2>& _points, const std::string& _method) {
	std::vector<std::vector<vec2>> tmp;
	tmp.push_back(_points);
	return recognize(tmp, _method);
}
#define MAX_RESULT_NUMBER (5)

dollar::Results dollar::Engine::recognize(const std::vector<std::vector<vec2>>& _strokes, const std::string& _method) {
	std::vector<vec2> points = dollar::combineStrokes(_strokes);
	// Make sure we have some templates to compare this to
	//  or else recognition will be impossible
	if (m_gestures.empty()) {
		DOLLAR_WARNING("No templates loaded so no symbols to match.");
		return Results();
	}
	points = dollar::normalizePath(points, m_numPointsInGesture, m_paramterIgnoreRotation);
	vec2 startv = dollar::getStartVector(points, m_numPointsInGesture/RATIO_START_VECTOR);
	std::vector<vec2> vector = normalyse(points);
	// Keep maximum 5 results ...
	float bestDistance[MAX_RESULT_NUMBER];
	int32_t indexOfBestMatch[MAX_RESULT_NUMBER];
	for (size_t iii=0; iii<MAX_RESULT_NUMBER; ++iii) {
		bestDistance[iii] = MAX_FLOAT;
		indexOfBestMatch[iii] = -1;
	}
	// for each multistroke
	for (size_t iii=0; iii<m_gestures.size(); ++iii) {
		DOLLAR_DEBUG("[" << iii << "] '" << m_gestures[iii].getName() << "'");
		Gesture gesture = m_gestures[iii];
		for (size_t jjj=0; jjj<gesture.getEngineSize(); ++jjj) {
			if (gesture.getEnginePath(jjj).size() == 0) {
				DOLLAR_ERROR("Reference path with no Value");
				continue;
			}
			// strokes start in the same direction
			if(angleBetweenUnitVectors(startv, gesture.getEngineStartVector(jjj)) > ANGLE_THRESHOLD_START_PROCESSING) {
				continue;
			}
			float distance = MAX_FLOAT;
			// for Protractor
			if (_method=="protractor") {
				distance = Engine::optimalCosineDistance(vector, gesture.getEngineVector(jjj));
			} else {
				// Golden Section Search (original $N)
				distance = Engine::distanceAtBestAngle(points, gesture.getEnginePath(jjj));
			}
			for (size_t kkk=0; kkk<MAX_RESULT_NUMBER; ++kkk) {
				if (distance < bestDistance[kkk]) {
					if (kkk == 0) {
						DOLLAR_DEBUG("[" << iii << "," << jjj << "]                     d=" << distance << " < bd=" << bestDistance << " ");
					}
					if (indexOfBestMatch[kkk] != int64_t(iii)) {
						for (int32_t rrr=MAX_RESULT_NUMBER-1; rrr>int32_t(kkk); --rrr) {
							bestDistance[rrr] = bestDistance[rrr-1];
							indexOfBestMatch[rrr] = indexOfBestMatch[rrr-1];
						}
						indexOfBestMatch[kkk] = iii;
					}
					bestDistance[kkk] = distance;
					break;
				} else {
					if (kkk == 0) {
						DOLLAR_VERBOSE("[" << iii << "," << jjj << "]                     d=" << distance << " < bd=" << bestDistance << " ");
					}
				}
			}
		}
	}
	// Make sure we actually found a good match
	// Sometimes we don't, like when the user doesn't draw enough points
	if (-1 == indexOfBestMatch[0]) {
		DOLLAR_WARNING("Couldn't find a good match.");
		return Results();
	}
	Results res;
	// Turn the distance into a percentage by dividing it by half the maximum possible distance (across the diagonal of the square we scaled everything too)
	// Distance = hwo different they are subtract that from 1 (100%) to get the similarity
	if (_method == "protractor") {
		for (size_t iii=0; iii<MAX_RESULT_NUMBER; ++iii) {
			if (-1 != indexOfBestMatch[0]) {
				float score = MAX_FLOAT;;
				if (bestDistance[iii] != 0.0) {
					score = 1.0f / bestDistance[iii];
				}
				res.addValue(m_gestures[indexOfBestMatch[iii]].getName(), score);
			}
		}
	} else {
		for (size_t iii=0; iii<MAX_RESULT_NUMBER; ++iii) {
			if (-1 != indexOfBestMatch[0]) {
				float score = 1.0 - (bestDistance[iii] / HALF_DIAGONAL);
				res.addValue(m_gestures[indexOfBestMatch[iii]].getName(), score);
			}
		}
	}
	return res;
}

