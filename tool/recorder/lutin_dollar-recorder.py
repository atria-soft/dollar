#!/usr/bin/python
import lutin.debug as debug
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

def configure(target, my_module):
	my_module.add_src_file([
	    'appl/Main.cpp',
	    'appl/debug.cpp',
	    'appl/Windows.cpp',
	    'appl/widget/TextAreaRecognition.cpp',
	    ])
	my_module.add_depend([
	    'ewol',
	    'ejson',
	    'esvg',
	    'dollar',
	    #'dollar-data'
	    ])
	my_module.add_flag('c++', [
	    "-DPROJECT_NAME=\"\\\"" + my_module.get_name() + "\\\"\"",
	    "-DAPPL_VERSION=\"\\\"" + tools.version_to_string(get_version()) + "\\\"\""
	    ])
	my_module.add_path(".")
	my_module.set_pkg("VERSION_CODE", 1)
	my_module.add_pkg("RIGHT", "WRITE_EXTERNAL_STORAGE")
	
	my_module.copy_path('data/icon.*','')
	my_module.copy_path("data/freefont/*","fonts/")
	my_module.copy_path('data/reference/*', "reference")
	
	my_module.set_pkg("ICON", "data/icon.png")
	return True




