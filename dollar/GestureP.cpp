/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <dollar/debug.hpp>
#include <dollar/GestureP.hpp>
#include <dollar/tools.hpp>
#include <etk/os/FSNode.hpp>
#include <ejson/ejson.hpp>
#include <esvg/esvg.hpp>
#include <etk/stdTools.hpp>

dollar::GestureP::GestureP() {
	
}


void dollar::GestureP::configure(size_t _nbSample) {
	m_enginePoints.clear();
	// Generates dots:
	std::vector<vec2> points = dollar::combineStrokes(m_path);
	m_enginePoints = dollar::normalizePath(points, _nbSample, false, false);
}