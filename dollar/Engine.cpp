/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <dollar/Engine.hpp>
#include <dollar/debug.hpp>
#include <dollar/Rectangle.hpp>
#include <dollar/tools.hpp>
#include <etk/os/FSNode.hpp>
#include <dollar/EngineN.hpp>
#include <dollar/EngineP.hpp>
#include <dollar/EnginePPlus.hpp>


dollar::Engine::Engine():
  m_nbResult(5) {
	
}

void dollar::Engine::setNumberResult(size_t _value) {
	m_nbResult = _value;
}

size_t dollar::Engine::getNumberResult() {
	return m_nbResult;
}


bool dollar::Engine::loadPath(const etk::String& _path) {
	DOLLAR_INFO("Load Path: " << _path);
	etk::FSNode path(_path);
	etk::Vector<etk::String> files = path.folderGetSub(false, true, "*.json");
	for (auto &it : files) {
		if (etk::end_with(it, ".json") == true) {
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
