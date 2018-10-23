/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <dollar/debug.hpp>
#include <dollar/GestureN.hpp>
#include <dollar/tools.hpp>
#include <ejson/ejson.hpp>
#include <esvg/esvg.hpp>
#include <etk/stdTools.hpp>



dollar::GestureN::GestureN() {
	
}

void dollar::GestureN::configure(float _startAngleIndex, size_t _nbSample, bool _ignoreRotation, bool _keepAspectRatio) {
	m_enginePath.clear();
	m_engineVector.clear();
	m_engineStartV.clear();
	m_path2 = dollar::scaleToOne(m_path, _keepAspectRatio);
	// for debug only
	//storeSVG("out_dollar/lib/gestures/" + m_name + "_" + etk::toString(m_subId) + ".svg", true);
	// Simplyfy paths
	etk::Vector<etk::Vector<vec2>> uniPath = dollar::makeReferenceStrokes(m_path);
	// normalize paths
	for (auto &it : uniPath) {
		etk::Vector<vec2> val = dollar::normalizePath(it, _nbSample, _ignoreRotation, _keepAspectRatio);
		m_enginePath.pushBack(val);
		// calculate start vector:
		vec2 startv = dollar::getStartVector(val, _startAngleIndex);
		m_engineStartV.pushBack(startv);
		m_engineVector.pushBack(dollar::normalyse(val));
	}
}