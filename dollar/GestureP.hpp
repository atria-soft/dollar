/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/math/Vector2D.hpp>
#include <dollar/Gesture.hpp>

#include <string>

namespace dollar {
	class GestureP : public dollar::Gesture {
		public:
			GestureP();
		protected:
			std::vector<vec2> m_enginePoints;
		public:
			// Configure the reference gesture for recognition...
			void configure(size_t _nbSample);
			const std::vector<vec2>& getEnginePoints() const {
				return m_enginePoints;
			}
			const std::vector<vec2>& getPath() const {
				return m_enginePoints;
			}
			std::vector<vec2>& getPath() {
				return m_enginePoints;
			}
	};
}
