#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools
import os

def get_type():
	return "BINARY"

def get_sub_type():
	return "TOOL"

def get_desc():
	return "LGDT tools: Handwriting recorder"

def get_licence():
	return "PROPRIETARY"

def get_compagny_type():
	return "com"

def get_compagny_name():
	return "Edouard DUPIN"

def get_maintainer():
	return ["Mr DUPIN Edouard <yui.heero@gmail.com>"]

def get_version():
	return [0,1]

def create(target, module_name):
	my_module = module.Module(__file__, module_name, get_type())
	my_module.add_src_file([
	    'appl/Main.cpp',
	    'appl/debug.cpp',
	    'appl/Windows.cpp',
	    'appl/widget/TextAreaRecognition.cpp',
	    ])
	my_module.add_module_depend([
	    'ewol',
	    'ejson',
	    'esvg',
	    'dollar',
	    #'dollar-data'
	    ])
	my_module.compile_flags('c++', [
	    "-DPROJECT_NAME=\"\\\"" + my_module.name + "\\\"\"",
	    "-DAPPL_VERSION=\"\\\"" + tools.version_to_string(get_version()) + "\\\"\""
	    ])
	my_module.add_path(tools.get_current_path(__file__))
	my_module.pkg_set("VERSION_CODE", 1)
	my_module.pkg_add("RIGHT", "WRITE_EXTERNAL_STORAGE")
	
	my_module.copy_path('data/icon.*','')
	my_module.copy_path("data/freefont/*","fonts/")
	my_module.copy_path('data/reference/*', "reference")
	
	my_module.pkg_set("ICON", os.path.join(tools.get_current_path(__file__), "data/icon.png"))
	return my_module




