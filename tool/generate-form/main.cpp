/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <dollar/EnginePPlus.hpp>
#include <dollar/tools.hpp>
#include <etk/etk.hpp>
#include <test-debug/debug.hpp>
#include <etk/os/FSNode.hpp>

#include <iostream>
#include <etk/Map.hpp>



static bool keepAspectRatio = false;
static float distanceReference = 0.1f; // distance of the gesture reference [0.02, 0.3]
static float distanceExclude = 0.2f; // distance of the exclusion point     [0.1, 1.0]
static float distanceGroupLimiting = 1.0f; // square distance of the grouping genereation
static float penalityRef = 0.1;
static float penalitySample = 0.1;
static float penalityAspectRatio = 0.2; //!< ==> result += delta aspect ratio * penality

void usage(const etk::String& _progName) {
	TEST_PRINT("usage:");
	TEST_PRINT("    " << _progName << " [option] corpus_path");
	TEST_PRINT("        ");
	TEST_PRINT("        [option]");
	TEST_PRINT("            -h --help                    Display this help");
	TEST_PRINT("            --keep_ratio                 Keep aspect ratio for the form recognition (default:" + etk::toString(keepAspectRatio) + ")");
	TEST_PRINT("            --dist-check=flaot           Distance between points in the system recognition (default:" + etk::toString(distanceReference) + ")");
	TEST_PRINT("            --dist-excl=flaot            Distance to exclude a point in a pathern matching ... (default:" + etk::toString(distanceExclude) + ")");
	TEST_PRINT("            --group-size=flaot           Size of the distance between point to stop grouping in one form (default:" + etk::toString(distanceGroupLimiting) + ")");
	TEST_PRINT("            --penal-ref=float            Penality for reference when not connected (default:" + etk::toString(penalityRef) + ")");
	TEST_PRINT("            --penal-sample=float         Penality for sample when not connected (default:" + etk::toString(penalitySample) + ")");
	TEST_PRINT("            --penal-aspect-ratio=float   Penality for the distance of aspect ratio (default:" + etk::toString(penalityAspectRatio) + ")");
	TEST_PRINT("        ");
	TEST_PRINT("        parameters (must be here)");
	TEST_PRINT("            corpus_path         Path of the corpus files");
	TEST_PRINT("        ");
	TEST_PRINT("        The generate data is in out_dollar/xxx");
	TEST_PRINT("        example:");
}

bool testCorpus(const etk::String& _srcCorpus);


int main(int _argc, const char *_argv[]) {
	// init etk log system and file interface:
	etk::init(_argc, _argv);
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
		if (etk::start_with(arg,"--group-size=") == true) {
			etk::String val(&arg[13]);
			distanceGroupLimiting = etk::string_to_float(val);
			TEST_PRINT("configure distanceGroupLimiting=" << distanceGroupLimiting);
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
		if (    arg[0] == '-'
		     && arg[1] == '-') {
			// subLibrary usage ...
			continue;
		}
		if (srcCorpus == "") {
			srcCorpus = arg;
			continue;
		}
		usage(_argv[0]);
		return -1;
	}
	if (srcCorpus == "") {
		TEST_ERROR("Missing input or output");
		usage(_argv[0]);
		return -1;
	}
	return testCorpus(srcCorpus);
}

void generateFile(const etk::String& _fileName, const etk::Vector<etk::String>& _list, const etk::String& _refName) {
	TEST_PRINT("    " << _fileName);
	etk::String data("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
	data += "<svg height=\"100\" width=\"100\">\n";
	for (auto &itFile : _list) {
		etk::Vector<etk::Vector<vec2>> strokes = dollar::scaleToOne(dollar::loadPoints(itFile), keepAspectRatio);
		for (auto &itLines : strokes) {
			if (itLines.size() == 1) {
				// TODO: This is a line ....
			}
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
	if (_refName != "") {
		etk::Vector<etk::Vector<vec2>> strokes = dollar::scaleToOne(dollar::loadPoints(_refName), keepAspectRatio);
		for (auto &itLines : strokes) {
			if (itLines.size() == 1) {
				// TODO: This is a line ....
			}
			data += "	<polyline fill=\"none\" stroke=\"red\" stroke-opacity=\"1.0\" stroke-width=\"2\"\n";
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

#define OUT_OF_RANGE (999999.0f)

bool testCorpus(const etk::String& _srcCorpus) {
	TEST_PRINT("---------------------------------------------------------------------------");
	TEST_PRINT("-- Create list of files: " << _srcCorpus);
	TEST_PRINT("---------------------------------------------------------------------------");
	etk::FSNode path(_srcCorpus);
	etk::Vector<etk::String> files = path.folderGetSub(false, true, "*.json");

	TEST_PRINT("corpus have " << files.size() << " files");
	etk::Vector<etk::String> listOfElementInCorpus;
	for (auto &it : files) {
		if (etk::end_with(it, ".json") == true) {
			etk::Vector<etk::String> path = etk::split(it, '/');
			etk::String elemName = etk::split(path[path.size()-1],'_')[0];
			if (elemName == "slash") {
				elemName = "/";
			}if (elemName == "back-slash") {
				elemName = "\\";
			}
			if (std::find(listOfElementInCorpus.begin(), listOfElementInCorpus.end(), elemName) == listOfElementInCorpus.end()) {
				listOfElementInCorpus.pushBack(elemName);
			}
		}
	}
	// remove generation path ...
	etk::FSNodeRemove("out_dollar/generate-form");
//listOfElementInCorpus.clear();
//listOfElementInCorpus.pushBack("slash");


	TEST_PRINT(" will done for: " << listOfElementInCorpus);
	int32_t nbElementGenerated = 0;
	for (auto &itTypeOfCorpus : listOfElementInCorpus) {
		TEST_PRINT("---------------------------------------------------------------------------");
		TEST_PRINT("-- Generate FOR: '" << itTypeOfCorpus << "'");
		TEST_PRINT("---------------------------------------------------------------------------");
		etk::Vector<etk::String> fileFiltered;
		etk::String fileNameIt = itTypeOfCorpus;
		if (fileNameIt == "/") {
			fileNameIt = "slash";
		} else if (fileNameIt == "\\") {
			fileNameIt = "back-slash";
		}
		for (auto &it : files) {
			if (etk::end_with(it, ".json") == true) {
				etk::Vector<etk::String> path = etk::split(it, '/');
				etk::String filename = path[path.size()-1];
				if (etk::start_with(filename, fileNameIt + "_") == true) {
					fileFiltered.pushBack(it);
				}
			}
		}
		TEST_PRINT("correlation of " << fileFiltered.size() << " files");
		etk::Vector<etk::Vector<float>> results;
		results.resize(fileFiltered.size());
		for (auto &it : results) {
			it.resize(fileFiltered.size(), OUT_OF_RANGE);
		}
		// Generate Full Files:
		etk::String itTypeOfCorpusFileName = itTypeOfCorpus;
		if (itTypeOfCorpusFileName == "/") {
			itTypeOfCorpusFileName = "slash";
		} else if (itTypeOfCorpusFileName == "\\") {
			itTypeOfCorpusFileName = "back-slash";
		} else if (itTypeOfCorpusFileName == "?") {
			itTypeOfCorpusFileName = "question";
		}
		{
			etk::Vector<etk::String> listPath;
			for (size_t iii=0; iii<fileFiltered.size(); ++iii) {
				listPath.pushBack(fileFiltered[iii]);
			}
			generateFile("out_dollar/generate-form/pre_generate/" + itTypeOfCorpusFileName + "_FULL.svg", listPath, "");
		}
		for (size_t iii=0; iii<fileFiltered.size(); ++iii) {
			ememory::SharedPtr<dollar::GesturePPlus> gest = ememory::makeShared<dollar::GesturePPlus>();
			etk::Vector<etk::Vector<vec2>> listPoints = dollar::loadPoints(fileFiltered[iii]);
			gest->set(itTypeOfCorpus, 0, listPoints);
			dollar::EnginePPlus reco;
			reco.setScaleKeepRatio(keepAspectRatio);
			reco.setPPlusDistance(distanceReference);
			reco.setPPlusExcludeDistance(distanceExclude);
			reco.setPenalityNotLinkRef(penalityRef);
			reco.setPenalityNotLinkSample(penalitySample);
			reco.setPenalityAspectRatio(penalityAspectRatio);
			reco.addGesture(gest);
			etk::Vector<etk::String> path = etk::split(fileFiltered[iii], '/');
			etk::String filename = path[path.size()-1];
			TEST_DEBUG("Test :    " << fileFiltered[iii]);
			for (size_t jjj=0; jjj<fileFiltered.size(); ++jjj) {
				if (iii >= jjj) {
					// same element to compare ... result will be 0 ...
					continue;
				}
				listPoints = dollar::loadPoints(fileFiltered[jjj]);
				dollar::Results res = reco.recognize(listPoints);
				results[iii][jjj] = res.getConfidence();
				results[jjj][iii] = res.getConfidence();
				path = etk::split(fileFiltered[jjj], '/');
				etk::String filename2 = path[path.size()-1];
				TEST_DEBUG("         " << res.getConfidence() << "        " << filename2);
			}
		}
		TEST_PRINT("---------------------------------------------------------------------------");
		TEST_PRINT("-- annalyse result to create groups: " << fileFiltered.size());
		TEST_PRINT("---------------------------------------------------------------------------");
		int32_t residualValues = fileFiltered.size();
		int32_t subId = 1;
		
		while (residualValues > 0) {
			etk::Vector<int32_t> countMinimum;
			countMinimum.resize(fileFiltered.size(), 0);
			for (size_t iii=0; iii<fileFiltered.size(); ++iii) {
				for (size_t jjj=0; jjj<fileFiltered.size(); ++jjj) {
					if (results[iii][jjj] < distanceGroupLimiting) {
						countMinimum[iii] ++;
					}
				}
			}
			
			int32_t bestCount = 0;
			int32_t bestId = -1;
			for (size_t iii=0; iii<countMinimum.size(); ++iii) {
				if (countMinimum[iii] > bestCount) {
					bestCount = countMinimum[iii];
					bestId = iii;
				}
			}
			if (bestId == -1) {
				TEST_ERROR("No more elements ... residualValues=" << residualValues);
				
				// TODO : Add the rest of the elements ...
				
				
				//==> Exit loop
				residualValues = 0;
				continue;
			} else {
				TEST_INFO("find NB element : " << countMinimum[bestId] << " for ID=" << bestId);
			}
			// Order the result for the bestID ==> permit to show if it is possible to do a better case ...
			etk::Vector<int32_t> linkIds;
			for (size_t jjj=0; jjj<fileFiltered.size(); ++jjj) {
				if (results[bestId][jjj] < distanceGroupLimiting) {
					linkIds.pushBack(jjj);
				}
			}
			TEST_INFO("        nbEle(tmp) " << linkIds.size() << " / " << residualValues << " / " << fileFiltered.size());
			{
				etk::Vector<etk::Pair<float, size_t>> ordered;
				for (size_t jjj=0; jjj<fileFiltered.size(); ++jjj) {
					float val = results[bestId][jjj];
					if (val >= OUT_OF_RANGE) {
						continue;
					}
					auto it = ordered.begin();
					bool added = false;
					while (it != ordered.end()) {
						if (it->first > val) {
							ordered.insert(it, etk::makePair(val, jjj));
							added = true;
							break;
						}
						++it;
					}
					if (added == false) {
						ordered.pushBack(etk::makePair(val, jjj));
					}
				}
				// enable/disable grouping auto ...
				// TODO : Check if it is possible ...
				if (false) {
					float lastValue = 0.0;
					linkIds.clear();
					for (size_t jjj=0; jjj<ordered.size(); ++jjj) {
						if (ordered[jjj].first < distanceGroupLimiting) {
							linkIds.pushBack(ordered[jjj].second);
						} else {
							// auto find a separation in the group ...
							if (ordered[jjj].first <= (lastValue + 0.15)) {
								linkIds.pushBack(ordered[jjj].second);
							} else {
								break;
							}
						}
						lastValue = ordered[jjj].first;
					}
				}
				TEST_INFO("        nbElement : " << linkIds.size() << " / " << residualValues << " / " << fileFiltered.size());
				TEST_INFO("        values : " << linkIds);
				for (size_t jjj=0; jjj<ordered.size(); ++jjj) {
					etk::Vector<etk::String> path = etk::split(fileFiltered[ordered[jjj].second], '/');
					etk::String filename = path[path.size()-1];
					etk::String tmppp = " ";
					if (jjj<linkIds.size()) {
						tmppp = "*";
					}
					TEST_INFO("         " << tmppp << "  values : " << ordered[jjj].first << "        " << ordered[jjj].second << "  " << filename);
				}
			}
			
			// Clean Best ID line
			for (size_t jjj=0; jjj<results.size(); ++jjj) {
				results[bestId][jjj] = OUT_OF_RANGE;
				results[jjj][bestId] = OUT_OF_RANGE;
			}
			// clean used line by the Best ID...
			for (size_t iii=0; iii<linkIds.size(); ++iii) {
				for (size_t jjj=0; jjj<results.size(); ++jjj) {
					results[linkIds[iii]][jjj] = OUT_OF_RANGE;
					results[jjj][linkIds[iii]] = OUT_OF_RANGE;
				}
			}
			if (linkIds.size() <= 3) {
				TEST_ERROR("Group is too small ... residualValues=" << residualValues);
				// TODO : Add the rest of the elements ...
				//==> Exit loop
				residualValues = 0;
				continue;
			}
			residualValues -= (linkIds.size() +1);
			TEST_DEBUG("Generate output files (SVG with all added path in one file)");
			// Generate Files:
			etk::Vector<etk::String> listPath;
			for (size_t iii=0; iii<linkIds.size(); ++iii) {
				listPath.pushBack(fileFiltered[linkIds[iii]]);
			}
			generateFile("out_dollar/generate-form/pre_generate/" + itTypeOfCorpusFileName + "_" + etk::toString(subId) + ".svg", listPath, fileFiltered[bestId]);
			TEST_DEBUG("Generate output file (corpus ...)");
			// declare a Gesture (internal API)
			dollar::Gesture ref;
			ref.set(itTypeOfCorpus, subId, dollar::loadPoints(fileFiltered[bestId]));
			// Store gesture with his extention type:
			ref.store("out_dollar/generate-form/corpus/" + itTypeOfCorpusFileName + "_" + etk::toString(subId) + ".json");
			ref.store("out_dollar/generate-form/corpus_svg/" + itTypeOfCorpusFileName + "_" + etk::toString(subId) + ".svg");
			
			
			// Increment subId...
			subId++;
			nbElementGenerated++;
		}
	}
	TEST_PRINT("===========================================================================");
	TEST_PRINT("== Gererate Done: " << nbElementGenerated);
	TEST_PRINT("===========================================================================");
	return 0;
}



