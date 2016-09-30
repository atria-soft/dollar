/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <dollar/debug.hpp>
#include <dollar/GesturePPlus.hpp>
#include <dollar/tools.hpp>
#include <etk/os/FSNode.hpp>
#include <ejson/ejson.hpp>
#include <esvg/esvg.hpp>
#include <etk/stdTools.hpp>

dollar::GesturePPlus::GesturePPlus() {
	
}

void dollar::GesturePPlus::configure(float _distance, bool _keepAspectRatio) {
	m_enginePoints.clear();
	// Generates dots:
	m_enginePoints = dollar::normalizePathToPoints(m_path, _distance, _keepAspectRatio);
	DOLLAR_VERBOSE("create " << m_enginePoints.size() << " points");
}