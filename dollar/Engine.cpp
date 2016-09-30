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


dollar::Results dollar::Engine::recognize(const std::vector<vec2>& _points) {
	std::vector<std::vector<vec2>> tmp;
	tmp.push_back(_points);
	return recognize2(tmp);
}
dollar::Results dollar::Engine::recognize(const std::vector<std::vector<vec2>>& _points) {
	return recognize2(_points);
}

ememory::SharedPtr<dollar::Engine> dollar::createEngine(const std::string& _method) {
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
	return nullptr;
}
