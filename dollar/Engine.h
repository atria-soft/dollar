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
#include <ememory/memory.h>

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
			dollar::Results recognize(const std::vector<vec2>& _paths);
			dollar::Results recognize(const std::vector<std::vector<vec2>>& _paths);
		protected:
			virtual dollar::Results recognize2(const std::vector<std::vector<vec2>>& _paths) = 0;
		public:
			virtual bool loadPath(const std::string& _path);
			virtual bool loadGesture(const std::string& _filename) = 0;
			virtual void addGesture(ememory::SharedPtr<dollar::Gesture> _gesture) = 0;
	};
	
	ememory::SharedPtr<dollar::Engine> createEngine(const std::string& _method="$N");
}


