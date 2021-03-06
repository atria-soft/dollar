/** @file
 * @author Edouard DUPIN 
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license PROPRIETARY (see license file)
 */

#include <appl/debug.hpp>
#include <appl/widget/TextAreaRecognition.hpp>
#include <ewol/object/Manager.hpp>
#include <etk/tool.hpp>
#include <ejson/ejson.hpp>
#include <esvg/esvg.hpp>

appl::widget::TextAreaRecognition::TextAreaRecognition() {
	addObjectType("appl::widget::TextAreaRecognition");
}

void appl::widget::TextAreaRecognition::init() {
	ewol::Widget::init();
	m_updateDone = false;
	
	//m_dollarEngine.loadPath("DATA:///text");
	m_dollarEngine = dollar::createEngine("$P+");
	m_dollarEngine->loadPath("DATA:///reference");
	// TODO: m_dollarEngine->setScaleKeepRatio(true);
	markToRedraw();
	// connect a periodic update ...
	m_periodicConnection = getObjectManager().periodicCall.connect(this, &appl::widget::TextAreaRecognition::callbackPeriodicUpdate);
}


appl::widget::TextAreaRecognition::~TextAreaRecognition() {
	
}


void appl::widget::TextAreaRecognition::clear() {
	m_dataList.clear();
	m_current.clear();
	m_time = echrono::Clock::now();
	markToRedraw();
}

void appl::widget::TextAreaRecognition::undo() {
	if (m_dataList.size() <= 0) {
		return;
	}
	m_dataList.erase(m_dataList.begin()+(m_dataList.size()-1));
	m_time = echrono::Clock::now();
	markToRedraw();
}

void appl::widget::TextAreaRecognition::setCompare(const etk::String& _compare) {
	m_compare = _compare;
}

void appl::widget::TextAreaRecognition::store(const etk::String& _userName, const etk::String& _value, const etk::String& _type) {
	if (m_dataList.size() == 0) {
		return;
	}
	// create the buffer
	ejson::Document doc;
	doc.add("user", ejson::String(_userName));
	doc.add("value", ejson::String(_value));
	doc.add("type", ejson::String(_type));
	doc.add("time", ejson::Number(m_time.get()));
	ejson::Array list;
	doc.add("data", list);
	for (auto &it : m_dataList) {
		ejson::Object obj;
		obj.add("type", ejson::String(etk::toString(it.m_type)));
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
	etk::String streamOut = doc.generateMachineString();
	etk::Path fileName = "~/DOLLAR/corpus";
	if (_value == "/") {
		fileName /= "slash";
	} else if (_value == "\\") {
		fileName /= "back-slash";
	} else {
		fileName /= _value;
	}
	fileName += "_";
	fileName += _type;
	fileName += "_";
	fileName += _userName;
	fileName += "_";
	fileName += etk::toString(m_time.get());
	fileName += ".json";
	{
		ememory::SharedPtr<etk::io::Interface> fileIO = etk::uri::get(fileName);
		if (fileIO->open(etk::io::OpenMode::Write) == false) {
			return;
		}
		fileIO->writeAll(streamOut);
		fileIO->close();
	}
	APPL_WARNING("store: " << fileName);
}

void appl::widget::TextAreaRecognition::onDraw() {
	m_draw.draw();
	m_text.draw();
}

etk::Vector<etk::Vector<vec2>> scalePoints(etk::Vector<etk::Vector<vec2>> _list, float _objectSize) {
	// get min/max point
	vec2 minPos(FLT_MAX, FLT_MAX);
	vec2 maxPos(FLT_MIN, FLT_MIN);
	for (auto &itLines : _list) {
		for (auto& itPoints : itLines) {
			minPos.setMin(itPoints);
			maxPos.setMax(itPoints);
		}
	}
	// center and reduce to a size of XXX
	float scale = 1.0f;
	vec2 size = maxPos-minPos;
	vec2 center(0, 0);
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

etk::Vector<etk::Color<float,4>> renderWithSVG(const etk::Vector<etk::Vector<vec2>>& _list, int32_t _objectSize, const etk::Uri& _filename) {
	// generate SVG to render:
	esvg::Document docSvg;
	etk::String data("<?xml version='1.0' encoding='UTF-8' standalone='no'?>\n");
	data += "<svg height='" + etk::toString(_objectSize) + "' width='" + etk::toString(_objectSize) + "'>\n";
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
			data += etk::toString(itPoints.x()) + "," + etk::toString(itPoints.y());
		}
		data += "'\n";
		data += "	          />\n";
	}
	data += "</svg>\n";
	docSvg.parse(data);
	//docSvg.store(list[index]->getName()+ ".svg");
	{
		ememory::SharedPtr<etk::io::Interface> fileIO = etk::uri::get(_filename);
		if (fileIO->open(etk::io::OpenMode::Write) == true) {
			fileIO->writeAll(data);
			fileIO->close();
		}
	}
	
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
			m_text.print(m_dollarResults.getName(iii) + " " + etk::toString(m_dollarResults.getConfidence(iii)) + "%");
		}
		m_text.setColor(etk::color::white);
		m_text.setPos(vec2(0, m_text.getHeight()*2));
		m_text.print("Dollar=" + etk::toString(m_dollarTime.get()) + " ms");
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
		m_time = echrono::Clock::now();
		if(_event.getStatus() == gale::key::status::down) {
			m_current.m_type = _event.getType();
			m_current.addPoint(relativePosition(_event.getPos()));
		}
		if(_event.getStatus() == gale::key::status::up) {
			m_current.addPoint(relativePosition(_event.getPos()));
			m_dataList.pushBack(m_current);
			m_current.clear();
		}
		if(_event.getStatus() == gale::key::status::move) {
			m_current.addPoint(relativePosition(_event.getPos()));
		}
		markToRedraw();
		m_lastEvent = echrono::Clock::now();
		m_updateDone = false;
		m_findValue = "";
		m_dollarResults.clear();
		return true;
	}
	return false;
}

static etk::Vector<etk::Vector<vec2>> convertInLines(const etk::Vector<appl::DrawingLine>& _list) {
	etk::Vector<etk::Vector<vec2>> out;
	for (auto &it : _list) {
		if (it.m_data.size() > 1) {
			out.pushBack(it.m_data);
		} else {
			// TODO
		}
	}
	return out;
}


void appl::widget::TextAreaRecognition::callbackPeriodicUpdate(const ewol::event::Time& _event) {
	if (    (echrono::Clock::now() - m_lastEvent) > echrono::milliseconds(1200)
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
		etk::Vector<etk::Vector<vec2>> fullListlines = convertInLines(m_dataList);
		if (fullListlines.size() == 0) {
			APPL_ERROR(" can not manage an objest with no line ...");
			return;
		}
		
		echrono::Clock tic = echrono::Clock::now();
		// First Test with dollar engine
		m_dollarResults = m_dollarEngine->recognize(fullListlines);
		m_findValue = m_dollarResults.getName();
		m_dollarTime = echrono::Clock::now() - tic;
		markToRedraw();
		m_updateDone = true;
	}
}



