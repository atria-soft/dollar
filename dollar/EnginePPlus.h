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
#include <dollar/Engine.h>
#include <dollar/GesturePPlus.h>

namespace dollar {
	class EnginePPlus : public dollar::Engine {
		protected:
			float m_PPlusDistance;
		public:
			void setPPlusDistance(float _value);
			float getPPlusDistance();
		protected:
			float m_PPlusExcludeDistance;
		public:
			void setPPlusExcludeDistance(float _value);
			float getPPlusExcludeDistance();
		protected:
			bool m_scaleKeepRatio; // when rescale the path, keep the aspect ration for processing
		public:
			void setScaleKeepRatio(bool _value);
			bool getScaleKeepRatio();
		protected:
			float m_penalityNotLinkRef;
		public:
			void setPenalityNotLinkRef(float _value);
			float getPenalityNotLinkRef();
		protected:
			float m_penalityNotLinkSample;
		public:
			void setPenalityNotLinkSample(float _value);
			float getPenalityNotLinkSample();
		protected:
			std::vector<ememory::SharedPtr<dollar::GesturePPlus>> m_gestures; //!< List of all loaded gesture in the engine
		public:
			EnginePPlus();
			dollar::Results recognize2(const std::vector<std::vector<vec2>>& _paths) override;
			bool loadGesture(const std::string& _filename) override;
			void addGesture(ememory::SharedPtr<dollar::Gesture> _gesture) override;
		protected:
			float calculatePPlusDistance(const std::vector<vec2>& _points,
			                             const std::vector<vec2>& _reference,
			                             std::vector<std::pair<int32_t, int32_t>>& _dataDebug);
			float calculatePPlusDistanceSimple(const std::vector<vec2>& _points,
			                                   const std::vector<vec2>& _reference,
			                                   std::vector<std::pair<int32_t, int32_t>>& _dataDebug);
	};
}


