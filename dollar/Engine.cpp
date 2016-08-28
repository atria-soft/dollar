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

dollar::Engine::Engine():
  m_PPlusDistance(0.10f),
  m_PPlusExcludeDistance(0.2*0.2),
  m_scaleKeepRatio(false) {
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
		it.configure(m_numPointsInGesture/RATIO_START_VECTOR, m_numPointsInGesture, m_paramterIgnoreRotation, m_PPlusDistance, m_scaleKeepRatio);
	}
}

size_t dollar::Engine::getNumberPointInGesture() {
	return m_numPointsInGesture;
}

void dollar::Engine::setPPlusDistance(float _value) {
	if (_value*_value == m_PPlusDistance*) {
		return;
	}
	m_PPlusDistance = _value*_value;
	for (auto &it: m_gestures) {
		it.configure(m_numPointsInGesture/RATIO_START_VECTOR, m_numPointsInGesture, m_paramterIgnoreRotation, m_PPlusDistance, m_scaleKeepRatio);
	}
}

float dollar::Engine::getPPlusDistance() {
	return std::sqrt(m_PPlusDistance);
}

void dollar::Engine::setPPlusExcludeDistance(float _value) {
	if (_value == m_PPlusExcludeDistance) {
		return;
	}
	m_PPlusExcludeDistance = _value;
}

float dollar::Engine::getPPlusExcludeDistance() {
	return m_PPlusExcludeDistance;
}
void dollar::Engine::setScaleKeepRatio(bool _value) {
	if (_value == m_scaleKeepRatio) {
		return;
	}
	m_scaleKeepRatio = _value;
	for (auto &it: m_gestures) {
		it.configure(m_numPointsInGesture/RATIO_START_VECTOR, m_numPointsInGesture, m_paramterIgnoreRotation, m_PPlusDistance, m_scaleKeepRatio);
	}
}

bool dollar::Engine::getScaleKeepRatio() {
	return m_scaleKeepRatio;
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


static float calculatePPlusDistance(const std::vector<vec2>& _points,
                                    const std::vector<vec2>& _reference,
                                    std::vector<std::pair<int32_t, int32_t>>& _dataDebug) {
	std::vector<float> distance; // note: use square distance (faster, we does not use std::sqrt())
	distance.resize(_points.size(), MAX_FLOAT);
	// point Id that is link on the reference.
	std::vector<int32_t> usedId;
	usedId.resize(_reference.size(), -1);
	for (int32_t iii=0; iii<_points.size(); iii++) {
		if (distance[iii] < 100.0) {
			continue;
		}
		float bestDistance = MAX_FLOAT;
		int32_t kkkBest = -1;
		for (int32_t kkk=0; kkk<_reference.size(); ++kkk) {
			float dist = (_points[iii]-_reference[kkk]).length2();
			if (usedId[kkk] != -1) {
				if (dist < distance[usedId[kkk]]) {
					if (dist < bestDistance) {
						bestDistance = dist;
						kkkBest = kkk;
					}
				}
			} else {
				if (dist < bestDistance) {
					bestDistance = dist;
					kkkBest = kkk;
				}
			}
		}
		if (kkkBest != -1) {
			// reject the distance ... if too big ...
			if (bestDistance <= m_PPlusExcludeDistance) {
				int32_t previous = usedId[kkkBest];
				usedId[kkkBest] = iii;
				distance[iii] = bestDistance;
				//DOLLAR_INFO("set new link: " << iii << " with " << kkkBest << "     d=" << bestDistance);
				if (previous != -1) {
					//DOLLAR_INFO("     Reject : " << previous);
					distance[previous] = MAX_FLOAT;
					iii = previous-1;
				}
			}
		}
	}
	double fullDistance = 0;
	int32_t nbTestNotUsed = 0;
	int32_t nbReferenceNotUsed = 0;
	// now we count the full distance use and the number of local gesture not use
	for (auto &it : distance) {
		if (it < 100.0) {
			fullDistance += it;
		} else {
			nbTestNotUsed++;
		}
	}
	// we count the number of point in the gesture reference not used:
	for (auto &it : usedId) {
		if (it == -1) {
			nbReferenceNotUsed++;
		}
	}
	// now we add panality:
	fullDistance += float(nbTestNotUsed)* 0.1f;
	fullDistance += float(nbReferenceNotUsed)* 0.1f;
	
	for (int32_t kkk=0; kkk<usedId.size(); ++kkk) {
		if (usedId[kkk] != -1) {
			_dataDebug.push_back(std::make_pair(usedId[kkk], kkk));
		}
	}
	DOLLAR_DEBUG("test distance : " << fullDistance << " nbTestNotUsed=" << nbTestNotUsed << " nbReferenceNotUsed=" << nbReferenceNotUsed);
	return fullDistance;
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
		if (etk::end_with(it, ".json") == true) {
			loadGesture(it);
		}
	}
	return true;
}

bool dollar::Engine::loadGesture(const std::string& _filename) {
	dollar::Gesture ref;
	DOLLAR_DEBUG("Load Gesture: " << _filename);
	if (ref.load(_filename) == true) {
		addGesture(std::move(ref));
		return true;
	}
	return false;
}

void dollar::Engine::addGesture(Gesture _gesture) {
	_gesture.configure(m_numPointsInGesture/RATIO_START_VECTOR, m_numPointsInGesture, m_paramterIgnoreRotation, m_PPlusDistance, m_scaleKeepRatio);
	m_gestures.push_back(std::move(_gesture));
}

dollar::Results dollar::Engine::recognize(const std::vector<vec2>& _points, const std::string& _method) {
	std::vector<std::vector<vec2>> tmp;
	tmp.push_back(_points);
	return recognize(tmp, _method);
}
#define MAX_RESULT_NUMBER (5)

dollar::Results dollar::Engine::recognize(const std::vector<std::vector<vec2>>& _strokes, const std::string& _method) {
	// Check if we have gestures...
	if (m_gestures.empty()) {
		DOLLAR_WARNING("No templates loaded so no symbols to match.");
		return Results();
	}
	if (    _method == "$N-protractor"
	     || _method == "$N"
	     || _method == "$1") {
		return recognizeN(_strokes, _method);
	} else if (_method == "$P") {
		return recognizeP(_strokes);
	} else if (_method == "$P+") {
		return recognizePPlus(_strokes);
	}
	DOLLAR_WARNING("Un-recognise methode ... '" << _method << "' supported: [$1,$N,$N-protractor,$P,$P+]" );
	return Results();
}


dollar::Results dollar::Engine::recognizeN(const std::vector<std::vector<vec2>>& _strokes, const std::string& _method) {
	std::vector<vec2> points = dollar::combineStrokes(_strokes);
	points = dollar::normalizePath(points, m_numPointsInGesture, m_paramterIgnoreRotation, m_scaleKeepRatio);
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
			if (_method=="$p-protractor") {
				distance = optimalCosineDistance(vector, gesture.getEngineVector(jjj));
			} else {
				// Golden Section Search (original $N)
				distance = distanceAtBestAngle(points, gesture.getEnginePath(jjj));
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
	// Check if we have match ...
	if (-1 == indexOfBestMatch[0]) {
		DOLLAR_WARNING("Couldn't find a good match.");
		return Results();
	}
	Results res;
	// transform distance in a % range
	if (_method == "$p-protractor") {
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


dollar::Results dollar::Engine::recognizeP(const std::vector<std::vector<vec2>>& _strokes) {
	std::vector<vec2> points = dollar::combineStrokes(_strokes);
	points = dollar::normalizePath(points, m_numPointsInGesture, m_paramterIgnoreRotation, m_scaleKeepRatio);
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
		if (gesture.getEngineSize() == 0) {
			// Only need the first gesture ...
			continue;
		}
		size_t jjj = 0;
		if (gesture.getEnginePath(jjj).size() == 0) {
			DOLLAR_ERROR("Reference path with no Value");
			continue;
		}
		float distance = MAX_FLOAT;
		distance = calculateBestDistance(points, gesture.getEnginePath(jjj));
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
	// Check if we have match ...
	if (-1 == indexOfBestMatch[0]) {
		DOLLAR_WARNING("Couldn't find a good match.");
		return Results();
	}
	Results res;
	for (size_t iii=0; iii<MAX_RESULT_NUMBER; ++iii) {
		if (-1 != indexOfBestMatch[iii]) {
			//float score = std::max((2.0 - bestDistance[iii])/2.0, 0.0);
			float score = bestDistance[iii];
			res.addValue(m_gestures[indexOfBestMatch[iii]].getName(), score);
		}
	}

	return res;
}


static void storeSVG(const std::string& _fileName,
                     const dollar::Gesture& gesture,
                     const std::vector<std::vector<vec2>>& _strokes,
                     const std::vector<vec2>& _points,
                     std::vector<std::pair<int32_t, int32_t>> _links,
                     bool _keepAspectRatio) {
	std::string data("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
	data += "<svg height=\"100\" width=\"100\">\n";
	for (auto &itLines : gesture.getPath()) {
		data += "	<polyline fill=\"none\" stroke=\"black\" stroke-opacity=\"0.8\" stroke-width=\"2\"\n";
		data += "	          points=\"";
		bool first = true;
		for (auto& itPoints : itLines) {
			if (first == false) {
				data += " ";
			}
			first = false;
			data += etk::to_string(itPoints.x()*100.0f) + "," + etk::to_string((1.0-itPoints.y())*100.0f);
		}
		data += "\"\n";
		data += "	          />\n";
	}
	for (auto &itLines : dollar::scaleToOne(_strokes, _keepAspectRatio)) {
		data += "	<polyline fill=\"none\" stroke=\"purple\" stroke-opacity=\"0.8\" stroke-width=\"2\"\n";
		data += "	          points=\"";
		bool first = true;
		for (auto& itPoints : itLines) {
			if (first == false) {
				data += " ";
			}
			first = false;
			data += etk::to_string(itPoints.x()*100.0f) + "," + etk::to_string((1.0-itPoints.y())*100.0f);
		}
		data += "\"\n";
		data += "	          />\n";
	}
	std::vector<vec2> refListPoint = gesture.getEnginePoints();
	for (auto &it : refListPoint) {
		data += "	<circle fill=\"red\" cx=\"" + etk::to_string(it.x()*100.0f) + "\" cy=\"" + etk::to_string((1.0-it.y())*100.0f) + "\" r=\"0.6\"/>\n";
	}
	std::vector<vec2> testListPoint = _points;
	for (auto &it : testListPoint) {
		data += "	<circle fill=\"orange\" cx=\"" + etk::to_string(it.x()*100.0f) + "\" cy=\"" + etk::to_string((1.0-it.y())*100.0f) + "\" r=\"0.6\"/>\n";
	}
	for (auto &it : _links) {
		data += "	<polyline fill=\"none\" stroke=\"blue\" stroke-opacity=\"0.8\" stroke-width=\"0.5\"\n";
		data += "	          points=\"";
		data += etk::to_string(refListPoint[it.second].x()*100.0f) + "," + etk::to_string((1.0-refListPoint[it.second].y())*100.0f);
		data += " ";
		data += etk::to_string(testListPoint[it.first].x()*100.0f) + "," + etk::to_string((1.0-testListPoint[it.first].y())*100.0f);
		data += "\"\n";
		data += "	          />\n";
	}
	data += "</svg>\n";
	etk::FSNodeWriteAllData(_fileName, data);
}


dollar::Results dollar::Engine::recognizePPlus(const std::vector<std::vector<vec2>>& _strokes) {
	std::vector<vec2> points = dollar::normalizePathToPoints(_strokes, m_PPlusDistance, m_scaleKeepRatio);
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
		if (gesture.getEngineSize() == 0) {
			// Only need the first gesture ...
			continue;
		}
		if (gesture.getEnginePoints().size() == 0) {
			//DOLLAR_ERROR("Reference path with no Value");
			continue;
		}
		float distance = MAX_FLOAT;
		std::vector<std::pair<int32_t, int32_t>> dataPair;
		distance = calculatePPlusDistance(points, gesture.getEnginePoints(), dataPair);
		//storeSVG("out_dollar/lib/recognizePPlus/" + gesture.getName() + "_" + etk::to_string(gesture.getId()) + ".svg", gesture, _strokes, points, dataPair, m_scaleKeepRatio);
		for (size_t kkk=0; kkk<MAX_RESULT_NUMBER; ++kkk) {
			if (distance < bestDistance[kkk]) {
				if (kkk == 0) {
					DOLLAR_DEBUG("[" << iii << "]                     d=" << distance << " < bd=" << bestDistance << " ");
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
	for (size_t iii=0; iii<MAX_RESULT_NUMBER; ++iii) {
		if (-1 != indexOfBestMatch[iii]) {
			//float score = std::max((2.0 - bestDistance[iii])/2.0, 0.0);
			float score = bestDistance[iii];
			res.addValue(m_gestures[indexOfBestMatch[iii]].getName(), score);
		}
	}

	return res;
}