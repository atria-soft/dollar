/** @file
 * @author Edouard DUPIN 
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license PROPRIETARY (see license file)
 */

#include <appl/debug.h>
#include <appl/widget/TextAreaRecognition.h>
#include <ewol/object/Manager.h>
#include <etk/tool.h>
#include <ejson/ejson.h>
#include <esvg/esvg.h>
//#include <algue/sha512.h>
#include <etk/os/FSNode.h>

appl::widget::TextAreaRecognition::TextAreaRecognition() {
	addObjectType("appl::widget::TextAreaRecognition");
}

void appl::widget::TextAreaRecognition::init() {
	ewol::Widget::init();
	m_updateDone = false;
	
	//m_dollarEngine.loadPath("DATA:text");
	m_dollarEngine = dollar::createEngine("$P+");
	m_dollarEngine->loadPath("DATA:reference");
	markToRedraw();
	// connect a periodic update ...
	m_periodicConnection = getObjectManager().periodicCall.connect(this, &appl::widget::TextAreaRecognition::callbackPeriodicUpdate);
}


appl::widget::TextAreaRecognition::~TextAreaRecognition() {
	
}


void appl::widget::TextAreaRecognition::clear() {
	m_dataList.clear();
	m_current.clear();
	m_time = std::chrono::system_clock::now();
	markToRedraw();
}

void appl::widget::TextAreaRecognition::undo() {
	if (m_dataList.size() <= 0) {
		return;
	}
	m_dataList.erase(m_dataList.begin()+(m_dataList.size()-1));
	m_time = std::chrono::system_clock::now();
	markToRedraw();
}

void appl::widget::TextAreaRecognition::setCompare(const std::string& _compare) {
	m_compare = _compare;
}

void appl::widget::TextAreaRecognition::store(const std::string& _userName, const std::string& _value, const std::string& _type) {
	if (m_dataList.size() == 0) {
		return;
	}
	// create the buffer
	ejson::Document doc;
	doc.add("user", ejson::String(_userName));
	doc.add("value", ejson::String(_value));
	doc.add("type", ejson::String(_type));
	doc.add("time", ejson::Number(m_time.time_since_epoch().count()));
	ejson::Array list;
	doc.add("data", list);
	for (auto &it : m_dataList) {
		ejson::Object obj;
		obj.add("type", ejson::String(etk::to_string(it.m_type)));
		ejson::Array listPoint;
		obj.add("list", listPoint);
		for (size_t iii=0; iii<it.m_data.size(); ++iii) {
			ejson::Array point;
			point.add(ejson::Number(it.m_data[iii].x()));
			point.add(ejson::Number(it.m_data[iii].y()));
			listPoint.add(point);
		}
		list.add(obj);
	}
	std::string streamOut = doc.generateMachineString();
	std::string fileName;
	fileName = "HOME:DOLLAR/corpus/";
	if (_value == "/") {
		fileName += "slash";
	} else {
		fileName += _value;
	}
	fileName += "_";
	fileName += _type;
	fileName += "_";
	fileName += _userName;
	fileName += "_";
	fileName += etk::to_string(m_time.time_since_epoch().count());
	fileName += ".json";
	etk::FSNodeWriteAllData(fileName, streamOut);
	APPL_WARNING("store: " << fileName);
}

void appl::widget::TextAreaRecognition::onDraw() {
	m_draw.draw();
	m_text.draw();
}

std::vector<std::vector<vec2>> scalePoints(std::vector<std::vector<vec2>> _list, float _objectSize) {
	// get min/max point
	vec2 minPos(99999999,99999999);
	vec2 maxPos(0,0);
	for (auto &itLines : _list) {
		for (auto& itPoints : itLines) {
			minPos.setMin(itPoints);
			maxPos.setMax(itPoints);
		}
	}
	// center and reduce to a size of XXX
	float scale = 1.0f;
	vec2 size = maxPos-minPos;
	vec2 center(0,0);
	if (size.x() > size.y()) {
		scale = _objectSize/size.x();
		center.setY((_objectSize-size.y()*scale)*0.5);
	} else {
		scale = _objectSize/size.y();
		center.setX((_objectSize-size.x()*scale)*0.5);
	}
	for (auto &itLines : _list) {
		for (auto& itPoints : itLines) {
			itPoints -= minPos;
			itPoints *= scale;
			itPoints += center;
		}
	}
	return _list;
}

std::vector<etk::Color<float,4>> renderWithSVG(const std::vector<std::vector<vec2>>& _list, int32_t _objectSize, const std::string& _filename) {
	// generate SVG to render:
	esvg::Document docSvg;
	std::string data("<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n");
	data += "<svg height='" + etk::to_string(_objectSize) + "' width='" + etk::to_string(_objectSize) + "'>\n";
	for (auto &itLines : _list) {
		data += "	<polyline\n";
		data += "	          fill='none'\n";
		data += "	          stroke='black'\n";
		data += "	          stroke-opacity='1'\n";
		data += "	          stroke-width='2'\n";
		data += "	          points='";
		bool first = true;
		for (auto& itPoints : itLines) {
			if (first == false) {
				data += " ";
			}
			first = false;
			data += etk::to_string(itPoints.x()) + "," + etk::to_string(itPoints.y());
		}
		data += "'\n";
		data += "	          />\n";
	}
	data += "</svg>\n";
	docSvg.parse(data);
	//docSvg.store(list[index]->getName()+ ".svg");
	etk::FSNodeWriteAllData(_filename, data);
	
	// generate SVG output ...
	ivec2 renderSize = ivec2(_objectSize,_objectSize);
	return docSvg.renderImageFloatRGBA(renderSize);
}


void appl::widget::TextAreaRecognition::onRegenerateDisplay() {
	if (needRedraw() == false) {
		return;
	}
	// remove previous data
	m_draw.clear();
	m_text.clear();
	// set background
	m_draw.setColor(etk::color::black);
	m_draw.setPos(vec2(0,0));
	m_draw.rectangleWidth(m_size);
	m_draw.setColor(etk::color::orange);
	m_draw.setThickness(1);
	m_draw.setPos(vec2(0,m_size.y()*0.0));
	m_draw.lineTo(vec2(m_size.x(),m_size.y()*0.0));
	m_draw.setColor(etk::color::blue);
	m_draw.setThickness(1);
	m_draw.setPos(vec2(0,m_size.y()*0.1));
	m_draw.lineTo(vec2(m_size.x(),m_size.y()*0.1));
	m_draw.setPos(vec2(0,m_size.y()*0.2));
	m_draw.lineTo(vec2(m_size.x(),m_size.y()*0.2));
	m_draw.setPos(vec2(0,m_size.y()*0.3));
	m_draw.lineTo(vec2(m_size.x(),m_size.y()*0.3));
	m_draw.setColor(etk::color::orange);
	m_draw.setThickness(2);
	m_draw.setPos(vec2(0,m_size.y()*0.4));
	m_draw.lineTo(vec2(m_size.x(),m_size.y()*0.4));
	m_draw.setColor(etk::color::blue);
	m_draw.setThickness(1);
	m_draw.setPos(vec2(0,m_size.y()*0.5));
	m_draw.lineTo(vec2(m_size.x(),m_size.y()*0.5));
	m_draw.setPos(vec2(0,m_size.y()*0.6));
	m_draw.lineTo(vec2(m_size.x(),m_size.y()*0.6));
	m_draw.setPos(vec2(0,m_size.y()*0.7));
	m_draw.lineTo(vec2(m_size.x(),m_size.y()*0.7));
	m_draw.setColor(etk::color::orange);
	m_draw.setPos(vec2(0,m_size.y()*0.8));
	m_draw.lineTo(vec2(m_size.x(),m_size.y()*0.8));
	m_draw.setColor(etk::color::blue);
	m_draw.setPos(vec2(0,m_size.y()*0.9));
	m_draw.lineTo(vec2(m_size.x(),m_size.y()*0.9));
	m_draw.setPos(vec2(0,m_size.y()*1.0));
	m_draw.lineTo(vec2(m_size.x(),m_size.y()*1.0));
	
	m_draw.setColor(etk::color::orange);
	m_draw.setThickness(1);
	float base = m_size.y()*0.4;
	float pos = m_size.x()*0.5f - base*0.5;
	while (pos > 0.0f) {
		m_draw.setPos(vec2(pos,0));
		m_draw.lineTo(vec2(pos,m_size.y()));
		pos -= base;
	}
	pos = m_size.x()*0.5f + base*0.5;
	while (pos < m_size.x()) {
		m_draw.setPos(vec2(pos,0));
		m_draw.lineTo(vec2(pos,m_size.y()));
		pos += base;
	}
	
	
	vec2 minPos(99999999,99999999);
	vec2 maxPos(0,0);
	// set all the line:
	m_draw.setColor(etk::color::white);
	m_draw.setThickness(2);
	// Draw olds
	for (auto &it : m_dataList) {
		for (size_t iii=0; iii<it.m_data.size(); ++iii) {
			if (iii == 0) {
				m_draw.setPos(it.m_data[iii]);
			} else {
				m_draw.lineTo(it.m_data[iii]);
			}
			minPos.setMin(it.m_data[iii]);
			maxPos.setMax(it.m_data[iii]);
		}
	}
	// draw current:
	m_draw.setColor(etk::color::red);
	for (size_t iii=0; iii<m_current.m_data.size(); ++iii) {
		if (iii == 0) {
			m_draw.setPos(m_current.m_data[iii]);
		} else {
			m_draw.lineTo(m_current.m_data[iii]);
		}
		minPos.setMin(m_current.m_data[iii]);
		maxPos.setMax(m_current.m_data[iii]);
	}
	maxPos += vec2(2,2);
	minPos -= vec2(2,2);
	if (    minPos.x() < 9999999
	     && m_updateDone == true) {
		// Draw a cadre of the outline of the element ...
		m_draw.setThickness(3);
		if (m_findValue != m_compare) {
			m_draw.setColor(etk::color::red);
			m_text.setColor(etk::color::red);
		} else {
			m_draw.setColor(etk::color::green);
			m_text.setColor(etk::color::green);
		}
		m_draw.setPos(minPos);
		m_draw.lineTo(vec2(minPos.x(), maxPos.y()));
		m_draw.lineTo(maxPos);
		m_draw.lineTo(vec2(maxPos.x(), minPos.y()));
		m_draw.lineTo(minPos);
		
		for (size_t iii=0; iii<m_dollarResults.getSize(); ++iii) {
			m_text.setPos(vec2(maxPos.x()+5, maxPos.y()-m_text.getHeight()*float(iii+1)));
			if (m_dollarResults.getName(iii) == m_compare) {
				if (iii == 0) {
					m_text.setColor(etk::color::green);
				} else {
					m_text.setColor(etk::color::orange);
				}
			} else {
				m_text.setColor(etk::color::red);
			}
			m_text.print(m_dollarResults.getName(iii) + " " + etk::to_string(m_dollarResults.getConfidence(iii)) + "%");
		}
		m_text.setColor(etk::color::white);
		m_text.setPos(vec2(0, m_text.getHeight()*2));
		m_text.print("Dollar=" + etk::to_string(m_dollarTime.count()) + " ms");
	}
}

bool appl::widget::TextAreaRecognition::onEventInput(const ewol::event::Input& _event) {
	if (   _event.getId() == 1
	    && (    (    m_current.m_type == gale::key::type::unknow
	              && _event.getStatus() == gale::key::status::down
	            )
	         || m_current.m_type == _event.getType()
	       )
	    ) {
		m_time = std::chrono::system_clock::now();
		if(_event.getStatus() == gale::key::status::down) {
			m_current.m_type = _event.getType();
			m_current.addPoint(relativePosition(_event.getPos()));
		}
		if(_event.getStatus() == gale::key::status::up) {
			m_current.addPoint(relativePosition(_event.getPos()));
			m_dataList.push_back(m_current);
			m_current.clear();
		}
		if(_event.getStatus() == gale::key::status::move) {
			m_current.addPoint(relativePosition(_event.getPos()));
		}
		markToRedraw();
		m_lastEvent = std::chrono::system_clock::now();
		m_updateDone = false;
		m_findValue = "";
		m_dollarResults.clear();
		return true;
	}
	return false;
}

static std::vector<std::vector<vec2>> convertInLines(const std::vector<appl::DrawingLine>& _list) {
	std::vector<std::vector<vec2>> out;
	for (auto &it : _list) {
		if (it.m_data.size() > 1) {
			out.push_back(it.m_data);
		} else {
			// TODO
		}
	}
	return out;
}


void appl::widget::TextAreaRecognition::callbackPeriodicUpdate(const ewol::event::Time& _event) {
	if (    (std::chrono::system_clock::now() - m_lastEvent) > std::chrono::milliseconds(1200)
	     && m_updateDone == false) {
		if (m_current.m_data.size() != 0) {
			// ==> writing in progress
			return;
		}
		if (m_dataList.size() == 0) {
			// ==> no writing
			markToRedraw();
			m_updateDone = true;
			return;
		}
		// extract lines from json file:
		std::vector<std::vector<vec2>> fullListlines = convertInLines(m_dataList);
		if (fullListlines.size() == 0) {
			APPL_ERROR(" can not manage an objest with no line ...");
			return;
		}
		
		std::chrono::system_clock::time_point tic = std::chrono::system_clock::now();
		// First Test with dollar engine
		m_dollarResults = m_dollarEngine->recognize(fullListlines);
		m_findValue = m_dollarResults.getName();
		m_dollarTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tic);
		markToRedraw();
		m_updateDone = true;
	}
}



