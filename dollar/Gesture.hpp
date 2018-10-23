/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/math/Vector2D.hpp>
#include <ememory/memory.hpp>

#include <etk/uri/uri.hpp>
#include <etk/String.hpp>

namespace dollar {
	class Gesture {
		protected:
			etk::String m_name;
			uint32_t m_subId;
			float m_aspectRatio;
			etk::Vector<etk::Vector<vec2>> m_path;
		public:
			Gesture();
			virtual ~Gesture() = default;
			void setAspectRatio(float _value) {
				m_aspectRatio = _value;
			}
			float getKeepAspectRatio() {
				return m_aspectRatio;
			}
			bool load(const etk::Uri& _filename);
			bool store(const etk::Uri& _filename);
			void set(const etk::String& _name, uint32_t _subId, etk::Vector<etk::Vector<vec2>> _path);
		protected:
			bool loadJSON(const etk::Uri& _filename);
			bool loadSVG(const etk::Uri& _filename);
			void storeJSON(const etk::Uri& _filename);
			void storeSVG(const etk::Uri& _filename, bool _storeDot=false);
		public:
			const etk::String& getName() {
				return m_name;
			}
			const uint32_t& getId() {
				return m_subId;
			}
	};
	/**
	 * @brief Load all point from a specific file
	 *
	 */
	etk::Vector<etk::Vector<vec2>> loadPoints(const etk::Uri& _fileName, etk::String* _label=null, etk::String* _type=null);
}
