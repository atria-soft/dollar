/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <cmath>
#include <etk/String.hpp>
#include <list>
#include <etk/Vector.hpp>

namespace dollar {
	class ResultData {
		public:
			etk::String value;
			float confidence;
			#ifdef DOLLAR_ANNALYSER_ENABLE
				float distance;
				int32_t deltaLines; //!< number of line in delta if > 0 ==> more lines in the reference
				float deltaRatio:
				int32_t deltaDots; //!< number of dots in delta if > 0 ==> more dots in the reference
			#endif
		public:
			ResultData(const etk::String& _value, float _confidence):
			  value(_value),
			  confidence(_confidence) {
				
			}
	};
	class Results {
		protected:
			etk::Vector<ResultData> m_values;
		public:
			Results();
			bool haveMatch() const;
			size_t getSize() const;
			etk::String getName(size_t _id=0) const;
			float getConfidence(size_t _id=0) const;
			void addValue(const etk::String& _name, float _confidence);
			void clear();
	};
}


