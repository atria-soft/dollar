/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#pragma once

#include <etk/math/Vector2D.hpp>
#include <dollar/Results.hpp>
#include <dollar/Engine.hpp>
#include <dollar/GesturePPlus.hpp>
#include <limits>
#include <iostream>
#include <etk/String.hpp>

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
			float m_penalityAspectRatio;
		public:
			void setPenalityAspectRatio(float _value);
			float getPenalityAspectRatio();
		protected:
			etk::Vector<ememory::SharedPtr<dollar::GesturePPlus>> m_gestures; //!< List of all loaded gesture in the engine
		public:
			EnginePPlus();
			dollar::Results recognize2(const etk::Vector<etk::Vector<vec2>>& _paths) override;
			bool loadGesture(const etk::String& _filename) override;
			void addGesture(ememory::SharedPtr<dollar::Gesture> _gesture) override;
		protected:
			float calculatePPlusDistance(const etk::Vector<vec2>& _points,
			                             const etk::Vector<vec2>& _reference,
			                             etk::Vector<etk::Pair<int32_t, int32_t>>& _dataDebug,
			                             float _inputAspectRatio,
			                             float _referenceAspectRatio);
			float calculatePPlusDistanceSimple(const etk::Vector<vec2>& _points,
			                                   const etk::Vector<vec2>& _reference,
			                                   etk::Vector<etk::Pair<int32_t, int32_t>>& _dataDebug);
	};
}


