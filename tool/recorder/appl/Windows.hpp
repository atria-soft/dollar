/** @file
 * @author Edouard DUPIN 
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license PROPRIETARY (see license file)
 */
#pragma once

#include <ewol/widget/Windows.hpp>
#include <ewol/widget/Composer.hpp>

namespace appl {
	class Windows;
	using WindowsShared = ememory::SharedPtr<appl::Windows>;
	using WindowsWeak = ememory::WeakPtr<appl::Windows>;
	class Windows : public ewol::widget::Windows {
		private:
			ewol::widget::ComposerShared m_composer;
			etk::Vector<etk::String> m_listValue;
			size_t m_currentId;
			etk::Vector<etk::String> m_listType;
			size_t m_currentTypeId;
			etk::String m_userName;
		protected:
			Windows();
			void init();
		public:
			DECLARE_FACTORY(Windows);
		public: // callback functions
			void onCallbackChangeNameUser(const etk::String& _value);
			void onCallbackClear();
			void onCallbackUndo();
			void onCallbackStore();
			void onCallbackPrevious();
			void onCallbackNext();
			void onCallbackPreviousType();
			void onCallbackNextType();
	};
}
