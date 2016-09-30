/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <dollar/debug.hpp>

int32_t dollar::getLogId() {
	static int32_t g_val = elog::registerInstance("dollar");
	return g_val;
}

