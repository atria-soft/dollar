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

namespace dollar {
	class Engine {
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
			std::vector<Gesture> m_gestures; //!< List of all loaded gesture in the engine
		public:
			Engine();
			float distanceAtBestAngle(const std::vector<vec2>& _points, const std::vector<vec2>& _reference);
			Results recognize(const std::vector<std::vector<vec2>>& _paths, const std::string& _method="normal");
			Results recognize(const std::vector<vec2>& _points, const std::string& _method="normal");
			float optimalCosineDistance(const std::vector<vec2>& _vect1, const std::vector<vec2>& _vect2);
			bool loadPath(const std::string& _path);
			bool loadGesture(const std::string& _filename);
			void addGesture(Gesture _gesture);
	};
}


