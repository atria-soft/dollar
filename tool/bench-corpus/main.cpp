/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <iostream>
#include <dollar/EnginePPlus.hpp>
#include <dollar/tools.hpp>

#include <etk/etk.hpp>
#include <test-debug/debug.hpp>
#include <etk/os/FSNode.hpp>
#include <etk/Map.hpp>

static bool keepAspectRatio = false;
static float distanceReference = 0.1f; // distance of the gesture reference [0.02, 0.3]
static float distanceExclude = 0.2f; // distance of the exclusion point     [0.1, 1.0]
static float penalityRef = 0.1;
static float penalitySample = 0.1;
static float penalityAspectRatio = 0.2; //!< ==> result += delta aspect ratio * penality

void usage(const etk::String& _progName) {
	TEST_PRINT("usage:");
	TEST_PRINT("    " << _progName << " [option] reference_gesture corpus_path");
	TEST_PRINT("        [option]");
	TEST_PRINT("            -h --help             Display this help");
	TEST_PRINT("            --keep_ratio                 Keep aspect ratio for the form recognition (default:" + etk::toString(keepAspectRatio) + ")");
	TEST_PRINT("            --dist-check=flaot           Distance between points in the system recognition (default:" + etk::toString(distanceReference) + ")");
	TEST_PRINT("            --dist-excl=flaot            Distance to exclude a point in a pathern matching ... (default:" + etk::toString(distanceExclude) + ")");
	TEST_PRINT("            --penal-ref=float            Penality for reference when not connected (default:" + etk::toString(penalityRef) + ")");
	TEST_PRINT("            --penal-sample=float         Penality for sample when not connected (default:" + etk::toString(penalitySample) + ")");
	TEST_PRINT("            --penal-aspect-ratio=float   Penality for the distance of aspect ratio (default:" + etk::toString(penalityAspectRatio) + ")");
	TEST_PRINT("        parameters (must be here)");
	TEST_PRINT("            reference_gesture   Path of the reference gestures");
	TEST_PRINT("            corpus_path         Path of the corpus files");
}

bool testCorpus(const etk::String& _srcGesture, const etk::String& _srcCorpus);


int main(int _argc, const char *_argv[]) {
	// init etk log system and file interface:
	etk::init(_argc, _argv);
	etk::String srcGesture;
	etk::String srcCorpus;
	for (int32_t iii=1; iii<_argc; ++iii) {
		etk::String arg = _argv[iii];
		if (    arg == "-h"
		     || arg == "--help") {
			usage(_argv[0]);
			return 0;
		}
		if (arg == "--keep_ratio") {
			keepAspectRatio = true;
			continue;
		}
		if (etk::start_with(arg,"--dist-ref=") == true) {
			etk::String val(&arg[11]);
			distanceReference = etk::string_to_float(val);
			TEST_PRINT("configure distanceReference=" << distanceReference);
			continue;
		}
		if (etk::start_with(arg,"--dist-excl=") == true) {
			etk::String val(&arg[12]);
			distanceExclude = etk::string_to_float(val);
			TEST_PRINT("configure distanceExclude=" << distanceExclude);
			continue;
		}
		if (etk::start_with(arg,"--penal-ref=") == true) {
			etk::String val(&arg[12]);
			penalityRef = etk::string_to_float(val);
			TEST_PRINT("configure penalityRef=" << penalityRef);
			continue;
		}
		if (etk::start_with(arg,"--penal-sample=") == true) {
			etk::String val(&arg[15]);
			penalityRef = etk::string_to_float(val);
			TEST_PRINT("configure penalitySample=" << penalitySample);
			continue;
		}
		if (etk::start_with(arg,"--penal-aspect-ratio=") == true) {
			etk::String val(&arg[20]);
			penalityAspectRatio = etk::string_to_float(val);
			TEST_PRINT("configure penalityAspectRatio=" << penalityAspectRatio);
			continue;
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

void generateFile(const etk::String& _fileName, const etk::Vector<etk::String>& _list) {
	etk::String data("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
	data += "<svg height=\"100\" width=\"100\">\n";
	for (auto &itFile : _list) {
		etk::Vector<etk::Vector<vec2>> strokes = dollar::scaleToOne(dollar::loadPoints(itFile));
		for (auto &itLines : strokes) {
			data += "	<polyline fill=\"none\" stroke=\"black\" stroke-opacity=\"0.5\" stroke-width=\"1\"\n";
			data += "	          points=\"";
			bool first = true;
			for (auto& itPoints : itLines) {
				if (first == false) {
					data += " ";
				}
				first = false;
				data += etk::toString(itPoints.x()*100.0f) + "," + etk::to_string((1.0-itPoints.y())*100.0f);
			}
			data += "\"\n";
			data += "	          />\n";
		}
	}
	data += "</svg>\n";
	etk::FSNodeWriteAllData(_fileName, data);
}

void annalyseResult(etk::Map<etk::String, etk::Vector<etk::Pair<dollar::Results, etk::String>>>& _result) {
	TEST_PRINT("Full results:");
	for (auto &it : _result) {
		int32_t nbRecognise = 0;
		int32_t nbtested = 0;
		etk::String label = etk::split(it.first, ' ')[0];
		etk::String type;
		if (etk::split(it.first, ' ').size() > 1) {
			type = etk::split(it.first, ' ')[1];
		}
		etk::Vector<etk::String> listFull;
		etk::Vector<etk::String> listWrong;
		etk::Map<etk::String, int32_t> wrongValues;
		for (auto itRes : it.second) {
			nbtested ++;
			listFull.pushBack(itRes.second);
			if (label == itRes.first.getName()) {
				nbRecognise++;
			} else {
				listWrong.pushBack(itRes.second);
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

bool testCorpus(const etk::String& _srcGesture, const etk::String& _srcCorpus) {
	// declare a Gesture (internal API)
	dollar::EnginePPlus reco;
	reco.setScaleKeepRatio(keepAspectRatio);
	reco.setPPlusDistance(distanceReference);
	reco.setPPlusExcludeDistance(distanceExclude);
	reco.setPenalityNotLinkRef(penalityRef);
	reco.setPenalityNotLinkSample(penalitySample);
	reco.setPenalityAspectRatio(penalityAspectRatio);
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
	etk::Vector<etk::String> files = path.folderGetSub(false, true, "*.json");
	TEST_PRINT("---------------------------------------------------------------------------");
	TEST_PRINT("-- test gestures: ");
	TEST_PRINT("---------------------------------------------------------------------------");
	
	// "label_type" ==> list of (result, file test name)
	etk::Map<etk::String, etk::Vector<etk::Pair<dollar::Results, etk::String>>> agregateResults;
	int32_t nbRecognise = 0;
	int32_t nbRecognise2 = 0;
	int32_t nbtested = 0;
	for (auto &it : files) {
		etk::String label;
		etk::String type;
		etk::Vector<etk::Vector<vec2>> listPoints = dollar::loadPoints(it, &label, &type);
		if (type == "hand") {
			//continue; // rejest for now ...
		}
		nbtested++;
		etk::Vector<etk::String> path = etk::split(it, '/');
		etk::String filename = path[path.size()-1];
		TEST_PRINT("Test '" << label << "' type=" << type << "       " << filename);
		dollar::Results res = reco.recognize(listPoints);
		
		//agregateResults[label+" "+type].pushBack(etk::makePair(res,it));
		agregateResults[label].pushBack(etk::makePair(res,it));
		
		if (res.haveMatch() == false) {
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
				for (size_t iii=1; iii<res.getSize() || iii<4; ++iii) {
					TEST_WARNING("         " << res.getName(iii) << " score=" << res.getConfidence(iii));
				}
			}
		#endif
	}
	annalyseResult(agregateResults);
	TEST_PRINT("Recognise:        " << nbRecognise << " / " << nbtested << " ==> " << (float(nbRecognise) / float(nbtested) * 100.0f) << " %");
	TEST_PRINT("Recognise (a==A): " << nbRecognise2 << " / " << nbtested << " ==> " << (float(nbRecognise2) / float(nbtested) * 100.0f) << " %");
	// All is done corectly
	return 0;
}



