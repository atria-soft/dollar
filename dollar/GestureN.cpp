/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <dollar/debug.h>
#include <dollar/GestureN.h>
#include <dollar/tools.h>
#include <etk/os/FSNode.h>
#include <ejson/ejson.h>
#include <esvg/esvg.h>
#include <etk/stdTools.h>



dollar::GestureN::GestureN() {
	
}

void dollar::GestureN::configure(float _startAngleIndex, size_t _nbSample, bool _ignoreRotation, bool _keepAspectRatio) {
	m_enginePath.clear();
	m_engineVector.clear();
	m_engineStartV.clear();
	m_path2 = dollar::scaleToOne(m_path, _keepAspectRatio);
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