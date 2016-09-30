/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <dollar/EngineN.hpp>
#include <dollar/debug.hpp>
#include <dollar/Rectangle.hpp>
#include <dollar/tools.hpp>
#include <etk/os/FSNode.hpp>

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

dollar::EngineN::EngineN(bool _protractor):
  m_protractor(_protractor),
  m_numPointsInGesture(128) {
	DOLLAR_ASSERT(m_numPointsInGesture>16, "NB element in a path must be > 16 ...");
	setRotationInvariance(false);
}


void dollar::EngineN::setNumberPointInGesture(size_t _value) {
	if (_value == m_numPointsInGesture) {
		return;
	}
	m_numPointsInGesture = _value;
	DOLLAR_ASSERT(m_numPointsInGesture>16, "NB element in a path must be > 16 ...");
	for (auto &it: m_gestures) {
		if (it == nullptr) {
			continue;
		}
		it->configure(m_numPointsInGesture/RATIO_START_VECTOR, m_numPointsInGesture, m_paramterIgnoreRotation);
	}
}

size_t dollar::EngineN::getNumberPointInGesture() {
	return m_numPointsInGesture;
}

float dollar::EngineN::distanceAtBestAngle(const std::vector<vec2>& _points, const std::vector<vec2>& _reference) {
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


float dollar::EngineN::optimalCosineDistance(const std::vector<vec2>& _vect1, const std::vector<vec2>& _vect2) {
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



void dollar::EngineN::setRotationInvariance(bool _ignoreRotation) {
	m_paramterIgnoreRotation = _ignoreRotation;
	if (m_paramterIgnoreRotation == true) {
		m_angleRange = 45.0;
	} else {
		m_angleRange = 15.0;
	}
}

bool dollar::EngineN::loadGesture(const std::string& _filename) {
	ememory::SharedPtr<dollar::Gesture> ref = ememory::makeShared<dollar::GestureN>();
	DOLLAR_DEBUG("Load Gesture: " << _filename);
	if (ref->load(_filename) == true) {
		addGesture(ref);
		return true;
	}
	return false;
}

void dollar::EngineN::addGesture(ememory::SharedPtr<dollar::Gesture> _gesture) {
	ememory::SharedPtr<dollar::GestureN> gest = ememory::dynamicPointerCast<dollar::GestureN>(_gesture);
	if (gest != nullptr) {
		gest->configure(m_numPointsInGesture/RATIO_START_VECTOR, m_numPointsInGesture, m_paramterIgnoreRotation);
		m_gestures.push_back(gest);
	}
}


dollar::Results dollar::EngineN::recognize2(const std::vector<std::vector<vec2>>& _strokes) {
	std::vector<vec2> points = dollar::combineStrokes(_strokes);
	points = dollar::normalizePath(points, m_numPointsInGesture, m_paramterIgnoreRotation, false);
	vec2 startv = dollar::getStartVector(points, m_numPointsInGesture/RATIO_START_VECTOR);
	std::vector<vec2> vector = normalyse(points);
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
		ememory::SharedPtr<GestureN> gesture = m_gestures[iii];
		for (size_t jjj=0; jjj<gesture->getEngineSize(); ++jjj) {
			if (gesture->getEnginePath(jjj).size() == 0) {
				DOLLAR_ERROR("Reference path with no Value");
				continue;
			}
			// strokes start in the same direction
			if(angleBetweenUnitVectors(startv, gesture->getEngineStartVector(jjj)) > ANGLE_THRESHOLD_START_PROCESSING) {
				continue;
			}
			float distance = MAX_FLOAT;
			// for Protractor
			if (m_protractor == true) {
				distance = optimalCosineDistance(vector, gesture->getEngineVector(jjj));
			} else {
				// Golden Section Search (original $N)
				distance = distanceAtBestAngle(points, gesture->getEnginePath(jjj));
			}
			for (size_t kkk=0; kkk<m_nbResult; ++kkk) {
				if (distance < bestDistance[kkk]) {
					if (kkk == 0) {
						DOLLAR_DEBUG("[" << iii << "," << jjj << "]                     d=" << distance << " < bd=" << bestDistance << " ");
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
						DOLLAR_VERBOSE("[" << iii << "," << jjj << "]                     d=" << distance << " < bd=" << bestDistance << " ");
					}
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
	// transform distance in a % range
	if (m_protractor == true) {
		for (size_t iii=0; iii<m_nbResult; ++iii) {
			if (-1 != indexOfBestMatch[0]) {
				float score = MAX_FLOAT;;
				if (bestDistance[iii] != 0.0) {
					score = 1.0f / bestDistance[iii];
				}
				res.addValue(m_gestures[indexOfBestMatch[iii]]->getName(), score);
			}
		}
	} else {
		for (size_t iii=0; iii<m_nbResult; ++iii) {
			if (-1 != indexOfBestMatch[0]) {
				float score = 1.0 - (bestDistance[iii] / HALF_DIAGONAL);
				res.addValue(m_gestures[indexOfBestMatch[iii]]->getName(), score);
			}
		}
	}
	return res;
}

