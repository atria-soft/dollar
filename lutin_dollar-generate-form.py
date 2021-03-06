#!/usr/bin/python
import realog.debug as debug
import lutin.tools as tools


def get_type():
	return "BINARY"

def get_sub_type():
	return "TOOL"

def get_desc():
	return "Dollar generator of all needed form to have the best text recognition"

def get_licence():
	return "APACHE-2"

def get_compagny_type():
	return "com"

def get_compagny_name():
	return "atria-soft"

def get_maintainer():
	return "authors.txt"

def configure(target, my_module):
	my_module.add_src_file([
	    'tool/generate-form/main.cpp'
	    ])
	my_module.add_depend([
	    'dollar',
	    'test-debug',
	    ])
	return True

