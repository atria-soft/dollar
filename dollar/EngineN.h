/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#pragma once

#include <etk/math/Vector2D.h>
#include <limits>
#include <iostream>
#include <dollar/Results.h>
#include <string>
#include <dollar/Gesture.h>
#include <dollar/Engine.h>
#include <dollar/GestureN.h>

namespace dollar {
	class EngineN : public dollar::Engine {
		private:
			bool m_protractor;
		protected:
			float m_angleRange;
			bool m_paramterIgnoreRotation; //!< Ignore the start rotation of the gesture
		public:
			void setRotationInvariance(bool _ignoreRotation);
		protected:
			size_t m_numPointsInGesture; //!< Number of point in a gesture to recognise patern ...
		public:
			void setNumberPointInGesture(size_t _value);
			size_t getNumberPointInGesture();
		protected:
			std::vector<ememory::SharedPtr<dollar::GestureN>> m_gestures; //!< List of all loaded gesture in the engine
		public:
			EngineN(bool _protractor);
			dollar::Results recognize2(const std::vector<std::vector<vec2>>& _points) override;
			bool loadGesture(const std::string& _filename) override;
			void addGesture(ememory::SharedPtr<dollar::Gesture> _gesture) override;
		protected:
			float distanceAtBestAngle(const std::vector<vec2>& _points, const std::vector<vec2>& _reference);
			float optimalCosineDistance(const std::vector<vec2>& _vect1, const std::vector<vec2>& _vect2);
	};
}


