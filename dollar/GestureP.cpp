/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <dollar/debug.h>
#include <dollar/GestureP.h>
#include <dollar/tools.h>
#include <etk/os/FSNode.h>
#include <ejson/ejson.h>
#include <esvg/esvg.h>
#include <etk/stdTools.h>

dollar::GestureP::GestureP() {
	
}


void dollar::GestureP::configure(size_t _nbSample) {
	m_enginePoints.clear();
	// Generates dots:
	std::vector<vec2> points = dollar::combineStrokes(m_path);
	m_enginePoints = dollar::normalizePath(points, _nbSample, false, false);
}