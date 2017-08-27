/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/math/Vector2D.hpp>
#include <dollar/Gesture.hpp>

#include <etk/String.hpp>

namespace dollar {
	class GestureP : public dollar::Gesture {
		public:
			GestureP();
		protected:
			etk::Vector<vec2> m_enginePoints;
		public:
			// Configure the reference gesture for recognition...
			void configure(size_t _nbSample);
			const etk::Vector<vec2>& getEnginePoints() const {
				return m_enginePoints;
			}
			const etk::Vector<vec2>& getPath() const {
				return m_enginePoints;
			}
			etk::Vector<vec2>& getPath() {
				return m_enginePoints;
			}
	};
}
