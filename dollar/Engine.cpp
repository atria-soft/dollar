/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <dollar/Engine.hpp>
#include <dollar/debug.hpp>
#include <dollar/Rectangle.hpp>
#include <dollar/tools.hpp>
#include <dollar/EngineN.hpp>
#include <dollar/EngineP.hpp>
#include <dollar/EnginePPlus.hpp>
#include <etk/path/fileSystem.hpp>


dollar::Engine::Engine():
  m_nbResult(5) {
	
}

void dollar::Engine::setNumberResult(size_t _value) {
	m_nbResult = _value;
}

size_t dollar::Engine::getNumberResult() {
	return m_nbResult;
}


bool dollar::Engine::loadPath(const etk::Uri& _uri) {
	DOLLAR_INFO("Load Path: " << _uri);
	etk::Vector<etk::Uri> files = etk::uri::list(_uri);
	for (auto &it : files) {
		if (etk::uri::isFile(it) == false) {
			continue;
		}
		if (it.getPath().getExtention().toLower() == "json") {
			loadGesture(it);
		}
	}
	return true;
}


dollar::Results dollar::Engine::recognize(const etk::Vector<vec2>& _points) {
	etk::Vector<etk::Vector<vec2>> tmp;
	tmp.pushBack(_points);
	return recognize2(tmp);
}
dollar::Results dollar::Engine::recognize(const etk::Vector<etk::Vector<vec2>>& _points) {
	return recognize2(_points);
}

ememory::SharedPtr<dollar::Engine> dollar::createEngine(const etk::String& _method) {
	if (    _method == "$N"
	     || _method == "$1") {
		return ememory::makeShared<dollar::EngineN>(false);
	}
	if (_method == "$N-protractor") {
		return ememory::makeShared<dollar::EngineN>(true);
	}
	if (_method == "$P") {
		return ememory::makeShared<dollar::EngineP>();
	}
	if (_method == "$P+") {
		return ememory::makeShared<dollar::EnginePPlus>();
	}
	return null;
}
