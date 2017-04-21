/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <dollar/debug.hpp>
#include <dollar/Results.hpp>


dollar::Results::Results() {
	// nothing to do ...
}

bool dollar::Results::haveMatch() const {
	return m_values.size() != 0;
}

size_t dollar::Results::getSize() const {
	return m_values.size();
}

std::string dollar::Results::getName(size_t _id) const {
	if (_id >= m_values.size()) {
		DOLLAR_ERROR("request acces error result out of range (name)");
		return "";
	}
	return m_values[_id].value;
}

float dollar::Results::getConfidence(size_t _id) const {
	if (_id >= m_values.size()) {
		DOLLAR_ERROR("request acces error result out of range (value)");
		return 0;
	}
	return m_values[_id].confidence;
}

void dollar::Results::addValue(const std::string& _name, float _confidence) {
	m_values.push_back(ResultData(_name, _confidence));
}


void dollar::Results::clear() {
	m_values.clear();
}

