/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#pragma once

#include <etk/math/Vector2D.hpp>
#include <dollar/Results.hpp>
#include <dollar/Gesture.hpp>
#include <ememory/memory.hpp>
#include <limits>
#include <iostream>
#include <etk/String.hpp>

/**
 * @brief dollar library main namespace
 */
namespace dollar {
	class Engine {
		protected:
			size_t m_nbResult; // Number of result in the recognition parsing
		public:
			void setNumberResult(size_t _value);
			size_t getNumberResult();
		public:
			Engine();
			virtual ~Engine() = default;
			dollar::Results recognize(const etk::Vector<vec2>& _paths);
			dollar::Results recognize(const etk::Vector<etk::Vector<vec2>>& _paths);
		protected:
			virtual dollar::Results recognize2(const etk::Vector<etk::Vector<vec2>>& _paths) = 0;
		public:
			virtual bool loadPath(const etk::Uri& _uri);
			virtual bool loadGesture(const etk::Uri& _uri) = 0;
			virtual void addGesture(ememory::SharedPtr<dollar::Gesture> _gesture) = 0;
	};
	
	ememory::SharedPtr<dollar::Engine> createEngine(const etk::String& _method="$N");
}


