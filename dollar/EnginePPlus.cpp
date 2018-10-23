/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <dollar/EnginePPlus.hpp>
#include <dollar/debug.hpp>
#include <dollar/Rectangle.hpp>
#include <dollar/tools.hpp>

#include <cmath>
#include <algorithm>

#define MAX_FLOAT std::numeric_limits<float>::max()

dollar::EnginePPlus::EnginePPlus():
  m_PPlusDistance(0.10f),
  m_PPlusExcludeDistance(0.2*0.2),
  m_scaleKeepRatio(false),
  m_penalityNotLinkRef(0.1),
  m_penalityNotLinkSample(0.1) {
	
}


void dollar::EnginePPlus::setPPlusDistance(float _value) {
	if (_value == m_PPlusDistance) {
		return;
	}
	m_PPlusDistance = _value;
	for (auto &it: m_gestures) {
		if (it == null) {
			continue;
		}
		it->configure(m_PPlusDistance, m_scaleKeepRatio);
	}
}

float dollar::EnginePPlus::getPPlusDistance() {
	return m_PPlusDistance;
}

void dollar::EnginePPlus::setPPlusExcludeDistance(float _value) {
	if (_value*_value == m_PPlusExcludeDistance) {
		return;
	}
	m_PPlusExcludeDistance = _value*_value;
}

float dollar::EnginePPlus::getPPlusExcludeDistance() {
	return std::sqrt(m_PPlusExcludeDistance);
}
void dollar::EnginePPlus::setScaleKeepRatio(bool _value) {
	if (_value == m_scaleKeepRatio) {
		return;
	}
	m_scaleKeepRatio = _value;
	for (auto &it: m_gestures) {
		if (it == null) {
			continue;
		}
		it->configure(m_PPlusDistance, m_scaleKeepRatio);
	}
}

bool dollar::EnginePPlus::getScaleKeepRatio() {
	return m_scaleKeepRatio;
}

void dollar::EnginePPlus::setPenalityNotLinkRef(float _value) {
	m_penalityNotLinkRef = _value;
}

float dollar::EnginePPlus::getPenalityNotLinkRef() {
	return m_penalityNotLinkRef;
}

void dollar::EnginePPlus::setPenalityNotLinkSample(float _value) {
	m_penalityNotLinkSample = _value;
}

float dollar::EnginePPlus::getPenalityNotLinkSample() {
	return m_penalityNotLinkSample;
}

void dollar::EnginePPlus::setPenalityAspectRatio(float _value) {
	m_penalityAspectRatio = _value;
}

float dollar::EnginePPlus::getPenalityAspectRatio() {
	return m_penalityAspectRatio;
}


float dollar::EnginePPlus::calculatePPlusDistanceSimple(const etk::Vector<vec2>& _points,
                                                        const etk::Vector<vec2>& _reference,
                                                        etk::Vector<etk::Pair<int32_t, int32_t>>& _dataDebug) {
	etk::Vector<float> distance; // note: use square distance (faster, we does not use std::sqrt())
	distance.resize(_points.size(), MAX_FLOAT);
	// point Id that is link on the reference.
	etk::Vector<int32_t> usedId;
	usedId.resize(_points.size(), -1);
	for (size_t iii=0; iii<_points.size(); iii++) {
		float bestDistance = MAX_FLOAT;
		int32_t kkkBest = -1;
		for (size_t kkk=0; kkk<_reference.size(); ++kkk) {
			float dist = (_points[iii]-_reference[kkk]).length2();
			if (dist < bestDistance) {
				bestDistance = dist;
				kkkBest = kkk;
			}
		}
		if (kkkBest != -1) {
			// reject the distance ... if too big ...
			if (bestDistance <= m_PPlusExcludeDistance) {
				usedId[iii] = kkkBest;
				distance[iii] = bestDistance;
				//DOLLAR_INFO("set new link: " << iii << " with " << kkkBest << "     d=" << bestDistance);
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
	fullDistance += float(nbTestNotUsed)* m_penalityNotLinkSample;
	fullDistance += float(nbReferenceNotUsed)* m_penalityNotLinkRef;
	
	for (size_t kkk=0; kkk<usedId.size(); ++kkk) {
		if (usedId[kkk] != -1) {
			_dataDebug.pushBack(etk::makePair(kkk, usedId[kkk]));
		}
	}
	DOLLAR_DEBUG("test distance : " << fullDistance << " nbTestNotUsed=" << nbTestNotUsed << " nbReferenceNotUsed=" << nbReferenceNotUsed);
	return fullDistance;
}


float dollar::EnginePPlus::calculatePPlusDistance(const etk::Vector<vec2>& _points,
                                                  const etk::Vector<vec2>& _reference,
                                                  etk::Vector<etk::Pair<int32_t, int32_t>>& _dataDebug,
                                                  float _inputAspectRatio,
                                                  float _referenceAspectRatio) {
	etk::Vector<float> distance; // note: use square distance (faster, we does not use std::sqrt())
	distance.resize(_points.size(), MAX_FLOAT);
	// point Id that is link on the reference.
	etk::Vector<int32_t> usedId;
	usedId.resize(_reference.size(), -1);
	for (int32_t iii=0; iii<int32_t(_points.size()); iii++) {
		if (distance[iii] < 100.0) {
			continue;
		}
		float bestDistance = MAX_FLOAT;
		int32_t kkkBest = -1;
		for (size_t kkk=0; kkk<_reference.size(); ++kkk) {
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
	fullDistance += float(nbTestNotUsed)* m_penalityNotLinkSample;
	fullDistance += float(nbReferenceNotUsed)* m_penalityNotLinkRef;
	fullDistance += etk::abs(_inputAspectRatio - _referenceAspectRatio)*m_penalityAspectRatio;
	
	
	for (size_t kkk=0; kkk<usedId.size(); ++kkk) {
		if (usedId[kkk] != -1) {
			_dataDebug.pushBack(etk::makePair(usedId[kkk], kkk));
		}
	}
	DOLLAR_DEBUG("test distance : " << fullDistance << " nbTestNotUsed=" << nbTestNotUsed << " nbReferenceNotUsed=" << nbReferenceNotUsed);
	return fullDistance;
}



bool dollar::EnginePPlus::loadGesture(const etk::Uri& _uri) {
	ememory::SharedPtr<dollar::Gesture> ref = ememory::makeShared<dollar::GesturePPlus>();
	DOLLAR_DEBUG("Load Gesture: " << _uri);
	if (ref->load(_uri) == true) {
		addGesture(ref);
		return true;
	}
	return false;
}

void dollar::EnginePPlus::addGesture(ememory::SharedPtr<dollar::Gesture> _gesture) {
	ememory::SharedPtr<dollar::GesturePPlus> gest = ememory::dynamicPointerCast<dollar::GesturePPlus>(_gesture);
	if (gest != null) {
		gest->configure(m_PPlusDistance, m_scaleKeepRatio);
		m_gestures.pushBack(gest);
	}
}

static void storeSVG(const etk::Uri& _fileName,
                     const ememory::SharedPtr<dollar::GesturePPlus>& _gesture,
                     const etk::Vector<etk::Vector<vec2>>& _strokes,
                     const etk::Vector<vec2>& _points,
                     etk::Vector<etk::Pair<int32_t, int32_t>> _links,
                     bool _keepAspectRatio) {
	etk::String data("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
	data += "<svg height=\"100\" width=\"100\">\n";
	for (auto &itLines : dollar::scaleToOne(_gesture->getPath(), _keepAspectRatio)) {
		data += "	<polyline fill=\"none\" stroke=\"black\" stroke-opacity=\"0.8\" stroke-width=\"2\"\n";
		data += "	          points=\"";
		bool first = true;
		for (auto& itPoints : itLines) {
			if (first == false) {
				data += " ";
			}
			first = false;
			data += etk::toString(itPoints.x()*100.0f) + "," + etk::toString((1.0-itPoints.y())*100.0f);
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
			data += etk::toString(itPoints.x()*100.0f) + "," + etk::toString((1.0-itPoints.y())*100.0f);
		}
		data += "\"\n";
		data += "	          />\n";
	}
	etk::Vector<vec2> refListPoint = _gesture->getEnginePoints();
	for (auto &it : refListPoint) {
		data += "	<circle fill=\"red\" cx=\"" + etk::toString(it.x()*100.0f) + "\" cy=\"" + etk::toString((1.0-it.y())*100.0f) + "\" r=\"0.6\"/>\n";
	}
	etk::Vector<vec2> testListPoint = _points;
	for (auto &it : testListPoint) {
		data += "	<circle fill=\"orange\" cx=\"" + etk::toString(it.x()*100.0f) + "\" cy=\"" + etk::toString((1.0-it.y())*100.0f) + "\" r=\"0.6\"/>\n";
	}
	for (auto &it : _links) {
		data += "	<polyline fill=\"none\" stroke=\"blue\" stroke-opacity=\"0.8\" stroke-width=\"0.5\"\n";
		data += "	          points=\"";
		data += etk::toString(refListPoint[it.second].x()*100.0f) + "," + etk::toString((1.0-refListPoint[it.second].y())*100.0f);
		data += " ";
		data += etk::toString(testListPoint[it.first].x()*100.0f) + "," + etk::toString((1.0-testListPoint[it.first].y())*100.0f);
		data += "\"\n";
		data += "	          />\n";
	}
	data += "</svg>\n";
	{
		ememory::SharedPtr<etk::io::Interface> fileIO = etk::uri::get(_fileName);
		if (fileIO->open(etk::io::OpenMode::Write) == false) {
			return;
		}
		fileIO->writeAll(data);
		fileIO->close();
	}
}


dollar::Results dollar::EnginePPlus::recognize2(const etk::Vector<etk::Vector<vec2>>& _strokes) {
	etk::Vector<vec2> points = dollar::normalizePathToPoints(_strokes, m_PPlusDistance, m_scaleKeepRatio);
	float inputAspectRatio = dollar::getAspectRatio(_strokes);
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
		ememory::SharedPtr<GesturePPlus> gesture = m_gestures[iii];
		if (gesture->getEnginePoints().size() == 0) {
			//DOLLAR_ERROR("Reference path with no Value");
			continue;
		}
		int32_t nbStrokeSample = _strokes.size();
		int32_t nbStrokeRef = gesture->getPath().size();
		/*
		if (nbStrokeSample != nbStrokeRef) {
			continue; //==> must have the same number of stroke ...
		}
		*/
		float distance = MAX_FLOAT;
		etk::Vector<etk::Pair<int32_t, int32_t>> dataPair;
		distance = calculatePPlusDistance(points, gesture->getEnginePoints(), dataPair, inputAspectRatio, gesture->getAspectRatio());
		//distance = calculatePPlusDistanceSimple(points, gesture->getEnginePoints(), dataPair);
		if (nbStrokeRef != nbStrokeSample) {
			distance += 0.1f*float(etk::abs(nbStrokeRef-nbStrokeSample));
		}
		//storeSVG("out_dollar/lib/recognizePPlus/" + gesture->getName() + "_" + etk::toString(gesture->getId()) + ".svg", gesture, _strokes, points, dataPair, m_scaleKeepRatio);
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
			//float score = etk::max((2.0 - bestDistance[iii])/2.0, 0.0);
			float score = bestDistance[iii];
			res.addValue(m_gestures[indexOfBestMatch[iii]]->getName(), score);
		}
	}

	return res;
}