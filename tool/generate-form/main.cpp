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
#include <map>



static bool keepAspectRatio = false;
static float distanceReference = 0.1f; // distance of the gesture reference [0.02, 0.3]
static float distanceExclude = 0.2f; // distance of the exclusion point     [0.1, 1.0]
static float distanceGroupLimiting = 1.0f; // square distance of the grouping genereation
static float penalityRef = 0.1;
static float penalitySample = 0.1;

void usage(const std::string& _progName) {
	TEST_PRINT("usage:");
	TEST_PRINT("    " << _progName << " [option] corpus_path");
	TEST_PRINT("        ");
	TEST_PRINT("        [option]");
	TEST_PRINT("            -h --help             Display this help");
	TEST_PRINT("            --keep_ratio          Keep aspect ratio for the form recognition (default:" + etk::to_string(keepAspectRatio) + ")");
	TEST_PRINT("            --dist-check=flaot    distance between points in the system recognition (default:" + etk::to_string(distanceReference) + ")");
	TEST_PRINT("            --dist-excl=flaot     distance to exclude a point in a pathern matching ... (default:" + etk::to_string(distanceExclude) + ")");
	TEST_PRINT("            --group-size=flaot    size of the distance between point to stop grouping in one form (default:" + etk::to_string(distanceGroupLimiting) + ")");
	TEST_PRINT("            --penal-ref=float     Penality for reference when not connected (default:" + etk::to_string(penalityRef) + ")");
	TEST_PRINT("            --penal-sample=float  Penality for sample when not connected (default:" + etk::to_string(penalitySample) + ")");
	TEST_PRINT("        ");
	TEST_PRINT("        parameters (must be here)");
	TEST_PRINT("            corpus_path         Path of the corpus files");
	TEST_PRINT("        ");
	TEST_PRINT("        The generate data is in out_dollar/xxx");
	TEST_PRINT("        example:");
}

bool testCorpus(const std::string& _srcCorpus);


int main(int _argc, const char *_argv[]) {
	// init etk log system and file interface:
	etk::init(_argc, _argv);
	std::string srcCorpus;
	
	for (int32_t iii=1; iii<_argc; ++iii) {
		std::string arg = _argv[iii];
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
			std::string val(&arg[11]);
			distanceReference = etk::string_to_float(val);
			TEST_PRINT("configure distanceReference=" << distanceReference);
			continue;
		}
		if (etk::start_with(arg,"--dist-excl=") == true) {
			std::string val(&arg[12]);
			distanceExclude = etk::string_to_float(val);
			TEST_PRINT("configure distanceExclude=" << distanceExclude);
			continue;
		}
		if (etk::start_with(arg,"--group-size=") == true) {
			std::string val(&arg[13]);
			distanceGroupLimiting = etk::string_to_float(val);
			TEST_PRINT("configure distanceGroupLimiting=" << distanceGroupLimiting);
			continue;
		}
		if (etk::start_with(arg,"--penal-ref=") == true) {
			std::string val(&arg[12]);
			penalityRef = etk::string_to_float(val);
			TEST_PRINT("configure penalityRef=" << penalityRef);
			continue;
		}
		if (etk::start_with(arg,"--penal-sample=") == true) {
			std::string val(&arg[15]);
			penalityRef = etk::string_to_float(val);
			TEST_PRINT("configure penalitySample=" << penalitySample);
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

void generateFile(const std::string& _fileName, const std::vector<std::string>& _list, const std::string& _refName) {
	TEST_PRINT("    " << _fileName);
	std::string data("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
	data += "<svg height=\"100\" width=\"100\">\n";
	for (auto &itFile : _list) {
		std::vector<std::vector<vec2>> strokes = dollar::scaleToOne(dollar::loadPoints(itFile), keepAspectRatio);
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
	if (_refName != "") {
		std::vector<std::vector<vec2>> strokes = dollar::scaleToOne(dollar::loadPoints(_refName), keepAspectRatio);
		for (auto &itLines : strokes) {
			data += "	<polyline fill=\"none\" stroke=\"red\" stroke-opacity=\"1.0\" stroke-width=\"2\"\n";
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

#define OUT_OF_RANGE (999999.0f)

bool testCorpus(const std::string& _srcCorpus) {
	TEST_PRINT("---------------------------------------------------------------------------");
	TEST_PRINT("-- Create list of files: " << _srcCorpus);
	TEST_PRINT("---------------------------------------------------------------------------");
	etk::FSNode path(_srcCorpus);
	std::vector<std::string> files = path.folderGetSub(false, true, "*.json");

	TEST_PRINT("corpus have " << files.size() << " files");
	std::vector<std::string> listOfElementInCorpus;
	for (auto &it : files) {
		if (etk::end_with(it, ".json") == true) {
			std::vector<std::string> path = etk::split(it, '/');
			std::string elemName = etk::split(path[path.size()-1],'_')[0];
			if (elemName == "slash") {
				elemName = "/";
			}if (elemName == "back-slash") {
				elemName = "\\";
			}
			if (std::find(listOfElementInCorpus.begin(), listOfElementInCorpus.end(), elemName) == listOfElementInCorpus.end()) {
				listOfElementInCorpus.push_back(elemName);
			}
		}
	}
	// remove generation path ...
	etk::FSNodeRemove("out_dollar/generate-form");
//listOfElementInCorpus.clear();
//listOfElementInCorpus.push_back("slash");


	TEST_PRINT(" will done for: " << listOfElementInCorpus);
	int32_t nbElementGenerated = 0;
	for (auto &itTypeOfCorpus : listOfElementInCorpus) {
		TEST_PRINT("---------------------------------------------------------------------------");
		TEST_PRINT("-- Generate FOR: '" << itTypeOfCorpus << "'");
		TEST_PRINT("---------------------------------------------------------------------------");
		std::vector<std::string> fileFiltered;
		std::string fileNameIt = itTypeOfCorpus;
		if (fileNameIt == "/") {
			fileNameIt = "slash";
		} else if (fileNameIt == "\\") {
			fileNameIt = "back-slash";
		}
		for (auto &it : files) {
			if (etk::end_with(it, ".json") == true) {
				std::vector<std::string> path = etk::split(it, '/');
				std::string filename = path[path.size()-1];
				if (etk::start_with(filename, fileNameIt + "_") == true) {
					fileFiltered.push_back(it);
				}
			}
		}
		TEST_PRINT("correlation of " << fileFiltered.size() << " files");
		// Generate Full Files:
		std::string itTypeOfCorpusFileName = itTypeOfCorpus;
		if (itTypeOfCorpusFileName == "/") {
			itTypeOfCorpusFileName = "slash";
		} else if (itTypeOfCorpusFileName == "\\") {
			itTypeOfCorpusFileName = "back-slash";
		} else if (itTypeOfCorpusFileName == "?") {
			itTypeOfCorpusFileName = "question";
		}
		{
			std::vector<std::string> listPath;
			for (size_t iii=0; iii<fileFiltered.size(); ++iii) {
				listPath.push_back(fileFiltered[iii]);
			}
			generateFile("out_dollar/generate-form/pre_generate/" + itTypeOfCorpusFileName + "_FULL.svg", listPath, "");
		}
		///////////////////////////////////////////////////////////////////////////////
		// Test without keep Aspect ratio    NO:
		///////////////////////////////////////////////////////////////////////////////
		// Set empty results:
		std::vector<std::vector<float>> results;
		results.resize(fileFiltered.size());
		for (auto &it : results) {
			it.resize(fileFiltered.size(), OUT_OF_RANGE);
		}
		for (size_t iii=0; iii<fileFiltered.size(); ++iii) {
			ememory::SharedPtr<dollar::GesturePPlus> gest = ememory::makeShared<dollar::GesturePPlus>();
			std::vector<std::vector<vec2>> listPoints = dollar::loadPoints(fileFiltered[iii]);
			gest->set(itTypeOfCorpus, 0, listPoints);
			dollar::EnginePPlus reco;
			reco.setScaleKeepRatio(false);//keepAspectRatio);
			reco.setPPlusDistance(distanceReference);
			reco.setPPlusExcludeDistance(distanceExclude);
			reco.setPenalityNotLinkRef(penalityRef);
			reco.setPenalityNotLinkSample(penalitySample);
			reco.addGesture(gest);
			std::vector<std::string> path = etk::split(fileFiltered[iii], '/');
			std::string filename = path[path.size()-1];
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
				std::string filename2 = path[path.size()-1];
				TEST_DEBUG("         " << res.getConfidence() << "        " << filename2);
			}
		}
		///////////////////////////////////////////////////////////////////////////////
		// Test with keep Aspect ratio    YES:
		///////////////////////////////////////////////////////////////////////////////
		std::vector<std::vector<float>> resultsKeepAspectRatio;
		resultsKeepAspectRatio.resize(fileFiltered.size());
		for (auto &it : resultsKeepAspectRatio) {
			it.resize(fileFiltered.size(), OUT_OF_RANGE);
		}
		for (size_t iii=0; iii<fileFiltered.size(); ++iii) {
			ememory::SharedPtr<dollar::GesturePPlus> gest = ememory::makeShared<dollar::GesturePPlus>();
			std::vector<std::vector<vec2>> listPoints = dollar::loadPoints(fileFiltered[iii]);
			gest->set(itTypeOfCorpus, 0, listPoints);
			dollar::EnginePPlus reco;
			reco.setScaleKeepRatio(true);//keepAspectRatio);
			reco.setPPlusDistance(distanceReference);
			reco.setPPlusExcludeDistance(distanceExclude);
			reco.setPenalityNotLinkRef(penalityRef);
			reco.setPenalityNotLinkSample(penalitySample);
			reco.addGesture(gest);
			std::vector<std::string> path = etk::split(fileFiltered[iii], '/');
			std::string filename = path[path.size()-1];
			TEST_DEBUG("Test :    " << fileFiltered[iii]);
			for (size_t jjj=0; jjj<fileFiltered.size(); ++jjj) {
				if (iii >= jjj) {
					// same element to compare ... result will be 0 ...
					continue;
				}
				listPoints = dollar::loadPoints(fileFiltered[jjj]);
				dollar::Results res = reco.recognize(listPoints);
				resultsKeepAspectRatio[iii][jjj] = res.getConfidence();
				resultsKeepAspectRatio[jjj][iii] = res.getConfidence();
				path = etk::split(fileFiltered[jjj], '/');
				std::string filename2 = path[path.size()-1];
				TEST_DEBUG("         " << res.getConfidence() << "        " << filename2);
			}
		}
		
		
		
		TEST_PRINT("---------------------------------------------------------------------------");
		TEST_PRINT("-- annalyse result to create groups: " << fileFiltered.size());
		TEST_PRINT("---------------------------------------------------------------------------");
		int32_t residualValues = fileFiltered.size();
		int32_t subId = 1;
		
		while (residualValues > 0) {
			int32_t bestId = -1;
			int32_t bestIdKeep = -1;
			///////////////////////////////////////////////////////////////////////////////
			// Test without keep Aspect ratio    NO:
			///////////////////////////////////////////////////////////////////////////////
			std::vector<int32_t> countMinimum;
			{
				countMinimum.resize(fileFiltered.size(), 0);
				for (size_t iii=0; iii<fileFiltered.size(); ++iii) {
					for (size_t jjj=0; jjj<fileFiltered.size(); ++jjj) {
						if (results[iii][jjj] < distanceGroupLimiting) {
							countMinimum[iii] ++;
						}
					}
				}
				
				int32_t bestCount = 0;
				for (size_t iii=0; iii<countMinimum.size(); ++iii) {
					if (countMinimum[iii] > bestCount) {
						bestCount = countMinimum[iii];
						bestId = iii;
					}
				}
				if (bestId != -1) {
					TEST_INFO("find NB element : " << countMinimum[bestId] << " for ID=" << bestId);
				}
			}
			///////////////////////////////////////////////////////////////////////////////
			// Test with keep Aspect ratio    YES:
			///////////////////////////////////////////////////////////////////////////////
			std::vector<int32_t> countMinimumKeep;
			{
				countMinimumKeep.resize(fileFiltered.size(), 0);
				for (size_t iii=0; iii<fileFiltered.size(); ++iii) {
					for (size_t jjj=0; jjj<fileFiltered.size(); ++jjj) {
						if (resultsKeepAspectRatio[iii][jjj] < distanceGroupLimiting) {
							countMinimumKeep[iii] ++;
						}
					}
				}
				int32_t bestCount = 0;
				for (size_t iii=0; iii<countMinimumKeep.size(); ++iii) {
					if (countMinimumKeep[iii] > bestCount) {
						bestCount = countMinimumKeep[iii];
						bestIdKeep = iii;
					}
				}
				if (bestIdKeep != -1) {
					TEST_INFO("find NB element : " << countMinimumKeep[bestId] << " for ID=" << bestId << "        KEEP" );
				}
			}
			// Exit main loop if needed:
			if (    bestId == -1
			     && bestIdKeep == -1) {
				TEST_ERROR("No more elements ... residualValues=" << residualValues);
				// TODO : Add the rest of the elements ...
				//==> Exit loop
				residualValues = 0;
				continue;
			}
			std::vector<int32_t> linkIds;
			if (countMinimum[bestId] >= countMinimumKeep[bestIdKeep]) {
				TEST_WARNING("    ==> select No aspect ratio");
			} else {
				TEST_WARNING("    ==> select KEEP aspect ratio");
			}
			if (countMinimum[bestId] >= countMinimumKeep[bestIdKeep]) {
				// Order the result for the bestID ==> permit to show if it is possible to do a better case ...
				for (size_t jjj=0; jjj<fileFiltered.size(); ++jjj) {
					if (results[bestId][jjj] < distanceGroupLimiting) {
						linkIds.push_back(jjj);
					}
				}
				TEST_INFO("        nbEle(tmp) " << linkIds.size() << " / " << residualValues << " / " << fileFiltered.size());
				{
					std::vector<std::pair<float, size_t>> ordered;
					for (size_t jjj=0; jjj<fileFiltered.size(); ++jjj) {
						float val = results[bestId][jjj];
						if (val >= OUT_OF_RANGE) {
							continue;
						}
						auto it = ordered.begin();
						bool added = false;
						while (it != ordered.end()) {
							if (it->first > val) {
								ordered.insert(it, std::make_pair(val, jjj));
								added = true;
								break;
							}
							++it;
						}
						if (added == false) {
							ordered.push_back(std::make_pair(val, jjj));
						}
					}
					// enable/disable grouping auto ...
					// TODO : Check if it is possible ...
					if (false) {
						float lastValue = 0.0;
						linkIds.clear();
						for (size_t jjj=0; jjj<ordered.size(); ++jjj) {
							if (ordered[jjj].first < distanceGroupLimiting) {
								linkIds.push_back(ordered[jjj].second);
							} else {
								// auto find a separation in the group ...
								if (ordered[jjj].first <= (lastValue + 0.15)) {
									linkIds.push_back(ordered[jjj].second);
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
						std::vector<std::string> path = etk::split(fileFiltered[ordered[jjj].second], '/');
						std::string filename = path[path.size()-1];
						std::string tmppp = " ";
						if (jjj<linkIds.size()) {
							tmppp = "*";
						}
						TEST_INFO("         " << tmppp << "  values : " << ordered[jjj].first << "        " << ordered[jjj].second << "  " << filename);
					}
				}
			} else {
				// Order the result for the bestIdKeep ==> permit to show if it is possible to do a better case ...
				for (size_t jjj=0; jjj<fileFiltered.size(); ++jjj) {
					if (resultsKeepAspectRatio[bestIdKeep][jjj] < distanceGroupLimiting) {
						linkIds.push_back(jjj);
					}
				}
				TEST_INFO("        nbEle(tmp) " << linkIds.size() << " / " << residualValues << " / " << fileFiltered.size());
				{
					std::vector<std::pair<float, size_t>> ordered;
					for (size_t jjj=0; jjj<fileFiltered.size(); ++jjj) {
						float val = resultsKeepAspectRatio[bestIdKeep][jjj];
						if (val >= OUT_OF_RANGE) {
							continue;
						}
						auto it = ordered.begin();
						bool added = false;
						while (it != ordered.end()) {
							if (it->first > val) {
								ordered.insert(it, std::make_pair(val, jjj));
								added = true;
								break;
							}
							++it;
						}
						if (added == false) {
							ordered.push_back(std::make_pair(val, jjj));
						}
					}
					// enable/disable grouping auto ...
					// TODO : Check if it is possible ...
					if (false) {
						float lastValue = 0.0;
						linkIds.clear();
						for (size_t jjj=0; jjj<ordered.size(); ++jjj) {
							if (ordered[jjj].first < distanceGroupLimiting) {
								linkIds.push_back(ordered[jjj].second);
							} else {
								// auto find a separation in the group ...
								if (ordered[jjj].first <= (lastValue + 0.15)) {
									linkIds.push_back(ordered[jjj].second);
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
						std::vector<std::string> path = etk::split(fileFiltered[ordered[jjj].second], '/');
						std::string filename = path[path.size()-1];
						std::string tmppp = " ";
						if (jjj<linkIds.size()) {
							tmppp = "*";
						}
						TEST_INFO("         " << tmppp << "  values : " << ordered[jjj].first << "        " << ordered[jjj].second << "  " << filename);
					}
				}
			}
			// Clean Best ID line
			bool removeId = bestIdKeep;
			if (countMinimum[bestId] >= countMinimumKeep[bestIdKeep]) {
				removeId = bestId;
			}
			for (size_t jjj=0; jjj<results.size(); ++jjj) {
				results[removeId][jjj] = OUT_OF_RANGE;
				results[jjj][removeId] = OUT_OF_RANGE;
			}
			for (size_t jjj=0; jjj<resultsKeepAspectRatio.size(); ++jjj) {
				resultsKeepAspectRatio[removeId][jjj] = OUT_OF_RANGE;
				resultsKeepAspectRatio[jjj][removeId] = OUT_OF_RANGE;
			}
			// clean used line by the Best ID...
			for (size_t iii=0; iii<linkIds.size(); ++iii) {
				for (size_t jjj=0; jjj<results.size(); ++jjj) {
					results[linkIds[iii]][jjj] = OUT_OF_RANGE;
					results[jjj][linkIds[iii]] = OUT_OF_RANGE;
				}
			}
			for (size_t iii=0; iii<linkIds.size(); ++iii) {
				for (size_t jjj=0; jjj<resultsKeepAspectRatio.size(); ++jjj) {
					resultsKeepAspectRatio[linkIds[iii]][jjj] = OUT_OF_RANGE;
					resultsKeepAspectRatio[jjj][linkIds[iii]] = OUT_OF_RANGE;
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
			std::vector<std::string> listPath;
			for (size_t iii=0; iii<linkIds.size(); ++iii) {
				listPath.push_back(fileFiltered[linkIds[iii]]);
			}
			generateFile("out_dollar/generate-form/pre_generate/" + itTypeOfCorpusFileName + "_" + etk::to_string(subId) + ".svg", listPath, fileFiltered[bestId]);
			TEST_DEBUG("Generate output file (corpus ...)");
			// declare a Gesture (internal API)
			dollar::Gesture ref;
			if (countMinimum[bestId] >= countMinimumKeep[bestIdKeep]) {
				ref.setKeepAspectRatio(false);
			} else {
				ref.setKeepAspectRatio(true);
			}
			ref.set(itTypeOfCorpus, subId, dollar::loadPoints(fileFiltered[bestId]));
			// Store gesture with his extention type:
			ref.store("out_dollar/generate-form/corpus/" + itTypeOfCorpusFileName + "_" + etk::to_string(subId) + ".json");
			ref.store("out_dollar/generate-form/corpus_svg/" + itTypeOfCorpusFileName + "_" + etk::to_string(subId) + ".svg");
			
			
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



