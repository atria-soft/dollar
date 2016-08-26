/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <iostream>
#include <dollar/Engine.h>
#include <dollar/tools.h>

#include <etk/etk.h>
#include <test-debug/debug.h>
#include <etk/os/FSNode.h>
#include <map>

void usage(const std::string& _progName) {
	TEST_PRINT("usage:");
	TEST_PRINT("    " << _progName << " [option] reference_gesture corpus_path");
	TEST_PRINT("        [option]");
	TEST_PRINT("            -h --help           Display this help");
	TEST_PRINT("        parameters (must be here)");
	TEST_PRINT("            reference_gesture   Path of the reference gestures");
	TEST_PRINT("            corpus_path         Path of the corpus files");
}

bool testCorpus(const std::string& _srcGesture, const std::string& _srcCorpus);

int main(int _argc, const char *_argv[]) {
	// init etk log system and file interface:
	etk::init(_argc, _argv);
	std::string srcGesture;
	std::string srcCorpus;
	for (int32_t iii=1; iii<_argc; ++iii) {
		std::string arg = _argv[iii];
		if (    arg == "-h"
		     || arg == "--help") {
			usage(_argv[0]);
			return 0;
		}
		if(    arg[0] == '-'
		    && arg[1] == '-') {
			// subLibrary usage ...
			continue;
		}
		if (srcGesture == "") {
			srcGesture = arg;
			continue;
		}
		if (srcCorpus == "") {
			srcCorpus = arg;
			continue;
		}
		usage(_argv[0]);
		return -1;
	}
	if (    srcGesture == ""
	     || srcCorpus == "") {
		TEST_ERROR("Missing input or output");
		usage(_argv[0]);
		return -1;
	}
	return testCorpus(srcGesture, srcCorpus);
}

void generateFile(const std::string& _fileName, const std::vector<std::string>& _list) {
	std::string data("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
	data += "<svg height=\"100\" width=\"100\">\n";
	for (auto &itFile : _list) {
		std::vector<std::vector<vec2>> strokes = dollar::scaleToOne(dollar::loadPoints(itFile));
		for (auto &itLines : strokes) {
			data += "	<polyline fill=\"none\" stroke=\"black\" stroke-opacity=\"0.5\" stroke-width=\"1\"\n";
			data += "	          points=\"";
			bool first = true;
			for (auto& itPoints : itLines) {
				if (first == false) {
					data += " ";
				}
				first = false;
				data += etk::to_string(itPoints.x()*100.0f) + "," + etk::to_string((1.0-itPoints.y())*100.0f);
			}
			data += "\"\n";
			data += "	          />\n";
		}
	}
	data += "</svg>\n";
	etk::FSNodeWriteAllData(_fileName, data);
}

void annalyseResult(std::map<std::string, std::vector<std::pair<dollar::Results, std::string>>>& _result) {
	TEST_PRINT("Full results:");
	for (auto &it : _result) {
		int32_t nbRecognise = 0;
		int32_t nbtested = 0;
		std::string label = etk::split(it.first, ' ')[0];
		std::string type = etk::split(it.first, ' ')[1];
		std::vector<std::string> listFull;
		std::vector<std::string> listWrong;
		std::map<std::string, int32_t> wrongValues;
		for (auto itRes : it.second) {
			nbtested ++;
			listFull.push_back(itRes.second);
			if (label == itRes.first.getName()) {
				nbRecognise++;
			} else {
				listWrong.push_back(itRes.second);
				if (wrongValues.find(itRes.first.getName()) != wrongValues.end()) {
					wrongValues[itRes.first.getName()]++;
				} else {
					wrongValues[itRes.first.getName()] = 1;
				}
			}
		}
		float ratio = float(nbRecognise) / float(nbtested) * 100.0f;
		if (ratio <= 50.0) {
			TEST_ERROR("    '" << label << " " << type << "': " << nbRecognise << " / " << nbtested << " ==> " << ratio << " %");
		} else if (ratio <= 75.0) {
			TEST_WARNING("    '" << label << " " << type << "': " << nbRecognise << " / " << nbtested << " ==> " << ratio << " %");
		} else {
			TEST_INFO("    '" << label << " " << type << "': " << nbRecognise << " / " << nbtested << " ==> " << ratio << " %");
		}
		if (ratio != 100.0f) {
			for (auto &it : wrongValues) {
				TEST_WARNING("            " << it.first << " " << it.second << "");
			}
		}
		generateFile("out_dollar/bench-corpus/OK/" + label + "_" + type + ".svg", listFull);
		if (listWrong.size() != 0) {
			generateFile("out_dollar/bench-corpus/ERROR/" + label + "_" + type + "_ERROR.svg", listWrong);
		} else {
			etk::FSNodeRemove("out_dollar/bench-corpus/ERROR/" + label + "_" + type + "_ERROR.svg");
		}
	}
}

bool testCorpus(const std::string& _srcGesture, const std::string& _srcCorpus) {
	// declare a Gesture (internal API)
	dollar::Engine reco;
	reco.setScaleKeepRatio(false);
	TEST_PRINT("---------------------------------------------------------------------------");
	TEST_PRINT("-- Load Gestures: " << _srcGesture);
	TEST_PRINT("---------------------------------------------------------------------------");
	// Load gesture with his extention type:
	if (reco.loadPath(_srcGesture) == false) {
		TEST_ERROR("Error when loading the gestures");
		return -1;
	}
	TEST_DEBUG("List all file in the corpus path");
	etk::FSNode path(_srcCorpus);
	std::vector<std::string> files = path.folderGetSub(false, true, "*.json");
	TEST_PRINT("---------------------------------------------------------------------------");
	TEST_PRINT("-- test gestures: ");
	TEST_PRINT("---------------------------------------------------------------------------");
	
	// "label_type" ==> list of (result, file test name)
	std::map<std::string, std::vector<std::pair<dollar::Results, std::string>>> agregateResults;
	int32_t nbRecognise = 0;
	int32_t nbRecognise2 = 0;
	int32_t nbtested = 0;
	for (auto &it : files) {
		std::string label;
		std::string type;
		std::vector<std::vector<vec2>> listPoints = dollar::loadPoints(it, &label, &type);
		if (type == "hand") {
			//continue; // rejest for now ...
		}
		nbtested++;
		std::vector<std::string> path = etk::split(it, '/');
		std::string filename = path[path.size()-1];
		TEST_PRINT("Test '" << label << "' type=" << type << "       " << filename);
		dollar::Results res = reco.recognize(listPoints, "$P+");
		
		agregateResults[label+" "+type].push_back(std::make_pair(res,it));
		
		if (res.haveMath() == false) {
			TEST_INFO("   Recognise noting ...");
			continue;
		}
		#if 0
			TEST_INFO("Results : ");
			for (size_t iii=0; iii<res.getSize(); ++iii) {
				TEST_INFO("    - " << res.getName(iii) << " score=" << res.getConfidence(iii));
			}
		#else
			if (res.getName() == label) {
				nbRecognise++;
				nbRecognise2++;
				TEST_INFO("         " << res.getName() << " score=" << res.getConfidence());
			} else if (etk::toupper(res.getName()) == etk::toupper(label)) {
				nbRecognise2++;
				TEST_WARNING("         " << res.getName() << " score=" << res.getConfidence());
			}else {
				TEST_ERROR("         " << res.getName() << " score=" << res.getConfidence());
			}
		#endif
	}
	annalyseResult(agregateResults);
	TEST_PRINT("Recognise:        " << nbRecognise << " / " << nbtested << " ==> " << (float(nbRecognise) / float(nbtested) * 100.0f) << " %");
	TEST_PRINT("Recognise (a==A): " << nbRecognise2 << " / " << nbtested << " ==> " << (float(nbRecognise2) / float(nbtested) * 100.0f) << " %");
	// All is done corectly
	return 0;
}



