/** @file
 * @author Edouard DUPIN 
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license PROPRIETARY (see license file)
 */
#pragma once

#include <ewol/widget/Widget.hpp>
#include <ewol/compositing/Drawing.hpp>
#include <ewol/compositing/Text.hpp>
#include <ewol/widget/Manager.hpp>

#include <dollar/Engine.hpp>

namespace appl {
	class DrawingLine {
		public:
			DrawingLine():
			  m_type(gale::key::type::unknow) {
				
			}
			enum gale::key::type m_type;
			etk::Vector<vec2> m_data;
			void clear() {
				m_type = gale::key::type::unknow;
				m_data.clear();
			}
			void addPoint(const vec2& _point) {
				if (m_data.size() != 0) {
					if (m_data[m_data.size()-1] == _point) {
						return;
					}
				}
				m_data.pushBack(_point);
			}
	};
	namespace widget {
		class TextAreaRecognition : public ewol::Widget {
			protected:
				ewol::compositing::Drawing m_draw; //!< drawing instance
				ewol::compositing::Text m_text; //!< drawing instance
				etk::Vector<DrawingLine> m_dataList;
				DrawingLine m_current;
				std::chrono::system_clock::time_point m_time;
				std::chrono::system_clock::time_point m_lastEvent;
				esignal::Connection m_periodicConnection;
				bool m_updateDone;
				etk::String m_svgData;
				int32_t m_detectId;
				etk::String m_compare;
				ememory::SharedPtr<dollar::Engine> m_dollarEngine;
				dollar::Results m_dollarResults;
				etk::String m_findValue;
				std::chrono::milliseconds m_dollarTime;
			protected:
				//! @brief constructor
				TextAreaRecognition();
				void init() override;
			public:
				DECLARE_WIDGET_FACTORY(TextAreaRecognition, "TextAreaRecognition");
				//! @brief destructor
				virtual ~TextAreaRecognition();
			public:
				void clear();
				void undo();
				void store(const etk::String& _userName, const etk::String& _value, const etk::String& _type);
				void setCompare(const etk::String& _compare);
			public:
				void onDraw() override;
				void onRegenerateDisplay() override;
				bool onEventInput(const ewol::event::Input& _event) override;
				void callbackPeriodicUpdate(const ewol::event::Time& _event);
		};
	}
}

