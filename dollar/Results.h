/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <math.h>
#include <string>
#include <list>
#include <vector>

namespace dollar {
	class Results {
		protected:
			std::vector<std::pair<std::string,float>> m_values;
		public:
			Results();
			bool haveMath() const;
			size_t getSize() const;
			std::string getName(size_t _id=0) const;
			float getConfidence(size_t _id=0) const;
			void addValue(const std::string& _name, float _confidence);
			void clear();
	};
}


