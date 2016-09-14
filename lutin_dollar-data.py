#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools


def get_type():
	return "DATA"

def get_desc():
	return "dallar test-unit"

def get_licence():
	return "APACHE-2"

def get_compagny_type():
	return "com"

def get_compagny_name():
	return "atria-soft"

def get_maintainer():
	return "authors.txt"

def create(target, module_name):
	my_module = module.Module(__file__, module_name, get_type())
	my_module.copy_path('data/text/*.json', 'text')
	my_module.copy_path('data/figure/*.json', 'figure')
	return my_module

