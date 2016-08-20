/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <iostream>
#include <dollar/Engine.h>

#include <etk/etk.h>
#include <test-debug/debug.h>

void usage(const std::string& _progName) {
	TEST_PRINT("usage:");
	TEST_PRINT("    " << _progName << " [option] source destination");
	TEST_PRINT("        [option]");
	TEST_PRINT("            -h --help         Display this help");
	TEST_PRINT("        parameters (must be here)");
	TEST_PRINT("            source            file .json or .svg");
	TEST_PRINT("            destination       file .json or .svg");
}

int main(int _argc, const char *_argv[]) {
	// init etk log system and file interface:
	etk::init(_argc, _argv);
	std::string src;
	std::string dst;
	for (int32_t iii=1; iii<_argc; ++iii) {
		std::string arg = _argv[iii];
		if (    arg == "-h"
		     || arg == "--help") {
			usage(_argv[0]);
			return 0;
		}
		if (src == "") {
			src = arg;
			continue;
		}
		if (dst == "") {
			dst = arg;
			continue;
		}
		if(    arg[0] == '-'
		    && arg[1] == '-') {
			// subLibrary usage ...
			continue;
		}
		usage(_argv[0]);
		return -1;
	}
	if (    src == ""
	     || dst == "") {
		TEST_ERROR("Missing input or output");
		usage(_argv[0]);
		return -1;
	}
	// declare a Gesture (internal API)
	dollar::Gesture ref;
	TEST_PRINT("---------------------------------------------------------------------------");
	TEST_PRINT("-- Load gesture: " << src);
	TEST_PRINT("---------------------------------------------------------------------------");
	// Load gesture with his extention type:
	std::string tmpName = etk::tolower(src);
	if (ref.load(src) == false) {
		TEST_ERROR("Error when loading the gesture");
		return -1;
	}
	TEST_PRINT("---------------------------------------------------------------------------");
	TEST_PRINT("-- Store gesture: " << dst);
	TEST_PRINT("---------------------------------------------------------------------------");
	// Store gesture with his extention type:
	if (ref.store(dst) == false) {
		TEST_ERROR("UNKNOW 'destination' extention type");
		usage(_argv[0]);
		return -1;
	}
	// All is done corectly
	return 0;
}