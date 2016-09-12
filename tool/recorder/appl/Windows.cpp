/** @file
 * @author Edouard DUPIN 
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license PROPRIETARY (see license file)
 */

#include <ewol/ewol.h>
#include <appl/debug.h>
#include <appl/Windows.h>
#include <ewol/widget/Label.h>
#include <ewol/widget/Button.h>
#include <ewol/widget/Entry.h>
#include <appl/widget/TextAreaRecognition.h>
#include <etk/tool.h>

appl::Windows::Windows() :
  m_composer(nullptr),
  m_currentId(0),
  m_currentTypeId(0),
  m_userName("Edouard DUPIN") {
	addObjectType("appl::Windows");
	propertyTitle.setDirectCheck(std::string("sample ") + PROJECT_NAME);
}

void appl::Windows::init() {
	ewol::widget::Windows::init();
	m_listType.push_back("hand");
	m_listType.push_back("print");
	m_currentTypeId = 0;
	std::string tmp;
	for (char iii='0'; iii<='9'; ++iii) {
		tmp = iii;
		m_listValue.push_back(tmp);
	}
	for (char iii='a'; iii<='z'; ++iii) {
		tmp = iii;
		m_listValue.push_back(tmp);
	}
	for (char iii='A'; iii<='Z'; ++iii) {
		tmp = iii;
		m_listValue.push_back(tmp);
	}
	m_listValue.push_back("+");
	m_listValue.push_back("-");
	m_listValue.push_back("*");
	m_listValue.push_back("=");
	m_listValue.push_back("/");
	m_listValue.push_back("?");
	m_listValue.push_back("!");
	m_listValue.push_back("@");
	m_listValue.push_back("#");
	m_listValue.push_back("~");
	m_listValue.push_back("&");
	m_listValue.push_back("(");
	m_listValue.push_back(")");
	m_listValue.push_back("[");
	m_listValue.push_back("]");
	m_listValue.push_back("{");
	m_listValue.push_back("}");
	m_listValue.push_back("^");
	m_listValue.push_back("%");
	m_listValue.push_back(";");
	m_listValue.push_back(".");
	m_listValue.push_back(",");
	m_listValue.push_back("<");
	m_listValue.push_back(">");
	m_listValue.push_back("µ");
	m_listValue.push_back("$");
	m_listValue.push_back("\"");
	m_listValue.push_back("'");
	m_listValue.push_back("|");
	m_listValue.push_back("\\");
	m_listValue.push_back("€");
	m_listValue.push_back("ù");
	m_listValue.push_back("é");
	m_listValue.push_back("è");
	m_listValue.push_back("ç");
	m_listValue.push_back("à");
	m_listValue.push_back("÷");
	m_listValue.push_back("≠");
	m_listValue.push_back("≡");
	m_listValue.push_back("≤");
	m_listValue.push_back("≥");
	m_listValue.push_back("∀");
	m_listValue.push_back("∧");
	m_listValue.push_back("→");
	m_listValue.push_back("←");
	m_listValue.push_back("⇔");
	m_listValue.push_back("⌊");
	m_listValue.push_back("⌋");
	m_listValue.push_back("⌈");
	m_listValue.push_back("⌉");
	m_listValue.push_back("∆");
	m_listValue.push_back("∇");
	m_listValue.push_back("□");
	m_listValue.push_back("◊");
	m_listValue.push_back("⊳");
	m_listValue.push_back("⊵");
	m_listValue.push_back("⊲");
	m_listValue.push_back("⊴");
	m_listValue.push_back("∃");
	m_listValue.push_back("∑");
	m_listValue.push_back("≈");
	m_listValue.push_back("∝");
	m_listValue.push_back("∞");
	m_currentId = 0;
	std::string composition = std::string("");
	composition += "<sizer mode='vert'>\n";
	composition += "	<spacer min-size='4%'/>\n";
	composition += "	<sizer mode='hori' lock='false,true' min-size='9%'>\n";
	composition += "		<button name='bt-clear' fill='true' expand='true'>\n";
	composition += "			<label expand='true' fill='false'>\n";
	composition += "				Clear\n";
	composition += "			</label>\n";
	composition += "		</button>\n";
	composition += "		<button name='bt-undo' fill='true' expand='true'>\n";
	composition += "			<label expand='true' fill='false'>\n";
	composition += "				Undo\n";
	composition += "			</label>\n";
	composition += "		</button>\n";
	composition += "		<button name='bt-store' fill='true' expand='true'>\n";
	composition += "			<label expand='true' fill='false'>\n";
	composition += "				Store\n";
	composition += "			</label>\n";
	composition += "		</button>\n";
	composition += "	</sizer>\n";
	composition += "	<spacer min-size='4%'/>\n";
	composition += "	<TextAreaRecognition name='recorder' expand='true' fill='true'/>\n";
	composition += "	<spacer min-size='2%'/>\n";
	composition += "	<sizer mode='hori' lock='false,true' min-size='7%'>\n";
	composition += "		<entry name='user-name' value='Edouard DUPIN' max='30' regex='(\\w| )*' min-size='50%,0%'/>\n";
	composition += "		<label name='current-type' min-size='25%,0%'/>\n";
	composition += "		<label name='current-lettre' min-size='25%,0%' expand='true,false'/>\n";
	composition += "	</sizer>\n";
	composition += "	<spacer min-size='2%'/>\n";
	composition += "	<sizer mode='hori' lock='false,true' min-size='7%'>\n";
	composition += "		<spacer expand='true,false'/>\n";
	composition += "		<button name='bt-previous' fill='true' expand='true'>\n";
	composition += "			<label expand='true' fill='false'>\n";
	composition += "				Previous\n";
	composition += "			</label>\n";
	composition += "		</button>\n";
	composition += "		<button name='bt-next' fill='true' expand='true'>\n";
	composition += "			<label expand='true' fill='false'>\n";
	composition += "				Next\n";
	composition += "			</label>\n";
	composition += "		</button>\n";
	composition += "		<spacer expand='true,false'/>\n";
	composition += "		<button name='bt-previous-type' fill='true' expand='true'>\n";
	composition += "			<label expand='true' fill='false'>\n";
	composition += "				Type prev\n";
	composition += "			</label>\n";
	composition += "		</button>\n";
	composition += "		<button name='bt-next-type' fill='true' expand='true'>\n";
	composition += "			<label expand='true' fill='false'>\n";
	composition += "				Type next\n";
	composition += "			</label>\n";
	composition += "		</button>\n";
	composition += "		<spacer expand='true,false'/>\n";
	composition += "	</sizer>\n";
	composition += "</sizer>\n";
	m_composer = ewol::widget::Composer::create();
	if (m_composer == nullptr) {
		APPL_CRITICAL(" An error occured ... in the windows creatrion ...");
		return;
	}
	m_composer->loadFromString(composition);
	setSubWidget(m_composer);
	subBind(ewol::widget::Button, "bt-clear", signalPressed, sharedFromThis(), &appl::Windows::onCallbackClear);
	subBind(ewol::widget::Button, "bt-undo", signalPressed, sharedFromThis(), &appl::Windows::onCallbackUndo);
	subBind(ewol::widget::Button, "bt-store", signalPressed, sharedFromThis(), &appl::Windows::onCallbackStore);
	subBind(ewol::widget::Button, "bt-previous", signalPressed, sharedFromThis(), &appl::Windows::onCallbackPrevious);
	subBind(ewol::widget::Button, "bt-next", signalPressed, sharedFromThis(), &appl::Windows::onCallbackNext);
	subBind(ewol::widget::Button, "bt-previous-type", signalPressed, sharedFromThis(), &appl::Windows::onCallbackPreviousType);
	subBind(ewol::widget::Button, "bt-next-type", signalPressed, sharedFromThis(), &appl::Windows::onCallbackNextType);
	subBind(ewol::widget::Entry, "user-name", signalModify, sharedFromThis(), &appl::Windows::onCallbackChangeNameUser);
	propertySetOnWidgetNamed("current-lettre", "value", m_listValue[m_currentId]);
	propertySetOnWidgetNamed("current-type", "value", m_listType[m_currentTypeId]);
	
	auto tmpDisp = ememory::dynamicPointerCast<appl::widget::TextAreaRecognition>(getSubObjectNamed("recorder"));
	if (tmpDisp != nullptr) {
		tmpDisp->setCompare(m_listType[m_currentTypeId] + " " + m_listValue[m_currentId]);
	}
}

void appl::Windows::onCallbackChangeNameUser(const std::string& _value) {
	m_userName = _value;
}

void appl::Windows::onCallbackClear() {
	auto tmpDisp = ememory::dynamicPointerCast<appl::widget::TextAreaRecognition>(getSubObjectNamed("recorder"));
	if (tmpDisp != nullptr) {
		tmpDisp->clear();
	}
}

void appl::Windows::onCallbackStore() {
	auto tmpDisp = ememory::dynamicPointerCast<appl::widget::TextAreaRecognition>(getSubObjectNamed("recorder"));
	if (tmpDisp != nullptr) {
		tmpDisp->store(m_userName, m_listValue[m_currentId], m_listType[m_currentTypeId]);
		tmpDisp->clear();
	}
}

void appl::Windows::onCallbackUndo() {
	auto tmpDisp = ememory::dynamicPointerCast<appl::widget::TextAreaRecognition>(getSubObjectNamed("recorder"));
	if (tmpDisp != nullptr) {
		tmpDisp->undo();
	}
}

void appl::Windows::onCallbackPrevious() {
	if (m_currentId == 0) {
		m_currentId = m_listValue.size();
	}
	m_currentId--;
	if (m_listValue[m_currentId] == "<") {
		propertySetOnWidgetNamed("current-lettre", "value", "&lt;");
	} else if (m_listValue[m_currentId] == ">") {
		propertySetOnWidgetNamed("current-lettre", "value", "&gt;");
	} else {
		propertySetOnWidgetNamed("current-lettre", "value", m_listValue[m_currentId]);
	}
	auto tmpDisp = ememory::dynamicPointerCast<appl::widget::TextAreaRecognition>(getSubObjectNamed("recorder"));
	if (tmpDisp != nullptr) {
		tmpDisp->setCompare(m_listType[m_currentTypeId] + " " + m_listValue[m_currentId]);
	}
	onCallbackClear();
}

void appl::Windows::onCallbackNext() {
	m_currentId++;
	if (m_currentId == m_listValue.size()) {
		m_currentId = 0;
	}
	if (m_listValue[m_currentId] == "<") {
		propertySetOnWidgetNamed("current-lettre", "value", "&lt;");
	} else if (m_listValue[m_currentId] == ">") {
		propertySetOnWidgetNamed("current-lettre", "value", "&gt;");
	} else {
		propertySetOnWidgetNamed("current-lettre", "value", m_listValue[m_currentId]);
	}
	auto tmpDisp = ememory::dynamicPointerCast<appl::widget::TextAreaRecognition>(getSubObjectNamed("recorder"));
	if (tmpDisp != nullptr) {
		tmpDisp->setCompare(m_listType[m_currentTypeId] + " " + m_listValue[m_currentId]);
	}
	onCallbackClear();
}

void appl::Windows::onCallbackPreviousType() {
	if (m_currentTypeId == 0) {
		m_currentTypeId = m_listType.size();
	}
	m_currentTypeId--;
	propertySetOnWidgetNamed("current-type", "value", m_listType[m_currentTypeId]);
	auto tmpDisp = ememory::dynamicPointerCast<appl::widget::TextAreaRecognition>(getSubObjectNamed("recorder"));
	if (tmpDisp != nullptr) {
		tmpDisp->setCompare(m_listType[m_currentTypeId] + " " + m_listValue[m_currentId]);
	}
	onCallbackClear();
}

void appl::Windows::onCallbackNextType() {
	m_currentTypeId++;
	if (m_currentTypeId == m_listType.size()) {
		m_currentTypeId = 0;
	}
	propertySetOnWidgetNamed("current-type", "value", m_listType[m_currentTypeId]);
	auto tmpDisp = ememory::dynamicPointerCast<appl::widget::TextAreaRecognition>(getSubObjectNamed("recorder"));
	if (tmpDisp != nullptr) {
		tmpDisp->setCompare(m_listType[m_currentTypeId] + " " + m_listValue[m_currentId]);
	}
	onCallbackClear();
}
