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
	TEST_PRINT("    " << _progName << " [option] corpus_path");
	TEST_PRINT("        [option]");
	TEST_PRINT("            -h --help           Display this help");
	TEST_PRINT("        parameters (must be here)");
	TEST_PRINT("            corpus_path         Path of the corpus files");
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
		if(    arg[0] == '-'
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
	if (_refName != "") {
		std::vector<std::vector<vec2>> strokes = dollar::scaleToOne(dollar::loadPoints(_refName));
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
			if (std::find(listOfElementInCorpus.begin(), listOfElementInCorpus.end(), elemName) == listOfElementInCorpus.end()) {
				listOfElementInCorpus.push_back(elemName);
			}
		}
	}
//listOfElementInCorpus.clear();
//listOfElementInCorpus.push_back("z");
// Value to stop grouping in the same element ...
float groupSize = 1.0;
groupSize = 1.0;
bool keepAspectRatio = false;

	TEST_PRINT(" will done for: " << listOfElementInCorpus);
	for (auto &itTypeOfCorpus : listOfElementInCorpus) {
		TEST_PRINT("---------------------------------------------------------------------------");
		TEST_PRINT("-- Generate FOR: '" << itTypeOfCorpus << "'");
		TEST_PRINT("---------------------------------------------------------------------------");
		std::vector<std::string> fileFiltered;
		for (auto &it : files) {
			if (etk::end_with(it, ".json") == true) {
				std::vector<std::string> path = etk::split(it, '/');
				std::string filename = path[path.size()-1];
				if (etk::start_with(filename, itTypeOfCorpus) == true) {
					fileFiltered.push_back(it);
				}
			}
		}
		TEST_PRINT("correlation of " << fileFiltered.size() << " files");
		std::vector<std::vector<float>> results;
		results.resize(fileFiltered.size());
		for (auto &it : results) {
			it.resize(fileFiltered.size(), OUT_OF_RANGE);
		}
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
		for (size_t iii=0; iii<fileFiltered.size(); ++iii) {
			dollar::Gesture gest;
			std::vector<std::vector<vec2>> listPoints = dollar::loadPoints(fileFiltered[iii]);
			gest.set(itTypeOfCorpus, 0, listPoints);
			gest.configure(10, 8, false, 0.1, keepAspectRatio);
			dollar::Engine reco;
			reco.setScaleKeepRatio(keepAspectRatio);
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
				dollar::Results res = reco.recognize(listPoints, "$P+");
				results[iii][jjj] = res.getConfidence();
				results[jjj][iii] = res.getConfidence();
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
			std::vector<int32_t> countMinimum;
			countMinimum.resize(fileFiltered.size(), 0);
			for (size_t iii=0; iii<fileFiltered.size(); ++iii) {
				for (size_t jjj=0; jjj<fileFiltered.size(); ++jjj) {
					if (results[iii][jjj] < groupSize) {
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
			std::vector<int32_t> linkIds;
			for (size_t jjj=0; jjj<fileFiltered.size(); ++jjj) {
				if (results[bestId][jjj] < groupSize) {
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
				if (false) {
					float lastValue = 0.0;
					linkIds.clear();
					for (size_t jjj=0; jjj<ordered.size(); ++jjj) {
						if (ordered[jjj].first < groupSize) {
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
			std::vector<std::string> listPath;
			for (size_t iii=0; iii<linkIds.size(); ++iii) {
				listPath.push_back(fileFiltered[linkIds[iii]]);
			}
			generateFile("out_dollar/generate-form/pre_generate/" + itTypeOfCorpusFileName + "_" + etk::to_string(subId) + ".svg", listPath, fileFiltered[bestId]);
			TEST_DEBUG("Generate output file (corpus ...)");
			// declare a Gesture (internal API)
			dollar::Gesture ref;
			ref.set(itTypeOfCorpus, subId, dollar::loadPoints(fileFiltered[bestId]));
			// Store gesture with his extention type:
			ref.store("out_dollar/generate-form/corpus/" + itTypeOfCorpusFileName + "_" + etk::to_string(subId) + ".json");
			ref.store("out_dollar/generate-form/corpus_svg/" + itTypeOfCorpusFileName + "_" + etk::to_string(subId) + ".svg");
			
			
			// Increment subId...
			subId++;
		}
	}
	return 0;
}



