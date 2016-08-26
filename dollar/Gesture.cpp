/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <dollar/debug.h>
#include <dollar/Gesture.h>
#include <dollar/tools.h>
#include <etk/os/FSNode.h>
#include <ejson/ejson.h>
#include <esvg/esvg.h>
#include <etk/stdTools.h>


static std::vector<std::vector<vec2>> loadPointsJson(const ejson::Document& _doc) {
	// extract lines:
	std::vector<std::vector<vec2>> out;
	const ejson::Array listOfLines = _doc["data"].toArray();
	if (listOfLines.exist() == false) {
		DOLLAR_WARNING("Reference element has no element named 'data' " << _doc["data"]);
		return out;
	}
	for (auto itLines : listOfLines) {
		ejson::Array listOfpoint = itLines.toArray();
		std::vector<vec2> line;
		for (auto itPoints : listOfpoint) {
			ejson::Array pointsArray = itPoints.toArray();
			line.push_back(vec2(pointsArray[0].toNumber().get(), pointsArray[1].toNumber().get()));
		}
		if (line.size() > 1) {
			out.push_back(std::move(line));
		}
	}
	return out;
}

std::vector<std::vector<vec2>> dollar::loadPoints(const std::string& _fileName, std::string* _label, std::string* _type) {
	ejson::Document doc;
	doc.load(_fileName);
	if (_label != nullptr) {
		*_label = doc["value"].toString().get();
	}
	if (_type != nullptr) {
		*_type = doc["type"].toString().get();
	}
	// extract lines:
	std::vector<std::vector<vec2>> out;
	ejson::Array listOfLines = doc["data"].toArray();
	for (auto itLines : listOfLines) {
		ejson::Array listOfpoint = itLines.toObject()["list"].toArray();
		std::vector<vec2> line;
		for (auto itPoints : listOfpoint) {
			ejson::Array pointsArray = itPoints.toArray();
			line.push_back(vec2(pointsArray[0].toNumber().get(), pointsArray[1].toNumber().get()));
		}
		if (line.size() > 1) {
			out.push_back(std::move(line));
		}
	}
	return out;
}


dollar::Gesture::Gesture():
  m_name(""),
  m_subId(0),
  m_path(){
	
}

bool dollar::Gesture::load(const std::string& _fileName) {
	std::string tmpName = etk::tolower(_fileName);
	if (etk::end_with(tmpName, ".json") == true) {
		return loadJSON(_fileName);
	} else if (etk::end_with(tmpName, ".svg") == true) {
		return loadSVG(_fileName);
	}
	DOLLAR_ERROR("Un-sopported LOAD extention : " << _fileName);
	DOLLAR_ERROR("    supported: [svg,json]");
	return false;
}

bool dollar::Gesture::loadJSON(const std::string& _fileName) {
	ejson::Document doc;
	doc.load(_fileName);
	if (doc["type"].toString().get() != "REFERENCE") {
		DOLLAR_WARNING("can not LOAD file that is not a reference");
	}
	m_name = doc["value"].toString().get();
	m_subId = doc["sub-id"].toNumber().getU64(),
	m_path = loadPointsJson(doc);
	DOLLAR_DEBUG("Load gesture : " << m_name << " id=" << m_subId << " nb_elem=" << m_path.size());
	return true;
}

bool dollar::Gesture::loadSVG(const std::string& _fileName) {
	esvg::Document doc;
	doc.load(_fileName);
	std::vector<std::string> plop = etk::split(_fileName, '.');
	plop = etk::split(plop[plop.size() -2], '/');
	plop = etk::split(plop[plop.size() -1], '_');
	m_name = plop[0];
	m_subId = etk::string_to_int32_t(plop[1]);
	m_path = doc.getLines();
	// SVG is written in the oposite mode ==> need to invert it ...
	for (auto &it : m_path) {
		for (auto &it2 : it) {
			it2.setY(it2.y()*-1);
		}
	}
	DOLLAR_DEBUG("Load gesture : " << m_name << " id=" << m_subId << " nb_elem=" << m_path.size());
	return true;
}


bool dollar::Gesture::store(const std::string& _fileName) {
	std::string tmpName = etk::tolower(_fileName);
	if (etk::end_with(tmpName, ".json") == true) {
		storeJSON(_fileName);
		return true;
	} else if (etk::end_with(tmpName, ".svg") == true) {
		storeSVG(_fileName);
		return true;
	}
	DOLLAR_ERROR("Un-sopported STORE extention : " << _fileName);
	DOLLAR_ERROR("    supported: [svg,json]");
	return false;
}

void dollar::Gesture::storeJSON(const std::string& _fileName) {
	ejson::Document doc;
	doc.add("type", ejson::String("REFERENCE"));
	doc.add("value", ejson::String(m_name));
	doc.add("sub-id", ejson::Number(m_subId));
	ejson::Array data;
	doc.add("data", data);
	for (auto &it : m_path) {
		ejson::Array path;
		for (auto &pathIt : it) {
			ejson::Array value;
			value.add(ejson::Number(pathIt.x()));
			value.add(ejson::Number(pathIt.y()));
			path.add(value);
		}
		data.add(path);
	}
	doc.store(_fileName);
}

void dollar::Gesture::storeSVG(const std::string& _fileName, bool _storeDot) {
	std::vector<std::vector<vec2>> strokes = dollar::scaleToOne(m_path, true);
	std::string data("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
	data += "<svg height=\"100\" width=\"100\">\n";
	for (auto &itLines : strokes) {
		data += "	<polyline fill=\"none\" stroke=\"black\" stroke-opacity=\"1\" stroke-width=\"2\"\n";
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
	if (_storeDot == true) {
		for (auto &it : m_enginePoints) {
			data += "	<circle fill=\"red\" cx=\"" + etk::to_string(it.x()*100.0f) + "\" cy=\"" + etk::to_string((1.0-it.y())*100.0f) + "\" r=\"0.6\"/>\n";
		}
	}
	data += "</svg>\n";
	etk::FSNodeWriteAllData(_fileName, data);
}

void dollar::Gesture::set(const std::string& _name, uint32_t _subId, std::vector<std::vector<vec2>> _path) {
	m_name = _name;
	m_subId = _subId;
	m_path = _path;
	m_enginePath.clear();
	m_engineVector.clear();
	m_engineStartV.clear();
	m_enginePoints.clear();
	m_path2.clear();
}

void dollar::Gesture::configure(float _startAngleIndex, size_t _nbSample, bool _ignoreRotation, float _distance, bool _keepAspectRatio) {
	m_enginePath.clear();
	m_engineVector.clear();
	m_engineStartV.clear();
	m_enginePoints.clear();
	m_path2 = dollar::scaleToOne(m_path, _keepAspectRatio);
	// Generates dots:
	m_enginePoints = dollar::normalizePathToPoints(m_path, _distance, _keepAspectRatio);
	DOLLAR_VERBOSE("create " << m_enginePoints.size() << " points");
	// for debug only
	//storeSVG("out_dollar/lib/gestures/" + m_name + "_" + etk::to_string(m_subId) + ".svg", true);
	// Simplyfy paths
	std::vector<std::vector<vec2>> uniPath = dollar::makeReferenceStrokes(m_path);
	// normalize paths
	for (auto &it : uniPath) {
		std::vector<vec2> val = dollar::normalizePath(it, _nbSample, _ignoreRotation, _keepAspectRatio);
		m_enginePath.push_back(val);
		// calculate start vector:
		vec2 startv = dollar::getStartVector(val, _startAngleIndex);
		m_engineStartV.push_back(startv);
		m_engineVector.push_back(dollar::normalyse(val));
	}
}