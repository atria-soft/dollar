/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#pragma once
#include <string>
#include <etk/math/Vector2D.h>

namespace dollar {
	class Gesture {
		protected:
			std::string m_name;
			uint32_t m_subId;
			std::vector<std::vector<vec2>> m_path;
		public:
			Gesture();
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
			const std::vector<std::vector<vec2>>& getPath() const {
				return m_path;
			}
			std::vector<std::vector<vec2>>& getPath() {
				return m_path;
			}
		protected:
			std::vector<std::vector<vec2>> m_enginePath; // Singulized path with every conbinaison
			std::vector<std::vector<vec2>> m_engineVector;
			std::vector<vec2> m_engineStartV;
			std::vector<vec2> m_enginePoints;
		public:
			// Configure the reference gesture for recognition...
			void configure(float _startAngleIndex, size_t _nbSample, bool _ignoreRotation, float _distance);
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
			const std::vector<vec2>& getEnginePoints() const {
				return m_enginePoints;
			}
	};
	std::vector<std::vector<vec2>> loadPoints(const std::string& _fileName);
}
