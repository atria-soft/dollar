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
	class GestureN : public dollar::Gesture {
		protected:
			std::vector<std::vector<vec2>> m_path2;
		public:
			GestureN();
		public:
			const std::vector<std::vector<vec2>>& getPath() const {
				return m_path2;
			}
			std::vector<std::vector<vec2>>& getPath() {
				return m_path2;
			}
		protected:
			std::vector<std::vector<vec2>> m_enginePath; // Singulized path with every conbinaison
			std::vector<std::vector<vec2>> m_engineVector;
			std::vector<vec2> m_engineStartV;
		public:
			// Configure the reference gesture for recognition...
			void configure(float _startAngleIndex, size_t _nbSample, bool _ignoreRotation, bool _keepAspectRatio=false);
			size_t getEngineSize() const {
				return m_enginePath.size();
			}
			const std::vector<vec2>& getEnginePath(size_t _id) const {
				return m_enginePath[_id];
			}
			const std::vector<vec2>& getEngineVector(size_t _id) const {
				return m_engineVector[_id];
			}
			const vec2& getEngineStartVector(size_t _id) const {
				return m_engineStartV[_id];
			}
	};
}
