/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/math/Vector2D.hpp>
#include <ememory/memory.hpp>

#include <string>

namespace dollar {
	class Gesture {
		protected:
			std::string m_name;
			uint32_t m_subId;
			bool m_keepAspectRatio;
			std::vector<std::vector<vec2>> m_path;
		public:
			Gesture();
			virtual ~Gesture() = default;
			void setKeepAspectRatio(bool _value) {
				m_keepAspectRatio = _value;
			}
			bool getKeepAspectRatio() {
				return m_keepAspectRatio;
			}
			bool load(const std::string& _filename);
			bool store(const std::string& _filename);
			void set(const std::string& _name, uint32_t _subId, std::vector<std::vector<vec2>> _path);
		protected:
			bool loadJSON(const std::string& _filename);
			bool loadSVG(const std::string& _filename);
			void storeJSON(const std::string& _filename);
			void storeSVG(const std::string& _filename, bool _storeDot=false);
		public:
			const std::string& getName() {
				return m_name;
			}
			const uint32_t& getId() {
				return m_subId;
			}
	};
	std::vector<std::vector<vec2>> loadPoints(const std::string& _fileName, std::string* _label=nullptr, std::string* _type=nullptr);
}
