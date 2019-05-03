#!/usr/bin/python
import realog.debug as debug
import lutin.tools as tools


def get_type():
	return "LIBRARY"

def get_desc():
	return "$N gesture recognition"

def get_licence():
	return "BSD-3"

def get_compagny_type():
	return "com"

def get_compagny_name():
	return "atria-soft"

def get_maintainer():
	return "authors.txt"

def get_version():
	return "version.txt"

def configure(target, my_module):
	my_module.add_extra_flags()
	# add the file to compile:
	my_module.add_src_file([
	    'dollar/debug.cpp',
	    'dollar/Engine.cpp',
	    'dollar/EngineN.cpp',
	    'dollar/EngineP.cpp',
	    'dollar/EnginePPlus.cpp',
	    'dollar/Gesture.cpp',
	    'dollar/GestureN.cpp',
	    'dollar/GestureP.cpp',
	    'dollar/GesturePPlus.cpp',
	    'dollar/Results.cpp',
	    'dollar/tools.cpp',
	    'dollar/Rectangle.cpp'
	    ])
	
	my_module.add_header_file([
	    'dollar/*.hpp',
	    ],
	    destination_path="dollar")
	
	# build in C++ mode
	my_module.compile_version("c++", 2011)
	# add dependency of the generic C++ library:
	my_module.add_depend([
	    'cxx',
	    'elog',
	    'etk',
	    'ejson',
	    'esvg',
	    'ememory',
	    ])
	return True


