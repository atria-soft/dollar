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
	class GesturePPlus : public dollar::Gesture {
		public:
			GesturePPlus();
		protected:
			etk::Vector<vec2> m_enginePoints;
			float m_aspectRatio; // original aspect ratio
		public:
			// Configure the reference gesture for recognition...
			void configure(float _distance, bool _keepAspectRatio);
			const etk::Vector<vec2>& getEnginePoints() const {
				return m_enginePoints;
			}
			const etk::Vector<etk::Vector<vec2>>& getPath() const {
				return m_path;
			}
			etk::Vector<etk::Vector<vec2>>& getPath() {
				return m_path;
			}
			const float& getAspectRatio() const {
				return m_aspectRatio;
			}
	};
}
