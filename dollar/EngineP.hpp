/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#pragma once

#include <etk/math/Vector2D.hpp>
#include <dollar/Results.hpp>
#include <dollar/Engine.hpp>
#include <dollar/GestureP.hpp>
#include <limits>
#include <iostream>
#include <string>

namespace dollar {
	class EngineP : public dollar::Engine {
		protected:
			bool m_scaleKeepRatio; // when rescale the path, keep the aspect ration for processing
		public:
			void setScaleKeepRatio(bool _value);
			bool getScaleKeepRatio();
		protected:
			size_t m_numPointsInGesture; //!< Number of point in a gesture to recognise patern ...
		public:
			void setNumberPointInGesture(size_t _value);
			size_t getNumberPointInGesture();
		protected:
			std::vector<ememory::SharedPtr<dollar::GestureP>> m_gestures; //!< List of all loaded gesture in the engine
		public:
			EngineP();
			dollar::Results recognize2(const std::vector<std::vector<vec2>>& _paths) override;
			bool loadGesture(const std::string& _filename) override;
			void addGesture(ememory::SharedPtr<dollar::Gesture> _gesture) override;
	};
}


