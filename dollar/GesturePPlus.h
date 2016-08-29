/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#pragma once
#include <string>
#include <etk/math/Vector2D.h>
#include <dollar/Gesture.h>

namespace dollar {
	class GesturePPlus : public dollar::Gesture {
		public:
			GesturePPlus();
		protected:
			std::vector<vec2> m_enginePoints;
		public:
			// Configure the reference gesture for recognition...
			void configure(float _distance, bool _keepAspectRatio);
			const std::vector<vec2>& getEnginePoints() const {
				return m_enginePoints;
			}
			const std::vector<std::vector<vec2>>& getPath() const {
				return m_path;
			}
			std::vector<std::vector<vec2>>& getPath() {
				return m_path;
			}
	};
}
