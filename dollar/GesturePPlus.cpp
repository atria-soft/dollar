/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <dollar/debug.h>
#include <dollar/GesturePPlus.h>
#include <dollar/tools.h>
#include <etk/os/FSNode.h>
#include <ejson/ejson.h>
#include <esvg/esvg.h>
#include <etk/stdTools.h>

dollar::GesturePPlus::GesturePPlus() {
	
}

void dollar::GesturePPlus::configure(float _distance, bool _keepAspectRatio) {
	m_enginePoints.clear();
	// Generates dots:
	m_enginePoints = dollar::normalizePathToPoints(m_path, _distance, _keepAspectRatio);
	DOLLAR_VERBOSE("create " << m_enginePoints.size() << " points");
}