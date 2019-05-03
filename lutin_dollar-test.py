#!/usr/bin/python
import realog.debug as debug
import lutin.tools as tools


def get_type():
	return "BINARY"

def get_sub_type():
	return "TEST"

def get_desc():
	return "dollar test-unit"

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
		'test/main.cpp'
		])
	my_module.add_depend([
	    'dollar',
	    'dollar-data',
	    'test-debug',
	    'etest',
	    ])
	my_module.copy_path('data/test/*.json', 'test')
	return True

