/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <dollar/debug.hpp>
#include <dollar/Gesture.hpp>
#include <dollar/tools.hpp>
#include <etk/os/FSNode.hpp>
#include <ejson/ejson.hpp>
#include <esvg/esvg.hpp>
#include <etk/stdTools.hpp>


static etk::Vector<etk::Vector<vec2>> loadPointsJson(const ejson::Document& _doc) {
	// extract lines:
	etk::Vector<etk::Vector<vec2>> out;
	const ejson::Array listOfLines = _doc["data"].toArray();
	if (listOfLines.exist() == false) {
		DOLLAR_WARNING("Reference element has no element named 'data' " << _doc["data"]);
		return out;
	}
	for (auto itLines : listOfLines) {
		ejson::Array listOfpoint = itLines.toArray();
		etk::Vector<vec2> line;
		for (auto itPoints : listOfpoint) {
			ejson::Array pointsArray = itPoints.toArray();
			line.pushBack(vec2(pointsArray[0].toNumber().get(), pointsArray[1].toNumber().get()));
		}
		if (line.size() > 1) {
			out.pushBack(etk::move(line));
		}
	}
	return out;
}

etk::Vector<etk::Vector<vec2>> dollar::loadPoints(const etk::String& _fileName, etk::String* _label, std::string* _type) {
	ejson::Document doc;
	doc.load(_fileName);
	if (_label != nullptr) {
		*_label = doc["value"].toString().get();
	}
	if (_type != nullptr) {
		*_type = doc["type"].toString().get();
	}
	// extract lines:
	etk::Vector<etk::Vector<vec2>> out;
	ejson::Array listOfLines = doc["data"].toArray();
	for (auto itLines : listOfLines) {
		ejson::Array listOfpoint = itLines.toObject()["list"].toArray();
		etk::Vector<vec2> line;
		for (auto itPoints : listOfpoint) {
			ejson::Array pointsArray = itPoints.toArray();
			line.pushBack(vec2(pointsArray[0].toNumber().get(), pointsArray[1].toNumber().get()));
		}
		if (line.size() > 1) {
			out.pushBack(etk::move(line));
		}
	}
	return out;
}


dollar::Gesture::Gesture():
  m_name(""),
  m_subId(0),
  m_aspectRatio(0.0f),
  m_path(){
	
}

bool dollar::Gesture::load(const etk::String& _fileName) {
	etk::String tmpName = etk::tolower(_fileName);
	if (etk::end_with(tmpName, ".json") == true) {
		return loadJSON(_fileName);
	} else if (etk::end_with(tmpName, ".svg") == true) {
		return loadSVG(_fileName);
	}
	DOLLAR_ERROR("Un-sopported LOAD extention : " << _fileName);
	DOLLAR_ERROR("    supported: [svg,json]");
	return false;
}

bool dollar::Gesture::loadJSON(const etk::String& _fileName) {
	ejson::Document doc;
	doc.load(_fileName);
	if (doc["type"].toString().get() != "REFERENCE") {
		DOLLAR_WARNING("can not LOAD file that is not a reference");
	}
	m_name = doc["value"].toString().get();
	m_subId = doc["sub-id"].toNumber().getU64(),
	m_aspectRatio = doc["aspect-ratio"].toNumber().get(),
	m_path = loadPointsJson(doc);
	DOLLAR_DEBUG("Load gesture : " << m_name << " id=" << m_subId << " nb_elem=" << m_path.size());
	return true;
}

bool dollar::Gesture::loadSVG(const etk::String& _fileName) {
	esvg::Document doc;
	doc.load(_fileName);
	etk::Vector<etk::String> plop = etk::split(_fileName, '.');
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


bool dollar::Gesture::store(const etk::String& _fileName) {
	etk::String tmpName = etk::tolower(_fileName);
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

void dollar::Gesture::storeJSON(const etk::String& _fileName) {
	ejson::Document doc;
	doc.add("type", ejson::String("REFERENCE"));
	doc.add("value", ejson::String(m_name));
	doc.add("sub-id", ejson::Number(m_subId));
	doc.add("aspect-ratio", ejson::Number(m_aspectRatio));
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

void dollar::Gesture::storeSVG(const etk::String& _fileName, bool _storeDot) {
	etk::Vector<etk::Vector<vec2>> strokes = dollar::scaleToOne(m_path, true);
	etk::String data("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
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
			data += etk::toString(itPoints.x()*100.0f) + "," + etk::to_string((1.0-itPoints.y())*100.0f);
		}
		data += "\"\n";
		data += "	          />\n";
	}
	if (_storeDot == true) {
		/*
		for (auto &it : m_enginePoints) {
			data += "	<circle fill=\"red\" cx=\"" + etk::toString(it.x()*100.0f) + "\" cy=\"" + etk::to_string((1.0-it.y())*100.0f) + "\" r=\"0.6\"/>\n";
		}
		*/
	}
	data += "</svg>\n";
	etk::FSNodeWriteAllData(_fileName, data);
}

void dollar::Gesture::set(const etk::String& _name, uint32_t _subId, etk::Vector<etk::Vector<vec2>> _path) {
	m_name = _name;
	m_subId = _subId;
	m_path = _path;
}
